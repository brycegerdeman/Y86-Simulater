#include <string>
#include <cstdint>
#include "RegisterFile.h"
#include "PipeRegField.h"
#include "PipeReg.h"
#include "Stage.h"
#include "Status.h"
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
	uint64_t stat = wreg->getstat()->getOutput();
	return (stat != SAOK);
}

/* 
 * doClockHigh
 */
void WritebackStage::doClockHigh(PipeReg ** pregs) {
	RegisterFile * regfile = regfile->getInstance();
	W * wreg = (W *) pregs[WREG];

	uint64_t W_valE = wreg->getvalE()->getOutput();
	uint64_t W_dstE = wreg->getdstE()->getOutput();
	uint64_t W_dstM = wreg->getdstM()->getOutput();
	uint64_t W_valM = wreg->getvalM()->getOutput();
	bool error = false;
	regfile->writeRegister(W_valE, W_dstE, error);
	regfile->writeRegister(W_valM, W_dstM, error);
}
