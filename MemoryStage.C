#include <string>
#include <cstdint>
#include "RegisterFile.h"
#include "PipeRegField.h"
#include "PipeReg.h"
#include "M.h"
#include "W.h"
#include "Stage.h"
#include "MemoryStage.h"
#include "Status.h"
#include "Debug.h"


/*
 * doClockLow
 */
bool MemoryStage::doClockLow(PipeReg ** pregs, Stage ** stages) {
	M * mreg = (M *) pregs[MREG];
	W * wreg = (W *) pregs[WREG];
	uint64_t stat = SAOK, icode = 0, valE = 0, valM = 0, dstE = 0, dstM = 0;

	stat = mreg->getstat(); 
	icode = mreg->geticode(); 
	valE = mreg->getvalE(); 
	valA = mreg->getvalA(); 
	dstE = mreg->getdstE(); 
	dstM = mreg->getdstM(); 

	setWInput(wreg, stat, icode, valE, valA, dstE, dstM);
	return false;
}


/* 
 * doClockHigh 
 */
void MemoryStage::doClockHigh(PipeReg ** pregs) {
	W * wreg = (W *) pregs[WREG];
	wreg->getstat()->normal();
	wreg->geticode()->normal();
	wreg->getvalE()->normal();
	wreg->getvalM()->normal();
	wreg->getdstE()->normal();
	wreg->getdstM()->normal();
}

/* 
 * setWInput
 */
void MemoryStage::setWInput(W * wreg, uint64_t stat, uint64_t icode, uint64_t valE, 
	uint64_t valM, dstE, uint64_t dstM) {
	wreg->getstat()->setInput(stat);
	wreg->geticode()->setInput(icode);
	wreg->getvalE()->setInput(valE);
	wreg->getvalM()->setInput(valM);
	wreg->getdstE()->setInput(dstE);
	wreg->getdstM()->setInput(dstM);
}
