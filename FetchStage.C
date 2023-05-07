#include <string>
#include <cstdint>
#include "RegisterFile.h"
#include "PipeRegField.h"
#include "PipeReg.h"
#include "F.h"
#include "D.h"
#include "E.h"
#include "M.h"
#include "W.h"
#include "Stage.h"
#include "Instructions.h"
#include "Tools.h"
#include "FetchStage.h"
#include "ExecuteStage.h"
#include "DecodeStage.h"
#include "Status.h"
#include "Debug.h"
#include "Memory.h"


/*
 * doClockLow:
 * Performs the Fetch stage combinational logic that is performed when
 * the clock edge is low.
 *
 * @param: pregs - array of the pipeline register sets (F, D, E, M, W instances)
 * @param: stages - array of stages (FetchStage, DecodeStage, ExecuteStage,
 *         MemoryStage, WritebackStage instances)
 */
bool FetchStage::doClockLow(PipeReg ** pregs, Stage ** stages) {
	F * freg = (F *) pregs[FREG];
	D * dreg = (D *) pregs[DREG];
	E * ereg = (E *) pregs[EREG];
	M * mreg = (M *) pregs[MREG];
	W * wreg = (W *) pregs[WREG];

	uint64_t f_pc = 0, icode = 0, ifun = 0, valC = 0, valP = 0;
	uint64_t rA = RNONE, rB = RNONE, stat = SAOK;
	bool need_regId = false, need_valC = false;

	Memory * mem = mem->getInstance();
	bool mem_error = false;

	f_pc = selectPC(freg, mreg, wreg); 
	uint64_t byte0 = mem->getByte(f_pc, mem_error);

	freg->getpredPC()->setInput(predictPC(valP, icode, valC));
	icode = ficode(mem_error, Tools::getBits(byte0, 4, 7));
	ifun = fifun(mem_error, Tools::getBits(byte0, 0, 3));

	stat = fstat(mem_error, icode, instrValid(icode));
	need_regId = needRegIds(icode);
	uint64_t byte1 = mem->getByte(f_pc + 1, mem_error);
	if (need_regId) getRegIds(byte1, rA, rB);

	need_valC = needValC(icode);
	if (need_valC) {
		uint8_t bytes[LONGSIZE];
		uint8_t offset = 1;
		if (need_regId) offset = 2;

		for (int i = 0; i < LONGSIZE; i++) {
			bytes[i] = mem->getByte(f_pc + offset + i, mem_error);
		}	
		valC = buildValC(bytes);
	}


	DecodeStage * dstage = (DecodeStage *) stages[DSTAGE];
	ExecuteStage * estage = (ExecuteStage *) stages[ESTAGE];
	calculateControlSignals(ereg->geticode()->getOutput(), 
		ereg->getdstM()->getOutput(), dstage->getd_srcA(), 
		dstage->getd_srcB(), estage->gete_Cnd(), 
		mreg->geticode()->getOutput(), dreg->geticode()->getOutput());

	valP = PCincrement(f_pc, need_regId, need_valC);
	freg->getpredPC()->setInput(predictPC(valP, icode, valC));

	setDInput(dreg, stat, icode, ifun, rA, rB, valC, valP);
	return false;
}

/* doClockHigh
 * applies the appropriate control signal to the F
 *
 *
 * @param: pregs - array of the pipeline register (F, D, E, M, W instances)
 */
void FetchStage::doClockHigh(PipeReg ** pregs) {
	F * freg = (F *) pregs[FREG];
	D * dreg = (D *) pregs[DREG];

	if (!F_stall) {
		freg->getpredPC()->normal();
	}

	if (!D_stall && !D_bubble) {
		dreg->getstat()->normal();
		dreg->geticode()->normal();
		dreg->getifun()->normal();
		dreg->getrA()->normal();
		dreg->getrB()->normal();
		dreg->getvalC()->normal();
		dreg->getvalP()->normal();
	}

	if (D_bubble) {
		dreg->getstat()->bubble(SAOK);
		dreg->geticode()->bubble(INOP);
		dreg->getifun()->bubble();
		dreg->getrA()->bubble(RNONE);
		dreg->getrB()->bubble(RNONE);
		dreg->getvalC()->bubble();
		dreg->getvalP()->bubble();
	}
}

/* setDInput
 * provides the input to potentially be stored in the D register
 * during doClockHigh
 *
 * @param: dreg - pointer to the D register instance
 * @param: stat - value to be stored in the stat pipeline register within D
 * @param: icode - value to be stored in the icode pipeline register within D
 * @param: ifun - value to be stored in the ifun pipeline register within D
 * @param: rA - value to be stored in the rA pipeline register within D
 * @param: rB - value to be stored in the rB pipeline register within D
 * @param: valC - value to be stored in the valC pipeline register within D
 * @param: valP - value to be stored in the valP pipeline register within D
*/
void FetchStage::setDInput(D * dreg, uint64_t stat, uint64_t icode, 
                           uint64_t ifun, uint64_t rA, uint64_t rB,
                           uint64_t valC, uint64_t valP)
{
	dreg->getstat()->setInput(stat);
	dreg->geticode()->setInput(icode);
	dreg->getifun()->setInput(ifun);
	dreg->getrA()->setInput(rA);
	dreg->getrB()->setInput(rB);
	dreg->getvalC()->setInput(valC);
	dreg->getvalP()->setInput(valP);
}
    
