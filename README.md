# My analysis tools for Kanazawa U. lab. CMOS event data 

## Event data file binary-to-fits converter 
### - Download program source files in this repository 
```
Makefile       capreolib.c    capreolib.h    dumpbindat.c   evtbin2fits.c
```

### - Build program `evtbin2fits`
Edit Makefile for cfitsion library and make `evtbin2fits`
```
$ make evtbin2fits
```

### - Usage
```
$ ./evtbin2fits (binary data filename) (output fits filename)

Example 
$ ./evtbin2fits (some data directory)/24-0613-2003-31.bin 24-0613-2003-31.evt
```

## Analysis example
- Example of plotting PHA histogram and image in juypyter-notebook [ql_kzcmosevt.ipynb](sample/ql_kzcmosevt.ipynb)
