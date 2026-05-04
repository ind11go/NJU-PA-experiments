/***************************************************************************************
* Copyright (c) 2014-2024 Zihao Yu, Nanjing University
*
* NEMU is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*          http://license.coscl.org.cn/MulanPSL2
*
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
*
* See the Mulan PSL v2 for more details.
***************************************************************************************/

#include <isa.h>

word_t isa_raise_intr(word_t NO, vaddr_t epc) {
  /* Save the return address and exception code. */
  cpu.mepc = epc;
  cpu.mcause = NO;

  /* Update mstatus: save MIE to MPIE, then clear MIE. */
  /* mstatus.MPIE is bit 7, mstatus.MIE is bit 3 */
  uint32_t mie = (cpu.mstatus >> 3) & 1;
  cpu.mstatus = (cpu.mstatus & ~(1 << 7)) | (mie << 7);  // MPIE = MIE
  cpu.mstatus &= ~(1 << 3);                               // MIE = 0

  /* Return the trap vector base address. */
  return cpu.mtvec;
}

word_t isa_query_intr() {
  return INTR_EMPTY;
}
