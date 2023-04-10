
class DecodeStage: public Stage {
   private:
      void setEInput( E * ereg, uint64_t stat, uint64_t icode,
        uint64_t ifun, uint64_t valC, uint64_t valA, uint64_t valB,
         uint64_t dstE, uint64_t dstM, uint64_t srcA, uint64_t srcB);
   public:
      bool doClockLow(PipeReg ** pregs, Stage ** stages);
      void doClockHigh(PipeReg ** pregs);
      uint64_t getsrcA(uint64_t icode, uint64_t rA);
      uint64_t getsrcB(uint64_t icode, uint64_t rB);
      uint64_t getdstE(uint64_t icode, uint64_t rB);
      uint64_t getdstM(uint64_t icode, uint64_t rA);
      uint64_t fwdsrcA(D * dreg, M * mreg, W * wreg, ExecuteStage * xstage, uint64_t srcA, uint64_t valA);
      uint64_t fwdsrcB(D * dreg, M * mreg, W * wreg, ExecuteStage * xstage, uint64_t srcB, uint64_t valB);
};
