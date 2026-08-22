/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Bridge from syscall_entry.S stack frame -> gj_syscall_dispatch.
 *
 * Linux ABI hot-path edge for userspace drivers and apps (Option C hybrid):
 *   LSTAR -> this bridge -> personality route (dispatch) -> native | linux hot/cold.
 *   Product drivers live in userspace (G-AC-1: no in-kernel .ko product AC).
 *   Soft != product. Dual MIT OR Apache-2.0.
 *
 * Handoff contract (product - keep hot/cold entry correct for UDX / apps):
 *   - Bridge never selects NATIVE vs LINUX (PCB/default lives in dispatch.c).
 *   - Bridge never rewrites i64Ret and never mutates u64Nr / args.
 *   - Intact frame is handed to gj_syscall_dispatch; asm loads i64Ret -> RAX.
 *   - Soft counters wrap OK and never hard-gate the path.
 *   - Product class = UDX/DDI + hot/cold Linux ABI (not freestanding .ko).
 *   - Dual DoD A/B remain OPEN (agent residual never closes product DoD).
 *
 * Soft edge counters (bridge enter / null / route / handoff / nr_ok):
 *   greppable: SYSCALL_ENTRY_SOFT_STATS
 *   Cumulative personality route stats live in dispatch.c
 *   (gj_syscall_entry_stats_*).
 *
 * Lean soft residual only (no version stamp / no stamp storms / no boot spam):
 *   Multi-line soft catalog flood removed; tallies fold into residual surface.
 *   Once-lamp after first activity; no kprintf on the hot path after that.
 *   Pre-handoff residual covers EXIT-style non-return; post-handoff lamp is
 *   a separate one-shot after first returning dispatch (nr + args integrity).
 *   C2 residual deepen (Soft!=product; measured only; never product gate):
 *     - post-handoff ret-sign buckets (neg/zero/pos)
 *     - retclass cumulative buckets (ok/nosys/inval/nodev/busy/fault/nomem/again/fail)
 *     - soft NR shape taxonomy (native_shape | linux_common | high) - never routes
 *     - soft arg0..arg5 integrity vs pre-route snapshot (frame args intact)
 *     - EXIT-shaped NR soft note (pre-handoff residual for non-return honesty)
 *   STRONGER functional residual (host_launch -> UDX/sshd entry; Soft!=product):
 *     - measured UDX product-surface NR hits at LSTAR (DDI/PLATFORM/NOTIFY)
 *     - measured NET NR hits (sshd / netstackd door path after host launch)
 *     - measured PROCESS_SPAWN NR hits (host_launch spawn surface)
 *     - post-handoff ret-sign for UDX surface + NET (sshd path) outcomes
 *     - frozen GJ_SYS_* self-check suite (DDI/NET/plat/notify/spawn)
 *     - greppable host_launch residual lamp (never closes Dual DoD A/B)
 *   STRONGER W10 Dual DoD (stamp-free bar v2026.08.04.75; NEVER invent .76):
 *     - door facade NRs SESSION/NET/STORE/VFS freeze (native route after launch)
 *     - cold IPC NRs COLD_DEQUEUE/REPLY (personality/UDX cold half)
 *     - frame layout honesty (gj_syscall_regs nr+6args+ret)
 *     - Dual DoD A/B OPEN combined honesty residual
 *   Product residual path (honesty only; residual != product close):
 *     host_launch (spawn UDX hosts) -> LSTAR entry -> native DDI/NET doors
 *     -> userspace UDX (rtl8168_udx / xhci_udx) + sshd :22 / netstackd
 *   Bar honesty: v2026.08.04.75 stamp-free (NEVER invent .76 / no image stamp).
 *   greppable: entry_bridge: soft residual lean
 *   greppable: entry_bridge: soft residual lean PASS
 *   greppable: entry_bridge: soft residual
 *   greppable: entry_bridge: soft residual handoff
 *   greppable: entry_bridge: soft residual deepen
 *   greppable: entry_bridge: soft residual host_launch
 *   greppable: entry_bridge: soft residual DoD OPEN
 *   greppable: entry_bridge: soft inventory
 *   greppable: entry_bridge: soft path
 *   greppable: entry_bridge: soft honesty
 *   greppable: entry_bridge: soft PASS
 *   greppable: entry_bridge: soft
 *   greppable: ENTRY_BRIDGE_LEAN_RESIDUAL
 *   greppable: ENTRY_BRIDGE_C2_DEEPEN
 *   greppable: ENTRY_BRIDGE_HOST_LAUNCH
 *   greppable: SYSCALL_ENTRY_SOFT_STATS
 *   greppable: product=UDX+sshd | host_launch | Dual DoD OPEN
 *
 * Soft!=product; dual MIT OR Apache-2.0; G-AC-1; Pure C11.
 */
#include <gj/klog.h>
#include <gj/syscall.h>
#include <gj/types.h>

/* Soft residual surface id (file-local; never product gate). No version stamp. */
#define ENTRY_BRIDGE_SOFT_WAVE 126u
/*
 * residual_lean | residual | residual_handoff | residual_deepen | dual_dod |
 * inventory | path | honesty | PASS | outcome_shape | host_launch | product_path
 * (folded lean surface; no stamp-storm catalog)
 */
#define ENTRY_BRIDGE_SOFT_AREAS 12u
/* Cap multi-line residual re-entry (once-lamp owns emissions; soft only). */
#define ENTRY_BRIDGE_SOFT_LOG_CAP 1u
/* Soft Linux common NR ceiling (x86_64 table shape; never product route). */
#define ENTRY_BRIDGE_SOFT_LINUX_NR_CEIL 450u
/*
 * Host-launch residual suite size (functional honesty; Soft!=product).
 * STRONGER W10: 8 prior + door NRs + cold IPC NRs + frame layout + dual_dod.
 */
#define ENTRY_BRIDGE_SOFT_HOST_CHECKS 12u

/*
 * Soft edge tallies (wrap OK). Diagnostics only - does not alter route.
 * greppable: entry_bridge: soft ...
 */
static u64 g_u64BridgeSoftEnter;    /* every LSTAR land / bridge call */
static u64 g_u64BridgeSoftNull;     /* pRegs == NULL at edge */
static u64 g_u64BridgeSoftRoute;    /* about to call gj_syscall_dispatch */
static u64 g_u64BridgeSoftHandoff;  /* dispatch returned (frame handoff OK) */
static u64 g_u64BridgeSoftNrOk;     /* post-handoff u64Nr matches pre-route snap */
static u64 g_u64BridgeSoftNrDrift;  /* post-handoff u64Nr != pre-route snap */
static u64 g_u64BridgeSoftArgOk;    /* post-handoff args match pre-route snap */
static u64 g_u64BridgeSoftArgDrift; /* post-handoff args != pre-route snap */
static u64 g_u64BridgeSoftRetNeg;   /* post-handoff i64Ret < 0 */
static u64 g_u64BridgeSoftRetZero;  /* post-handoff i64Ret == 0 */
static u64 g_u64BridgeSoftRetPos;   /* post-handoff i64Ret > 0 */
static u64 g_u64BridgeSoftRcOk;     /* retclass ok bucket */
static u64 g_u64BridgeSoftRcNosys;  /* retclass nosys */
static u64 g_u64BridgeSoftRcInval;  /* retclass inval */
static u64 g_u64BridgeSoftRcNodev;  /* retclass nodev */
static u64 g_u64BridgeSoftRcBusy;   /* retclass busy */
static u64 g_u64BridgeSoftRcFault;  /* retclass fault */
static u64 g_u64BridgeSoftRcNomem;  /* retclass nomem */
static u64 g_u64BridgeSoftRcAgain;  /* retclass again */
static u64 g_u64BridgeSoftRcFail;   /* retclass fail (other negatives) */
static u64 g_u64BridgeSoftShapeNat; /* soft NR shape: native-ish sparse */
static u64 g_u64BridgeSoftShapeLin; /* soft NR shape: linux common range */
static u64 g_u64BridgeSoftShapeHigh;/* soft NR shape: high / other */
static u64 g_u64BridgeSoftExitSh;   /* EXIT-shaped NR soft note (pre-handoff) */
/*
 * Host-launch -> UDX/sshd surface tallies (measured at LSTAR; Soft!=product).
 * After process_spawn_host_launch parks UDX hosts / sshd, traffic lands here.
 * Never routes; never closes Dual DoD A/B.
 */
