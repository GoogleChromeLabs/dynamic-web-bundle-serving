// Copyright 2020 Google Inc. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <stdlib.h>
#include <stdio.h>
#include <brotli/encode.h>
#include <zlib.h>
#include <time.h>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <stdexcept>
#include <string>
#include "json.hpp"

using json = nlohmann::json;

int DEFAULT_WINDOW = 24;

struct CompressionStatistics {
    float compressed_size;
    float compression_time;
    CompressionStatistics(float size, float time) : compressed_size(size), compression_time(time) {}
};

size_t FileSize(FILE* file) {
  fseek(file, 0, SEEK_END);
  size_t size = ftell(file);
  fseek(file, 0, SEEK_SET);
  return size;
}

FILE* OpenFile(const char* filename, const char* mode) {
  FILE* file = fopen(filename, mode);
  if (file == NULL) {
    perror("fopen failed");
  }
  return file;
}

void ReadData(FILE* file, unsigned char** data, size_t* size) {
  *size = FileSize(file);
  *data = (unsigned char*) malloc(*size);
  if (0 == fread(*data, 1, *size, file)) {
    throw "Failed to read from file";
  }
  return;
}

void GetNamesFromFile(std::string file_name, std::vector<std::string>& names) {
    std::string line;
    std::ifstream infile(file_name);
    while (std::getline(infile, line)) {
        names.push_back(line);
    }
    infile.close();
}

size_t BrotliCompress(int level, int window, const unsigned char* input_data, size_t input_size, unsigned char* output_data, size_t output_buffer_size) {
  if (!BrotliEncoderCompress(level, window, BROTLI_MODE_GENERIC, input_size, input_data, &output_buffer_size, output_data)) {
    throw "Failure in BrotliCompress";
  }
  return output_buffer_size;
}

size_t ZlibCompress(int level, int window, const unsigned char* input_data, size_t input_size, unsigned char* output_data, size_t output_buffer_size) {
  z_stream strm;
  strm.zalloc = Z_NULL;
  strm.zfree = Z_NULL;
  strm.opaque = Z_NULL;
  if (Z_OK != deflateInit2(&strm, level, Z_DEFLATED, 15 + 16, 8, Z_DEFAULT_STRATEGY)) {
    throw "Failure in deflateInit";
  }
  strm.avail_in = input_size;
  strm.next_in = (unsigned char*) input_data;
  strm.avail_out = output_buffer_size;
  strm.next_out = output_data;
  if (Z_STREAM_ERROR == deflate(&strm, Z_FINISH)) {
    throw "Failure in deflate";
  }
  if (0 != strm.avail_in) {
    throw "Failed to consume entire input in deflate";
  }
  size_t output_size = output_buffer_size - strm.avail_out;
  deflateEnd(&strm);
  return output_size;
}

typedef size_t (*CompressionFunc)(int, int, const unsigned char*, size_t, unsigned char*, size_t);

CompressionStatistics MeasureCompress(int level, int window, 
                                       const unsigned char* input_data, size_t input_size, 
                                       unsigned char* output_data, size_t output_buffer_size, 
                                       CompressionFunc compress, int repetitions) {
  size_t total_output_size = 0;
  clock_t start = clock();
  for (int i = 0 ; i < repetitions ; i++) {
    total_output_size += compress(level, window, input_data, input_size, output_data, output_buffer_size);
  }
  clock_t end = clock();
  float elapsed_time = (float) (end - start) / CLOCKS_PER_SEC;
  
  float compressed_size = (float) total_output_size / repetitions;
  return CompressionStatistics(compressed_size, elapsed_time);
}

int MinWindowLargerThanFile(int fileSize, int max) {
    int window = 24;
    if (fileSize > 0) {
        window = 10;
        while (((size_t)1 << (window)) - 16 < (uint64_t)fileSize) {
            ++window;
            if (window == max) break;
        }
    }
    return window;
}

bool Execute(const char* cmd) {
    char buffer[128];
    std::string stdout = "";
    FILE* pipe = popen(cmd, "r");
    if (!pipe) throw std::runtime_error("popen() failed!");
    try {
        while (fgets(buffer, sizeof buffer, pipe) != NULL) {
            stdout += buffer;
        }
    } catch (...) {
        pclose(pipe);
        throw;
    }
    pclose(pipe);
    if (stdout.find("TypeError") != std::string::npos) {
        return false;
    }
    return true;
}

void BundledCompression(const unsigned char* input_data, size_t input_size, 
                         unsigned char* output_data, size_t output_buffer_size, 
                         std::ostream & results, int repetitions) {

    results << "\"bundled\":{";
    int window = MinWindowLargerThanFile(input_size, DEFAULT_WINDOW);
    std::string name = "brotli";
    for (int level = 1 ; level <= 11 ; level ++) {
      CompressionStatistics comp_results = MeasureCompress(level, window, input_data, input_size, 
                                                            output_data, output_buffer_size, 
                                                            BrotliCompress, repetitions);
      float rate = input_size / comp_results.compressed_size;
      float speed = (float) (input_size * repetitions) / (comp_results.compression_time * 1024 * 1024);
      results << "\"" << name << level << "_compression_rate\":" << std::setprecision(4) << rate << ", \"";
      results << "\"" << name << level << "_compressed_size\":" << std::setprecision(4) << comp_results.compressed_size << ", \"";
      results << name << level << "_speed\":" << std::setprecision(4) << speed << ",\n";
    }

    name = "zlib";
    for (int level = 1 ; level <= 9 ; level ++) {
      CompressionStatistics comp_results = MeasureCompress(level, window, input_data, input_size, 
                                                            output_data, output_buffer_size, 
                                                            ZlibCompress, repetitions);
      float rate = input_size / comp_results.compressed_size;
      float speed = (float) (input_size * repetitions) / (comp_results.compression_time * 1024 * 1024);
      results << "\"" << name << level << "_compression_rate\":" << std::setprecision(4) << rate << ", \"";
      results << "\"" << name << level << "_compressed_size\":" << std::setprecision(4) << comp_results.compressed_size << ", \"";
      results << name << level << "_speed\":" << std::setprecision(4) << speed;
      if (level < 9) {
          results << ",\n";
      } else {
          results << "}\n";
      }
    }
}


