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
#include "Instructions.h"
#include "Tools.h"
#include "FetchStage.h"
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
   M * mreg = (M *) pregs[MREG];
   W * wreg = (W *) pregs[WREG];
   D * dreg = (D *) pregs[DREG];
   uint64_t f_pc = selectPC(freg, mreg, wreg), 
      icode = 0, ifun = 0, valC = 0, valP = 0;
   uint64_t rA = RNONE, rB = RNONE, stat = SAOK;

   Memory * mem = mem->getInstance();
   bool memmer = false;

   uint64_t byte = mem->getByte(f_pc, memmer);
   icode = Tools::getBits(byte, 4, 7);
   ifun = Tools::getBits(byte, 0, 3);

   //f_pc = selectPC(freg, mreg, wreg);
   valP = PCincrement(f_pc, needRegIds(icode), needValC(icode));

  
   //The value passed to setInput below will need to be changed
   freg->getpredPC()->setInput(predictPC(valP, icode, valC));

   //provide the input values for the D register
   setDInput(dreg, stat, icode, ifun, rA, rB, valC, valP);
   return false;
}

/* doClockHigh
 * applies the appropriate control signal to the F
 * and D register intances
 *
 * @param: pregs - array of the pipeline register (F, D, E, M, W instances)
 */
void FetchStage::doClockHigh(PipeReg ** pregs) {
   F * freg = (F *) pregs[FREG];
   D * dreg = (D *) pregs[DREG];

   freg->getpredPC()->normal();
   dreg->getstat()->normal();
   dreg->geticode()->normal();
   dreg->getifun()->normal();
   dreg->getrA()->normal();
   dreg->getrB()->normal();
   dreg->getvalC()->normal();
   dreg->getvalP()->normal();
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
	if (needRegIds && needValC) return f_pc + 10;
	if (!needRegIds && needValC) return f_pc + 9;	
	if (needRegIds && !needValC) return f_pc + 2;	
	return f_pc + 1;		
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
 * needValC
 */
bool FetchStage::needValC(uint64_t f_icode) {
	return (f_icode == IIRMOVQ) || (f_icode == IRMMOVQ) ||
	       (f_icode == IMRMOVQ) || (f_icode == IJXX) ||
	       (f_icode == ICALL);
}