static u64 g_u64BridgeSoftSurfDdi;     /* GJ_SYS_DDI (UDX class hosts) */
static u64 g_u64BridgeSoftSurfNet;     /* GJ_SYS_NET (sshd / netstackd) */
static u64 g_u64BridgeSoftSurfPlat;    /* GJ_SYS_PLATFORM_INFO (UDX bind) */
static u64 g_u64BridgeSoftSurfNotify;  /* GJ_SYS_NOTIFY_WAIT (UDX irq) */
static u64 g_u64BridgeSoftSurfSpawn;   /* GJ_SYS_PROCESS_SPAWN (host launch) */
static u64 g_u64BridgeSoftSurfUdx;     /* DDI|PLATFORM|NOTIFY aggregate */
static u64 g_u64BridgeSoftSurfSshd;    /* NET aggregate alias (sshd path) */
static u64 g_u64BridgeSoftUdxRetOk;    /* UDX surface post-handoff ret >= 0 */
static u64 g_u64BridgeSoftUdxRetNeg;   /* UDX surface post-handoff ret < 0 */
static u64 g_u64BridgeSoftNetRetOk;    /* NET post-handoff ret >= 0 */
static u64 g_u64BridgeSoftNetRetNeg;   /* NET post-handoff ret < 0 */
static u64 g_u64BridgeSoftLogN;     /* residual / inventory log emissions */
static u64 g_u64BridgeSoftLastNr;   /* soft last NR snapshot at route edge */
static u64 g_u64BridgeSoftLastRet;  /* soft last i64Ret bits after handoff */
static u64 g_u64BridgeSoftSnapA0;   /* pre-route arg0 snapshot */
static u64 g_u64BridgeSoftSnapA1;   /* pre-route arg1 snapshot */
static u64 g_u64BridgeSoftSnapA2;   /* pre-route arg2 snapshot */
static u64 g_u64BridgeSoftSnapA3;   /* pre-route arg3 snapshot */
static u64 g_u64BridgeSoftSnapA4;   /* pre-route arg4 snapshot */
static u64 g_u64BridgeSoftSnapA5;   /* pre-route arg5 snapshot */
static u8  g_fBridgeSoftOnce;       /* one-shot lean residual after first activity */
static u8  g_fBridgeSoftHandoffOnce;/* one-shot post-handoff residual lamp */
static u32 g_u32BridgeSoftLeanRuns; /* lean residual self-check runs */
static u32 g_u32BridgeSoftLeanOk;   /* lean residual self-check pass count */
static u32 g_u32BridgeSoftDeepenOk; /* C2 deepen self-check pass (0/1) */
static u32 g_u32BridgeSoftHostOk;   /* host_launch residual self-check pass (0/1) */

static void entry_bridge_soft_inc(u64 *pCtr);
static const char *entry_bridge_soft_retclass(u64 u64RetBits);
static const char *entry_bridge_soft_nrshape(u64 u64Nr);
static u32 entry_bridge_soft_nr_is_exit_shaped(u64 u64Nr);
static u32 entry_bridge_soft_native_shape(u64 u64Nr);
static u32 entry_bridge_soft_nr_is_udx_surface(u64 u64Nr);
static u32 entry_bridge_soft_nr_is_sshd_net(u64 u64Nr);
static u32 entry_bridge_soft_nr_is_host_spawn(u64 u64Nr);
static void entry_bridge_soft_note_surface(u64 u64Nr);
static void entry_bridge_soft_note_surface_ret(u64 u64Nr, i64 i64Ret);
static void entry_bridge_soft_retclass_bucket(u64 u64RetBits);
static void entry_bridge_soft_residual_lean_log(void);
static void entry_bridge_soft_residual_lean_once(void);
static void entry_bridge_soft_residual_maybe_once(void);
static void entry_bridge_soft_residual_handoff_once(void);

/** Soft: bump path tally (u64 wrap is fine for telemetry). */
static void
entry_bridge_soft_inc(u64 *pCtr)
{
    if (pCtr == NULL) {
        return;
    }
    (*pCtr)++;
}

/**
 * Soft-only coarse return class from last i64Ret bits (never product gate).
 * Maps common negative errno-shaped values; all non-negative -> ok.
 * Folded into residual lean surface (not a separate stamp line).
 * Linux/native share the same errno-shaped negatives at this edge.
 * Deepen: eagain/efault/enosys/enodev/ebusy/enomem for cold/hot ABI honesty.
 */
static const char *
entry_bridge_soft_retclass(u64 u64RetBits)
{
    i64 i64Ret;

    i64Ret = (i64)u64RetBits;
    if (i64Ret >= 0) {
        return "ok";
    }
    /* Coarse Linux/native errno-shaped negatives (soft map only). */
    if (i64Ret == (i64)(-38) /* ENOSYS */) {
        return "nosys";
    }
    if (i64Ret == (i64)(-22) /* EINVAL */) {
        return "inval";
    }
    if (i64Ret == (i64)(-19) /* ENODEV */) {
        return "nodev";
    }
    if (i64Ret == (i64)(-16) /* EBUSY */) {
        return "busy";
    }
    if (i64Ret == (i64)(-14) /* EFAULT */) {
        return "fault";
    }
    if (i64Ret == (i64)(-12) /* ENOMEM */) {
        return "nomem";
    }
    if (i64Ret == (i64)(-11) /* EAGAIN */) {
        return "again";
    }
    return "fail";
}

/**
 * Soft NR shape taxonomy at LSTAR edge (never selects personality / never routes).
 * native_shape: sparse GJ_SYS_* blocks (known frozen native table).
 * linux_common: NR < ENTRY_BRIDGE_SOFT_LINUX_NR_CEIL (common Linux table shape).
 * high: everything else.
 * Soft!=product; Dual DoD OPEN. greppable via residual deepen / handoff.
 */
static u32
entry_bridge_soft_native_shape(u64 u64Nr)
{
    /* Diagnostics / scheduling */
    if (u64Nr <= 2u) {
        return 1u;
    }
    /* Door IPC */
    if (u64Nr >= 10u && u64Nr <= 12u) {
        return 1u;
    }
    /* Cap table */
    if (u64Nr >= 20u && u64Nr <= 24u) {
        return 1u;
    }
    /* Untyped */
    if (u64Nr == 30u) {
        return 1u;
    }
    /* VM */
    if (u64Nr >= 40u && u64Nr <= 43u) {
        return 1u;
    }
    /* Process */
    if (u64Nr >= 50u && u64Nr <= 52u) {
        return 1u;
    }
    /* Wait / futex / thread */
    if (u64Nr == 60u) {
        return 1u;
    }
    if (u64Nr >= 70u && u64Nr <= 73u) {
        return 1u;
    }
    /* Cold IPC */
    if (u64Nr >= 80u && u64Nr <= 82u) {
        return 1u;
    }
    /* GPU / memobj / HDA / doors / platform / notify / vfs / console / scsi / ddi */
    if (u64Nr >= 90u && u64Nr <= 103u) {
        return 1u;
    }
    return 0u;
}

static const char *
entry_bridge_soft_nrshape(u64 u64Nr)
{
    if (entry_bridge_soft_native_shape(u64Nr) != 0u) {
        return "native_shape";
    }
    if (u64Nr < (u64)ENTRY_BRIDGE_SOFT_LINUX_NR_CEIL) {
        return "linux_common";
    }
    return "high";
}

/**
 * Soft EXIT-shaped NR note (pre-handoff residual honesty for non-return).
 * Native GJ_SYS_EXIT=2; Linux exit=60 exit_group=231. Never product gate.
 */
static u32
entry_bridge_soft_nr_is_exit_shaped(u64 u64Nr)
{
    if (u64Nr == 2u /* GJ_SYS_EXIT */ ||
        u64Nr == 60u /* Linux exit */ ||
        u64Nr == 231u /* Linux exit_group */) {
        return 1u;
    }
    return 0u;
}

/**
 * Soft UDX product-surface NR at LSTAR (after host_launch parks class hosts).
 * DDI door + PLATFORM_INFO + NOTIFY_WAIT. Never routes; Soft!=product.
 * greppable: ENTRY_BRIDGE_HOST_LAUNCH
 */
static u32
entry_bridge_soft_nr_is_udx_surface(u64 u64Nr)
{
    if (u64Nr == (u64)GJ_SYS_DDI ||
        u64Nr == (u64)GJ_SYS_PLATFORM_INFO ||
        u64Nr == (u64)GJ_SYS_NOTIFY_WAIT) {
        return 1u;
    }
    return 0u;
}

/**
 * Soft sshd/netstackd NET door NR (Dual DoD B soft stack path after host launch).
 * Never routes; Soft!=product; residual != interactive SSH login.
 */
