class FetchStage: public Stage {
   private:
     void setDInput(D * dreg, uint64_t stat, uint64_t icode, uint64_t ifun, 
                    uint64_t rA, uint64_t rB,
                    uint64_t valC, uint64_t valP);
     bool F_stall;
     bool D_stall;
     bool D_bubble;
     
   public:
     bool doClockLow(PipeReg ** pregs, Stage ** stages);
     void doClockHigh(PipeReg ** pregs);

	uint64_t predictPC(uint64_t valP, uint64_t icode, uint64_t valC);
	uint64_t PCincrement(uint64_t f_pc, bool needRegIds, bool needValC); 
	uint64_t selectPC(F * f, M * m, W * w);

	bool needRegIds(uint64_t f_icode);
	void getRegIds(uint64_t word, uint64_t &rA, uint64_t &rB); 

	bool needValC(uint64_t f_icode);
	uint64_t buildValC(uint8_t bytes[8]);
  bool instrValid(uint64_t f_icode);
  uint64_t fstat(bool mem_error, uint64_t f_icode, bool insrt_valid);
  uint64_t ficode(bool mem_error, uint64_t mem_icode);   
  uint64_t fifun(bool mem_error, uint64_t mem_ifun);

  bool getD_stall(uint64_t E_icode, uint64_t E_dstM, uint64_t d_srcA, uint64_t d_srcB);
  bool getF_stall(uint64_t E_icode, uint64_t E_dstM, uint64_t d_srcA, 
	    uint64_t d_srcB, uint64_t D_icode, uint64_t M_icode);
  bool getD_bubble(uint64_t E_icode, uint64_t e_Cnd, 
      uint64_t E_dstM, uint64_t d_srcA, uint64_t d_srcB, 
      uint64_t D_icode, uint64_t M_icode);
  void calculateControlSignals(uint64_t E_icode, uint64_t E_dstM,
      uint64_t d_srcA, uint64_t d_srcB, uint64_t e_Cnd, uint64_t D_icode, 
      uint64_t M_icode);
};