/*
 * PredictPC
 */ 
uint64_t FetchStage::predictPC(uint64_t valP, uint64_t icode, uint64_t valC) {
	if (icode == IJXX || icode == ICALL) return valC;	
	return valP;			
}

/*
 * PCincrement
 */ 
uint64_t FetchStage::PCincrement(uint64_t f_pc, bool needRegIds, bool needValC) {
	return f_pc + 1 + (needRegIds ? 1 : 0) + (needValC ? 8 : 0);
}

/*
 * selectPC
 */ 
uint64_t FetchStage::selectPC(F * f, M * m, W * w) {
	uint64_t M_valA = m->getvalA()->getOutput();
	uint64_t M_Cnd = m->getCnd()->getOutput();
	uint64_t M_icode = m->geticode()->getOutput();

	uint64_t W_icode = w->geticode()->getOutput();
	uint64_t W_valM = w->getvalM()->getOutput();

	uint64_t F_predPC = f->getpredPC()->getOutput();

	if (M_icode == IJXX && !M_Cnd) return M_valA; 
	if (W_icode == IRET) return W_valM;	
	return F_predPC;	
}

/*
 * needRegIds
 */
bool FetchStage::needRegIds(uint64_t f_icode) {
	return (f_icode == IRRMOVQ) || (f_icode == IOPQ) ||
	       (f_icode == IPUSHQ) || (f_icode == IPOPQ) ||
	       (f_icode == IIRMOVQ) || (f_icode == IRMMOVQ) ||
	       (f_icode == IMRMOVQ);
}

/*
 * getRegIds
 */
void FetchStage::getRegIds(uint64_t byte1, uint64_t &rA, uint64_t &rB) {
	rA = Tools::getBits(byte1, 4, 7);
	rB = Tools::getBits(byte1, 0, 3);
}

/*
 * needValC
 */
bool FetchStage::needValC(uint64_t f_icode) {
	return (f_icode == IIRMOVQ) || (f_icode == IRMMOVQ) ||
	       (f_icode == IMRMOVQ) || (f_icode == IJXX) ||
	       (f_icode == ICALL);
}

/*
 * buildValC
 */
uint64_t FetchStage::buildValC(uint8_t bytes[LONGSIZE]) {
	return Tools::buildLong(bytes);
}

bool FetchStage::instrValid(uint64_t f_icode) {
	return (f_icode == INOP || f_icode == IHALT || f_icode == IRRMOVQ || f_icode == IIRMOVQ 
			|| f_icode == IRMMOVQ || f_icode == IMRMOVQ || f_icode == IOPQ || f_icode == IJXX 
			|| f_icode == ICALL || f_icode == IRET || f_icode == IPUSHQ || f_icode == IPOPQ);
}

uint64_t FetchStage::fstat(bool mem_error, uint64_t f_icode, bool insrt_valid) {
	if (mem_error) return SADR;
	if (!(insrt_valid)) return SINS;
	if (f_icode == IHALT) return SHLT;
	return SAOK;
}

uint64_t FetchStage::ficode(bool mem_error, uint64_t mem_icode) {
	if(mem_error) return INOP;
	return mem_icode;
}

uint64_t FetchStage::fifun(bool mem_error, uint64_t mem_ifun) {
	if (mem_error) return FNONE;
	return mem_ifun;
}

bool FetchStage::getF_stall(uint64_t E_icode, uint64_t E_dstM, uint64_t d_srcA, 
	uint64_t d_srcB, uint64_t D_icode, uint64_t M_icode) {
	return ((E_icode == IMRMOVQ || E_icode == IPOPQ) && 
		(E_dstM == d_srcA || E_dstM == d_srcB)) ||
		(IRET == D_icode || IRET == E_icode || IRET == M_icode);
}

bool FetchStage::getD_stall(uint64_t E_icode, uint64_t E_dstM, uint64_t d_srcA, uint64_t d_srcB) {
	return (E_icode == IMRMOVQ || E_icode == IPOPQ) && (E_dstM == d_srcA || E_dstM == d_srcB);
}

void FetchStage::calculateControlSignals(uint64_t E_icode, uint64_t E_dstM,
		uint64_t d_srcA, uint64_t d_srcB, uint64_t e_Cnd, uint64_t D_icode, 
		uint64_t M_icode) {
	F_stall = getF_stall(E_icode, E_dstM, d_srcA, d_srcB, D_icode, M_icode);
	D_stall = getD_stall(E_icode, E_dstM, d_srcA, d_srcB);
	D_bubble = getD_bubble(E_icode, e_Cnd, E_dstM, d_srcA, d_srcB, D_icode, M_icode);
}

bool FetchStage::getD_bubble(uint64_t E_icode, uint64_t e_Cnd, 
	uint64_t E_dstM, uint64_t d_srcA, uint64_t d_srcB, 
	uint64_t D_icode, uint64_t M_icode) {
		return (E_icode == IJXX && !e_Cnd) ||
		(!( (E_icode == IMRMOVQ || E_icode == IPOPQ) && (E_dstM == d_srcA || E_dstM == d_srcB) ) &&
		(IRET == D_icode || IRET == E_icode || IRET == M_icode));
}