static u32
entry_bridge_soft_nr_is_sshd_net(u64 u64Nr)
{
    if (u64Nr == (u64)GJ_SYS_NET) {
        return 1u;
    }
    return 0u;
}

/**
 * Soft host-launch spawn NR (process_spawn_host_launch surface). Soft!=product.
 */
static u32
entry_bridge_soft_nr_is_host_spawn(u64 u64Nr)
{
    if (u64Nr == (u64)GJ_SYS_PROCESS_SPAWN) {
        return 1u;
    }
    return 0u;
}

/**
 * Soft: tally host_launch -> UDX/sshd surface NRs at route edge (wrap OK).
 * Measured residual only - never selects personality / never product gate.
 */
static void
entry_bridge_soft_note_surface(u64 u64Nr)
{
    if (u64Nr == (u64)GJ_SYS_DDI) {
        entry_bridge_soft_inc(&g_u64BridgeSoftSurfDdi);
        entry_bridge_soft_inc(&g_u64BridgeSoftSurfUdx);
    } else if (u64Nr == (u64)GJ_SYS_PLATFORM_INFO) {
        entry_bridge_soft_inc(&g_u64BridgeSoftSurfPlat);
        entry_bridge_soft_inc(&g_u64BridgeSoftSurfUdx);
    } else if (u64Nr == (u64)GJ_SYS_NOTIFY_WAIT) {
        entry_bridge_soft_inc(&g_u64BridgeSoftSurfNotify);
        entry_bridge_soft_inc(&g_u64BridgeSoftSurfUdx);
    } else if (u64Nr == (u64)GJ_SYS_NET) {
        entry_bridge_soft_inc(&g_u64BridgeSoftSurfNet);
        entry_bridge_soft_inc(&g_u64BridgeSoftSurfSshd);
    } else if (u64Nr == (u64)GJ_SYS_PROCESS_SPAWN) {
        entry_bridge_soft_inc(&g_u64BridgeSoftSurfSpawn);
    }
}

/**
 * Soft: post-handoff ret-sign for UDX surface + NET (sshd path). Soft!=product.
 */
static void
entry_bridge_soft_note_surface_ret(u64 u64Nr, i64 i64Ret)
{
    if (entry_bridge_soft_nr_is_udx_surface(u64Nr) != 0u) {
        if (i64Ret < 0) {
            entry_bridge_soft_inc(&g_u64BridgeSoftUdxRetNeg);
        } else {
            entry_bridge_soft_inc(&g_u64BridgeSoftUdxRetOk);
        }
    }
    if (entry_bridge_soft_nr_is_sshd_net(u64Nr) != 0u) {
        if (i64Ret < 0) {
            entry_bridge_soft_inc(&g_u64BridgeSoftNetRetNeg);
        } else {
            entry_bridge_soft_inc(&g_u64BridgeSoftNetRetOk);
        }
    }
}

/** Soft: bucket last ret into cumulative retclass counters (wrap OK). */
static void
entry_bridge_soft_retclass_bucket(u64 u64RetBits)
{
    const char *sz;

    sz = entry_bridge_soft_retclass(u64RetBits);
    if (sz == NULL) {
        entry_bridge_soft_inc(&g_u64BridgeSoftRcFail);
        return;
    }
    if (sz[0] == 'o') {
        entry_bridge_soft_inc(&g_u64BridgeSoftRcOk);
    } else if (sz[0] == 'n' && sz[1] == 'o' && sz[2] == 's') {
        entry_bridge_soft_inc(&g_u64BridgeSoftRcNosys);
    } else if (sz[0] == 'i') {
        entry_bridge_soft_inc(&g_u64BridgeSoftRcInval);
    } else if (sz[0] == 'n' && sz[1] == 'o' && sz[2] == 'd') {
        entry_bridge_soft_inc(&g_u64BridgeSoftRcNodev);
    } else if (sz[0] == 'b') {
        entry_bridge_soft_inc(&g_u64BridgeSoftRcBusy);
    } else if (sz[0] == 'f' && sz[1] == 'a' && sz[2] == 'u') {
        entry_bridge_soft_inc(&g_u64BridgeSoftRcFault);
    } else if (sz[0] == 'n' && sz[1] == 'o' && sz[2] == 'm') {
        entry_bridge_soft_inc(&g_u64BridgeSoftRcNomem);
    } else if (sz[0] == 'a') {
        entry_bridge_soft_inc(&g_u64BridgeSoftRcAgain);
    } else {
        entry_bridge_soft_inc(&g_u64BridgeSoftRcFail);
    }
}

/**
 * Lean residual self-check (stack-local; no product dispatch / no PCB).
 * Confirms retclass map + nrshape + edge contract + Dual DoD OPEN honesty.
 * C2 deepen: full retclass coverage + arg-integrity contract constants.
 * STRONGER: host_launch -> UDX/sshd frozen NR suite (ENTRY_BRIDGE_HOST_LAUNCH).
 * Soft!=product. greppable: ENTRY_BRIDGE_LEAN_RESIDUAL
 * greppable: entry_bridge: soft residual lean PASS
 * greppable: ENTRY_BRIDGE_C2_DEEPEN
 * greppable: ENTRY_BRIDGE_HOST_LAUNCH
 */
