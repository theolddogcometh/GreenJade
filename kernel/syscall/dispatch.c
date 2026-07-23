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
 * Soft deepen: SYSCALL entry soft stats (LSTAR bridge + personality route).
 *   greppable: SYSCALL_ENTRY_SOFT_STATS / "syscall: soft stats"
 *
 * Pure C11. Dual-licensed MIT OR Apache-2.0.
 */
#include <gj/cold_ipc.h>
#include <gj/cpu.h>
#include <gj/futex.h>
#include <gj/klog.h>
#include <gj/linux_dispatch.h>
#include <gj/process.h>
#include <gj/string.h>
#include <gj/syscall.h>
#include <gj/thread.h>

/* Used only when no process is bound (early boot / standalone unit tests). */
static enum gj_personality g_eDefaultPersonality = GJ_PERSONALITY_LINUX;

/*
 * Top-level entry soft counters (wrap OK). See gj_syscall_entry_stats.
 * greppable: SYSCALL_ENTRY_SOFT_STATS
 */
static struct gj_syscall_entry_stats g_entryStats;

/**
 * Refresh 0/1 mirrors of the boot default personality (not cumulative).
 */
static void
entry_soft_mirror_default(void)
{
    if (g_eDefaultPersonality == GJ_PERSONALITY_LINUX) {
        g_entryStats.u64DefaultIsLinux = 1;
        g_entryStats.u64DefaultIsNative = 0;
    } else {
        g_entryStats.u64DefaultIsLinux = 0;
        g_entryStats.u64DefaultIsNative = 1;
    }
}

/**
 * Soft outcome buckets after a live frame finished (handler wrote i64Ret).
 */
static void
entry_soft_note_complete(struct gj_syscall_regs *pRegs)
{
    i64 i64Ret;

    if (pRegs == NULL) {
        return;
    }
    g_entryStats.u64Complete++;
    i64Ret = pRegs->i64Ret;
    g_entryStats.u64LastRetBits = (u64)i64Ret;
    if (i64Ret < 0) {
        g_entryStats.u64RetNeg++;
    } else if (i64Ret == 0) {
        g_entryStats.u64RetZero++;
    } else {
        g_entryStats.u64RetPos++;
    }
}

void
gj_syscall_entry_soft_note_bridge(struct gj_syscall_regs *pRegs)
{
    g_entryStats.u64BridgeEnter++;
    if (pRegs == NULL) {
        g_entryStats.u64BridgeNull++;
        return;
    }
    /* Soft snapshot nr at the hardware edge (before route). */
    g_entryStats.u64LastNr = pRegs->u64Nr;
}

void
gj_syscall_entry_stats_get(struct gj_syscall_entry_stats *pOut)
{
    if (pOut == NULL) {
        return;
    }
    entry_soft_mirror_default();
    *pOut = g_entryStats;
}

void
gj_syscall_entry_stats_reset(void)
{
    memset(&g_entryStats, 0, sizeof(g_entryStats));
    entry_soft_mirror_default();
}

