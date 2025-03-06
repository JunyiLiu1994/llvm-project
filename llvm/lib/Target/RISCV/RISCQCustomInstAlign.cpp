#include "RISCV.h"
#include "RISCVInstrInfo.h"
#include "RISCVTargetMachine.h"

#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"

using namespace llvm;

#define RISCQ_CUSTOM_ALIGN_NAME "RISC-Q customize instruction align pass"

namespace {

class RISCQCustomInstAlign : public MachineFunctionPass {
public:
  const RISCVSubtarget *STI;
  const RISCVInstrInfo *TII;
  static char ID;

  RISCQCustomInstAlign() : MachineFunctionPass(ID) {}

  bool runOnMachineFunction(MachineFunction &MF) override;

  StringRef getPassName() const override { return RISCQ_CUSTOM_ALIGN_NAME; }
};

char RISCQCustomInstAlign::ID = 0;

bool RISCQCustomInstAlign::runOnMachineFunction(MachineFunction &MF) {
  STI = &MF.getSubtarget<RISCVSubtarget>();
  TII = STI->getInstrInfo();
  for (auto &MBB : MF) {
    MachineBasicBlock::iterator MBBI = MBB.begin(), E = MBB.end();
    while (MBBI != E) {
      MachineBasicBlock::iterator NMBBI = std::next(MBBI);
      MachineInstr &MI = *MBBI;
      DebugLoc DL = MI.getDebugLoc();
      if(!MI.getDesc().isPseudo()) {
        BuildMI(MBB, MBBI, DL, TII->get(TargetOpcode::INLINEASM))
            .addExternalSymbol(".align 4");
      }
      MBBI = NMBBI;
    }
  }
  return true;
}
}

INITIALIZE_PASS(RISCQCustomInstAlign, "riscq-custom-align",
                RISCQ_CUSTOM_ALIGN_NAME, false, false)

namespace llvm {

FunctionPass *createRISCQCustomInstAlignPass() { return new RISCQCustomInstAlign(); }

} // end of namespace llvm
