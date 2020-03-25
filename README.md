# fitsReader
FITS file read and modify.

# How to build
1. This program uses conan as package manager make sure to config conan before building
2. For Windows use cmake 
   ```
   cmake .. -G "Visual Studio 16"
   cmake --build . --config Release
   ```

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