void UnbundledCompression(const unsigned char* input_data, size_t input_size, 
                           unsigned char* output_data, size_t output_buffer_size, 
                           std::ostream & results, int repetitions, std::string file_name) {
    
    std::string command = "node --experimental-modules bundle_analyzer/get_chunks.js " + file_name +
        " parsed_bundle.json 2>&1";
    bool execution_result = Execute(command.c_str());
    if (!execution_result) {
        results << "\"unbundled\":{\"execution_result\": : " << execution_result << "}\n";
        return;
    }
    
    std::ifstream parsed_bundle("parsed_bundle.json");
    json chunks;
    parsed_bundle >> chunks;
    std::string code;
    std::vector<float> compressed_sizes(11 + 9, 0);
    std::vector<float> compression_times(11 + 9, 0);
    int overall_size = 0;
    for (const auto& chunk : chunks) {
        code = chunk["code"];
        int chunk_size = code.size();
        std::vector<CompressionStatistics> statistics_chunk;
        int window = MinWindowLargerThanFile(chunk_size, DEFAULT_WINDOW);
        std::string name = "brotli";
        overall_size += chunk_size;
        for (int level = 1 ; level <= 11 ; level ++) {
          CompressionStatistics comp_results = MeasureCompress(level, window, (unsigned char*)code.c_str(), chunk_size, 
                                                                output_data, output_buffer_size, BrotliCompress, repetitions);
          compressed_sizes[level - 1] += comp_results.compressed_size;
          compression_times[level - 1] += comp_results.compression_time;
        }
        for (int level = 1 ; level <= 9 ; level ++) {
          CompressionStatistics comp_results = MeasureCompress(level, window, (unsigned char*)code.c_str(), chunk_size, 
                                                                output_data, output_buffer_size, ZlibCompress, repetitions);
          compressed_sizes[11 + level - 1] += comp_results.compressed_size;
          compression_times[11 + level - 1] += comp_results.compression_time;
        }
    }
    
    results << "\"overall_chunks_size\":" << overall_size << ",\n";
    results << "\"unbundled\":{";
    std::string name = "brotli";
    for (int level = 1 ; level <= 11 ; level ++) {
      float rate = input_size / compressed_sizes[level - 1];
      float speed = (float) (input_size * repetitions) / (compression_times[level - 1] * 1024 * 1024);
      results << "\"" << name << level << "_compression_rate\":" << std::setprecision(4) << rate << ", \"";
      results << "\"" << name << level << "_compressed_size\":" << std::setprecision(4) << compressed_sizes[level - 1] << ", \"";
      results << name << level << "_speed\":" << std::setprecision(4) << speed << ",\n";
    }

    name = "zlib";
    for (int level = 1 ; level <= 9 ; level ++) {
      float rate = input_size / compressed_sizes[11 + level - 1];
      float speed = (float) (input_size * repetitions) / (compression_times[11 + level - 1] * 1024 * 1024);
      results << "\"" << name << level << "_compression_rate\":" << std::setprecision(4) << rate << ", \"";
      results << "\"" << name << level << "_compressed_size\":" << std::setprecision(4) << compressed_sizes[11 + level - 1] << ", \"";
      results << name << level << "_speed\":" << std::setprecision(4) << speed;
      if (level < 9) {
          results << ",\n";
      } else {
          results << "},\n";
      }
    }
}


int main (int argc, char** argv) {
  try {
    std::vector<std::string> bundle_files;
    GetNamesFromFile("bundles_source/bundle_files.txt", bundle_files);
    int repetitions = std::stoi(std::string(argv[1]));
    std::ostringstream results;
    results << "[";
    for (int i = 0; i < bundle_files.size(); ++i) {
        FILE* infile = OpenFile(bundle_files[i].c_str(), "rb");
        if (infile == NULL) {
          exit(1);
        }
        unsigned char* input_data = NULL;
        size_t input_size = 0;
        ReadData(infile, &input_data, &input_size);
        fclose(infile);
        size_t output_buffer_size = input_size * 2;
        unsigned char* output_data = (unsigned char*) malloc(output_buffer_size);


        results << "{\"valid\":true, \"original_size\":" << input_size << ",\n";

        UnbundledCompression(input_data, input_size, output_data, output_buffer_size, 
                            results, repetitions, bundle_files[i]);

        BundledCompression(input_data, input_size, output_data, output_buffer_size, 
                           results, repetitions);
        if (i == bundle_files.size() - 1) {
            results << "}]\n";
        } else {
           results << "},\n"; 
        }
        std::cout << "bundle number " << i << " completed\n";
    }
    std::ofstream out("compression_results.json");
    out << results.str();
    out.close();
  } catch (const char* message) {
    std::ofstream out("compression_results.json");
    out << "{\"valid\":false, \"message\":\"" << message << "\"}\n";
    out.close();
  }
  return 0;
}
