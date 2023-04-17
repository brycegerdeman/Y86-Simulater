#include <string>
#include <cstdint>
#include "Instructions.h"
#include "RegisterFile.h"
#include "PipeRegField.h"
#include "PipeReg.h"
#include "D.h"
#include "E.h"
#include "Stage.h"
#include "DecodeStage.h"
#include "Status.h"
#include "Debug.h"



bool DecodeStage::doClockLow(PipeReg ** pregs, Stage ** stages) {
	D * dreg = (D *) pregs[DREG];
	E * ereg = (E *) pregs[EREG];
	uint64_t stat = dreg->getstat()->getOutput(),
 		icode = dreg->geticode()->getOutput(),
		ifun = dreg->getifun()->getOutput(),
		valC = dreg->getvalC()->getOutput(),
		rA = dreg->getrA()->getOutput(),
		rB = dreg->getrB()->getOutput(),
		valA = 0, 
		valB = 0, 	
		dstE = RNONE, 
		dstM = RNONE, 
		srcA = RNONE, 
		srcB = RNONE;

	srcA = getsrcA(icode, rA);
	srcB = getsrcB(icode, rB);
	dstE = getdstE(icode, rB);
	dstM = getdstM(icode, rA);
	fwdsrcA(rA, dreg);
	fwdsrcB(rB, dreg);

	setEInput(ereg, stat, icode, ifun, valC, valA, valB, dstE, dstM, srcA, srcB);
	return false;
}


void DecodeStage::doClockHigh(PipeReg ** pregs) {
	E * ereg = (E *) pregs[EREG];
	
	ereg->getstat()->normal();
   	ereg->geticode()->normal();
   	ereg->getifun()->normal();
	ereg->getvalC()->normal();
	ereg->getvalA()->normal();
	ereg->getvalB()->normal();
	ereg->getdstE()->normal();
	ereg->getdstM()->normal();
	ereg->getsrcA()->normal();
	ereg->getsrcB()->normal();
}

void DecodeStage::setEInput(E * ereg, uint64_t stat, uint64_t icode, 
	uint64_t ifun, uint64_t valC, uint64_t valA,
	uint64_t valB, uint64_t dstE, uint64_t dstM, uint64_t srcA, uint64_t srcB) {
	ereg->getstat()->setInput(stat);
   	ereg->geticode()->setInput(icode);
   	ereg->getifun()->setInput(ifun);
	ereg->getvalC()->setInput(valC);
	ereg->getvalA()->setInput(valA);
	ereg->getvalB()->setInput(valB);
	ereg->getdstE()->setInput(dstE);
	ereg->getdstM()->setInput(dstM);
	ereg->getsrcA()->setInput(srcA);
	ereg->getsrcB()->setInput(srcB);
}

uint64_t DecodeStage::getsrcA(uint64_t icode, uint64_t rA){
	if (icode == IRRMOVQ || icode == IRMMOVQ || icode == IOPQ || icode == IPUSHQ) return rA;
	if (icode == IPOPQ || icode == IRET) return RSP;
	return RNONE;
}

uint64_t DecodeStage::getsrcB(uint64_t icode, uint64_t rB){
	if (icode == IOPQ || icode == IRMMOVQ || icode == IMRMOVQ) return rB;
	if (icode == IPUSHQ || icode == IPOPQ || icode == ICALL || icode == IRET) return RSP;
	return RNONE;
}

uint64_t DecodeStage::getdstE(uint64_t icode, uint64_t rB){
	if (icode == IRRMOVQ || icode == IIRMOVQ || icode == IOPQ) return rB;
	if (icode == IPUSHQ || icode == IPOPQ || icode == ICALL || icode == IRET) return RSP;
	return RNONE;
}

uint64_t DecodeStage::getdstM(uint64_t icode, uint64_t rA){
	if (icode == IMRMOVQ || icode == IPOPQ) return rA;
	return RNONE;
}

void DecodeStage::fwdsrcA(uint64_t rA, D * dreg){
	dreg->getrA()->setInput(rA);
}

void DecodeStage::fwdsrcB(uint64_t rB, D * dreg){
	dreg->getrB()->setInput(rB);
}

