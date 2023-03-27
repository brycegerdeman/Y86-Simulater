CC = g++
CFLAGS = -c -Wall -g -std=c++11 -I ../include
OBJ = lab5.o Tools.o Memory.o ConditionCodes.o RegisterFile.o Loader.o 

.C.o:
	$(CC) $(CFLAGS) $< -o $@

lab5: $(OBJ)

lab5.o: Memory.h RegisterFile.h ConditionCodes.h Loader.h

Loader.o: Loader.h Memory.h

Tools.o: Tools.h

Memory.o: Memory.h Tools.h

ConditionCodes.o: ConditionCodes.h Tools.h

RegisterFile.o: RegisterFile.h Tools.h

run:
	make lab5
	./run.sh	

clean:
	rm $(OBJ) lab5
	rm -f *.o
