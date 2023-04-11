#include <string>
#include <cstdint>
#include "RegisterFile.h"
#include "PipeRegField.h"
#include "PipeReg.h"
#include "M.h"
#include "E.h"
#include "Tools.h"
#include "Stage.h"
#include "Instructions.h"
#include "ConditionCodes.h"
#include "ExecuteStage.h"
#include "Status.h"
#include "Debug.h"

/*
 * doClockLow
 */
bool ExecuteStage::doClockLow(PipeReg ** pregs, Stage ** stages) {
	E * ereg = (E *) pregs[EREG];	
	M * mreg = (M *) pregs[MREG];	
	uint64_t stat = SAOK, icode = 0, Cnd = 0, valA = 0,
	dstM = 0, valC = 0, valB = 0, ifun = 0;
	uint64_t aluA = 0, aluB = 0, alufun = 0; 

	stat = ereg->getstat()->getOutput();
	icode = ereg->geticode()->getOutput();
	ifun = ereg->getifun()->getOutput();
	valA = ereg->getvalA()->getOutput();
	valB = ereg->getvalB()->getOutput();
	valC = ereg->getvalC()->getOutput();
	dstM = ereg->getdstM()->getOutput();
	dstE = ereg->getdstE()->getOutput();

	setvalE(ereg, mreg, valE);

	aluA = getaluA(icode, valA, valC);
	aluB = getaluB(icode, valB);
	alufun = getalufun(icode, ifun);
	valE = ALU(icode, alufun, aluA, aluB);


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

/* 
 * setvalE
 */
void ExecuteStage::setvalE(E * ereg, M * mreg, uint64_t &valE) {
	valE = ereg->getvalC()->getOutput();
	mreg->getvalE()->setInput(valE);
} 

/*
 * getaluA
 */
uint64_t ExecuteStage::getaluA(uint64_t icode, uint64_t valA, uint64_t valC) {
	if (icode == IRRMOVQ || icode == IOPQ) return valA;
	if (icode == IIRMOVQ || icode == IRMMOVQ || icode == IMRMOVQ) return valC;
	if (icode == ICALL || icode == IPUSHQ) return -8;
	if (icode == IRET || icode == IPOPQ) return 8;
	return 0;
}

/*
 * getaluB
 */
uint64_t ExecuteStage::getaluB(uint64_t icode, uint64_t valB) {
	if (icode == IRMMOVQ || icode == IMRMOVQ || icode == IOPQ ||
		icode == ICALL || icode == IPUSHQ || icode == IRET || 
		icode == IPOPQ) return valB;
	if (icode == IRRMOVQ || icode == IIRMOVQ) return 0;
	return 0;
}

/*
 * getalufun
 */
uint64_t ExecuteStage::getalufun(uint64_t icode, uint64_t ifun) {
	if (icode == IOPQ) return ifun; 
	return ADDQ;
}

/*
 * setcc
 */
bool ExecuteStage::setcc(uint64_t icode) { 
	return (icode == IOPQ);
}

/*
 * getdstE
 */
uint64_t ExecuteStage::getdstE(uint64_t icode, uint64_t Cnd, uint64_t dstE) { 
	if (icode == IRRMOVQ && !Cnd) return RNONE;
	return dstE;
}

/*
 * ALU
 */
uint64_t ExecuteStage::ALU(uint64_t icode, uint64_t ifun, uint64_t aluA, uint64_t aluB) {
	// ADD
	if (ifun == ADDQ) {
		uint64_t out = aluA + aluB; 
		if (setcc(icode)) {
			CC(ZF, out == 0);
			CC(SF, out < 0);
			CC(OF, (aluA > 0 && aluB > 0 && out < 0) || 
				   (aluA < 0 && aluB < 0 && out > 0));
		}
		return out;	
	}

	// SUB
	if (ifun == SUBQ) {
		uint64_t out = aluB - aluA; 
		if (setcc(icode)) {
			CC(ZF, out == 0);
			CC(SF, out < 0);
			CC(OF, (Tools::sign(out) == Tools::sign(aluA)) && 
				   (Tools::sign(aluA) != Tools::sign(aluB)));
		}
		return out;
	}

	// AND
	if (ifun == ANDQ) {
		uint64_t out = aluA & aluB;
		if (setcc(icode)) {
			CC(ZF, out == 0);
			CC(SF, out < 0);
			CC(OF, false);
		}
		return out;
	}

	// XOR
	if (ifun == XORQ) {
		uint64_t out = aluA ^ aluB;
		if (setcc(icode)) {
			CC(ZF, out == 0);
			CC(SF, out < 0);
			CC(OF, false);
		}
		return out;
	}

	return -1;
}

/*
 * CC
 */
void ExecuteStage::CC(uint64_t ccNum, bool value) {
	ConditionCodes * codes = codes->getInstance();
	bool error = false;
	codes->setConditionCode(ccNum, value, error);
}

uint64_t ExecuteStage::gete_dstE(){
	return dstE;
}

uint64_t ExecuteStage::gete_valE(){
	return valE;
}