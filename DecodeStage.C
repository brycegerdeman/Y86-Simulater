#include <string>
#include <cstdint>
#include <iostream>
#include "Instructions.h"
#include "RegisterFile.h"
#include "PipeRegField.h"
#include "PipeReg.h"
#include "D.h"
#include "E.h"
#include "W.h"
#include "M.h"
#include "Stage.h"
#include "DecodeStage.h"
#include "ExecuteStage.h"
#include "Status.h"
#include "Simulate.h"
#include "Debug.h"



bool DecodeStage::doClockLow(PipeReg ** pregs, Stage ** stages) {
	D * dreg = (D *) pregs[DREG];
	E * ereg = (E *) pregs[EREG];
	M * mreg = (M *) pregs[EREG];
	W * wreg = (W *) pregs[EREG];

	uint64_t stat = dreg->getstat()->getOutput(),
 		icode = dreg->geticode()->getOutput(),
		ifun = dreg->getifun()->getOutput(),
		valC = dreg->getvalC()->getOutput(),
		rA = dreg->getrA()->getOutput(),
		rB = dreg->getrB()->getOutput(),
		valA = 0, 
		valB = 0, 	
		dstE = getdstE(icode, rB), 
		dstM = getdstM(icode, rA), 
		srcA = getsrcA(icode, rA), 
		srcB = getsrcB(icode, rB);
	std::cout << std::hex << "\n\n\n\n\n\n" << std::endl;
	std::cout << std::hex << rA << std::endl;
	std::cout << std::hex << srcA << std::endl;

	RegisterFile * reg = reg->getInstance();
	bool error = false;
	uint64_t rvalA = reg->readRegister(srcA, error);
	uint64_t rvalB = reg->readRegister(srcB, error);

	std::cout << std::hex << rvalA << std::endl;
	valA = fwdsrcA(mreg, wreg, stages, srcA, rvalA);
	valB = fwdsrcB(mreg, wreg, stages, srcB, rvalB);
	std::cout << std::hex << valA << std::endl;

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

uint64_t DecodeStage::fwdsrcA(M * mreg, W * wreg, Stage ** stages, uint64_t srcA, 
	uint64_t rvalA) {
	ExecuteStage * xstage = (ExecuteStage *) stages[ESTAGE];
	if (srcA == xstage->gete_dstE()) return xstage->gete_valE();
	if (srcA == mreg->getdstE()->getOutput()) return mreg->getvalE()->getOutput();
	if (srcA == wreg->getdstE()->getOutput()) return wreg->getvalE()->getOutput();
	return rvalA;
}

uint64_t DecodeStage::fwdsrcB(M * mreg, W * wreg, Stage ** stages, uint64_t srcB, 
	uint64_t rvalB) {
	ExecuteStage * xstage = (ExecuteStage *) stages[ESTAGE];
	if (srcB == xstage->gete_dstE()) return xstage->gete_valE();
	if (srcB == mreg->getdstE()->getOutput()) return mreg->getvalE()->getOutput();
	if (srcB == wreg->getdstE()->getOutput()) return wreg->getvalE()->getOutput();
	return rvalB;
}