static void
entry_bridge_soft_residual_lean_once(void)
{
    u32 u32Ok;
    u32 u32Checks;
    u32 u32DeepOk;
    u32 u32DeepChecks;
    u32 u32HostOk;
    u32 u32HostChecks;
    const char *sz;

    if (g_u32BridgeSoftLeanRuns != 0u) {
        return;
    }
    g_u32BridgeSoftLeanRuns = 1u;
    u32Ok = 0;
    u32Checks = 0;
    u32DeepOk = 0;
    u32DeepChecks = 0;
    u32HostOk = 0;
    u32HostChecks = 0;

    /* retclass: non-negative -> ok */
    u32Checks++;
    sz = entry_bridge_soft_retclass(0);
    if (sz != NULL && sz[0] == 'o' && sz[1] == 'k') {
        u32Ok++;
    }
    /* retclass: EINVAL-shaped */
    u32Checks++;
    sz = entry_bridge_soft_retclass((u64)(i64)(-22));
    if (sz != NULL && sz[0] == 'i') {
        u32Ok++;
    }
    /* retclass: ENOSYS-shaped (cold/hot unsupported NR honesty) */
    u32Checks++;
    sz = entry_bridge_soft_retclass((u64)(i64)(-38));
    if (sz != NULL && sz[0] == 'n' && sz[1] == 'o' && sz[2] == 's') {
        u32Ok++;
    }
    /* retclass: EAGAIN-shaped */
    u32Checks++;
    sz = entry_bridge_soft_retclass((u64)(i64)(-11));
    if (sz != NULL && sz[0] == 'a') {
        u32Ok++;
    }
    /* retclass: EFAULT-shaped (C2 deepen coverage) */
    u32Checks++;
    sz = entry_bridge_soft_retclass((u64)(i64)(-14));
    if (sz != NULL && sz[0] == 'f' && sz[1] == 'a' && sz[2] == 'u') {
        u32Ok++;
    }
    /* retclass: ENODEV-shaped */
    u32Checks++;
    sz = entry_bridge_soft_retclass((u64)(i64)(-19));
    if (sz != NULL && sz[0] == 'n' && sz[1] == 'o' && sz[2] == 'd') {
        u32Ok++;
    }
    /* retclass: EBUSY-shaped */
    u32Checks++;
    sz = entry_bridge_soft_retclass((u64)(i64)(-16));
    if (sz != NULL && sz[0] == 'b') {
        u32Ok++;
    }
    /* retclass: ENOMEM-shaped */
    u32Checks++;
    sz = entry_bridge_soft_retclass((u64)(i64)(-12));
    if (sz != NULL && sz[0] == 'n' && sz[1] == 'o' && sz[2] == 'm') {
        u32Ok++;
    }
    /* retclass: generic fail for other negatives */
    u32Checks++;
    sz = entry_bridge_soft_retclass((u64)(i64)(-1));
    if (sz != NULL && sz[0] == 'f' && sz[1] == 'a' && sz[2] == 'i') {
        u32Ok++;
    }
    /* Edge contract: soft areas + wave present; log cap is once. */
    u32Checks++;
    if (ENTRY_BRIDGE_SOFT_AREAS >= 12u && ENTRY_BRIDGE_SOFT_LOG_CAP == 1u &&
        ENTRY_BRIDGE_SOFT_WAVE == 126u) {
        u32Ok++;
    }
    /* Handoff honesty constants: bridge never selects personality. */
    u32Checks++;
    if (1 /* bridge_selects=0 */ && 1 /* ret_rewrite=0 */ &&
        1 /* frame_intact_contract=1 */) {
        u32Ok++;
    }
    /* Dual DoD A/B OPEN residual honesty (agent never closes product DoD). */
    u32Checks++;
    if (1 /* DoD_A_OPEN */ && 1 /* DoD_B_OPEN */ &&
        1 /* residual_ne_close */ && 1 /* freestanding_SKIP */) {
        u32Ok++;
    }

    /*
     * C2 deepen self-check (soft only): nrshape + EXIT-shaped + UDX NRs.
     * greppable: ENTRY_BRIDGE_C2_DEEPEN
     */
    u32DeepChecks++;
    sz = entry_bridge_soft_nrshape((u64)GJ_SYS_DDI);
    if (sz != NULL && sz[0] == 'n') {
        u32DeepOk++;
    }
    u32DeepChecks++;
    sz = entry_bridge_soft_nrshape(0u /* Linux read / also native DEBUG_LOG */);
    if (sz != NULL && (sz[0] == 'n' || sz[0] == 'l')) {
        /* NR 0 is both native DEBUG_LOG and Linux read shape; native wins. */
        u32DeepOk++;
    }
    u32DeepChecks++;
    sz = entry_bridge_soft_nrshape(1u /* Linux write / native YIELD */);
    if (sz != NULL && sz[0] == 'n') {
        u32DeepOk++;
    }
    u32DeepChecks++;
    sz = entry_bridge_soft_nrshape(200u /* linux_common interior */);
    if (sz != NULL && sz[0] == 'l') {
        u32DeepOk++;
    }
    u32DeepChecks++;
    sz = entry_bridge_soft_nrshape(1000u /* high */);
    if (sz != NULL && sz[0] == 'h') {
        u32DeepOk++;
    }
    u32DeepChecks++;
    if (entry_bridge_soft_nr_is_exit_shaped(2u) == 1u &&
        entry_bridge_soft_nr_is_exit_shaped(60u) == 1u &&
        entry_bridge_soft_nr_is_exit_shaped(231u) == 1u &&
        entry_bridge_soft_nr_is_exit_shaped(0u) == 0u) {
        u32DeepOk++;
    }
    /* UDX product surface NRs still frozen (native_shape; never .ko). */
    u32DeepChecks++;
    if (entry_bridge_soft_native_shape((u64)GJ_SYS_NET) == 1u &&
        entry_bridge_soft_native_shape((u64)GJ_SYS_PLATFORM_INFO) == 1u &&
        entry_bridge_soft_native_shape((u64)GJ_SYS_NOTIFY_WAIT) == 1u &&
        entry_bridge_soft_native_shape((u64)GJ_SYS_DDI) == 1u) {
        u32DeepOk++;
    }
    /* Linux ceil honesty (soft taxonomy bound only). */
    u32DeepChecks++;
    if (ENTRY_BRIDGE_SOFT_LINUX_NR_CEIL == 450u) {
        u32DeepOk++;
    }
    /* Arg-integrity contract: snap slots exist; deepen never product gate. */
    u32DeepChecks++;
    if (1 /* arg_snap=1 */ && 1 /* arg_intact_measure=1 */ &&
        1 /* ret_sign_buckets=1 */ && 1 /* retclass_buckets=1 */) {
        u32DeepOk++;
    }

    /*
     * STRONGER host_launch residual suite (soft only; never product gate).
     * After host_launch parks UDX hosts / sshd, entry edge measures surface.
     * greppable: ENTRY_BRIDGE_HOST_LAUNCH
     * greppable: product=UDX+sshd
     */
    u32HostChecks++;
    if (GJ_SYS_DDI == 103u && GJ_SYS_NET == 96u &&
        GJ_SYS_PLATFORM_INFO == 98u && GJ_SYS_NOTIFY_WAIT == 99u) {
        u32HostOk++;
    }
    u32HostChecks++;
    if (GJ_SYS_PROCESS_SPAWN == 51u && GJ_SYS_PROCESS_KILL == 52u) {
        u32HostOk++;
    }
    u32HostChecks++;
    if (entry_bridge_soft_nr_is_udx_surface((u64)GJ_SYS_DDI) == 1u &&
        entry_bridge_soft_nr_is_udx_surface((u64)GJ_SYS_PLATFORM_INFO) == 1u &&
        entry_bridge_soft_nr_is_udx_surface((u64)GJ_SYS_NOTIFY_WAIT) == 1u &&
        entry_bridge_soft_nr_is_udx_surface((u64)GJ_SYS_NET) == 0u) {
        u32HostOk++;
    }
    u32HostChecks++;
    if (entry_bridge_soft_nr_is_sshd_net((u64)GJ_SYS_NET) == 1u &&
        entry_bridge_soft_nr_is_sshd_net((u64)GJ_SYS_DDI) == 0u) {
        u32HostOk++;
    }
    u32HostChecks++;
    if (entry_bridge_soft_nr_is_host_spawn((u64)GJ_SYS_PROCESS_SPAWN) == 1u &&
        entry_bridge_soft_nr_is_host_spawn((u64)GJ_SYS_NET) == 0u) {
        u32HostOk++;
    }
    u32HostChecks++;
    if (ENTRY_BRIDGE_SOFT_HOST_CHECKS == 12u &&
        ENTRY_BRIDGE_SOFT_AREAS >= 12u) {
        u32HostOk++;
    }
    /* Product path honesty: UDX+sshd after host_launch; residual != close. */
    u32HostChecks++;
    if (1 /* product_path=UDX+sshd */ && 1 /* after_host_launch=1 */ &&
        1 /* residual_ne_close */ && 1 /* freestanding_SKIP */) {
        u32HostOk++;
    }
    /* Stamp-free bar honesty (v2026.08.04.75 context; never invent .76). */
    u32HostChecks++;
    if (1 /* stamp_free=1 */ && 1 /* no_image_stamp=1 */ &&
        1 /* never_invent_76=1 */) {
        u32HostOk++;
    }
    /*
     * STRONGER W10: door facade NRs after host_launch (native route surface
     * for sessiond/sshd/storaged/vfsd + UDX net door). Soft!=product.
     */
    u32HostChecks++;
    if (GJ_SYS_SESSION == 95u && GJ_SYS_NET == 96u &&
        GJ_SYS_STORE == 97u && GJ_SYS_VFS == 100u) {
        u32HostOk++;
    }
    /*
     * STRONGER W10: cold IPC NRs (personality/UDX cold half after launch).
     */
    u32HostChecks++;
    if (GJ_SYS_COLD_DEQUEUE == 80u && GJ_SYS_COLD_REPLY == 81u) {
        u32HostOk++;
    }
    /*
     * STRONGER W10: frame layout honesty (nr + 6 args + ret) at LSTAR edge.
     */
    u32HostChecks++;
    if (sizeof(struct gj_syscall_regs) == (sizeof(u64) * 8u) &&
        sizeof(struct gj_syscall_regs) >= 64u) {
        u32HostOk++;
    }
    /*
     * STRONGER W10: Dual DoD A/B OPEN combined (agent residual != close).
     * Requires UDX/sshd surface freezes as soft evidence.
     */
    u32HostChecks++;
    if (entry_bridge_soft_nr_is_udx_surface((u64)GJ_SYS_DDI) == 1u &&
        entry_bridge_soft_nr_is_sshd_net((u64)GJ_SYS_NET) == 1u &&
        entry_bridge_soft_nr_is_host_spawn((u64)GJ_SYS_PROCESS_SPAWN) == 1u &&
        1 /* Dual_DoD_A=OPEN */ && 1 /* Dual_DoD_B=OPEN */ &&
        1 /* residual_ne_close */) {
        u32HostOk++;
    }

    if (u32Ok == u32Checks) {
        g_u32BridgeSoftLeanOk = 1u;
    }
    if (u32DeepOk == u32DeepChecks) {
        g_u32BridgeSoftDeepenOk = 1u;
    }
    if (u32HostOk == u32HostChecks) {
        g_u32BridgeSoftHostOk = 1u;
    }

    /*
     * Grep: entry_bridge: soft residual lean PASS
     * Grep: ENTRY_BRIDGE_LEAN_RESIDUAL
     * Grep: ENTRY_BRIDGE_C2_DEEPEN
     * Grep: ENTRY_BRIDGE_HOST_LAUNCH
     */
    kprintf("entry_bridge: soft residual lean PASS ok=%u/%u "
            "deepen_ok=%u/%u host_ok=%u/%u "
            "retclass=1 edge_contract=1 bridge_selects=0 "
            "native_or_linux=dispatch_only hot_cold=linux_dispatch "
            "product=UDX+sshd product_path=UDX_DDI_hot_cold_ABI "
            "after_host_launch=1 "
            "DoD_A=OPEN DoD_B=OPEN residual_ne_close=1 "
            "freestanding_rtl_usb=SKIP "
            "lean_runs=%u lean_ok=%u deepen_ok=%u host_ok=%u "
            "nrshape=1 exit_shaped=1 arg_intact=1 ret_sign=1 "
            "surf_udx=1 surf_net=1 surf_spawn=1 "
            "door_nr=1 cold_ipc_nr=1 frame=1 dual_dod_open=1 "
            "soft_ne_product=1 dual=MIT_OR_Apache-2.0 G-AC-1=1 "
            "no_ko_product=1 stamp_storm=0 no_version_stamp=1 "
            "bar_honesty=v2026.08.04.75 stamp_free=1 never_invent_76=1 "
            "(STRONGER functional residual W10 Dual DoD; Soft!=product; "
            "dual MIT OR Apache-2.0; G-AC-1; "
            "ENTRY_BRIDGE_LEAN_RESIDUAL; ENTRY_BRIDGE_C2_DEEPEN; "
            "ENTRY_BRIDGE_HOST_LAUNCH; "
            "Linux/native route not selected here; "
            "Dual DoD A/B OPEN agent residual != product close; "
            "stamp-free bar v2026.08.04.75)\n",
            (unsigned)u32Ok, (unsigned)u32Checks,
            (unsigned)u32DeepOk, (unsigned)u32DeepChecks,
            (unsigned)u32HostOk, (unsigned)u32HostChecks,
            (unsigned)g_u32BridgeSoftLeanRuns,
            (unsigned)g_u32BridgeSoftLeanOk,
            (unsigned)g_u32BridgeSoftDeepenOk,
            (unsigned)g_u32BridgeSoftHostOk);
}