u64
gj_syscall_entry_stats_soft(void)
{
    entry_soft_mirror_default();
    /* Grep: syscall: soft stats */
    kprintf("syscall: soft stats bridge=%llu bridge_null=%llu "
            "disp=%llu disp_null=%llu native=%llu linux=%llu "
            "pcb_n=%llu pcb_l=%llu def_pers=%llu bound=%llu unbound=%llu "
            "init=%llu set_ok=%llu set_rej=%llu complete=%llu "
            "ret_neg=%llu ret0=%llu ret_pos=%llu last_nr=%llu "
            "def_linux=%llu def_native=%llu\n",
            (unsigned long long)g_entryStats.u64BridgeEnter,
            (unsigned long long)g_entryStats.u64BridgeNull,
            (unsigned long long)g_entryStats.u64DispatchEnter,
            (unsigned long long)g_entryStats.u64DispatchNull,
            (unsigned long long)g_entryStats.u64Native,
            (unsigned long long)g_entryStats.u64Linux,
            (unsigned long long)g_entryStats.u64PcbNative,
            (unsigned long long)g_entryStats.u64PcbLinux,
            (unsigned long long)g_entryStats.u64DefaultPers,
            (unsigned long long)g_entryStats.u64Bound,
            (unsigned long long)g_entryStats.u64Unbound,
            (unsigned long long)g_entryStats.u64Init,
            (unsigned long long)g_entryStats.u64SetPersOk,
            (unsigned long long)g_entryStats.u64SetPersReject,
            (unsigned long long)g_entryStats.u64Complete,
            (unsigned long long)g_entryStats.u64RetNeg,
            (unsigned long long)g_entryStats.u64RetZero,
            (unsigned long long)g_entryStats.u64RetPos,
            (unsigned long long)g_entryStats.u64LastNr,
            (unsigned long long)g_entryStats.u64DefaultIsLinux,
            (unsigned long long)g_entryStats.u64DefaultIsNative);
    return g_entryStats.u64DispatchEnter;
}

void
gj_syscall_init(void)
{
    cold_ipc_init();
    futex_init();
    gj_linux_dispatch_init();
    g_eDefaultPersonality = GJ_PERSONALITY_LINUX;
    g_entryStats.u64Init++;
    entry_soft_mirror_default();
}

void
gj_syscall_set_default_personality(enum gj_personality e)
{
    /* Reject out-of-range values so a corrupt caller cannot invent modes. */
    if (e != GJ_PERSONALITY_NATIVE && e != GJ_PERSONALITY_LINUX) {
        g_entryStats.u64SetPersReject++;
        return;
    }
    g_eDefaultPersonality = e;
    g_entryStats.u64SetPersOk++;
    entry_soft_mirror_default();
}

enum gj_personality
gj_syscall_get_default_personality(void)
{
    return g_eDefaultPersonality;
}

/**
 * Resolve personality for this trap.
 * Prefer the current process PCB; fall back to the boot default.
 * Soft: bumps PCB native/linux or default-personality counters.
 */
static enum gj_personality
current_personality(void)
{
    struct gj_thread *pThr = thread_current();

    if (pThr != NULL && pThr->pProc != NULL) {
        if (pThr->pProc->u32Personality == 0) {
            g_entryStats.u64PcbNative++;
            return GJ_PERSONALITY_NATIVE;
        }
        g_entryStats.u64PcbLinux++;
        return GJ_PERSONALITY_LINUX;
    }
    g_entryStats.u64DefaultPers++;
    return g_eDefaultPersonality;
}

void
gj_syscall_dispatch(struct gj_syscall_regs *pRegs)
{
    struct gj_linux_regs linuxRegs;
    struct gj_thread *pThr;
    enum gj_personality ePers;

    g_entryStats.u64DispatchEnter++;
    if (pRegs == NULL) {
        g_entryStats.u64DispatchNull++;
        return;
    }

    g_entryStats.u64LastNr = pRegs->u64Nr;

    /*
     * Bind Linux "current" before any hybrid work so mmap/AS, creds, and
     * per-process tables see the calling process (not a stale boot task).
     * Native path does not require this, but binding is cheap and keeps a
     * mixed call sequence consistent if personality flips mid-smoke.
     */
    pThr = thread_current();
    if (pThr != NULL && pThr->pProc != NULL) {
        g_entryStats.u64Bound++;
        gj_linux_set_current(pThr->pProc, 1,
                             pThr->u32Id != 0 ? pThr->u32Id : 1);
    } else {
        g_entryStats.u64Unbound++;
    }

    ePers = current_personality();
    if (ePers == GJ_PERSONALITY_NATIVE) {
        g_entryStats.u64Native++;
        gj_native_syscall_dispatch(pRegs);
        entry_soft_note_complete(pRegs);
        return;
    }

    /* Linux hybrid Option C: hot table first, cold door for the rest. */
    g_entryStats.u64Linux++;
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
    entry_soft_note_complete(pRegs);
}
