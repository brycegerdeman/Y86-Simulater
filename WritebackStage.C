#include <string>
#include <cstdint>
#include "RegisterFile.h"
#include "PipeRegField.h"
#include "PipeReg.h"
#include "Stage.h"
#include "W.h"
#include "WritebackStage.h"
#include "Status.h"
#include "Instructions.h"
#include "Debug.h"


/*
 * doClockLow
 */
bool WritebackStage::doClockLow(PipeReg ** pregs, Stage ** stages) {
	W * wreg = (W *) pregs[WREG];
	uint64_t icode = wreg->geticode()->getOutput();	
	return (icode == IHALT);
}

/* 
 * doClockHigh
 */
void WritebackStage::doClockHigh(PipeReg ** pregs) {
	RegisterFile * regfile = regfile->getInstance();
	W * wreg = (W *) pregs[WREG];

	uint64_t W_valE = wreg->getvalE()->getOutput();
	uint64_t W_dstE = wreg->getdstE()->getOutput();
	bool error = false;
	regfile->writeRegister(W_valE, W_dstE, error);
}