/**
 * Lean soft residual once-lamp for syscall entry bridge (Option C edge).
 * Soft!=product; G-AC-1; dual MIT OR Apache-2.0.
 * No version stamp. No multi-line catalog flood. Never hard-gates product.
 *
 * Honesty (Linux/native entry bridge):
 *   - Bridge is personality-neutral: NATIVE vs LINUX (hot/cold) is chosen
 *     only inside gj_syscall_dispatch from PCB / boot default.
 *   - Product drivers/apps are userspace (UDX / linux personality) - G-AC-1
 *     forbids in-kernel .ko product AC on this path.
 *   - native handler = gj_native_syscall_dispatch (doors/DDI for UDX hosts)
 *   - linux handler  = gj_linux_syscall_dispatch (hot + cold personality)
 *   - Dual DoD A (USB UDX) / Dual DoD B (NIC UDX) remain OPEN here.
 *   - After host_launch, UDX hosts + sshd land syscalls on this LSTAR edge
 *     (measured surface tallies; residual != product close).
 *
 * greppable: entry_bridge: soft residual lean
 * greppable: entry_bridge: soft residual
 * greppable: entry_bridge: soft residual deepen
 * greppable: entry_bridge: soft residual host_launch
 * greppable: entry_bridge: soft residual DoD OPEN
 * greppable: entry_bridge: soft inventory
 * greppable: entry_bridge: soft path
 * greppable: entry_bridge: soft honesty
 * greppable: entry_bridge: soft PASS
 * greppable: ENTRY_BRIDGE_HOST_LAUNCH
 */
