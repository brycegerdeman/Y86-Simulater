CC = g++
CFLAGS = -c -Wall -g -std=c++11 -I ../include
INC = ../include
OBJ = BubbleSort.o QuickSort.o MergeSort.o Sort.o List.o

.C.o:
	$(CC) $(CFLAGS) $< -o $@

sortProg: $(OBJ)

sortProg.o: $(INC)/BubbleSort.h $(INC)/QuickSort.h $(INC)/MergeSort.h $(INC)/Sort.h\
$(INC)/List.h

Sort.o: $(INC)/Sort.h

BubbleSort.o: $(INC)/Sort.h $(INC)/BubbleSort.h 

QuickSort.o: $(INC)/Sort.h $(INC)/QuickSort.h

MergeSort.o: $(INC)/Sort.h $(INC)/MergeSort.h

List.o: $(INC)/Sort.h $(INC)/List.h

install:
	make sortProg
	mv sortProg ../bin

clean:
	rm -f *.o
