CC=gcc
HFILES= timer.h

CFILES=bitonic_parallel.c timer.c
OFILES=$(CFILES:.c=.o)

CFILES1=mergesort.c timer.c
OFILES1=$(CFILES1:.c=.o)

CFILES2=merge_parallel.c timer.c
OFILES2=$(CFILES2:.c=.o)


all:mergesort merge_parallel bitonic_parallel



bitonic_parallel:	$(OFILES)
	$(CC) -o bitonic_parallel $(OFILES) -lpthread
  
mergesort:	$(OFILES1)
	$(CC) -o mergesort $(OFILES1) 
  
merge_parallel:	$(OFILES2)
	$(CC) -o merge_parallel $(OFILES2) -lpthread

  
clean::
	/bin/rm -f $(OFILES)
	/bin/rm -f $(OFILES1)
	/bin/rm -f $(OFILES2)
