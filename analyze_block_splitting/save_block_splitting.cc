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
#include <iostream>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <stdexcept>
#include <string>

int DEFAULT_WINDOW = 24;

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

void BrotliCompressAndSaveBlockSplitting(int level, int window, const unsigned char* input_data, size_t input_size, unsigned char* output_data, size_t output_buffer_size) {
  ShouldSaveBlockSplit();
  if (!BrotliEncoderCompress(level, window, BROTLI_MODE_GENERIC, input_size, input_data, &output_buffer_size, output_data)) {
    throw "Failure in BrotliCompress";
  }
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



int main (int argc, char** argv) {
  try {
    char* bundle_file = argv[1];
    FILE* infile = OpenFile(bundle_file, "rb");
    if (infile == NULL) {
        exit(1);
    }
    unsigned char* input_data = NULL;
    size_t input_size = 0;
    ReadData(infile, &input_data, &input_size);
    fclose(infile);
    size_t output_buffer_size = input_size * 2;
    unsigned char* output_data = (unsigned char*) malloc(output_buffer_size);
    int window = MinWindowLargerThanFile(input_size, DEFAULT_WINDOW);
    BrotliCompressAndSaveBlockSplitting(11, window, input_data, input_size, output_data, output_buffer_size);
  } catch (const char* message) {
      std::cout << "Error\n";
  }
  return 0;
}
