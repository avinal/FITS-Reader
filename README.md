# fitsReader
FITS file read and modify.


# How to build
1. This program uses conan as package manager make sure to config conan before building. 
   see [here](https://docs.conan.io/en/latest/getting_started.html) for more about conan.
2. To config conan on either machine follow these steps
   ```
   git clone https://github.com/avinal/fitsReader.git
   conan
   cd <your-path-here>/fitsReader
   mkdir build
   cd build
   conan install ..
   ```
3. For Windows using CMAKE
   ```
   cd build
   conan install ..
   cmake .. -G "Visual Studio 16"
   cmake --build . --config Release
   ```
4. For Linux Systems using CMAKE
   ```
   cd build
   conan install ..
   cmake .. -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release
   cmake --build .
   ```
5. Don't forget to put FITS file in fitsReader/build/bin directory when running. 

# Usage
Input file name with .fits extension.
```
display [arg] // arg = all , header name, xtension 

all for all headers including extensions
header name for a particular header
xtension for first extension

update [arg1] [arg2] // arg1 = header name, arg2 = new value

```

## What is working?
1. Read headers.
2. Modifies the headers
3. Modifies COMMENT and HISTORY keywords too.
4. Mandotory keywords are write protected.
5. Can read first encountered extension.

## Limitations
1. No Type Checking
2. If the file is not correct by even 1 byte, the program exits.

