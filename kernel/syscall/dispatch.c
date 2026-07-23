/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Top-level syscall dispatch: native GJ vs Linux hybrid personality.
 *
 * Product path:
 *   - Boot default is LINUX so freestanding embeds, glibc smokes, and Steam
 *     land on the Option C hybrid table without per-process setup.
 *   - PCB u32Personality == 0 selects native GJ_SYS_* (native.c).
 *   - This file only routes and binds Linux "current"; handlers live in
 *     native.c / linux_*.c. Do not put subsystem logic here.
 *
 * Pure C11. Dual-licensed MIT OR Apache-2.0.
 */
#include <gj/cold_ipc.h>
#include <gj/cpu.h>
#include <gj/futex.h>
#include <gj/linux_dispatch.h>
#include <gj/process.h>
#include <gj/string.h>
#include <gj/syscall.h>
#include <gj/thread.h>

/* Used only when no process is bound (early boot / standalone unit tests). */
static enum gj_personality g_eDefaultPersonality = GJ_PERSONALITY_LINUX;

void
gj_syscall_init(void)
{
    cold_ipc_init();
    futex_init();
    gj_linux_dispatch_init();
    g_eDefaultPersonality = GJ_PERSONALITY_LINUX;
}

void
gj_syscall_set_default_personality(enum gj_personality e)
{
    /* Reject out-of-range values so a corrupt caller cannot invent modes. */
    if (e != GJ_PERSONALITY_NATIVE && e != GJ_PERSONALITY_LINUX) {
        return;
    }
    g_eDefaultPersonality = e;
}

enum gj_personality
gj_syscall_get_default_personality(void)
{
    return g_eDefaultPersonality;
}

/**
 * Resolve personality for this trap.
 * Prefer the current process PCB; fall back to the boot default.
 */
static enum gj_personality
current_personality(void)
{
    struct gj_thread *pThr = thread_current();

    if (pThr != NULL && pThr->pProc != NULL) {
        if (pThr->pProc->u32Personality == 0) {
            return GJ_PERSONALITY_NATIVE;
        }
        return GJ_PERSONALITY_LINUX;
    }
    return g_eDefaultPersonality;
}

void
gj_syscall_dispatch(struct gj_syscall_regs *pRegs)
{
    struct gj_linux_regs linuxRegs;
    struct gj_thread *pThr;

    if (pRegs == NULL) {
        return;
    }

    /*
     * Bind Linux "current" before any hybrid work so mmap/AS, creds, and
     * per-process tables see the calling process (not a stale boot task).
     * Native path does not require this, but binding is cheap and keeps a
     * mixed call sequence consistent if personality flips mid-smoke.
     */
    pThr = thread_current();
    if (pThr != NULL && pThr->pProc != NULL) {
        gj_linux_set_current(pThr->pProc, 1,
                             pThr->u32Id != 0 ? pThr->u32Id : 1);
    }

    if (current_personality() == GJ_PERSONALITY_NATIVE) {
        gj_native_syscall_dispatch(pRegs);
        return;
    }

    /* Linux hybrid Option C: hot table first, cold door for the rest. */
    memset(&linuxRegs, 0, sizeof(linuxRegs));
    linuxRegs.u64Nr = pRegs->u64Nr;
    linuxRegs.u64Arg0 = pRegs->u64Arg0;
    linuxRegs.u64Arg1 = pRegs->u64Arg1;
    linuxRegs.u64Arg2 = pRegs->u64Arg2;
    linuxRegs.u64Arg3 = pRegs->u64Arg3;
    linuxRegs.u64Arg4 = pRegs->u64Arg4;
    linuxRegs.u64Arg5 = pRegs->u64Arg5;
    gj_linux_syscall_dispatch(&linuxRegs);
    pRegs->i64Ret = linuxRegs.i64Ret;
}
