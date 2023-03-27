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
	uint64_t stat = dreg->getstat(), icode = dreg->geticode(), ifun = dreg->getifun(), valC = dreg->getvalC(), valA = 0, valB = 0;
   	uint64_t dstE = RNONE, dstM = RNONE, srcA = RNONE, srcB = RNONE;

	setDInput
}


void DecodeStage::doClockHigh(PipeReg ** pregs) {
	return;
}

void DecodeStage::setEInput(E * dreg, uint64_t stat, uint64_t icode, 
                           uint64_t ifun, uint64_t rA, uint64_t rB,
				   uint64_t valC, uint64_t valP) {
	ereg->getstat()->setInput(stat);
   	ereg->geticode()->setInput(icode);
   	ereg->getifun()->setInput(ifun);
	ereg->
}
