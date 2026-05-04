#ifndef ARCH_H__
#define ARCH_H__

#ifdef __riscv_e
#define NR_REGS 16
#else
#define NR_REGS 32
#endif

struct Context {
    uintptr_t gpr[NR_REGS];    // 偏移 0，对应栈上 sp + 0
    uintptr_t mcause;          // 偏移 NR_REGS*4，对应 OFFSET_CAUSE
    uintptr_t mstatus;         // 偏移 (NR_REGS+1)*4，对应 OFFSET_STATUS
    uintptr_t mepc;            // 偏移 (NR_REGS+2)*4，对应 OFFSET_EPC
    void *pdir;
};

#ifdef __riscv_e
#define GPR1 gpr[15] // a5
#else
#define GPR1 gpr[17] // a7
#endif

#define GPR2 gpr[0]
#define GPR3 gpr[0]
#define GPR4 gpr[0]
#define GPRx gpr[0]

#endif
