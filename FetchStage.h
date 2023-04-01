//class to perform the combinational logic of
//the Fetch stage
class FetchStage: public Stage {
   private:
      void setDInput(D * dreg, uint64_t stat, uint64_t icode, uint64_t ifun, 
                     uint64_t rA, uint64_t rB,
                     uint64_t valC, uint64_t valP);
   public:
	uint64_t IRRMOVQ = 0x2, IRRMOVQ = 0x3, IRMMOVQ = 0x4, IMRMOVQ = 0x5,	
		 IOPQ = 0x6, IJXX = 0x7, ICALL = 0x8, IRET = 0x9, 
		 IPUSHQ = 0xA, IPOPQ = 0xB; 

        bool doClockLow(PipeReg ** pregs, Stage ** stages);
        void doClockHigh(PipeReg ** pregs);


	void selectPC(F * f, M * m, W * w);
	void needRegIds(uint64_t f_icode);
	void needValC(uint64_t f_icode);
	void predictPC(uint64_t f_icode, uint64_t f_valC, uint64_t f_valP); 