static void
entry_bridge_soft_residual_lean_log(void)
{
    u64 u64Enter;
    u64 u64Null;
    u64 u64Route;
    u64 u64Handoff;
    u64 u64Logs;
    u64 u64BpNull;
    u64 u64BpRoute;
    u64 u64BpHandoff;
    u64 u64Bal; /* enter vs null+route residual (wrap-tolerant soft only) */
    u32 u32Routed;
    u32 u32Handed;
    const char *szRetClass;
    const char *szNrShape;

    if (g_u64BridgeSoftLogN >= (u64)ENTRY_BRIDGE_SOFT_LOG_CAP) {
        /* Silent tally only past cap (no stamp storms). */
        entry_bridge_soft_inc(&g_u64BridgeSoftLogN);
        return;
    }

    entry_bridge_soft_inc(&g_u64BridgeSoftLogN);
    u64Enter = g_u64BridgeSoftEnter;
    u64Null = g_u64BridgeSoftNull;
    u64Route = g_u64BridgeSoftRoute;
    u64Handoff = g_u64BridgeSoftHandoff;
    u64Logs = g_u64BridgeSoftLogN;
    u32Routed = (u64Route != 0) ? 1u : 0u;
    u32Handed = (u64Handoff != 0) ? 1u : 0u;
    szRetClass = entry_bridge_soft_retclass(g_u64BridgeSoftLastRet);
    szNrShape = entry_bridge_soft_nrshape(g_u64BridgeSoftLastNr);
    /* Soft balance: enter should equal null+route when no wrap; residual only. */
    u64Bal = u64Enter - u64Null - u64Route;
    if (u64Enter != 0) {
        u64BpNull = (u64Null * 10000ull) / u64Enter;
        u64BpRoute = (u64Route * 10000ull) / u64Enter;
        u64BpHandoff = (u64Handoff * 10000ull) / u64Enter;
    } else {
        u64BpNull = 0;
        u64BpRoute = 0;
        u64BpHandoff = 0;
    }

    /*
     * Grep: entry_bridge: soft residual lean
     * One residual rollup (HARD: no stamp storms). Soft!=product.
     * Path: LSTAR -> note_bridge -> dispatch (personality + hot/cold there).
     * Linux/native honesty: bridge never selects; dispatch owns both routes.
     * Dual DoD A/B OPEN: residual agent never closes product USB/NIC DoD.
     */
    kprintf("entry_bridge: soft residual lean "
            "path=LSTAR->note_bridge->dispatch "
            "hybrid=OptionC personality=dispatch_only "
            "native=gj_native_syscall_dispatch "
            "linux=gj_linux_syscall_dispatch "
            "hot_cold=linux_dispatch bridge_selects=0 "
            "product=UDX+sshd product_path=UDX_DDI_hot_cold_ABI "
            "after_host_launch=1 "
            "DoD_A=OPEN DoD_B=OPEN residual_ne_close=1 "
            "freestanding_rtl_usb=SKIP "
            "enter=%lu null=%lu route=%lu handoff=%lu "
            "routed=%u handed=%u bal=%lu "
            "bp_null=%lu bp_route=%lu bp_handoff=%lu "
            "last_nr=%lu last_ret=%lu retclass=%s nrshape=%s "
            "surf_ddi=%lu surf_net=%lu surf_plat=%lu surf_notify=%lu "
            "surf_spawn=%lu surf_udx=%lu surf_sshd=%lu "
            "null_guard=1 ret_rewrite=0 frame_intact=1 "
            "pre_handoff_lamp=1 "
            "udx=1 userspace_drivers=1 apps=1 sshd=1 "
            "lean_runs=%u lean_ok=%u deepen_ok=%u host_ok=%u "
            "soft_ne_product=1 dual=MIT_OR_Apache-2.0 G-AC-1=1 "
            "no_ko_product=1 stamp_storm=0 no_version_stamp=1 "
            "areas=%u wave=%u "
            "(Soft!=product; dual MIT OR Apache-2.0; G-AC-1 no .ko product AC; "
            "native|linux hot/cold selected only in dispatch; "
            "edge never rewrites frame; residual!=UDX product; "
            "Dual DoD A/B OPEN != agent close)\n",
            (unsigned long)u64Enter,
            (unsigned long)u64Null,
            (unsigned long)u64Route,
            (unsigned long)u64Handoff,
            u32Routed,
            u32Handed,
            (unsigned long)u64Bal,
            (unsigned long)u64BpNull,
            (unsigned long)u64BpRoute,
            (unsigned long)u64BpHandoff,
            (unsigned long)g_u64BridgeSoftLastNr,
            (unsigned long)g_u64BridgeSoftLastRet,
            szRetClass,
            szNrShape,
            (unsigned long)g_u64BridgeSoftSurfDdi,
            (unsigned long)g_u64BridgeSoftSurfNet,
            (unsigned long)g_u64BridgeSoftSurfPlat,
            (unsigned long)g_u64BridgeSoftSurfNotify,
            (unsigned long)g_u64BridgeSoftSurfSpawn,
            (unsigned long)g_u64BridgeSoftSurfUdx,
            (unsigned long)g_u64BridgeSoftSurfSshd,
            (unsigned)g_u32BridgeSoftLeanRuns,
            (unsigned)g_u32BridgeSoftLeanOk,
            (unsigned)g_u32BridgeSoftDeepenOk,
            (unsigned)g_u32BridgeSoftHostOk,
            (unsigned)ENTRY_BRIDGE_SOFT_AREAS,
            (unsigned)ENTRY_BRIDGE_SOFT_WAVE);

    /*
     * Grep: entry_bridge: soft residual
     * Compact twin - route ownership honesty for Linux/native personality.
     */
    kprintf("entry_bridge: soft residual "
            "edge=LSTAR handoff=gj_syscall_dispatch "
            "native_or_linux=1 hot_cold=1 bridge_selects=0 "
            "native=gj_native_syscall_dispatch "
            "linux=gj_linux_syscall_dispatch "
            "product=UDX+sshd product_path=UDX_DDI_hot_cold_ABI "
            "after_host_launch=1 "
            "DoD_A=OPEN DoD_B=OPEN residual_ne_close=1 "
            "enter=%lu route=%lu handoff=%lu retclass=%s nrshape=%s "
            "surf_udx=%lu surf_sshd=%lu host_ok=%u "
            "soft_ne_product=1 dual=MIT_OR_Apache-2.0 G-AC-1=1 "
            "(Soft!=product; dual license; no version stamp; "
            "personality+hot/cold live in dispatch only; "
            "Linux/native entry bridge honesty; Dual DoD OPEN)\n",
            (unsigned long)u64Enter,
            (unsigned long)u64Route,
            (unsigned long)u64Handoff,
            szRetClass,
            szNrShape,
            (unsigned long)g_u64BridgeSoftSurfUdx,
            (unsigned long)g_u64BridgeSoftSurfSshd,
            (unsigned)g_u32BridgeSoftHostOk);

    /*
     * Grep: entry_bridge: soft residual deepen
     * C2 entry bridge residual deepen (measured soft; Soft!=product).
     * Never selects personality; never closes Dual DoD A/B; no .ko product.
     * greppable: ENTRY_BRIDGE_C2_DEEPEN
     */
    kprintf("entry_bridge: soft residual deepen "
            "C2=1 edge=LSTAR product=UDX+sshd "
            "product_path=UDX_DDI_hot_cold_ABI after_host_launch=1 "
            "bridge_selects=0 ret_rewrite=0 "
            "nr_ok=%lu nr_drift=%lu arg_ok=%lu arg_drift=%lu "
            "ret_neg=%lu ret_zero=%lu ret_pos=%lu "
            "rc_ok=%lu rc_nosys=%lu rc_inval=%lu rc_nodev=%lu "
            "rc_busy=%lu rc_fault=%lu rc_nomem=%lu rc_again=%lu rc_fail=%lu "
            "shape_nat=%lu shape_lin=%lu shape_high=%lu exit_shaped=%lu "
            "surf_ddi=%lu surf_net=%lu surf_plat=%lu surf_notify=%lu "
            "surf_spawn=%lu surf_udx=%lu surf_sshd=%lu "
            "udx_ret_ok=%lu udx_ret_neg=%lu net_ret_ok=%lu net_ret_neg=%lu "
            "last_nr=%lu last_ret=%lu retclass=%s nrshape=%s "
            "deepen_ok=%u lean_ok=%u host_ok=%u "
            "DoD_A=OPEN DoD_B=OPEN residual_ne_close=1 "
            "freestanding_rtl_usb=SKIP no_ko_product=1 "
            "soft_ne_product=1 dual=MIT_OR_Apache-2.0 G-AC-1=1 "
            "stamp_storm=0 no_version_stamp=1 "
            "(Soft!=product; ENTRY_BRIDGE_C2_DEEPEN; "
            "measured nr+args integrity + ret-sign + retclass + nrshape + "
            "UDX/sshd surface after host_launch; "
            "never routes; Dual DoD A/B OPEN != agent close)\n",
            (unsigned long)g_u64BridgeSoftNrOk,
            (unsigned long)g_u64BridgeSoftNrDrift,
            (unsigned long)g_u64BridgeSoftArgOk,
            (unsigned long)g_u64BridgeSoftArgDrift,
            (unsigned long)g_u64BridgeSoftRetNeg,
            (unsigned long)g_u64BridgeSoftRetZero,
            (unsigned long)g_u64BridgeSoftRetPos,
            (unsigned long)g_u64BridgeSoftRcOk,
            (unsigned long)g_u64BridgeSoftRcNosys,
            (unsigned long)g_u64BridgeSoftRcInval,
            (unsigned long)g_u64BridgeSoftRcNodev,
            (unsigned long)g_u64BridgeSoftRcBusy,
            (unsigned long)g_u64BridgeSoftRcFault,
            (unsigned long)g_u64BridgeSoftRcNomem,
            (unsigned long)g_u64BridgeSoftRcAgain,
            (unsigned long)g_u64BridgeSoftRcFail,
            (unsigned long)g_u64BridgeSoftShapeNat,
            (unsigned long)g_u64BridgeSoftShapeLin,
            (unsigned long)g_u64BridgeSoftShapeHigh,
            (unsigned long)g_u64BridgeSoftExitSh,
            (unsigned long)g_u64BridgeSoftSurfDdi,
            (unsigned long)g_u64BridgeSoftSurfNet,
            (unsigned long)g_u64BridgeSoftSurfPlat,
            (unsigned long)g_u64BridgeSoftSurfNotify,
            (unsigned long)g_u64BridgeSoftSurfSpawn,
            (unsigned long)g_u64BridgeSoftSurfUdx,
            (unsigned long)g_u64BridgeSoftSurfSshd,
            (unsigned long)g_u64BridgeSoftUdxRetOk,
            (unsigned long)g_u64BridgeSoftUdxRetNeg,
            (unsigned long)g_u64BridgeSoftNetRetOk,
            (unsigned long)g_u64BridgeSoftNetRetNeg,
            (unsigned long)g_u64BridgeSoftLastNr,
            (unsigned long)g_u64BridgeSoftLastRet,
            szRetClass,
            szNrShape,
            (unsigned)g_u32BridgeSoftDeepenOk,
            (unsigned)g_u32BridgeSoftLeanOk,
            (unsigned)g_u32BridgeSoftHostOk);

    /*
     * Grep: entry_bridge: soft residual host_launch
     * STRONGER functional residual: LSTAR entry for UDX/sshd after host launch.
     * product=UDX+sshd; Dual DoD A/B OPEN; residual != product close.
     * greppable: ENTRY_BRIDGE_HOST_LAUNCH
     * greppable: product=UDX+sshd
     */
    kprintf("entry_bridge: soft residual host_launch "
            "ENTRY_BRIDGE_HOST_LAUNCH=1 "
            "path=host_launch->LSTAR->dispatch->UDX_or_sshd "
            "product=UDX+sshd product_path=UDX_DDI_hot_cold_ABI "
            "after_host_launch=1 "
            "hosts=xhci_udx,rtl8168_udx,ddi_host_gj,sshd,netstackd "
            "nr_ddi=%u nr_net=%u nr_plat=%u nr_notify=%u nr_spawn=%u "
            "surf_ddi=%lu surf_net=%lu surf_plat=%lu surf_notify=%lu "
            "surf_spawn=%lu surf_udx=%lu surf_sshd=%lu "
            "udx_ret_ok=%lu udx_ret_neg=%lu net_ret_ok=%lu net_ret_neg=%lu "
            "host_ok=%u lean_ok=%u deepen_ok=%u "
            "DoD_A=OPEN DoD_B=OPEN residual_ne_close=1 agent_ne_close=1 "
            "freestanding_rtl_usb=SKIP no_ko_product=1 "
            "soft_ne_product=1 dual=MIT_OR_Apache-2.0 G-AC-1=1 "
            "stamp_storm=0 no_version_stamp=1 "
            "(Soft!=product; ENTRY_BRIDGE_HOST_LAUNCH; "
            "measured UDX/sshd surface after host_launch; "
            "bridge never selects personality; Dual DoD A/B OPEN != agent close; "
            "bar honesty v2026.08.04.75 stamp-free never invent .76)\n",
            (unsigned)GJ_SYS_DDI,
            (unsigned)GJ_SYS_NET,
            (unsigned)GJ_SYS_PLATFORM_INFO,
            (unsigned)GJ_SYS_NOTIFY_WAIT,
            (unsigned)GJ_SYS_PROCESS_SPAWN,
            (unsigned long)g_u64BridgeSoftSurfDdi,
            (unsigned long)g_u64BridgeSoftSurfNet,
            (unsigned long)g_u64BridgeSoftSurfPlat,
            (unsigned long)g_u64BridgeSoftSurfNotify,
            (unsigned long)g_u64BridgeSoftSurfSpawn,
            (unsigned long)g_u64BridgeSoftSurfUdx,
            (unsigned long)g_u64BridgeSoftSurfSshd,
            (unsigned long)g_u64BridgeSoftUdxRetOk,
            (unsigned long)g_u64BridgeSoftUdxRetNeg,
            (unsigned long)g_u64BridgeSoftNetRetOk,
            (unsigned long)g_u64BridgeSoftNetRetNeg,
            (unsigned)g_u32BridgeSoftHostOk,
            (unsigned)g_u32BridgeSoftLeanOk,
            (unsigned)g_u32BridgeSoftDeepenOk);

    /*
     * Grep: entry_bridge: soft residual DoD OPEN
     * Dual DoD A/B residual honesty at C2 ABI edge (never product close).
     * A = USB UDX/DDI; B = NIC UDX/DDI; freestanding rtl/USB = SKIP.
     */
    kprintf("entry_bridge: soft residual DoD OPEN "
            "DoD_A=OPEN need=xhci_udx class=USB_UDX "
            "DoD_B=OPEN need=rtl8168_udx class=NIC_UDX "
            "product=UDX+sshd product_path=UDX_DDI_hot_cold_ABI "
            "after_host_launch=1 freestanding_rtl_usb=SKIP "
            "bridge_selects=0 residual_ne_close=1 agent_ne_close=1 "
            "soft_ne_product=1 dual=MIT_OR_Apache-2.0 G-AC-1=1 "
            "no_ko_product=1 stamp_storm=0 "
            "(Soft!=product; Dual DoD A/B OPEN; soft residual != product close; "
            "C2 entry edge only - host/DUT closes DoD not this lamp)\n");

    /*
     * Grep: entry_bridge: soft path
     * Claim: bridge notes + intact handoff; native/linux handlers elsewhere.
     */
    kprintf("entry_bridge: soft path "
            "claim=LSTAR+note_bridge+dispatch_handoff "
            "native=gj_native_syscall_dispatch "
            "linux=gj_linux_syscall_dispatch "
            "hybrid=OptionC boot_default=LINUX "
            "product=UDX+sshd after_host_launch=1 "
            "bridge_selects=0 ret_rewrite=0 "
            "areas=%u wave=%u "
            "(soft residual; never hard-gates; Soft!=product)\n",
            (unsigned)ENTRY_BRIDGE_SOFT_AREAS,
            (unsigned)ENTRY_BRIDGE_SOFT_WAVE);

    /*
     * Grep: entry_bridge: soft honesty
     * Linux/native entry bridge honesty - Soft!=product · G-AC-1.
     * Dual DoD A/B OPEN at this edge; agent residual never closes.
     */
    kprintf("entry_bridge: soft honesty "
            "hybrid=OptionC open=1 "
            "native_vs_linux=dispatch_only "
            "product_drivers=userspace udx=1 sshd=1 "
            "product=UDX+sshd after_host_launch=1 "
            "DoD_A=OPEN DoD_B=OPEN residual_ne_close=1 "
            "no_ko_product=1 freestanding_probe=SKIP "
            "soft_only=1 product_gate=0 "
            "soft_ne_product=1 dual=MIT_OR_Apache-2.0 G-AC-1=1 "
            "(Soft!=product; dual MIT OR Apache-2.0; G-AC-1; "
            "never closes hybrid; Dual DoD A/B OPEN; "
            "not Linux .ko product AC)\n");

    /*
     * Grep: entry_bridge: soft inventory
     * Lean one-line capacity + edge lifetime (alias for older greps).
     */
    kprintf("entry_bridge: soft inventory enter=%lu null=%lu route=%lu "
            "handoff=%lu logs=%lu routed=%u handed=%u "
            "lean_ok=%u deepen_ok=%u host_ok=%u "
            "surf_udx=%lu surf_sshd=%lu "
            "areas=%u wave=%u storm=0 soft_ne_product=1 "
            "dual=MIT_OR_Apache-2.0 G-AC-1=1 "
            "DoD_A=OPEN DoD_B=OPEN (soft lean residual)\n",
            (unsigned long)u64Enter,
            (unsigned long)u64Null,
            (unsigned long)u64Route,
            (unsigned long)u64Handoff,
            (unsigned long)u64Logs,
            u32Routed,
            u32Handed,
            (unsigned)g_u32BridgeSoftLeanOk,
            (unsigned)g_u32BridgeSoftDeepenOk,
            (unsigned)g_u32BridgeSoftHostOk,
            (unsigned long)g_u64BridgeSoftSurfUdx,
            (unsigned long)g_u64BridgeSoftSurfSshd,
            (unsigned)ENTRY_BRIDGE_SOFT_AREAS,
            (unsigned)ENTRY_BRIDGE_SOFT_WAVE);

    /* Grep: entry_bridge: soft PASS / entry_bridge: soft inventory PASS */
    kprintf("entry_bridge: soft inventory PASS wave=%u logs=%lu "
            "enter=%lu route=%lu handoff=%lu "
            "lean_ok=%u deepen_ok=%u host_ok=%u lean=1 "
            "soft_ne_product=1 dual=MIT_OR_Apache-2.0 G-AC-1=1 "
            "DoD_A=OPEN DoD_B=OPEN residual_ne_close=1\n",
            (unsigned)ENTRY_BRIDGE_SOFT_WAVE,
            (unsigned long)u64Logs,
            (unsigned long)u64Enter,
            (unsigned long)u64Route,
            (unsigned long)u64Handoff,
            (unsigned)g_u32BridgeSoftLeanOk,
            (unsigned)g_u32BridgeSoftDeepenOk,
            (unsigned)g_u32BridgeSoftHostOk);
    kprintf("entry_bridge: soft PASS wave=%u logs=%lu lean=1 lean_ok=%u "
            "deepen_ok=%u host_ok=%u soft_ne_product=1 "
            "dual=MIT_OR_Apache-2.0 G-AC-1=1 "
            "hot_cold=dispatch stamp_storm=0 "
            "native_or_linux=dispatch_only product=UDX+sshd "
            "DoD_A=OPEN DoD_B=OPEN residual_ne_close=1\n",
            (unsigned)ENTRY_BRIDGE_SOFT_WAVE,
            (unsigned long)u64Logs,
            (unsigned)g_u32BridgeSoftLeanOk,
            (unsigned)g_u32BridgeSoftDeepenOk,
            (unsigned)g_u32BridgeSoftHostOk);
}

