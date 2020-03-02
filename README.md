# fitsOps
FITS file read and modify.

# Usage
Input file name with .fits extension.
```
display [arg] // arg = all, header name

update [arg1] [arg2] // arg1 = header name, arg2 = new value
```

## What is working?
1. Read headers.
2. Modifies the headers
3. Modifies COMMENT and HISTORY keywords too.
4. Mandotory keywords are write protected.

## Limitations
1. Cannot read Extentions.
2. No Type Checking

