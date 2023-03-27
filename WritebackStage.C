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
#include "WritebackStage.h"
#include "Status.h"
#include "Debug.h"


/*
 * doClockLow
 */
bool WritebackStage::doClockLow(PipeReg ** pregs, Stage ** stages) {
	return true;
}


/* 
 * doClockHigh
 */
void WritebackStage::doClockHigh(PipeReg ** pregs) {
	return;
}


