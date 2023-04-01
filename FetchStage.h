//class to perform the combinational logic of
//the Fetch stage

#define IRRMOVQ  0x2 
#define IIRMOVQ 0x3
#define IRMMOVQ 0x4
#define IMRMOVQ 0x5	
#define IOPQ 0x6
#define IJXX 0x7
#define ICALL 0x8
#define IRET 0x9 
#define IPUSHQ 0xA
#define IPOPQ 0xB

class FetchStage: public Stage {
   private:
        void setDInput(D * dreg, uint64_t stat, uint64_t icode, uint64_t ifun, 
                     uint64_t rA, uint64_t rB,
                     uint64_t valC, uint64_t valP);
   public:
        bool doClockLow(PipeReg ** pregs, Stage ** stages);
        void doClockHigh(PipeReg ** pregs);

	uint64_t PredictPC(uint64_t valP, uint64_t icode, uint64_t valC); 
	uint64_t PCincrement(uint64_t f_pc, bool needRegIds, bool needValC); 
	uint64_t selectPC(F * f, M * m, W * w);
	bool needRegIds(uint64_t f_icode);
	bool needValC(uint64_t f_icode);
	uint64_t predictPC(uint64_t f_icode, uint64_t f_valC, uint64_t f_valP); 
};
