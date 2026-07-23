/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Bridge from syscall_entry.S stack frame → gj_syscall_dispatch.
 * Pure C11 — dual MIT OR Apache-2.0.
 */
#include <gj/syscall.h>

void
gj_syscall_entry_asm_bridge(struct gj_syscall_regs *pRegs)
{
    if (pRegs == NULL) {
        return;
    }
    gj_syscall_dispatch(pRegs);
}