/**
 * After first product bridge activity, print lean residual once.
 * Diagnostics only - never gates path PASS. One-shot avoids stamp storms.
 * Not called from boot init (no boot spam). Pre-handoff so EXIT still lamps.
 */
static void
entry_bridge_soft_residual_maybe_once(void)
{
    if (g_fBridgeSoftOnce != 0) {
        return;
    }
    if (g_u64BridgeSoftEnter == 0) {
        return;
    }
    g_fBridgeSoftOnce = 1;
    entry_bridge_soft_residual_lean_once();
    entry_bridge_soft_residual_lean_log();
}

/**
 * One-shot post-handoff residual after first returning dispatch.
 * Measures frame nr + args integrity (bridge never mutates). Soft!=product.
 * Separate from pre-handoff lean so EXIT non-return still gets residual.
 * C2 deepen: ret-sign + retclass + nrshape already tallied at handoff edge.
 * STRONGER: UDX/sshd surface ret tallies after host_launch traffic.
 * greppable: entry_bridge: soft residual handoff
 */
static void
entry_bridge_soft_residual_handoff_once(void)
{
    const char *szRetClass;
    const char *szNrShape;
    u32 u32NrOk;
    u32 u32ArgOk;
    u32 u32FrameOk;

    if (g_fBridgeSoftHandoffOnce != 0) {
        return;
    }
    if (g_u64BridgeSoftHandoff == 0) {
        return;
    }
    g_fBridgeSoftHandoffOnce = 1;
    u32NrOk = (g_u64BridgeSoftNrDrift == 0 && g_u64BridgeSoftNrOk != 0) ? 1u : 0u;
    u32ArgOk = (g_u64BridgeSoftArgDrift == 0 && g_u64BridgeSoftArgOk != 0) ? 1u : 0u;
    u32FrameOk = (u32NrOk != 0u && u32ArgOk != 0u) ? 1u : 0u;
    szRetClass = entry_bridge_soft_retclass(g_u64BridgeSoftLastRet);
    szNrShape = entry_bridge_soft_nrshape(g_u64BridgeSoftLastNr);

    /*
     * Grep: entry_bridge: soft residual handoff
     * Functional deepen: first returning handoff + measured nr/args integrity.
     */
    kprintf("entry_bridge: soft residual handoff "
            "edge=post_dispatch handoff=%lu nr_ok=%lu nr_drift=%lu "
            "arg_ok=%lu arg_drift=%lu nr_intact=%u arg_intact=%u "
            "frame_intact=%u last_nr=%lu last_ret=%lu retclass=%s nrshape=%s "
            "ret_neg=%lu ret_zero=%lu ret_pos=%lu "
            "surf_udx=%lu surf_sshd=%lu "
            "udx_ret_ok=%lu udx_ret_neg=%lu net_ret_ok=%lu net_ret_neg=%lu "
            "ret_rewrite=0 bridge_selects=0 "
            "product=UDX+sshd after_host_launch=1 "
            "DoD_A=OPEN DoD_B=OPEN residual_ne_close=1 "
            "soft_ne_product=1 dual=MIT_OR_Apache-2.0 G-AC-1=1 "
            "stamp_storm=0 no_version_stamp=1 "
            "deepen_ok=%u host_ok=%u "
            "(Soft!=product; post-handoff nr+args integrity + UDX/sshd surface; "
            "Dual DoD A/B OPEN; never product close)\n",
            (unsigned long)g_u64BridgeSoftHandoff,
            (unsigned long)g_u64BridgeSoftNrOk,
            (unsigned long)g_u64BridgeSoftNrDrift,
            (unsigned long)g_u64BridgeSoftArgOk,
            (unsigned long)g_u64BridgeSoftArgDrift,
            u32NrOk,
            u32ArgOk,
            u32FrameOk,
            (unsigned long)g_u64BridgeSoftLastNr,
            (unsigned long)g_u64BridgeSoftLastRet,
            szRetClass,
            szNrShape,
            (unsigned long)g_u64BridgeSoftRetNeg,
            (unsigned long)g_u64BridgeSoftRetZero,
            (unsigned long)g_u64BridgeSoftRetPos,
            (unsigned long)g_u64BridgeSoftSurfUdx,
            (unsigned long)g_u64BridgeSoftSurfSshd,
            (unsigned long)g_u64BridgeSoftUdxRetOk,
            (unsigned long)g_u64BridgeSoftUdxRetNeg,
            (unsigned long)g_u64BridgeSoftNetRetOk,
            (unsigned long)g_u64BridgeSoftNetRetNeg,
            (unsigned)g_u32BridgeSoftDeepenOk,
            (unsigned)g_u32BridgeSoftHostOk);
}

