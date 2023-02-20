CC = g++
CFLAGS = -c -Wall -g -std=c++11 -I ../include
OBJ = lab4.o Tools.o Memory.o RegisterFile.o MemoryTester.o RegisterFileTester.o \
ConditionCodes.o ConditionCodesTester.o 

.C.o:
	$(CC) $(CFLAGS) $< -o $@

lab4: $(OBJ)

Memory.o: Memory.h Tools.h

Tools.o: Tools.h

lab4.o:	Memory.h RegisterFile.h MemoryTester.h RegisterFileTester.h ConditionCodes.h ConditionCodesTester.h 

MemoryTester.o: MemoryTest.h Memory.h

ConditionCodes.o: ConditionCodes.h Tools.h

ConditionCodesTester.o: ConditionCodesTester.h ConditionCodes.h

RegisterFile.o: RegisterFile.h Tools.h

RegisterFileTester.o: RegisterFileTester.h RegisterFile.h

run:
	make lab4
	./run.sh	

clean:
	rm $(OBJ) lab4
	rm -f *.o
