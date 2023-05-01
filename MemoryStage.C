#include <string>
#include <cstdint>
#include "RegisterFile.h"
#include "PipeRegField.h"
#include "PipeReg.h"
#include "M.h"
#include "W.h"
#include "Stage.h"
#include "MemoryStage.h"
#include "Memory.h"
#include "Instructions.h"
#include "Status.h"
#include "Debug.h"


/*
 * doClockLow
 */
bool MemoryStage::doClockLow(PipeReg ** pregs, Stage ** stages) {
	M * mreg = (M *) pregs[MREG];
	W * wreg = (W *) pregs[WREG];
	Memory * mem = mem->getInstance();
	uint64_t icode = 0, valE = 0, valA = 0, dstE = 0, dstM = 0, M_stat = 0;
	bool mem_error = false;

	M_stat = mreg->getstat()->getOutput(); 
	icode = mreg->geticode()->getOutput(); 
	valE = mreg->getvalE()->getOutput(); 
	dstE = mreg->getdstE()->getOutput(); 
	dstM = mreg->getdstM()->getOutput(); 
	valA = mreg->getvalA()->getOutput(); 

	valM = 0;
	if (mem_read(mreg)) valM = mem->getLong(addr(mreg), mem_error);
	if (mem_write(mreg)) mem->putLong(valA ,addr(mreg), mem_error);

	if (mem_error) m_stat = SADR;
	else m_stat = M_stat;

	setWInput(wreg, m_stat, icode, valE, valM, dstE, dstM);
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
	uint64_t valM, uint64_t dstE, uint64_t dstM) {
	wreg->getstat()->setInput(stat);
	wreg->geticode()->setInput(icode);
	wreg->getvalE()->setInput(valE);
	wreg->getvalM()->setInput(valM);
	wreg->getdstE()->setInput(dstE);
	wreg->getdstM()->setInput(dstM);
}

/* 
 * addr
 */
uint64_t MemoryStage::addr(M * mreg) {
	uint64_t M_icode = mreg->geticode()->getOutput();
	uint64_t M_valA = mreg->getvalA()->getOutput();
	uint64_t M_valE = mreg->getvalE()->getOutput();

	if (M_icode == IRMMOVQ || M_icode == IPUSHQ || M_icode == ICALL || M_icode == IMRMOVQ) return M_valE;
	if (M_icode == IPOPQ || M_icode == IRET) return M_valA;
	return 0;
}

bool MemoryStage::mem_read(M * mreg) {
	uint64_t M_icode = mreg->geticode()->getOutput();
	return (M_icode == IMRMOVQ || M_icode == IPOPQ || M_icode == IRET);
}

bool MemoryStage::mem_write(M * mreg) {
	uint64_t M_icode = mreg->geticode()->getOutput();
	return (M_icode == IRMMOVQ || M_icode == IPUSHQ || M_icode == ICALL);
}

/*
 * getm_valM
 */
uint64_t MemoryStage::getm_valM(){
	return valM;
}

/*
 * getm_stat
 */
uint64_t MemoryStage::getm_stat(){
	return m_stat;
}