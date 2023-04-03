//class to perform the combinational logic of
//the Fetch stage
class FetchStage: public Stage {
   private:
        void setDInput(D * dreg, uint64_t stat, uint64_t icode, uint64_t ifun, 
                     uint64_t rA, uint64_t rB,
                     uint64_t valC, uint64_t valP);
   public:
        bool doClockLow(PipeReg ** pregs, Stage ** stages);
        void doClockHigh(PipeReg ** pregs);

	uint64_t predictPC(uint64_t valP, uint64_t icode, uint64_t valC);
	uint64_t PCincrement(uint64_t f_pc, bool needRegIds, bool needValC); 
	uint64_t selectPC(F * f, M * m, W * w);

	bool needRegIds(uint64_t f_icode);
	void getRegIds(uint64_t word, uint64_t &rA, uint64_t &rB); 

	bool needValC(uint64_t f_icode);
	uint64_t buildValC(Memory * mem, uint64_t byte, bool &imem_error); 
};
