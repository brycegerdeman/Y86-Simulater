CC = g++
CFLAGS = -c -Wall -g -std=c++11 -I ../include
OBJ = yess.o Tools.o Memory.o ConditionCodes.o RegisterFile.o Loader.o Simulate.o FetchStage.o DecodeStage.o ExecuteStage.o MemoryStage.o WritebackStage.o F.o D.o E.o M.o W.o PipeReg.o PipeRegField.o

.C.o:
	$(CC) $(CFLAGS) $< -o $@

yess: $(OBJ)

yess.o: Debug.h Memory.h RegisterFile.h ConditionCodes.h Loader.h PipeReg.h Stage.h Simulate.h

Loader.o: Loader.h Memory.h

Tools.o: Tools.h

Memory.o: Memory.h Tools.h

ConditionCodes.o: ConditionCodes.h Tools.h

RegisterFile.o: RegisterFile.h Tools.h

FetchStage.o: FetchStage.h Stage.h

DecodeStage.o: DecodeStage.h Stage.h

ExecuteStage.o: ExecuteStage.h Stage.h

MemoryStage.o: MemoryStage.h Stage.h

WritebackStage.o: WritebackStage.h Stage.h

Simulate.o: Simulate.h FetchStage.h DecodeStage.h ExecuteStage.h MemoryStage.h WritebackStage.h 

PipeReg.o: PipeReg.h

PipeRegField.o: PipeRegField.h

F.o: F.h PipeReg.h

D.o: D.h PipeReg.h

E.o: E.h PipeReg.h

M.o: M.h PipeReg.h

W.o: W.h PipeReg.h

run:
	make yess

clean:
	rm $(OBJ) yess
	rm -f *.o
