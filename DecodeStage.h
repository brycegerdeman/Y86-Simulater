class DecodeStage: public Stage {
   private:
      void setEInput( E * ereg, uint64_t stat, uint64_t icode,
            uint64_t ifun, uint64_t valC, uint64_t valA, uint64_t valB,
            uint64_t dstE, uint64_t dstM, uint64_t srcA, uint64_t srcB);
      uint64_t d_srcA, d_srcB, E_bubble;
      
   public:
      bool doClockLow(PipeReg ** pregs, Stage ** stages);
      void doClockHigh(PipeReg ** pregs);
      uint64_t getsrcA(uint64_t icode, uint64_t rA);
      uint64_t getsrcB(uint64_t icode, uint64_t rB);
      uint64_t getdstE(uint64_t icode, uint64_t rB);
      uint64_t getdstM(uint64_t icode, uint64_t rA);
      uint64_t fwdsrcA(M * mreg, W * wreg, D * dreg, Stage ** stages, uint64_t srcA, uint64_t rvalA);
      uint64_t fwdsrcB(M * mreg, W * wreg, Stage ** stages, uint64_t srcB, uint64_t rvalB);
      uint64_t getd_srcA();
      uint64_t getd_srcB();
      void calculateControlSignals(uint64_t E_icode, uint64_t E_dstM, uint64_t d_srcA, uint64_t d_srcB, uint64_t e_Cnd);
};
