CC=gcc
HFILES= timer.h

CFILES=bintonic.c timer.c
OFILES=$(CFILES:.c=.o)

CFILES1=bintonic_parallel.c timer.c
OFILES1=$(CFILES1:.c=.o)

CFILES2=mergeSort.c timer.c
OFILES2=$(CFILES2:.c=.o)

all:bintonic bintonic_parallel mergeSort timer.h

bintonic:	$(OFILES)
	$(CC) -o bintonic $(OFILES)
  
bintonic_parallel:	$(OFILES1)
	$(CC) -o bintonic_parallel $(OFILES1) -lpthread

mergeSort:	$(OFILES2)
	$(CC) -g -o mergeSort $(OFILES2)

clean::
	/bin/rm -f $(OFILES)
	/bin/rm -f $(OFILES1)
	/bin/rm -f $(OFILES2)
