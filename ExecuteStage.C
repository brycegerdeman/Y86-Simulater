#include <string>
#include <cstdint>
#include "RegisterFile.h"
#include "PipeRegField.h"
#include "PipeReg.h"
#include "M.h"
#include "E.h"
#include "Stage.h"
#include "ExecuteStage.h"
#include "Status.h"
#include "Debug.h"

/*
 * doClockLow
 */
bool ExecuteStage::doClockLow(PipeReg ** pregs, Stage ** stages) {
	E * ereg = (E *) pregs[EREG];	
	M * mreg = (M *) pregs[MREG];	
	uint64_t stat = SAOK, icode = 0, Cnd = 0, valA = 0, valE = 0, 
		dstE = 0, dstM = 0;

	stat = ereg->getstat()->getOutput();
	icode = ereg->geticode()->getOutput();
	valA = ereg->getvalA()->getOutput();
	dstM = ereg->getdstM()->getOutput();
	dstE = ereg->getdstE()->getOutput();
		
	setMInput(mreg, stat, icode, Cnd, valE, valA, dstE, dstM);
	return false;
}


/* 
 * doClockHigh
 */
void ExecuteStage::doClockHigh(PipeReg ** pregs) {
	M * mreg = (M *) pregs[MREG];	
	
	mreg->getstat()->normal();	
	mreg->geticode()->normal();	
	mreg->getCnd()->normal();
	mreg->getvalE()->normal();
	mreg->getvalA()->normal();
	mreg->getdstE()->normal();
	mreg->getdstM()->normal();
}

/* 
 * setMInput
 */
void ExecuteStage::setMInput(M * mreg, uint64_t stat, uint64_t icode, uint64_t Cnd, 
	uint64_t valE, uint64_t valA, uint64_t dstE, uint64_t dstM) {
	mreg->getstat()->setInput(stat);	
	mreg->geticode()->setInput(icode);	
	mreg->getCnd()->setInput(Cnd);	
	mreg->getvalE()->setInput(valE);	
	mreg->getvalA()->setInput(valA);	
	mreg->getdstE()->setInput(dstE);	
	mreg->getdstM()->setInput(dstM);	
}

