#include <string>
#include <cstdint>
#include "RegisterFile.h"
#include "PipeRegField.h"
#include "PipeReg.h"
#include "M.h"
#include "E.h"
#include "W.h"
#include "Tools.h"
#include "Stage.h"
#include "Instructions.h"
#include "ConditionCodes.h"
#include "ExecuteStage.h"
#include "MemoryStage.h"
#include "Status.h"
#include "Debug.h"

/*
 * doClockLow
 */
bool ExecuteStage::doClockLow(PipeReg ** pregs, Stage ** stages) {
	E * ereg = (E *) pregs[EREG];	
	M * mreg = (M *) pregs[MREG];	
	W * wreg = (W *) pregs[WREG];	
	uint64_t stat = SAOK, icode = 0, valA = 0,
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

	MemoryStage * mstage = (MemoryStage *) stages[MSTAGE];
	uint64_t W_stat = wreg->getstat()->getOutput();
	uint64_t m_stat = mstage->getm_stat();
	valE = ALU(icode, alufun, aluA, aluB, m_stat, W_stat);
	M_bubble = calculateControlSignals(m_stat, W_stat);

	e_Cnd = cond(icode, ifun);
	dstE = getdstE(icode, e_Cnd, dstE);

	setMInput(mreg, stat, icode, e_Cnd, valE, valA, dstE, dstM);
	return false;
}


/* 
 * doClockHigh
 */
void ExecuteStage::doClockHigh(PipeReg ** pregs) {
	M * mreg = (M *) pregs[MREG];	

	if (M_bubble == false) {
		mreg->getstat()->normal();	
		mreg->geticode()->normal();	
		mreg->getCnd()->normal();
		mreg->getvalE()->normal();
		mreg->getvalA()->normal();
		mreg->getdstE()->normal();
		mreg->getdstM()->normal();
	} else {
		mreg->getstat()->bubble(SAOK);
		mreg->geticode()->bubble(INOP);
		mreg->getCnd()->bubble();
		mreg->getvalE()->bubble();
		mreg->getvalA()->bubble();
		mreg->getdstE()->bubble(RNONE);
		mreg->getdstM()->bubble(RNONE);
	}
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
void ExecuteStage::setvalE(E * ereg, M * mreg, uint64_t valE) {
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
 * getdstE
 */
uint64_t ExecuteStage::getdstE(uint64_t icode, uint64_t Cnd, uint64_t dstE) { 
	if (icode == IRRMOVQ && !Cnd) return RNONE;
	return dstE;
}

/*
 * ALU
 */
uint64_t ExecuteStage::ALU(uint64_t icode, uint64_t ifun, uint64_t aluA, 
	uint64_t aluB, uint64_t m_stat, uint64_t W_stat) {
	uint64_t out = 0;

	// ADD
	if (ifun == ADDQ) {
		out = aluA + aluB; 
		if (setcc(icode, m_stat, W_stat)) {
			CC(ZF, out == 0);
			CC(SF, Tools::sign(out) == 1);
			CC(OF, (Tools::sign(aluA) == 0 && Tools::sign(aluB) == 0 && Tools::sign(out) == 1) || 
				(Tools::sign(aluA) == 1 && Tools::sign(aluB) == 1 && Tools::sign(out) == 0)); 
		}
		return out;
	}


	// SUB
	if (ifun == SUBQ) {
		out = aluB - aluA; 
		if (setcc(icode, m_stat, W_stat)) {
			CC(ZF, out == 0);
			CC(SF, Tools::sign(out) == 1);
			CC(OF, (Tools::sign(out) == Tools::sign(aluA)) && 
				(Tools::sign(aluA) != Tools::sign(aluB)));
		}
		return out;
	}

	// AND
	if (ifun == ANDQ) {
		out = aluA & aluB;
		if (setcc(icode, m_stat, W_stat)) {
			CC(ZF, out == 0);
			CC(SF, Tools::sign(out) == 1);
			CC(OF, false);
		}
		return out;
	}

	// XOR
	if (ifun == XORQ) {
		out = aluA ^ aluB;
		if (setcc(icode, m_stat, W_stat)) {
			CC(ZF, out == 0);
			CC(SF, Tools::sign(out) == 1);
			CC(OF, false);
		}
		return out;
	}

	// ADD
	out = aluA + aluB; 
	return out;
}

/*
 * CC
 */
void ExecuteStage::CC(uint64_t ccNum, bool value) {
	ConditionCodes * codes = codes->getInstance();
	bool error = false;
	codes->setConditionCode(value, ccNum, error);
}

/*
 * setcc
 */
bool ExecuteStage::setcc(uint64_t icode, uint64_t m_stat, uint64_t W_stat) { 
	return (icode == IOPQ) && (m_stat != SADR && m_stat != SINS && m_stat != SHLT)
		&& (W_stat != SADR && W_stat != SINS && W_stat != SHLT);
}

/*
 * gete_dstE
 */
uint64_t ExecuteStage::gete_dstE(){
	return dstE;
}

/*
 * gete_valE
 */
uint64_t ExecuteStage::gete_valE(){
	return valE;
}

/*
 * cond
 */
uint64_t ExecuteStage::cond(uint64_t icode, uint64_t ifun) {
	if (icode != IJXX && icode != ICMOVXX) return 0;

	bool error = false;
	ConditionCodes * codes = codes->getInstance();
	bool zf = codes->getConditionCode(ZF, error);
	bool sf = codes->getConditionCode(SF, error);
	bool of = codes->getConditionCode(OF, error);

	if (ifun == UNCOND) return 1;
	if (ifun == LESSEQ) return (sf ^ of) | zf;
	if (ifun == LESS) return (sf ^ of);
	if (ifun == EQUAL) return zf;
	if (ifun == NOTEQUAL) return !zf;
	if (ifun == GREATER) return !(sf ^ of) & !zf;
	if (ifun == GREATEREQ) return !(sf ^ of);
	return 0;
}

/*
 * calculateControlSignals
 */
bool ExecuteStage::calculateControlSignals(uint64_t m_stat, uint64_t W_stat) {
	return (m_stat == SADR || m_stat == SINS || m_stat == SHLT) 
		|| (W_stat == SADR || W_stat == SINS || W_stat == SHLT);
}

uint64_t ExecuteStage::gete_Cnd() {
	return e_Cnd;
}