# Comparison of bundled and unbundled approaches for js scripts compression.
  
The script compares an unbundled approach with a bundled one for js scripts compression for gzip and brotli of different levels.
After running the script you will get a file `compression_results.json` of results of the compression. An example of such a file could be found [here](compression_results.json).


## How to run the script
Compression script has an argument ‘number_repetitions’ which shows how many times to compress each bundle so the result would be more stable.

```
$ cd bundle_analyzer
$ npm i
$ cd ..
$ make
$  ./compression 1
```

Before running make sure you have a folder named `bundles_source` in current directory. This folder should contain a file `bundle_files.txt` with names of files with bundles.
Example of `bundles_files.txt`:
```
bundles_source/bundle_0.txt
bundles_source/bundle_1.txt
bundles_source/bundle_2.txt
bundles_source/bundle_3.txt
bundles_source/bundle_4.txt
bundles_source/bundle_5.txt
```

The folder `bundles_source` should also have files written in `bundle_files.txt` containing text of one bundle each.
