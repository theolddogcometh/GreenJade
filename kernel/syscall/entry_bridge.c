/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Bridge from syscall_entry.S stack frame → gj_syscall_dispatch.
 *
 * Soft deepen: LSTAR edge counters (bridge enter / null) before route.
 *   greppable: SYSCALL_ENTRY_SOFT_STATS
 *   Cumulative route stats live in dispatch.c (gj_syscall_entry_stats_*).
 *
 * Pure C11 — dual MIT OR Apache-2.0.
 */
#include <gj/syscall.h>

void
gj_syscall_entry_asm_bridge(struct gj_syscall_regs *pRegs)
{
    /*
     * Soft entry edge: every LSTAR land is counted before personality route.
     * Smoke tests that call gj_syscall_dispatch directly skip this note.
     */
    gj_syscall_entry_soft_note_bridge(pRegs);
    if (pRegs == NULL) {
        return;
    }
    gj_syscall_dispatch(pRegs);
}