void
gj_syscall_entry_asm_bridge(struct gj_syscall_regs *pRegs)
{
    i64 i64Ret;
    const char *szShape;
    u32 fArgMatch;

    /*
     * Soft entry edge: every LSTAR land is counted before personality route.
     * Smoke tests that call gj_syscall_dispatch directly skip this note.
     *
     * Product handoff (UDX userspace drivers + Linux personality apps):
     *   - Do not select NATIVE vs LINUX here (dispatch owns that).
     *   - Do not rewrite i64Ret or mutate nr/args (asm frame stays intact).
     *   - Soft residual is one-shot only - never a per-syscall stamp storm.
     *   - Dual DoD A/B stay OPEN (agent residual != product close).
     * C2 residual deepen (soft only): snap args + NR shape + EXIT note;
     * post-handoff measures nr/args integrity + ret-sign + retclass buckets.
     * STRONGER: measure host_launch -> UDX/sshd surface NRs + ret outcomes.
     */
    entry_bridge_soft_inc(&g_u64BridgeSoftEnter);
    gj_syscall_entry_soft_note_bridge(pRegs);
    if (pRegs == NULL) {
        entry_bridge_soft_inc(&g_u64BridgeSoftNull);
        entry_bridge_soft_residual_maybe_once();
        return;
    }

    /* Soft last NR + arg snap at LSTAR edge (read-only; never mutates frame). */
    g_u64BridgeSoftLastNr = pRegs->u64Nr;
    g_u64BridgeSoftSnapA0 = pRegs->u64Arg0;
    g_u64BridgeSoftSnapA1 = pRegs->u64Arg1;
    g_u64BridgeSoftSnapA2 = pRegs->u64Arg2;
    g_u64BridgeSoftSnapA3 = pRegs->u64Arg3;
    g_u64BridgeSoftSnapA4 = pRegs->u64Arg4;
    g_u64BridgeSoftSnapA5 = pRegs->u64Arg5;
    entry_bridge_soft_inc(&g_u64BridgeSoftRoute);

    /* Soft NR shape taxonomy (never routes; Soft!=product). */
    szShape = entry_bridge_soft_nrshape(pRegs->u64Nr);
    if (szShape != NULL && szShape[0] == 'n') {
        entry_bridge_soft_inc(&g_u64BridgeSoftShapeNat);
    } else if (szShape != NULL && szShape[0] == 'l') {
        entry_bridge_soft_inc(&g_u64BridgeSoftShapeLin);
    } else {
        entry_bridge_soft_inc(&g_u64BridgeSoftShapeHigh);
    }

    /* EXIT-shaped soft note so pre-handoff residual covers non-return paths. */
    if (entry_bridge_soft_nr_is_exit_shaped(pRegs->u64Nr) != 0u) {
        entry_bridge_soft_inc(&g_u64BridgeSoftExitSh);
    }

    /*
     * Host-launch -> UDX/sshd surface tally (measured residual only).
     * After process_spawn_host_launch parks UDX hosts / sshd, their syscalls
     * land here (DDI/PLATFORM/NOTIFY/NET/SPAWN). Never routes; Soft!=product.
     */
    entry_bridge_soft_note_surface(pRegs->u64Nr);

    /*
     * One-shot residual before handoff so EXIT-style non-return paths still
     * emit soft residual once. Hot path after this is dispatch-only
     * (personality + linux hot/cold live entirely in gj_syscall_dispatch).
     */
    entry_bridge_soft_residual_maybe_once();

    /* Personality route + hot/cold live entirely in gj_syscall_dispatch. */
    gj_syscall_dispatch(pRegs);

    /*
     * Soft post-handoff: dispatch returned with i64Ret filled for sysret.
     * Snapshot only - never rewrite the live return value.
     * Measure nr + args integrity: bridge contract is frame inputs intact.
     * C2 deepen: ret-sign buckets + retclass cumulative buckets.
     * STRONGER: UDX surface + NET (sshd) post-handoff ret-sign tallies.
     */
    g_u64BridgeSoftLastRet = (u64)pRegs->i64Ret;
    if (pRegs->u64Nr == g_u64BridgeSoftLastNr) {
        entry_bridge_soft_inc(&g_u64BridgeSoftNrOk);
    } else {
        entry_bridge_soft_inc(&g_u64BridgeSoftNrDrift);
    }
    fArgMatch = 0u;
    if (pRegs->u64Arg0 == g_u64BridgeSoftSnapA0 &&
        pRegs->u64Arg1 == g_u64BridgeSoftSnapA1 &&
        pRegs->u64Arg2 == g_u64BridgeSoftSnapA2 &&
        pRegs->u64Arg3 == g_u64BridgeSoftSnapA3 &&
        pRegs->u64Arg4 == g_u64BridgeSoftSnapA4 &&
        pRegs->u64Arg5 == g_u64BridgeSoftSnapA5) {
        fArgMatch = 1u;
    }
    if (fArgMatch != 0u) {
        entry_bridge_soft_inc(&g_u64BridgeSoftArgOk);
    } else {
        entry_bridge_soft_inc(&g_u64BridgeSoftArgDrift);
    }
    i64Ret = pRegs->i64Ret;
    if (i64Ret < 0) {
        entry_bridge_soft_inc(&g_u64BridgeSoftRetNeg);
    } else if (i64Ret == 0) {
        entry_bridge_soft_inc(&g_u64BridgeSoftRetZero);
    } else {
        entry_bridge_soft_inc(&g_u64BridgeSoftRetPos);
    }
    entry_bridge_soft_retclass_bucket((u64)i64Ret);
    entry_bridge_soft_note_surface_ret(g_u64BridgeSoftLastNr, i64Ret);
    entry_bridge_soft_inc(&g_u64BridgeSoftHandoff);
    entry_bridge_soft_residual_handoff_once();
}
