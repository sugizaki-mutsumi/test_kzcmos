
TARGETS = dumpbindat \
	evtbin2fits 

#	evtbin2csv


### primary targets
.PHONY: all test
all: $(TARGETS)
test: $(TESTS)


### defined variables
CC = gcc
CFLAGS = -Wall -O2


### Case of using local cfitsio library 
#FITSIO_DIR   =  ${HOME}/work/cfitsio
#FITS_INCLUDE = -I${FITSIO_DIR}/include
#FITS_LIB     = -L${FITSIO_DIR}/lib -lcfitsio

### Case of using HEASoft cfitsio library
FITS_INCLUDE = -I${HEADAS}/include
FITS_LIB     = -L${HEADAS}/lib -lcfitsio



INCLUDE = ${FITS_INCLUDE}
#LIBDIR =  #-L/usr/local/lib
LIBS   =  #-framework Cocoa -framework OpenGL -lz -ljpeg -lpng


# suffix-rule 
.SUFFIXES: .c .o
.c.o:
	$(CC) $(CFLAGS) $(INCLUDE) -c $<

### each program 
dumpbindat: dumpbindat.o capreolib.o
	$(CC) $(CCFLAGS) -o $@ $^ $(LIBS)

evtbin2fits: evtbin2fits.o capreolib.o
	$(CC) $(CCFLAGS) -o $@ $^ $(FITS_LIB)



### clean 
.PHONY: clean
clean:
	$(RM) $(TARGETS) *.o depend.inc


# ヘッダファイルの依存関係
#.PHONY: depend
#depend: $(OBJS:.o=.c)
#	-@ $(RM) depend.inc
#	-@ for i in $^; do cpp -MM $$i | sed "s/\ [_a-zA-Z0-9][_a-zA-Z0-9]*\.c//g" >> depend.inc; done
#-include depend.inc
