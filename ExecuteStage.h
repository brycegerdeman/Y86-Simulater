class ExecuteStage: public Stage {
  private:
        void setMInput(M * mreg, uint64_t stat, 
        uint64_t icode, uint64_t Cnd, uint64_t valE, uint64_t valA, 
        uint64_t dstE, uint64_t dstM);

  public:
        bool doClockLow(PipeReg ** pregs, Stage ** stages);
        void doClockHigh(PipeReg ** pregs);
        void setvalE(E * ereg, M * mreg, uint64_t &valE);
        uint64_t getaluA(uint64_t icode, uint64_t valA, uint64_t valC);
        uint64_t getaluB(uint64_t icode, uint64_t valB);
        uint64_t getalufun(uint64_t icode, uint64_t ifun);
        bool setcc(uint64_t icode);
        uint64_t getdstE(uint64_t icode, uint64_t Cnd, uint64_t dstE);
};