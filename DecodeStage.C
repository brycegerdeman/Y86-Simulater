#include <string>
#include <cstdint>
#include "RegisterFile.h"
#include "PipeRegField.h"
#include "PipeReg.h"
#include "F.h"
#include "D.h"
#include "M.h"
#include "W.h"
#include "Stage.h"
#include "DecodeStage.h"
#include "Status.h"
#include "Debug.h"



bool DecodeStage::doClockLow(PipeReg ** pregs, Stage ** stages) {
	D * dreg = (D *) pregs[DREG];
	E * ereg = (E *) pregs[EREG];
	uint64_t stat = dreg->getstat()->getOutput(), icode = dreg->geticode()->getOutput(), ifun = dreg->getifun()->getOutput(), valC = dreg->getvalC()-getOutput(), valA = 0, valB = 0;
   	uint64_t dstE = RNONE, dstM = RNONE, srcA = RNONE, srcB = RNONE;

	setEInput(ereg, stat, icode, ifun, valC, valA, valB, dstE, dstM, srcA, srcB)
}


void DecodeStage::doClockHigh(PipeReg ** pregs) {
	return;
}

void DecodeStage::setEInput(E * dreg, uint64_t stat, uint64_t icode, 
                           uint64_t ifun, uint64_t valC, uint64_t valA,
				   uint64_t valB, uint64_t dstE, uint64_t dstM, uint64_t srcA, uint64_t srcB) {
	ereg->getstat()->setInput(stat);
   	ereg->geticode()->setInput(icode);
   	ereg->getifun()->setInput(ifun);
	ereg->getvalC()->setvalC(valC);
	ereg->getvalA()->setvalA(valA);
	ereg->getvalB()->setvalB(valB);
	ereg->getdstE()->setdstE(dstE);
	ereg->getdstM()->setdstM(dstM);
	ereg->getsrcA()->setsrcA(srcA);
	ereg->getsrcB()->setsrcA(srcB);
}
