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

## Limitations
1. Cannot read Extentions.
2. Can modify protected headers too. *Oops! Bad :|*
3. Ignores COMMENT and HISTORY headers
4. Cannot modify comments.
5. No Type Checking

