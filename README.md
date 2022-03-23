# NYCU CA2022 Homework 1

## Build instruction

Use script/unzip.py to unzip the zip file downloaded from the newE3.

Put the `code` folder's content from the script into `code` folder in this directory.

Then run

```bash=
cmake -S . -B build -D CMAKE_BUILD_TYPE=Release
cmake --build build --config Release --parallel 8
```
