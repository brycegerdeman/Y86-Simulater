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
#include "MemoryStage.h"
#include "Status.h"
#include "Simulate.h"
#include "Debug.h"



bool DecodeStage::doClockLow(PipeReg ** pregs, Stage ** stages) {
	D * dreg = (D *) pregs[DREG];
	E * ereg = (E *) pregs[EREG];
	M * mreg = (M *) pregs[MREG];
	W * wreg = (W *) pregs[WREG];

	uint64_t stat = dreg->getstat()->getOutput(),
 		icode = dreg->geticode()->getOutput(),
		ifun = dreg->getifun()->getOutput(),
		valC = dreg->getvalC()->getOutput(),
		rA = dreg->getrA()->getOutput(),
		rB = dreg->getrB()->getOutput(),
		valA = 0, 
		valB = 0, 	
		dstE = getdstE(icode, rB), 
		dstM = getdstM(icode, rA); 


	d_srcA = getsrcA(icode, rA); 
	d_srcB = getsrcB(icode, rB);
	
	DecodeStage * dstage = (DecodeStage *) stages[DSTAGE];
	ExecuteStage * estage = (ExecuteStage *) stages[ESTAGE];
	calculateControlSignals(ereg->geticode()->getOutput(), 
		ereg->getdstM()->getOutput(), dstage->getd_srcA(), dstage->getd_srcB(), estage->gete_Cnd());

	RegisterFile * reg = reg->getInstance();
	bool error = false;
	uint64_t rvalA = reg->readRegister(d_srcA, error);
	uint64_t rvalB = reg->readRegister(d_srcB, error);
	
	valA = fwdsrcA(mreg, wreg, dreg, stages, d_srcA, rvalA);
	valB = fwdsrcB(mreg, wreg, stages, d_srcB, rvalB);

	setEInput(ereg, stat, icode, ifun, valC, valA, valB, dstE, dstM, d_srcA, d_srcB);
	return false;
}


void DecodeStage::doClockHigh(PipeReg ** pregs) {
	E * ereg = (E *) pregs[EREG];
	
	if (E_bubble == false) {
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
	} else {
		ereg->getstat()->bubble(SAOK);
		ereg->geticode()->bubble(INOP);
		ereg->getifun()->bubble();
		ereg->getvalC()->bubble();
		ereg->getvalA()->bubble();
		ereg->getvalB()->bubble();
		ereg->getdstE()->bubble(RNONE);
		ereg->getdstM()->bubble(RNONE);
		ereg->getsrcA()->bubble(RNONE);
		ereg->getsrcB()->bubble(RNONE);
	}
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

uint64_t DecodeStage::fwdsrcA(M * mreg, W * wreg, D * dreg, Stage ** stages, uint64_t srcA, uint64_t rvalA) {
	uint64_t D_icode = dreg->geticode()->getOutput();
	uint64_t D_valP = dreg->getvalP()->getOutput();

	ExecuteStage * estage = (ExecuteStage *) stages[ESTAGE];
	uint64_t e_dstE = estage->gete_dstE();
	uint64_t e_valE = estage->gete_valE();

	MemoryStage * mstage = (MemoryStage *) stages[MSTAGE];
	uint64_t M_dstM = mreg->getdstM()->getOutput();
	uint64_t m_valM = mstage->getm_valM(); 

	uint64_t M_dstE = mreg->getdstE()->getOutput();
	uint64_t M_valE = mreg->getvalE()->getOutput();

	uint64_t W_dstM = wreg->getdstM()->getOutput();
	uint64_t W_valM = wreg->getvalM()->getOutput();

	uint64_t W_dstE = wreg->getdstE()->getOutput();
	uint64_t W_valE = wreg->getvalE()->getOutput();


	if (D_icode == ICALL || D_icode == IJXX) return D_valP;	
	if (srcA == RNONE) return 0;
	if (srcA == e_dstE) return e_valE;
	if (srcA == M_dstM) return m_valM;
	if (srcA == M_dstE) return M_valE;
	if (srcA == W_dstM) return W_valM;
	if (srcA == W_dstE) return W_valE;
	return rvalA;
}

uint64_t DecodeStage::fwdsrcB(M * mreg, W * wreg, Stage ** stages, uint64_t srcB, uint64_t rvalB) {
	ExecuteStage * estage = (ExecuteStage *) stages[ESTAGE];
	uint64_t e_dstE = estage->gete_dstE();
	uint64_t e_valE = estage->gete_valE();

	MemoryStage * mstage = (MemoryStage *) stages[MSTAGE];
	uint64_t M_dstM = mreg->getdstM()->getOutput();
	uint64_t m_valM = mstage->getm_valM(); 

	uint64_t M_dstE = mreg->getdstE()->getOutput();
	uint64_t M_valE = mreg->getvalE()->getOutput();

	uint64_t W_dstM = wreg->getdstM()->getOutput();
	uint64_t W_valM = wreg->getvalM()->getOutput();

	uint64_t W_dstE = wreg->getdstE()->getOutput();
	uint64_t W_valE = wreg->getvalE()->getOutput();

	if (srcB == RNONE) return 0;
	if (srcB == e_dstE) return e_valE;
	if (srcB == M_dstM) return m_valM;
	if (srcB == M_dstE) return M_valE;
	if (srcB == W_dstM) return W_valM;
	if (srcB == W_dstE) return W_valE;
	return rvalB;
}

/*
 * getd_srcA
 */
uint64_t DecodeStage::getd_srcA() {
	return d_srcA;
}

/*
 * getd_srcB
 */
uint64_t DecodeStage::getd_srcB() {
	return d_srcB;
}

void DecodeStage::calculateControlSignals(uint64_t E_icode, uint64_t E_dstM, uint64_t d_srcA, uint64_t d_srcB, uint64_t e_Cnd) {
	E_bubble = (E_icode == IJXX && !e_Cnd) || 
			(( E_icode == IMRMOVQ || E_icode == IPOPQ) && 
			(E_dstM == d_srcA || E_dstM == d_srcB));
}