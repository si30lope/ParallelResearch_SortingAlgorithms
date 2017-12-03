CC=gcc
HFILES= timer.h

CFILES=bintonic.c timer.c
OFILES=$(CFILES:.c=.o)

CFILES1=bintonic_parallel.c timer.c
OFILES1=$(CFILES1:.c=.o)

CFILES2=mergeSort.c timer.c
OFILES2=$(CFILES2:.c=.o)

CFILES4=PSBitonic.c timer.c
OFILES4=$(CFILES4:.c=.o)

CFILES5=PSBitonic_parallel.c timer.c
OFILES5=$(CFILES5:.c=.o)



all:bintonic bintonic_parallel mergeSort mergeSort_parallel PSBitonic PSBitonic_parallel timer.h



bintonic:	$(OFILES)
	$(CC) -o bintonic $(OFILES)
  
bintonic_parallel:	$(OFILES1)
	$(CC) -o bintonic_parallel $(OFILES1) -lpthread

mergeSort:	$(OFILES2)
	$(CC) -g -o mergeSort $(OFILES2)

mergeSort_parallel:	$(OFILES3)
	$(CC) -g -o mergeSort_parallel $(OFILES3) -lpthread

PSBitonic:	$(OFILES4)
	$(CC) -o PSBitonic $(OFILES4) 
  
PSBitonic_parallel:	$(OFILES5)
	$(CC) -o PSBitonic_parallel $(OFILES5) -lpthread
  
  
  
  
clean::
	/bin/rm -f $(OFILES)
	/bin/rm -f $(OFILES1)
	/bin/rm -f $(OFILES2)
	/bin/rm -f $(OFILES3)
	/bin/rm -f $(OFILES4)
	/bin/rm -f $(OFILES5)
