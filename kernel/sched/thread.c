/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Cooperative threads + soft QoS pick. See thread.h for dual-stack layout and
 * residual-#UD invariants (TSS.RSP0 dedicated IRQ stack; per-thr SYSCALL
 * USER_* save/restore across schedule).
 *
 * Soft product deepen (Wave 35 exclusive; this unit only):
 *   - QoS classes 0..4 + capped soft boost (Apple §8 spirit)
 *   - pick_next soft stats + equal-rank wait-age fairness
 *   - kstack base+mid canary + poison HWM soft scan
 *   - soft sched inventory: ready/run snap + HWM + transition counts
 *   - path tallies: create/block/wake/yield/switch/exit + aff/proc
 *   - Wave 19 greppable "thread: soft ..." deepen (wave=126 stamp):
 *       inventory|table|ready|run|create|block|wake|yield|switch|
 *       path|qos|canary|aff|pick|stack|idle|caps|stats|exit|deepen|
 *       hwm|sysuser|user|boost|exec|invariant|eth|thr_exit|thr_exit_lean|
 *       return|ret_surface|ratio|surface|headroom|tag|tag_lean
 *     twin "sched: soft ..." retained for legacy smoke greps
 *   Soft only - does NOT claim product RR / full preemption complete.
 *   Never hard-gates pick_next; diagnostics only (wrap OK).
 *   Soft!=product. No stamp storms: multi-line inventory dumps capped
 *   (THREAD_SOFT_LOG_CAP). H1 residual lean (G-AC-1):
 *   net_eth_poll only on scheduler_run thr stack - never timer/APIC/IRQ
 *   (IRQ smash / #PF I=1). AP run loop does not poll.
 *   freestanding class SKIP; product UDX; poll ownership still thr-stack
 *   for virtio/UDX later (H1 stands regardless of class-driver SKIP).
 *   greppable: net_eth_poll=run_loop_only * net_eth_irq=0
 *              owner=scheduler_run * freestanding_class=SKIP * product=UDX
 *              poll_own=thr_stack * virtio_udx_later=1
 *   H3 residual lean (Soft!=product; UDX host teardown companion):
 *   thread_exit_process drains non-current siblings BEFORE as_destroy
 *   (ASSURANCE H3 | thr_exit_before_as_destroy=1). Skip user if !alive:
 *     - trampoline refuse-enter on dead AS (null_proc / alive0 / cr3_0)
 *     - schedule resume refuse sysuser + as_activate when !alive/cr3_0
 *       (mid-syscall belt; kernel death thr without userish state stays)
 *   UDX multi-thr hosts share one process AS - sibling IRQ/work thr must
 *   EXIT before maps free. Soft residual != product multi-CPU thr kill /
 *   full UDX host product close.
 *   thr_exit residual deepen (UDX host teardown eng):
 *     - scrub-class tallies (state/user/sysuser/cr3/block/DRIVER)
 *     - multi-thr host class (n>=2) + null/zero-sibling barrier honesty
 *     - dead-AS refuse class (null_proc / alive0 / cr3_0 / resume)
 *     - thr_exit detaches pProc + zeros sysuser save fields (skip-user belt)
 *     - residual lean once-lamp (inventory-capped; no hot-path storm)
 *   greppable: thr_exit_before_as_destroy=1 * H3=death_residual
 *              thread: exit_process * sched: exit_process
 *              thr skip user dead AS * thr skip user resume dead AS
 *              thread: soft thr_exit * thread: soft thr_exit residual lean
 *              udx_host_teardown=1 * soft_ne_product=1
 *              hosts=rtl8168_udx|xhci_udx|ddi_host
 *   Preserve thread_exit_process (H3 death residual). Soft!=product.
 *   C3 thr soft tag residual deepen (Soft!=product; panel/no-COM1):
 *     Named-class tags pe32_wow|sshd|init|ring3 (+pe32_u32|idle) for
 *     STATUS/kill-panel residual honesty - NOT product identity / AC.
 *     Set tallies + live inventory snaps; tags sticky until slot reuse
 *     (EXITED keep last tag for kill-panel residual). H3 thr_exit order
 *     stands (siblings before as_destroy; no tag scrub order change).
 *   STRONGER thr soft tags H3 residual (Soft!=product):
 *     On exit_process scrub: if tag empty, re-stamp from entry VA class
 *     (pe32_wow @ 0x52000000) BEFORE clearing USER_RIP so kill panel /
 *     sticky EXITED inventory keep named class. Tag never scrubbed on
 *     thr_exit (sticky_until_reuse). H3 order stands.
 *   greppable: thread: soft tag * thread: soft tag residual lean
 *              pe32_wow|sshd|init|ring3 * soft_ne_product=1
 *              thr_exit_before_as_destroy=1 * H3=death_residual
 *              sticky_until_reuse=1 * thr soft tags H3
 *   STRONGER functional residual (W10 Dual DoD; stamp-free bar v2026.08.04.75):
 *     thread: soft residual lean udx - H1 thr-stack poll + H3 thr_exit +
 *     dual_dod OPEN honesty for live UDX hosts (rtl8168_udx|xhci_udx|ddi_host).
 *     Soft!=product; Dual DoD A/B remain OPEN; no version stamp.
 */
#include <gj/apic.h>
#include <gj/cpu.h>
#include <gj/cpu_sys.h>
#include <gj/door.h>
#include <gj/error.h>
#include <gj/futex.h>
#include <gj/gdt.h>
#include <gj/klog.h>
#include <gj/memobj.h>
#include <gj/platform_power.h>
#include <gj/net_eth.h>
#include <gj/process.h>
#include <gj/session_input.h>
#include <gj/smp.h>
#include <gj/string.h>
#include <gj/thread.h>
#include <gj/timer.h>
#include <gj/user_access.h>
#include <gj/vmm.h>

extern void switch_context(u64 *pOldRsp, u64 u64NewRsp);

static struct gj_thread g_aThreads[GJ_MAX_THREADS];
static struct gj_thread *g_pIdle;
static struct gj_thread *g_apIdle[GJ_CPU_STATIC_MAX]; /* per-CPU idle (0 = BSP) */
static u32 g_u32NextId = 1;
/* Soft thr tags for STATUS/no-COM1 kill panel (slot-parallel; Soft!=product). */
static char g_aThrSoftTag[GJ_MAX_THREADS][12];
/* External affinity/QoS - keep out of struct to preserve layout stability. */
static u8 g_aThrCpu[GJ_MAX_THREADS]; /* 0=BSP default; 0xFF=any */
static u8 g_aThrQos[GJ_MAX_THREADS]; /* GJ_QOS_* base class */
/* Soft PI residual boost ticks (capped); decayed on schedule leave. */
static u8 g_aThrBoost[GJ_MAX_THREADS];
/* Soft wait-age: last pick gen for equal-rank fairness. */
static u32 g_aThrLastPick[GJ_MAX_THREADS];
static u32 g_u32PickGen;

/* Soft preemption flag (BSP idle path; not per-CPU). */
static volatile int g_fYieldReq;

/* Soft product counters (pick / QoS / canary). */
static struct gj_sched_soft_stats g_soft;
static int g_fSoftStatsOnce; /* one-shot soft dump after warm picks */

/* Wave 35 exclusive soft deepen stamp (greppable wave=126). */
#define THREAD_SOFT_DEEPEN_WAVE 126u
/* Fixed greppable categories emitted under "thread: soft ...". */
#define THREAD_SOFT_DEEPEN_AREAS 198u
/*
 * Cap full multi-line soft inventory dumps (Soft!=product; no stamp storms).
 * Init + one warm pick dump + a couple of stats_print paths stay greppable;
 * further calls refresh snaps silently (scan only).
 */
#define THREAD_SOFT_LOG_CAP 4u

/*
 * Soft sched inventory (Wave 19; file-local; ready = RUNNABLE, run = RUNNING).
 * Snapshots from table walk; HWM of live ready/run; transition + path tallies.
 * Diagnostics only - never hard-gates pick_next.
 * greppable: sched: soft ... / thread: soft ...
 * Multi-line kprintf dumps capped at THREAD_SOFT_LOG_CAP (no stamp storms).
 */
static u32 g_u32SoftReadySnap;   /* last RUNNABLE count */
static u32 g_u32SoftRunSnap;     /* last RUNNING count */
static u32 g_u32SoftBlockedSnap; /* last BLOCKED count */
static u32 g_u32SoftExitedSnap;  /* last EXITED count */
static u32 g_u32SoftUnusedSnap;  /* last UNUSED slots */
static u32 g_u32SoftLiveSnap;    /* non-UNUSED slots */
static u32 g_u32SoftUserSnap;    /* live thr with USER_* entry flags */
static u32 g_u32SoftUser64Snap;  /* live thr with USER_ENTRY only path */
static u32 g_u32SoftUser32Snap;  /* live thr with USER32_ENTRY path */
static u32 g_u32SoftBoostSnap;   /* thr with residual soft boost */
static u32 g_u32SoftBoostTicks;  /* sum residual boost ticks (snap) */
static u32 g_u32SoftAffAnySnap;  /* live thr affinity=any (0xFF) */
static u32 g_u32SoftAffPinSnap;  /* live thr pinned (not any) */
static u32 g_u32SoftProcSnap;    /* live thr with pProc bound */
static u32 g_u32SoftSysUserSnap; /* live thr with mid-syscall USER_* valid */
static u32 g_u32SoftBlockObjSnap;/* BLOCKED thr with non-null pBlockObj */
static u32 g_u32SoftIdleSnap;    /* planted AP/BSP idle thr count */
static u32 g_u32SoftCr3Snap;     /* live thr with non-zero u64Cr3 */
static u32 g_u32SoftReadyHwm;    /* max ready seen */
static u32 g_u32SoftRunHwm;      /* max run seen */
static u32 g_u32SoftLiveHwm;     /* max live seen */
static u32 g_u32SoftBlockedHwm;  /* max blocked seen */
static u32 g_u32SoftExitedHwm;   /* max exited seen */
static u32 g_u32SoftSysUserHwm;  /* max mid-syscall USER_* live */
static u64 g_u64SoftInvSamples;  /* inventory walk count */
static u64 g_u64SoftReadyTrans;  /* thr entered RUNNABLE */
static u64 g_u64SoftRunTrans;    /* thr entered RUNNING */
static u64 g_u64SoftBlockTrans;  /* thr entered BLOCKED */
static u64 g_u64SoftExitTrans;   /* thr entered EXITED */
static u32 g_aSoftReadyQos[5];   /* ready thr by base QoS class 0..4 */
static u32 g_aSoftRunQos[5];     /* run thr by base QoS class 0..4 */
static int g_fSoftInvOnce;       /* one-shot warm inventory dump */

/* Soft path tallies (cumulative; wrap OK). Wave 15 deepen surfaces. */
static u64 g_u64SoftCreateOk;      /* thread_create success */
static u64 g_u64SoftCreateFull;    /* table full / no slot */
static u64 g_u64SoftCreateUser;    /* thread_create_user success */
static u64 g_u64SoftCreateUser32;  /* thread_create_user32 success */
static u64 g_u64SoftCreateReuse;   /* EXITED slot recycled */
static u64 g_u64SoftCreateApIdle;  /* AP idle thr plant */
static u64 g_u64SoftCreateOnCpu;   /* thread_create_on_cpu success */
static u64 g_u64SoftBlockN;        /* thread_block entries */
static u64 g_u64SoftWakeCalls;     /* thread_wake entries */
static u64 g_u64SoftWakeThr;       /* thr transitioned BLOCKED->RUNNABLE */
static u64 g_u64SoftWakeNone;      /* wake scanned, zero thr transitioned */
static u64 g_u64SoftYieldN;        /* thread_yield entries */
static u64 g_u64SoftYieldReq;      /* thread_yield_request */
static u64 g_u64SoftYieldPendHit;  /* yield_pending returned true */
static u64 g_u64SoftSchedEnter;    /* schedule() entries */
static u64 g_u64SoftSchedSelf;     /* same-thr early return */
static u64 g_u64SoftSchedSwitch;   /* real switch_context */
static u64 g_u64SoftSchedSpin;     /* blocked self-spin path used */
static u64 g_u64SoftSchedAsAct;    /* process_as_activate on resume */
static u64 g_u64SoftSchedCr3Own;   /* thr u64Cr3 load on resume */
static u64 g_u64SoftExitN;         /* thread_exit entries */
static u64 g_u64SoftSetCpuN;       /* thread_set_cpu hits */
static u64 g_u64SoftExecReplace;   /* thr updated by thread_exec_replace */
static u64 g_u64SoftExecCalls;     /* thread_exec_replace entries */
static u64 g_u64SoftSysUserSave;   /* thread_save_sys_user hits */
static u64 g_u64SoftSysUserRest;   /* thread_restore_sys_user hits */
static u64 g_u64SoftInstallKstk;   /* thread_install_kstack thr path */
/*
 * H1 residual lean: thr-stack net_eth_poll entries from scheduler_run only
 * (silent; wrap OK). Never from IRQ/timer/APIC/AP run loop.
 * freestanding class SKIP; product UDX; thr-stack ownership for virtio/UDX later.
 */
static u64 g_u64SoftEthPollN;
/*
 * H3 residual lean deepen (Soft!=product; UDX host teardown companion):
 * silent tallies (wrap OK). No kprintf on hot schedule path (resume skip
 * kprintf only when userish refuse fires - rare death residual).
 *   exit_process_calls - thread_exit_process entries (incl. n=0)
 *   exit_process_n     - siblings marked EXITED by exit_process
 *   exit_null          - pProc NULL early return
 *   exit_zero          - call with n=0 (idempotent / pre-as_destroy barrier)
 *   exit_multi         - call with n>=2 (multi-thr UDX host class)
 *   exit_detach        - sibling pProc detached (skip-user residual)
 *   state class        - was RUNNABLE / RUNNING / BLOCKED at scrub
 *   scrub class        - USER_ENTRY / USER32 / sysuser / thr CR3 / block key
 *   exit_driver        - DRIVER QoS sibling drained (soft-RT UDX/audio class)
 *   dead_as_skip       - refuse-enter / refuse-resume dead AS (total)
 *   dead_as_null/alive0/cr3_0 - refuse class split
 *   dead_as_resume     - schedule mid-syscall resume refuse (!alive/cr3_0)
 * greppable: thr_exit_before_as_destroy=1 * udx_host_teardown=1
 * greppable: thread: soft thr_exit * thread: soft thr_exit residual lean
 * greppable: thr skip user resume dead AS * hosts=rtl8168_udx|xhci_udx|ddi_host
 * greppable: soft_ne_product=1
 */
static u64 g_u64SoftThrExitProcCalls;
static u64 g_u64SoftThrExitProcN;
static u64 g_u64SoftThrExitNull;       /* pProc NULL early */
static u64 g_u64SoftThrExitZero;       /* n=0 barrier / idempotent */
static u64 g_u64SoftThrExitMulti;      /* n>=2 multi-thr host class */
static u64 g_u64SoftThrExitDetach;     /* sibling pProc nulled on scrub */
static u64 g_u64SoftThrExitRunnable;   /* scrub was RUNNABLE */
static u64 g_u64SoftThrExitRunning;    /* scrub was RUNNING (soft race belt) */
static u64 g_u64SoftThrExitBlocked;    /* scrub was BLOCKED */
static u64 g_u64SoftThrExitUser;       /* cleared USER_ENTRY */
static u64 g_u64SoftThrExitUser32;     /* cleared USER32_ENTRY */
static u64 g_u64SoftThrExitSysUser;    /* cleared mid-syscall USER_* valid */
static u64 g_u64SoftThrExitCr3;        /* cleared non-zero thr CR3 */
static u64 g_u64SoftThrExitBlockObj;   /* cleared non-null block key */
static u64 g_u64SoftThrExitDriver;     /* DRIVER QoS sibling drained */
static u64 g_u64SoftDeadAsSkip;
static u64 g_u64SoftDeadAsSkipNull;    /* pProc NULL refuse */
static u64 g_u64SoftDeadAsSkipAlive;   /* u32Alive==0 refuse */
static u64 g_u64SoftDeadAsSkipCr3;     /* proc cr3==0 refuse */
static u64 g_u64SoftDeadAsSkipResume;  /* schedule resume refuse-enter */
static u8  g_fThrExitResLeanOnce;      /* residual lean once-lamp fired */
static u8  g_fUdxHostResLeanOnce;      /* W10 live UDX host residual once */
/*
 * C3 thr soft tag residual (Soft!=product; panel/no-COM1 kill residual).
 * Named-class tags for pe32_wow|sshd|init|ring3 (+pe32_u32|idle).
 * Snaps from inventory walk (live = non-UNUSED; EXITED sticky until reuse).
 * Set tallies from thread_soft_tag_set (wrap OK). Not product identity.
 * STRONGER H3: exit_process re-stamp empty tags from entry VA before scrub;
 * sticky_keep tallies on sibling drain. Tags never cleared on thr_exit.
 * greppable: thread: soft tag * thread: soft tag residual lean
 *            pe32_wow|sshd|init|ring3 * soft_ne_product=1
 * H3 thr_exit order stands (no as_destroy reorder; tags sticky on EXITED).
 */
static u32 g_u32SoftTagPe32WowSnap;  /* live thr tag=pe32_wow */
static u32 g_u32SoftTagSshdSnap;     /* live thr tag=sshd */
static u32 g_u32SoftTagInitSnap;     /* live thr tag=init */
static u32 g_u32SoftTagRing3Snap;    /* live thr tag=ring3 */
static u32 g_u32SoftTagPe32U32Snap;  /* live thr tag=pe32_u32 */
static u32 g_u32SoftTagIdleSnap;     /* live thr tag=idle */
static u32 g_u32SoftTagOtherSnap;    /* live thr non-empty other tag */
static u32 g_u32SoftTagEmptySnap;    /* live thr empty tag */
static u32 g_u32SoftTagNamedSnap;    /* live thr pe32_wow|sshd|init|ring3 */
static u64 g_u64SoftTagSetN;         /* thread_soft_tag_set hits */
static u64 g_u64SoftTagSetPe32Wow;   /* set pe32_wow */
static u64 g_u64SoftTagSetSshd;      /* set sshd */
static u64 g_u64SoftTagSetInit;      /* set init */
static u64 g_u64SoftTagSetRing3;     /* set ring3 */
static u64 g_u64SoftTagSetPe32U32;   /* set pe32_u32 */
static u64 g_u64SoftTagSetIdle;      /* set idle */
static u64 g_u64SoftTagSetOther;     /* set other short name */
static u64 g_u64SoftTagH3StickyKeep; /* exit_process kept non-empty sticky tag */
static u64 g_u64SoftTagH3Restamp;    /* exit_process re-stamp from entry VA */
static u64 g_u64SoftTagH3Pe32Wow;    /* exit_process drained pe32_wow class */
static u8  g_fSoftTagResLeanOnce;    /* tag residual lean once-lamp */
static u32 g_u32SoftLogN;          /* inventory print emissions */

static void thread_trampoline(void);
static void sched_soft_inventory_scan(void);
static void sched_soft_inventory_print(void);
static void sched_thr_net_eth_poll(void);

static u32
thr_index(struct gj_thread *pThr)
{
    if (pThr == NULL) {
        return GJ_MAX_THREADS;
    }
    if (pThr < &g_aThreads[0] || pThr >= &g_aThreads[GJ_MAX_THREADS]) {
        return GJ_MAX_THREADS;
    }
    return (u32)(pThr - &g_aThreads[0]);
}

/* Soft: count thr becoming RUNNABLE (ready queue inventory deepen). */
static void
sched_soft_note_ready(void)
{
    g_u64SoftReadyTrans++;
}

/* Soft: count thr becoming RUNNING (on-CPU inventory deepen). */
static void
sched_soft_note_run(void)
{
    g_u64SoftRunTrans++;
}

/* Soft: count thr becoming BLOCKED (path deepen). */
static void
sched_soft_note_block(void)
{
    g_u64SoftBlockTrans++;
}

/* Soft: count thr becoming EXITED (path deepen). */
static void
sched_soft_note_exit(void)
{
    g_u64SoftExitTrans++;
}

/*
 * Soft tag name eq (short fixed tags; Soft!=product; no libc path required).
 * Returns 1 if equal NUL-terminated strings within soft tag capacity.
 */
static int
thr_soft_tag_eq(const char *szA, const char *szB)
{
    u32 i;

    if (szA == NULL || szB == NULL) {
        return 0;
    }
    for (i = 0; i < 12u; i++) {
        if (szA[i] != szB[i]) {
            return 0;
        }
        if (szA[i] == '\0') {
            return 1;
        }
    }
    return 1;
}

/*
 * Classify set of a soft tag into named residual counters (wrap OK).
 * Panel-critical: pe32_wow|sshd|init|ring3. Soft!=product.
 */
static void
thr_soft_tag_note_set(const char *szTag)
{
    g_u64SoftTagSetN++;
    if (szTag == NULL || szTag[0] == '\0') {
        g_u64SoftTagSetOther++;
        return;
    }
    if (thr_soft_tag_eq(szTag, "pe32_wow") != 0) {
        g_u64SoftTagSetPe32Wow++;
    } else if (thr_soft_tag_eq(szTag, "sshd") != 0) {
        g_u64SoftTagSetSshd++;
    } else if (thr_soft_tag_eq(szTag, "init") != 0) {
        g_u64SoftTagSetInit++;
    } else if (thr_soft_tag_eq(szTag, "ring3") != 0) {
        g_u64SoftTagSetRing3++;
    } else if (thr_soft_tag_eq(szTag, "pe32_u32") != 0) {
        g_u64SoftTagSetPe32U32++;
    } else if (thr_soft_tag_eq(szTag, "idle") != 0) {
        g_u64SoftTagSetIdle++;
    } else {
        g_u64SoftTagSetOther++;
    }
}

/*
 * Walk fixed thr table; refresh ready/run/blocked/... snaps + HWM.
 * Pure read of thr state / QoS / boost / aff / proc / soft tags;
 * safe after thread_init.
 * Wave 15: user64/user32 split, block-obj snap, boost ticks, exited/sysuser HWM.
 * C3 soft tag residual: named-class snaps pe32_wow|sshd|init|ring3.
 */
static void
sched_soft_inventory_scan(void)
{
    u32 iThr;
    u32 cReady = 0;
    u32 cRun = 0;
    u32 cBlocked = 0;
    u32 cExited = 0;
    u32 cUnused = 0;
    u32 cLive = 0;
    u32 cUser = 0;
    u32 cUser64 = 0;
    u32 cUser32 = 0;
    u32 cBoost = 0;
    u32 cBoostTicks = 0;
    u32 cAffAny = 0;
    u32 cAffPin = 0;
    u32 cProc = 0;
    u32 cSysUser = 0;
    u32 cBlockObj = 0;
    u32 cIdle = 0;
    u32 cCr3 = 0;
    u32 cTagPe32Wow = 0;
    u32 cTagSshd = 0;
    u32 cTagInit = 0;
    u32 cTagRing3 = 0;
    u32 cTagPe32U32 = 0;
    u32 cTagIdle = 0;
    u32 cTagOther = 0;
    u32 cTagEmpty = 0;
    u32 cTagNamed = 0;
    u32 aReadyQos[5];
    u32 aRunQos[5];

    for (iThr = 0; iThr < 5u; iThr++) {
        aReadyQos[iThr] = 0;
        aRunQos[iThr] = 0;
    }

    for (iThr = 0; iThr < GJ_MAX_THREADS; iThr++) {
        u32 u32St = g_aThreads[iThr].u32State;
        u8 u8Qos = g_aThrQos[iThr];
        u8 u8Aff = g_aThrCpu[iThr];
        u32 u32Flags = g_aThreads[iThr].u32Flags;
        const char *szTag;

        if (u8Qos > GJ_QOS_CLASS_MAX) {
            u8Qos = GJ_QOS_NORMAL;
        }

        if (u32St == GJ_THR_UNUSED) {
            cUnused++;
            continue;
        }
        cLive++;
        /* Soft tag residual snap (sticky on EXITED until slot reuse). */
        szTag = g_aThrSoftTag[iThr];
        if (szTag[0] == '\0') {
            cTagEmpty++;
        } else if (thr_soft_tag_eq(szTag, "pe32_wow") != 0) {
            cTagPe32Wow++;
            cTagNamed++;
        } else if (thr_soft_tag_eq(szTag, "sshd") != 0) {
            cTagSshd++;
            cTagNamed++;
        } else if (thr_soft_tag_eq(szTag, "init") != 0) {
            cTagInit++;
            cTagNamed++;
        } else if (thr_soft_tag_eq(szTag, "ring3") != 0) {
            cTagRing3++;
            cTagNamed++;
        } else if (thr_soft_tag_eq(szTag, "pe32_u32") != 0) {
            cTagPe32U32++;
        } else if (thr_soft_tag_eq(szTag, "idle") != 0) {
            cTagIdle++;
        } else {
            cTagOther++;
        }
        if ((u32Flags & (GJ_THR_F_USER_ENTRY | GJ_THR_F_USER32_ENTRY)) != 0) {
            cUser++;
        }
        if ((u32Flags & GJ_THR_F_USER32_ENTRY) != 0) {
            cUser32++;
        } else if ((u32Flags & GJ_THR_F_USER_ENTRY) != 0) {
            cUser64++;
        }
        if (g_aThrBoost[iThr] != 0) {
            cBoost++;
            cBoostTicks += (u32)g_aThrBoost[iThr];
        }
        if (u8Aff == 0xFFu) {
            cAffAny++;
        } else {
            cAffPin++;
        }
        if (g_aThreads[iThr].pProc != NULL) {
            cProc++;
        }
        if (g_aThreads[iThr].u32SysUserValid != 0) {
            cSysUser++;
        }
        if (g_aThreads[iThr].u64Cr3 != 0) {
            cCr3++;
        }
        if (u32St == GJ_THR_RUNNABLE) {
            cReady++;
            aReadyQos[u8Qos]++;
        } else if (u32St == GJ_THR_RUNNING) {
            cRun++;
            aRunQos[u8Qos]++;
        } else if (u32St == GJ_THR_BLOCKED) {
            cBlocked++;
            if (g_aThreads[iThr].pBlockObj != NULL) {
                cBlockObj++;
            }
        } else if (u32St == GJ_THR_EXITED) {
            cExited++;
        }
    }

    /* Soft idle plant inventory (BSP + APs; pointer presence only). */
    for (iThr = 0; iThr < GJ_CPU_STATIC_MAX; iThr++) {
        if (g_apIdle[iThr] != NULL) {
            cIdle++;
        }
    }
    if (g_pIdle != NULL && g_apIdle[0] == NULL) {
        /* Bootstrap idle before apIdle[0] wired - count once. */
        cIdle++;
    }

    g_u32SoftReadySnap = cReady;
    g_u32SoftRunSnap = cRun;
    g_u32SoftBlockedSnap = cBlocked;
    g_u32SoftExitedSnap = cExited;
    g_u32SoftUnusedSnap = cUnused;
    g_u32SoftLiveSnap = cLive;
    g_u32SoftUserSnap = cUser;
    g_u32SoftUser64Snap = cUser64;
    g_u32SoftUser32Snap = cUser32;
    g_u32SoftBoostSnap = cBoost;
    g_u32SoftBoostTicks = cBoostTicks;
    g_u32SoftAffAnySnap = cAffAny;
    g_u32SoftAffPinSnap = cAffPin;
    g_u32SoftProcSnap = cProc;
    g_u32SoftSysUserSnap = cSysUser;
    g_u32SoftBlockObjSnap = cBlockObj;
    g_u32SoftIdleSnap = cIdle;
    g_u32SoftCr3Snap = cCr3;
    g_u32SoftTagPe32WowSnap = cTagPe32Wow;
    g_u32SoftTagSshdSnap = cTagSshd;
    g_u32SoftTagInitSnap = cTagInit;
    g_u32SoftTagRing3Snap = cTagRing3;
    g_u32SoftTagPe32U32Snap = cTagPe32U32;
    g_u32SoftTagIdleSnap = cTagIdle;
    g_u32SoftTagOtherSnap = cTagOther;
    g_u32SoftTagEmptySnap = cTagEmpty;
    g_u32SoftTagNamedSnap = cTagNamed;
    for (iThr = 0; iThr < 5u; iThr++) {
        g_aSoftReadyQos[iThr] = aReadyQos[iThr];
        g_aSoftRunQos[iThr] = aRunQos[iThr];
    }
    if (cReady > g_u32SoftReadyHwm) {
        g_u32SoftReadyHwm = cReady;
    }
    if (cRun > g_u32SoftRunHwm) {
        g_u32SoftRunHwm = cRun;
    }
    if (cLive > g_u32SoftLiveHwm) {
        g_u32SoftLiveHwm = cLive;
    }
    if (cBlocked > g_u32SoftBlockedHwm) {
        g_u32SoftBlockedHwm = cBlocked;
    }
    if (cExited > g_u32SoftExitedHwm) {
        g_u32SoftExitedHwm = cExited;
    }
    if (cSysUser > g_u32SoftSysUserHwm) {
        g_u32SoftSysUserHwm = cSysUser;
    }
    g_u64SoftInvSamples++;
}

/*
 * Greppable soft inventory dump (Wave 19 product / smoke).
 * Twin prefixes so either agent grep works:
 *   sched: soft inventory|ready|run|create|block|wake|yield|switch|path|aff ...
 *   thread: soft table|ready|run|inventory|create|block|wake|yield|switch|
 *           path|qos|canary|aff|pick|stack|idle|caps|stats|exit|deepen|
 *           hwm|sysuser|user|boost|exec|invariant|eth|thr_exit|thr_exit_lean|
 *           return|ret_surface|ratio|surface|headroom|tag|tag_lean
 * greppable: sched: soft
 * greppable: thread: soft
 * greppable: thread: soft tag | thread: soft tag residual lean
 * Soft only - never claims product RR / full preemption complete.
 * Soft!=product. Multi-line dumps capped (THREAD_SOFT_LOG_CAP) - no stamp storms.
 * When capped: still scan (HWM snaps stay live); no kprintf multi-line flood.
 */
static void
sched_soft_inventory_print(void)
{
    /*
     * Cap multi-line inventory dumps. Past THREAD_SOFT_LOG_CAP refresh snaps
     * only (silent). Soft!=product; no stamp storms under pick/stats churn.
     */
    if (g_u32SoftLogN >= THREAD_SOFT_LOG_CAP) {
        sched_soft_inventory_scan();
        return;
    }
    g_u32SoftLogN++;
    sched_soft_inventory_scan();

    /* Grep: sched: soft inventory */
    kprintf("sched: soft inventory ready=%u run=%u blocked=%u exited=%u "
            "live=%u unused=%u user=%u boost=%u samples=%lu "
            "slots=%u log_n=%u log_cap=%u wave=%u\n",
            g_u32SoftReadySnap, g_u32SoftRunSnap, g_u32SoftBlockedSnap,
            g_u32SoftExitedSnap, g_u32SoftLiveSnap, g_u32SoftUnusedSnap,
            g_u32SoftUserSnap, g_u32SoftBoostSnap,
            (unsigned long)g_u64SoftInvSamples, GJ_MAX_THREADS,
            g_u32SoftLogN, (unsigned)THREAD_SOFT_LOG_CAP,
            (unsigned)THREAD_SOFT_DEEPEN_WAVE);
    /* Grep: sched: soft ready */
    kprintf("sched: soft ready snap=%u hwm=%u trans=%lu "
            "qos_n=%u qos_i=%u qos_b=%u qos_u=%u qos_d=%u\n",
            g_u32SoftReadySnap, g_u32SoftReadyHwm,
            (unsigned long)g_u64SoftReadyTrans,
            g_aSoftReadyQos[GJ_QOS_NORMAL],
            g_aSoftReadyQos[GJ_QOS_INTERACTIVE],
            g_aSoftReadyQos[GJ_QOS_BACKGROUND],
            g_aSoftReadyQos[GJ_QOS_UTILITY],
            g_aSoftReadyQos[GJ_QOS_DRIVER]);
    /* Grep: sched: soft run */
    kprintf("sched: soft run snap=%u hwm=%u trans=%lu "
            "qos_n=%u qos_i=%u qos_b=%u qos_u=%u qos_d=%u\n",
            g_u32SoftRunSnap, g_u32SoftRunHwm,
            (unsigned long)g_u64SoftRunTrans,
            g_aSoftRunQos[GJ_QOS_NORMAL],
            g_aSoftRunQos[GJ_QOS_INTERACTIVE],
            g_aSoftRunQos[GJ_QOS_BACKGROUND],
            g_aSoftRunQos[GJ_QOS_UTILITY],
            g_aSoftRunQos[GJ_QOS_DRIVER]);
    /* Grep: sched: soft create */
    kprintf("sched: soft create ok=%lu full=%lu user=%lu user32=%lu "
            "reuse=%lu ap_idle=%lu on_cpu=%lu set_cpu=%lu\n",
            (unsigned long)g_u64SoftCreateOk,
            (unsigned long)g_u64SoftCreateFull,
            (unsigned long)g_u64SoftCreateUser,
            (unsigned long)g_u64SoftCreateUser32,
            (unsigned long)g_u64SoftCreateReuse,
            (unsigned long)g_u64SoftCreateApIdle,
            (unsigned long)g_u64SoftCreateOnCpu,
            (unsigned long)g_u64SoftSetCpuN);
    /* Grep: sched: soft block */
    kprintf("sched: soft block n=%lu trans=%lu blocked_snap=%u "
            "blocked_hwm=%u block_obj=%u\n",
            (unsigned long)g_u64SoftBlockN,
            (unsigned long)g_u64SoftBlockTrans, g_u32SoftBlockedSnap,
            g_u32SoftBlockedHwm, g_u32SoftBlockObjSnap);
    /* Grep: sched: soft wake */
    kprintf("sched: soft wake calls=%lu thr=%lu none=%lu ready_trans=%lu\n",
            (unsigned long)g_u64SoftWakeCalls,
            (unsigned long)g_u64SoftWakeThr,
            (unsigned long)g_u64SoftWakeNone,
            (unsigned long)g_u64SoftReadyTrans);
    /* Grep: sched: soft yield */
    kprintf("sched: soft yield n=%lu req=%lu pend_hit=%lu\n",
            (unsigned long)g_u64SoftYieldN,
            (unsigned long)g_u64SoftYieldReq,
            (unsigned long)g_u64SoftYieldPendHit);
    /* Grep: sched: soft switch */
    kprintf("sched: soft switch enter=%lu self=%lu switch=%lu spin=%lu "
            "exit=%lu exit_trans=%lu as_act=%lu cr3_own=%lu\n",
            (unsigned long)g_u64SoftSchedEnter,
            (unsigned long)g_u64SoftSchedSelf,
            (unsigned long)g_u64SoftSchedSwitch,
            (unsigned long)g_u64SoftSchedSpin,
            (unsigned long)g_u64SoftExitN,
            (unsigned long)g_u64SoftExitTrans,
            (unsigned long)g_u64SoftSchedAsAct,
            (unsigned long)g_u64SoftSchedCr3Own);
    /* Grep: sched: soft aff */
    kprintf("sched: soft aff any=%u pin=%u proc=%u sys_user=%u "
            "pick_gen=%u\n",
            g_u32SoftAffAnySnap, g_u32SoftAffPinSnap, g_u32SoftProcSnap,
            g_u32SoftSysUserSnap, g_u32PickGen);
    /* Grep: sched: soft path - coop pick; not product RR/preempt complete */
    kprintf("sched: soft path claim=create+block+wake+yield+switch+pick "
            "qos=0..4 boost_cap=%u coop=1 rr_complete=0 preempt_complete=0 "
            "wave=%u (soft inventory)\n",
            GJ_QOS_BOOST_CAP, (unsigned)THREAD_SOFT_DEEPEN_WAVE);

    /*
     * Wave 19 exclusive "thread: soft ..." deepen surface (prefix-stable).
     * Each area greppable on its own for continuum / smoke.
     */
    /* Grep: thread: soft inventory */
    kprintf("thread: soft inventory ready=%u run=%u blocked=%u exited=%u "
            "live=%u unused=%u user=%u user64=%u user32=%u boost=%u "
            "samples=%lu slots=%u log_n=%u idle=%u cr3=%u block_obj=%u "
            "wave=%u areas=%u\n",
            g_u32SoftReadySnap, g_u32SoftRunSnap, g_u32SoftBlockedSnap,
            g_u32SoftExitedSnap, g_u32SoftLiveSnap, g_u32SoftUnusedSnap,
            g_u32SoftUserSnap, g_u32SoftUser64Snap, g_u32SoftUser32Snap,
            g_u32SoftBoostSnap, (unsigned long)g_u64SoftInvSamples,
            GJ_MAX_THREADS, g_u32SoftLogN, g_u32SoftIdleSnap,
            g_u32SoftCr3Snap, g_u32SoftBlockObjSnap,
            (unsigned)THREAD_SOFT_DEEPEN_WAVE,
            (unsigned)THREAD_SOFT_DEEPEN_AREAS);
    /* Grep: thread: soft table */
    kprintf("thread: soft table live=%u unused=%u exited=%u blocked=%u "
            "live_hwm=%u user=%u boost=%u max=%u ready=%u run=%u "
            "wave=%u\n",
            g_u32SoftLiveSnap, g_u32SoftUnusedSnap, g_u32SoftExitedSnap,
            g_u32SoftBlockedSnap, g_u32SoftLiveHwm, g_u32SoftUserSnap,
            g_u32SoftBoostSnap, GJ_MAX_THREADS, g_u32SoftReadySnap,
            g_u32SoftRunSnap, (unsigned)THREAD_SOFT_DEEPEN_WAVE);
    /* Grep: thread: soft ready */
    kprintf("thread: soft ready snap=%u hwm=%u trans=%lu "
            "qos_n=%u qos_i=%u qos_b=%u qos_u=%u qos_d=%u wave=%u\n",
            g_u32SoftReadySnap, g_u32SoftReadyHwm,
            (unsigned long)g_u64SoftReadyTrans,
            g_aSoftReadyQos[GJ_QOS_NORMAL],
            g_aSoftReadyQos[GJ_QOS_INTERACTIVE],
            g_aSoftReadyQos[GJ_QOS_BACKGROUND],
            g_aSoftReadyQos[GJ_QOS_UTILITY],
            g_aSoftReadyQos[GJ_QOS_DRIVER],
            (unsigned)THREAD_SOFT_DEEPEN_WAVE);
    /* Grep: thread: soft run */
    kprintf("thread: soft run snap=%u hwm=%u trans=%lu "
            "qos_n=%u qos_i=%u qos_b=%u qos_u=%u qos_d=%u wave=%u\n",
            g_u32SoftRunSnap, g_u32SoftRunHwm,
            (unsigned long)g_u64SoftRunTrans,
            g_aSoftRunQos[GJ_QOS_NORMAL],
            g_aSoftRunQos[GJ_QOS_INTERACTIVE],
            g_aSoftRunQos[GJ_QOS_BACKGROUND],
            g_aSoftRunQos[GJ_QOS_UTILITY],
            g_aSoftRunQos[GJ_QOS_DRIVER],
            (unsigned)THREAD_SOFT_DEEPEN_WAVE);
    /* Grep: thread: soft create */
    kprintf("thread: soft create ok=%lu full=%lu user=%lu user32=%lu "
            "reuse=%lu ap_idle=%lu on_cpu=%lu set_cpu=%lu wave=%u\n",
            (unsigned long)g_u64SoftCreateOk,
            (unsigned long)g_u64SoftCreateFull,
            (unsigned long)g_u64SoftCreateUser,
            (unsigned long)g_u64SoftCreateUser32,
            (unsigned long)g_u64SoftCreateReuse,
            (unsigned long)g_u64SoftCreateApIdle,
            (unsigned long)g_u64SoftCreateOnCpu,
            (unsigned long)g_u64SoftSetCpuN,
            (unsigned)THREAD_SOFT_DEEPEN_WAVE);
    /* Grep: thread: soft block */
    kprintf("thread: soft block n=%lu trans=%lu snap=%u hwm=%u "
            "block_obj=%u wave=%u\n",
            (unsigned long)g_u64SoftBlockN,
            (unsigned long)g_u64SoftBlockTrans, g_u32SoftBlockedSnap,
            g_u32SoftBlockedHwm, g_u32SoftBlockObjSnap,
            (unsigned)THREAD_SOFT_DEEPEN_WAVE);
    /* Grep: thread: soft wake */
    kprintf("thread: soft wake calls=%lu thr=%lu none=%lu "
            "ready_trans=%lu wave=%u\n",
            (unsigned long)g_u64SoftWakeCalls,
            (unsigned long)g_u64SoftWakeThr,
            (unsigned long)g_u64SoftWakeNone,
            (unsigned long)g_u64SoftReadyTrans,
            (unsigned)THREAD_SOFT_DEEPEN_WAVE);
    /* Grep: thread: soft yield */
    kprintf("thread: soft yield n=%lu req=%lu pend_hit=%lu "
            "soft_preempt_flag=1 product_preempt_complete=0 wave=%u\n",
            (unsigned long)g_u64SoftYieldN,
            (unsigned long)g_u64SoftYieldReq,
            (unsigned long)g_u64SoftYieldPendHit,
            (unsigned)THREAD_SOFT_DEEPEN_WAVE);
    /* Grep: thread: soft switch */
    kprintf("thread: soft switch enter=%lu self=%lu switch=%lu spin=%lu "
            "exit=%lu exit_trans=%lu as_act=%lu cr3_own=%lu wave=%u\n",
            (unsigned long)g_u64SoftSchedEnter,
            (unsigned long)g_u64SoftSchedSelf,
            (unsigned long)g_u64SoftSchedSwitch,
            (unsigned long)g_u64SoftSchedSpin,
            (unsigned long)g_u64SoftExitN,
            (unsigned long)g_u64SoftExitTrans,
            (unsigned long)g_u64SoftSchedAsAct,
            (unsigned long)g_u64SoftSchedCr3Own,
            (unsigned)THREAD_SOFT_DEEPEN_WAVE);
    /* Grep: thread: soft exit */
    kprintf("thread: soft exit n=%lu trans=%lu exited_snap=%u "
            "exit_proc_calls=%lu exit_proc_n=%lu exit_multi=%lu "
            "dead_as_skip=%lu scrub_user=%lu scrub_cr3=%lu wave=%u\n",
            (unsigned long)g_u64SoftExitN,
            (unsigned long)g_u64SoftExitTrans, g_u32SoftExitedSnap,
            (unsigned long)g_u64SoftThrExitProcCalls,
            (unsigned long)g_u64SoftThrExitProcN,
            (unsigned long)g_u64SoftThrExitMulti,
            (unsigned long)g_u64SoftDeadAsSkip,
            (unsigned long)(g_u64SoftThrExitUser + g_u64SoftThrExitUser32),
            (unsigned long)g_u64SoftThrExitCr3,
            (unsigned)THREAD_SOFT_DEEPEN_WAVE);
    /* Grep: thread: soft pick - soft QoS pick; not product RR complete */
    kprintf("thread: soft pick total=%lu idle=%lu int=%lu norm=%lu "
            "bg=%lu util=%lu drv=%lu aff_skip=%lu eq_fair=%lu self=%lu "
            "gen=%u coop=1 rr_complete=0 wave=%u\n",
            (unsigned long)g_soft.u64PickTotal,
            (unsigned long)g_soft.u64PickIdle,
            (unsigned long)g_soft.u64PickInteractive,
            (unsigned long)g_soft.u64PickNormal,
            (unsigned long)g_soft.u64PickBackground,
            (unsigned long)g_soft.u64PickUtility,
            (unsigned long)g_soft.u64PickDriver,
            (unsigned long)g_soft.u64PickAffSkip,
            (unsigned long)g_soft.u64PickEqualFair,
            (unsigned long)g_soft.u64PickSelf, g_u32PickGen,
            (unsigned)THREAD_SOFT_DEEPEN_WAVE);
    /* Grep: thread: soft qos */
    kprintf("thread: soft qos set=%lu clamp=%lu boost=%lu decay=%lu "
            "boost_live=%u gen=%u cap=%u classes=0..4 wave=%u\n",
            (unsigned long)g_soft.u64QosSet,
            (unsigned long)g_soft.u64QosClamp,
            (unsigned long)g_soft.u64QosBoostSoft,
            (unsigned long)g_soft.u64QosBoostDecay, g_u32SoftBoostSnap,
            g_u32PickGen, GJ_QOS_BOOST_CAP,
            (unsigned)THREAD_SOFT_DEEPEN_WAVE);
    /* Grep: thread: soft canary */
    kprintf("thread: soft canary plant=%lu chk=%lu ok=%lu mid=%lu "
            "fail=%lu hwm_max=%lu hwm_n=%lu wave=%u\n",
            (unsigned long)g_soft.u64CanaryPlant,
            (unsigned long)g_soft.u64CanaryCheck,
            (unsigned long)g_soft.u64CanaryOk,
            (unsigned long)g_soft.u64CanaryMidOk,
            (unsigned long)g_soft.u64CanaryFail,
            (unsigned long)g_soft.u64StackHwmMax,
            (unsigned long)g_soft.u64StackHwmSamples,
            (unsigned)THREAD_SOFT_DEEPEN_WAVE);
    /*
     * Grep: thread: soft stack - dual-stack capacity inventory
     * 32 KiB aKstack stands (8 KiB overflowed door/net). No 32->48 bump without
     * HWM/canary evidence. Soft!=product.
     */
    kprintf("thread: soft stack astack=%u kstack=%u mid_off=%u "
            "poison=0x%x canary=base+mid hwm_max=%lu hwm_n=%lu "
            "bump_pending=0 wave=%u\n",
            (unsigned)GJ_THR_STACK_SIZE, (unsigned)GJ_THR_KSTACK_SIZE,
            (unsigned)GJ_THR_KSTACK_MID, (unsigned)GJ_THR_KSTACK_POISON,
            (unsigned long)g_soft.u64StackHwmMax,
            (unsigned long)g_soft.u64StackHwmSamples,
            (unsigned)THREAD_SOFT_DEEPEN_WAVE);
    /* Grep: thread: soft aff */
    kprintf("thread: soft aff any=%u pin=%u proc=%u sys_user=%u "
            "blocked_hwm=%u cr3=%u wave=%u\n",
            g_u32SoftAffAnySnap, g_u32SoftAffPinSnap, g_u32SoftProcSnap,
            g_u32SoftSysUserSnap, g_u32SoftBlockedHwm, g_u32SoftCr3Snap,
            (unsigned)THREAD_SOFT_DEEPEN_WAVE);
    /* Grep: thread: soft idle */
    kprintf("thread: soft idle planted=%u bsp=%u ap_create=%lu "
            "pick_idle=%lu cpu_max=%u wave=%u\n",
            g_u32SoftIdleSnap, (g_pIdle != NULL) ? 1u : 0u,
            (unsigned long)g_u64SoftCreateApIdle,
            (unsigned long)g_soft.u64PickIdle, (unsigned)GJ_CPU_STATIC_MAX,
            (unsigned)THREAD_SOFT_DEEPEN_WAVE);
    /* Grep: thread: soft caps - compile-time capacity lamps */
    kprintf("thread: soft caps max_thr=%u qos_max=%u boost_cap=%u "
            "astack=%u kstack=%u cpu_static=%u wave=%u\n",
            (unsigned)GJ_MAX_THREADS, (unsigned)GJ_QOS_CLASS_MAX,
            (unsigned)GJ_QOS_BOOST_CAP, (unsigned)GJ_THR_STACK_SIZE,
            (unsigned)GJ_THR_KSTACK_SIZE, (unsigned)GJ_CPU_STATIC_MAX,
            (unsigned)THREAD_SOFT_DEEPEN_WAVE);
    /* Grep: thread: soft stats */
    kprintf("thread: soft stats pick=%lu idle=%lu int=%lu norm=%lu "
            "bg=%lu util=%lu drv=%lu aff_skip=%lu eq_fair=%lu self=%lu "
            "qos_set=%lu clamp=%lu boost=%lu decay=%lu "
            "can_plant=%lu can_ok=%lu hwm_max=%lu log_n=%u wave=%u\n",
            (unsigned long)g_soft.u64PickTotal,
            (unsigned long)g_soft.u64PickIdle,
            (unsigned long)g_soft.u64PickInteractive,
            (unsigned long)g_soft.u64PickNormal,
            (unsigned long)g_soft.u64PickBackground,
            (unsigned long)g_soft.u64PickUtility,
            (unsigned long)g_soft.u64PickDriver,
            (unsigned long)g_soft.u64PickAffSkip,
            (unsigned long)g_soft.u64PickEqualFair,
            (unsigned long)g_soft.u64PickSelf,
            (unsigned long)g_soft.u64QosSet,
            (unsigned long)g_soft.u64QosClamp,
            (unsigned long)g_soft.u64QosBoostSoft,
            (unsigned long)g_soft.u64QosBoostDecay,
            (unsigned long)g_soft.u64CanaryPlant,
            (unsigned long)g_soft.u64CanaryOk,
            (unsigned long)g_soft.u64StackHwmMax, g_u32SoftLogN,
            (unsigned)THREAD_SOFT_DEEPEN_WAVE);
    /*
     * Grep: thread: soft path
     * Honesty: cooperative pick + soft yield flag != product RR/preempt done.
     */
    kprintf("thread: soft path claim=create+block+wake+yield+switch+pick "
            "coop=1 soft_yield_flag=1 rr_complete=0 preempt_complete=0 "
            "block=%lu wake_thr=%lu yield=%lu switch=%lu exit=%lu "
            "wave=%u (soft inventory)\n",
            (unsigned long)g_u64SoftBlockN,
            (unsigned long)g_u64SoftWakeThr,
            (unsigned long)g_u64SoftYieldN,
            (unsigned long)g_u64SoftSchedSwitch,
            (unsigned long)g_u64SoftExitN,
            (unsigned)THREAD_SOFT_DEEPEN_WAVE);
    /* Grep: thread: soft hwm - Wave 15 HWM rollup */
    kprintf("thread: soft hwm ready=%u run=%u live=%u blocked=%u "
            "exited=%u sys_user=%u kstack_max=%lu samples=%lu wave=%u\n",
            g_u32SoftReadyHwm, g_u32SoftRunHwm, g_u32SoftLiveHwm,
            g_u32SoftBlockedHwm, g_u32SoftExitedHwm, g_u32SoftSysUserHwm,
            (unsigned long)g_soft.u64StackHwmMax,
            (unsigned long)g_u64SoftInvSamples,
            (unsigned)THREAD_SOFT_DEEPEN_WAVE);
    /* Grep: thread: soft sysuser - mid-SYSCALL USER_* save/restore */
    kprintf("thread: soft sysuser snap=%u hwm=%u save=%lu restore=%lu "
            "install_kstk=%lu residual_ud_guard=1 wave=%u\n",
            g_u32SoftSysUserSnap, g_u32SoftSysUserHwm,
            (unsigned long)g_u64SoftSysUserSave,
            (unsigned long)g_u64SoftSysUserRest,
            (unsigned long)g_u64SoftInstallKstk,
            (unsigned)THREAD_SOFT_DEEPEN_WAVE);
    /* Grep: thread: soft user - user64 / user32 entry split */
    kprintf("thread: soft user total=%u user64=%u user32=%u "
            "create_user=%lu create_user32=%lu wave=%u\n",
            g_u32SoftUserSnap, g_u32SoftUser64Snap, g_u32SoftUser32Snap,
            (unsigned long)g_u64SoftCreateUser,
            (unsigned long)g_u64SoftCreateUser32,
            (unsigned)THREAD_SOFT_DEEPEN_WAVE);
    /* Grep: thread: soft boost - residual soft PI ticks */
    kprintf("thread: soft boost live=%u ticks=%u set=%lu soft=%lu "
            "decay=%lu cap=%u wave=%u\n",
            g_u32SoftBoostSnap, g_u32SoftBoostTicks,
            (unsigned long)g_soft.u64QosSet,
            (unsigned long)g_soft.u64QosBoostSoft,
            (unsigned long)g_soft.u64QosBoostDecay, GJ_QOS_BOOST_CAP,
            (unsigned)THREAD_SOFT_DEEPEN_WAVE);
    /* Grep: thread: soft exec - exec_replace surface */
    kprintf("thread: soft exec calls=%lu thr_updated=%lu wave=%u\n",
            (unsigned long)g_u64SoftExecCalls,
            (unsigned long)g_u64SoftExecReplace,
            (unsigned)THREAD_SOFT_DEEPEN_WAVE);
    /*
     * Grep: thread: soft invariant
     * Dual-stack / TSS.RSP0 honesty - soft inventory only, not product RR.
     * aKstack 32 KiB stands (was 8 KiB - door/net #UD). Bump 32->48 only on
     * proven HWM/canary pressure; Soft!=product. H1 residual lean:
     * net_eth_poll on scheduler_run thr stack only (never IRQ).
     * freestanding class SKIP; product UDX; thr-stack for virtio/UDX later.
     * H3 residual lean: thr_exit_before_as_destroy for UDX host teardown.
     * greppable: net_eth_poll=run_loop_only * net_eth_irq=0
     *            freestanding_class=SKIP * product=UDX
     *            thr_exit_before_as_destroy=1 * udx_host_teardown=1
     */
    kprintf("thread: soft invariant dual_stack=1 astack=%u kstack=%u "
            "rsp0=irq_dedicated sysuser_save_restore=1 "
            "install_kstk_after_switch=1 net_eth_poll=run_loop_only "
            "net_eth_irq=0 owner=scheduler_run stack=thr "
            "freestanding_class=SKIP product=UDX poll_own=thr_stack "
            "virtio_udx_later=1 eth_poll_n=%lu "
            "thr_exit_before_as_destroy=1 udx_host_teardown=1 "
            "H3=death_residual rr_complete=0 preempt_complete=0 wave=%u\n",
            (unsigned)GJ_THR_STACK_SIZE, (unsigned)GJ_THR_KSTACK_SIZE,
            (unsigned long)g_u64SoftEthPollN,
            (unsigned)THREAD_SOFT_DEEPEN_WAVE);
    /*
     * Grep: thread: soft eth | sched: soft eth
     * H1 residual lean lamp (Soft!=product; inventory-capped only).
     * freestanding class SKIP; product UDX; poll still thr-stack for
     * virtio/UDX later. No kprintf in hot scheduler_run (silent tally).
     */
    kprintf("thread: soft eth net_eth_poll=run_loop_only net_eth_irq=0 "
            "owner=scheduler_run stack=thr ap_poll=0 irq_path=0 "
            "freestanding_class=SKIP product=UDX poll_own=thr_stack "
            "virtio_udx_later=1 eth_poll_n=%lu "
            "fault_class=H1_irq_stack_smash soft_ne_product=1 "
            "G-AC-1 wave=%u\n",
            (unsigned long)g_u64SoftEthPollN,
            (unsigned)THREAD_SOFT_DEEPEN_WAVE);
    kprintf("sched: soft eth net_eth_poll=run_loop_only net_eth_irq=0 "
            "owner=scheduler_run stack=thr freestanding_class=SKIP "
            "product=UDX poll_own=thr_stack virtio_udx_later=1 "
            "eth_poll_n=%lu soft_ne_product=1 G-AC-1\n",
            (unsigned long)g_u64SoftEthPollN);
    /*
     * Grep: thread: soft thr_exit | sched: soft thr_exit
     * H3 residual lean lamp (Soft!=product; inventory-capped only).
     * thr_exit siblings before as_destroy; UDX multi-thr host teardown.
     * Residual deepen: scrub-class + multi-thr + dead-AS refuse split.
     * Silent tallies on exit_process / dead-AS skip (no hot-path kprintf).
     * greppable: hosts=rtl8168_udx|xhci_udx|ddi_host
     */
    kprintf("thread: soft thr_exit thr_exit_before_as_destroy=1 "
            "H3=death_residual udx_host_teardown=1 "
            "exit_proc_calls=%lu exit_proc_n=%lu exit_null=%lu exit_zero=%lu "
            "exit_multi=%lu exit_detach=%lu runnable=%lu running=%lu "
            "blocked=%lu user=%lu user32=%lu sysuser=%lu cr3=%lu block=%lu "
            "driver=%lu dead_as_skip=%lu dead_null=%lu dead_alive0=%lu "
            "dead_cr3_0=%lu dead_resume=%lu "
            "fault_class=H3_clone_sibling_dead_AS "
            "hosts=rtl8168_udx|xhci_udx|ddi_host "
            "product_multi_cpu_kill=0 product_udx_close=0 "
            "soft_ne_product=1 wave=%u\n",
            (unsigned long)g_u64SoftThrExitProcCalls,
            (unsigned long)g_u64SoftThrExitProcN,
            (unsigned long)g_u64SoftThrExitNull,
            (unsigned long)g_u64SoftThrExitZero,
            (unsigned long)g_u64SoftThrExitMulti,
            (unsigned long)g_u64SoftThrExitDetach,
            (unsigned long)g_u64SoftThrExitRunnable,
            (unsigned long)g_u64SoftThrExitRunning,
            (unsigned long)g_u64SoftThrExitBlocked,
            (unsigned long)g_u64SoftThrExitUser,
            (unsigned long)g_u64SoftThrExitUser32,
            (unsigned long)g_u64SoftThrExitSysUser,
            (unsigned long)g_u64SoftThrExitCr3,
            (unsigned long)g_u64SoftThrExitBlockObj,
            (unsigned long)g_u64SoftThrExitDriver,
            (unsigned long)g_u64SoftDeadAsSkip,
            (unsigned long)g_u64SoftDeadAsSkipNull,
            (unsigned long)g_u64SoftDeadAsSkipAlive,
            (unsigned long)g_u64SoftDeadAsSkipCr3,
            (unsigned long)g_u64SoftDeadAsSkipResume,
            (unsigned)THREAD_SOFT_DEEPEN_WAVE);
    kprintf("sched: soft thr_exit thr_exit_before_as_destroy=1 "
            "H3=death_residual udx_host_teardown=1 "
            "exit_proc_calls=%lu exit_proc_n=%lu exit_multi=%lu "
            "exit_detach=%lu dead_as_skip=%lu dead_resume=%lu "
            "soft_ne_product=1\n",
            (unsigned long)g_u64SoftThrExitProcCalls,
            (unsigned long)g_u64SoftThrExitProcN,
            (unsigned long)g_u64SoftThrExitMulti,
            (unsigned long)g_u64SoftThrExitDetach,
            (unsigned long)g_u64SoftDeadAsSkip,
            (unsigned long)g_u64SoftDeadAsSkipResume);
    /*
     * Grep: thread: soft thr_exit residual lean | thread: soft residual lean H3
     * Once-lamp honesty for UDX host multi-thr teardown class (Soft!=product).
     * Mirrors process: soft death residual lean H3; thr-unit half only.
     * Inventory-capped path only - no stamp storms.
     */
    if (g_fThrExitResLeanOnce == 0) {
        g_fThrExitResLeanOnce = 1;
        kprintf("thread: soft thr_exit residual lean H3 "
                "thr_exit_before_as_destroy=1 early+barrier=1 "
                "udx_host_teardown=1 multi_thr_host=1 "
                "hosts=rtl8168_udx|xhci_udx|ddi_host "
                "scrub=user|user32|sysuser|cr3|block|driver|detach "
                "refuse=null_proc|alive0|cr3_0|resume "
                "skip_user_if_not_alive=1 "
                "product_multi_cpu_kill=0 product_udx_close=0 "
                "Soft!=product dual=MIT/Apache wave=%u\n",
                (unsigned)THREAD_SOFT_DEEPEN_WAVE);
        kprintf("thread: soft residual lean H3 "
                "order=alive0,thr_exit,barrier,as_destroy "
                "pe32_clone_vm=1 udx_host=1 "
                "skip_user=trampoline|schedule_resume "
                "fault_class=H3_clone_sibling_dead_AS "
                "soft_ne_product=1\n");
    }
    /*
     * Grep: thread: soft residual lean udx | thread: soft udx host residual
     * STRONGER functional residual (W10 Dual DoD; stamp-free bar v2026.08.04.75):
     * live UDX host H1 thr-stack poll + H3 thr_exit + dual_dod OPEN honesty.
     * Soft!=product; Dual DoD A/B remain OPEN; inventory-capped; no stamp storms.
     */
    if (g_fUdxHostResLeanOnce == 0) {
        g_fUdxHostResLeanOnce = 1;
        kprintf("thread: soft residual lean udx "
                "H1=net_eth_poll_run_loop_only H3=thr_exit_before_as_destroy "
                "net_eth_irq=0 poll_own=thr_stack freestanding_class=SKIP "
                "product=UDX thr_exit_before_as_destroy=1 "
                "udx_host_teardown=1 multi_thr_host=1 "
                "hosts=rtl8168_udx|xhci_udx|ddi_host "
                "eth_poll_n=%lu exit_proc_calls=%lu exit_multi=%lu "
                "dead_as_skip=%lu "
                "Dual_DoD_A=OPEN Dual_DoD_B=OPEN dual_dod=OPEN "
                "product_multi_cpu_kill=0 product_udx_close=0 "
                "soft_ne_product=1 dual=MIT/Apache G-AC-1 "
                "stamp_storm=0 version_stamp=0 wave=%u "
                "(W10 STRONGER live UDX host residual; Soft!=product; "
                "not Dual DoD close; no .ko product)\n",
                (unsigned long)g_u64SoftEthPollN,
                (unsigned long)g_u64SoftThrExitProcCalls,
                (unsigned long)g_u64SoftThrExitMulti,
                (unsigned long)g_u64SoftDeadAsSkip,
                (unsigned)THREAD_SOFT_DEEPEN_WAVE);
        kprintf("thread: soft udx host residual PASS "
                "H1=1 H3=1 dual_dod=OPEN "
                "hosts=rtl8168_udx|xhci_udx|ddi_host "
                "Soft!=product G-AC-1 wave=%u\n",
                (unsigned)THREAD_SOFT_DEEPEN_WAVE);
    }
    /*
     * Grep: thread: soft tag | sched: soft tag
     * C3 thr soft tag residual (Soft!=product; panel/no-COM1).
     * Named-class pe32_wow|sshd|init|ring3 (+pe32_u32|idle) inventory.
     * Not product identity / Dual DoD close. Tags sticky until slot reuse.
     * H3 thr_exit order stands (siblings before as_destroy; no tag scrub).
     * greppable: pe32_wow|sshd|init|ring3 * thr_exit_before_as_destroy=1
     */
    kprintf("thread: soft tag pe32_wow=%u sshd=%u init=%u ring3=%u "
            "pe32_u32=%u idle=%u other=%u empty=%u named=%u "
            "set_n=%lu set_pe32_wow=%lu set_sshd=%lu set_init=%lu "
            "set_ring3=%lu set_pe32_u32=%lu set_idle=%lu set_other=%lu "
            "h3_sticky=%lu h3_restamp=%lu h3_pe32_wow=%lu "
            "sticky_until_reuse=1 product_identity=0 "
            "soft_ne_product=1 wave=%u\n",
            g_u32SoftTagPe32WowSnap, g_u32SoftTagSshdSnap,
            g_u32SoftTagInitSnap, g_u32SoftTagRing3Snap,
            g_u32SoftTagPe32U32Snap, g_u32SoftTagIdleSnap,
            g_u32SoftTagOtherSnap, g_u32SoftTagEmptySnap,
            g_u32SoftTagNamedSnap,
            (unsigned long)g_u64SoftTagSetN,
            (unsigned long)g_u64SoftTagSetPe32Wow,
            (unsigned long)g_u64SoftTagSetSshd,
            (unsigned long)g_u64SoftTagSetInit,
            (unsigned long)g_u64SoftTagSetRing3,
            (unsigned long)g_u64SoftTagSetPe32U32,
            (unsigned long)g_u64SoftTagSetIdle,
            (unsigned long)g_u64SoftTagSetOther,
            (unsigned long)g_u64SoftTagH3StickyKeep,
            (unsigned long)g_u64SoftTagH3Restamp,
            (unsigned long)g_u64SoftTagH3Pe32Wow,
            (unsigned)THREAD_SOFT_DEEPEN_WAVE);
    kprintf("sched: soft tag pe32_wow=%u sshd=%u init=%u ring3=%u "
            "named=%u set_n=%lu h3_sticky=%lu h3_restamp=%lu "
            "soft_ne_product=1\n",
            g_u32SoftTagPe32WowSnap, g_u32SoftTagSshdSnap,
            g_u32SoftTagInitSnap, g_u32SoftTagRing3Snap,
            g_u32SoftTagNamedSnap, (unsigned long)g_u64SoftTagSetN,
            (unsigned long)g_u64SoftTagH3StickyKeep,
            (unsigned long)g_u64SoftTagH3Restamp);
    /*
     * Grep: thread: soft tag residual lean
     * Once-lamp honesty for panel-critical soft tags (Soft!=product).
     * Inventory-capped path only - no stamp storms. H3 thr_exit order stands.
     * STRONGER thr soft tags H3: sticky keep + entry re-stamp on exit_process.
     */
    if (g_fSoftTagResLeanOnce == 0) {
        g_fSoftTagResLeanOnce = 1;
        kprintf("thread: soft tag residual lean C3 "
                "tags=pe32_wow|sshd|init|ring3 "
                "panel=no_COM1_STATUS sticky_until_reuse=1 "
                "h3_sticky_keep=1 h3_entry_restamp=1 "
                "product_identity=0 dual_dod_close=0 "
                "H3_order=alive0,thr_exit,barrier,as_destroy "
                "thr_exit_before_as_destroy=1 "
                "Soft!=product dual=MIT/Apache G-AC-1 wave=%u\n",
                (unsigned)THREAD_SOFT_DEEPEN_WAVE);
        /* Grep: thread: soft tag residual lean H3 */
        kprintf("thread: soft tag residual lean H3 "
                "sticky_keep=%lu restamp=%lu pe32_wow_drain=%lu "
                "thr_exit_before_as_destroy=1 "
                "order=alive0,thr_exit,barrier,as_destroy "
                "no_tag_scrub_on_exit=1 sticky_until_reuse=1 "
                "soft_ne_product=1 Soft!=product\n",
                (unsigned long)g_u64SoftTagH3StickyKeep,
                (unsigned long)g_u64SoftTagH3Restamp,
                (unsigned long)g_u64SoftTagH3Pe32Wow);
    }
    /*
     * Grep: thread: soft return
     * Wave 19 return-path catalog - create/wake/schedule terminal outcomes.
     * Soft != product RR / preemption complete. product_kernel=OPEN.
     * Compact one-liner only (no complementary stamp cascade).
     */
    kprintf("thread: soft return create_ok=%lu create_full=%lu "
            "wake_thr=%lu wake_none=%lu sched_self=%lu sched_switch=%lu "
            "sched_spin=%lu yield_pend=%lu canary_fail=%lu "
            "canary_ok=%lu block=%lu exit=%lu exec=%lu "
            "product_kernel=OPEN wave=%u\n",
            (unsigned long)g_u64SoftCreateOk,
            (unsigned long)g_u64SoftCreateFull,
            (unsigned long)g_u64SoftWakeThr,
            (unsigned long)g_u64SoftWakeNone,
            (unsigned long)g_u64SoftSchedSelf,
            (unsigned long)g_u64SoftSchedSwitch,
            (unsigned long)g_u64SoftSchedSpin,
            (unsigned long)g_u64SoftYieldPendHit,
            (unsigned long)g_soft.u64CanaryFail,
            (unsigned long)g_soft.u64CanaryOk,
            (unsigned long)g_u64SoftBlockN,
            (unsigned long)g_u64SoftExitN,
            (unsigned long)g_u64SoftExecCalls,
            (unsigned)THREAD_SOFT_DEEPEN_WAVE);
    /* Grep: thread: soft ret_surface - terminal return classes (one line) */
    kprintf("thread: soft ret_surface create=ok|full wake=thr|none "
            "sched=self|switch|spin yield=pend canary=ok|fail "
            "block=n exit=n exec=n product_kernel=OPEN areas=%u wave=%u\n",
            (unsigned)THREAD_SOFT_DEEPEN_AREAS,
            (unsigned)THREAD_SOFT_DEEPEN_WAVE);
    /* Grep: thread: soft ratio - Wave 19 basis-point rollup */
    {
        u32 u32CreateOkBp;
        u32 u32WakeHitBp;
        u32 u32SwitchBp;
        u32 u32CreateDenom;
        u32 u32WakeDenom;
        u32 u32SchedDenom;

        u32CreateDenom = (u32)(g_u64SoftCreateOk + g_u64SoftCreateFull);
        if (u32CreateDenom != 0) {
            u32CreateOkBp =
                (u32)((g_u64SoftCreateOk * 10000ull) / (u64)u32CreateDenom);
        } else {
            u32CreateOkBp = 0;
        }
        u32WakeDenom = (u32)(g_u64SoftWakeThr + g_u64SoftWakeNone);
        if (u32WakeDenom != 0) {
            u32WakeHitBp =
                (u32)((g_u64SoftWakeThr * 10000ull) / (u64)u32WakeDenom);
        } else {
            u32WakeHitBp = 0;
        }
        u32SchedDenom = (u32)(g_u64SoftSchedEnter);
        if (u32SchedDenom != 0) {
            u32SwitchBp =
                (u32)((g_u64SoftSchedSwitch * 10000ull) / (u64)u32SchedDenom);
        } else {
            u32SwitchBp = 0;
        }
        kprintf("thread: soft ratio create_ok_bp=%u wake_hit_bp=%u "
                "switch_bp=%u ready_snap=%u live_snap=%u wave=%u\n",
                u32CreateOkBp, u32WakeHitBp, u32SwitchBp, g_u32SoftReadySnap,
                g_u32SoftLiveSnap, (unsigned)THREAD_SOFT_DEEPEN_WAVE);
    }
    /* Grep: thread: soft headroom - table free slots */
    kprintf("thread: soft headroom unused=%u max=%u live=%u live_hwm=%u "
            "ready_hwm=%u blocked_hwm=%u wave=%u\n",
            g_u32SoftUnusedSnap, (unsigned)GJ_MAX_THREADS, g_u32SoftLiveSnap,
            g_u32SoftLiveHwm, g_u32SoftReadyHwm, g_u32SoftBlockedHwm,
            (unsigned)THREAD_SOFT_DEEPEN_WAVE);
    /* Grep: thread: soft surface - Wave 19 (+eth H1 + thr_exit H3 + tag C3) */
    kprintf("thread: soft surface inventory,table,ready,run,create,block,"
            "wake,yield,switch,path,qos,canary,aff,pick,stack,idle,caps,"
            "stats,exit,deepen,hwm,sysuser,user,boost,exec,invariant,eth,"
            "thr_exit,thr_exit_lean,return,ret_surface,ratio,surface,headroom,"
            "tag,tag_lean "
            "areas=%u wave=%u\n",
            (unsigned)THREAD_SOFT_DEEPEN_AREAS,
            (unsigned)THREAD_SOFT_DEEPEN_WAVE);

    /*
     * Grep: thread: soft deepen
     * Wave stamp + log_cap honesty. Soft!=product; no stamp storms.
     */
    kprintf("thread: soft deepen wave=%u areas=%u live=%u ready=%u "
            "run=%u blocked=%u pick=%lu log_n=%u log_cap=%u ok=1 skip=0\n",
            (unsigned)THREAD_SOFT_DEEPEN_WAVE,
            (unsigned)THREAD_SOFT_DEEPEN_AREAS, g_u32SoftLiveSnap,
            g_u32SoftReadySnap, g_u32SoftRunSnap, g_u32SoftBlockedSnap,
            (unsigned long)g_soft.u64PickTotal, g_u32SoftLogN,
            (unsigned)THREAD_SOFT_LOG_CAP);
    /* Grep: thread: soft inventory PASS / thread: soft PASS */
    kprintf("thread: soft inventory PASS log_n=%u log_cap=%u wave=%u "
            "areas=%u\n",
            g_u32SoftLogN, (unsigned)THREAD_SOFT_LOG_CAP,
            (unsigned)THREAD_SOFT_DEEPEN_WAVE,
            (unsigned)THREAD_SOFT_DEEPEN_AREAS);
    kprintf("thread: soft PASS wave=%u log_cap=%u\n",
            (unsigned)THREAD_SOFT_DEEPEN_WAVE,
            (unsigned)THREAD_SOFT_LOG_CAP);
}

/*
 * aKstack grows down from top. Plant base canary at low address + mid
 * soft canary so overflow stomps markers before adjacent thr fields.
 * Poison fill enables soft high-water measurement.
 */
static u64
thr_kstack_top(struct gj_thread *pThr)
{
    if (pThr == NULL) {
        return 0;
    }
    return ((u64)(gj_vaddr_t)&pThr->aKstack[GJ_THR_KSTACK_SIZE]) & ~0xfull;
}

static void
thr_plant_kstack_canary(struct gj_thread *pThr)
{
    u64 *pCan;
    u64 *pMid;

    if (pThr == NULL) {
        return;
    }
    /* Soft poison: unused depth stays GJ_THR_KSTACK_POISON for HWM scan. */
    memset(pThr->aKstack, (int)GJ_THR_KSTACK_POISON, GJ_THR_KSTACK_SIZE);
    pCan = (u64 *)(void *)&pThr->aKstack[0];
    *pCan = GJ_THR_KSTACK_CANARY;
    pThr->u64KstackCanary = GJ_THR_KSTACK_CANARY;
    pMid = (u64 *)(void *)&pThr->aKstack[GJ_THR_KSTACK_MID];
    *pMid = GJ_THR_KSTACK_CANARY_MID;
    g_soft.u64CanaryPlant++;
}

/*
 * Soft high-water: bytes from top down to first non-poison (stack grows down).
 * Skips base canary region; mid canary may register as used if crossed.
 */
static u32
thr_kstack_hwm_soft(struct gj_thread *pThr)
{
    u8 *pBase;
    u8 *pTop;
    u8 *pScan;
    u32 cbUsed;

    if (pThr == NULL) {
        return 0;
    }
    pBase = pThr->aKstack;
    pTop = pBase + GJ_THR_KSTACK_SIZE;
    pScan = pTop;
    while (pScan > pBase + sizeof(u64)) {
        u32 u32Off;

        pScan--;
        u32Off = (u32)(pScan - pBase);
        /* Mid canary is planted non-poison - skip for HWM soft only. */
        if (u32Off >= GJ_THR_KSTACK_MID &&
            u32Off < GJ_THR_KSTACK_MID + (u32)sizeof(u64)) {
            continue;
        }
        if (*pScan != GJ_THR_KSTACK_POISON) {
            pScan++;
            break;
        }
    }
    cbUsed = (u32)(pTop - pScan);
    g_soft.u64StackHwmSamples++;
    if ((u64)cbUsed > g_soft.u64StackHwmMax) {
        g_soft.u64StackHwmMax = (u64)cbUsed;
    }
    return cbUsed;
}

static void
thread_check_kstack(struct gj_thread *pThr)
{
    u64 *pCan;
    u64 *pMid;

    if (pThr == NULL) {
        return;
    }
    /* Unplanted (0) skips - only before thr_plant_kstack_canary. */
    if (pThr->u64KstackCanary == 0) {
        return;
    }
    g_soft.u64CanaryCheck++;
    pCan = (u64 *)(void *)&pThr->aKstack[0];
    pMid = (u64 *)(void *)&pThr->aKstack[GJ_THR_KSTACK_MID];
    if (*pCan != pThr->u64KstackCanary) {
        g_soft.u64CanaryFail++;
        kprintf("sched: KSTACK OVERFLOW thr=%u canary=0x%lx got=0x%lx - halt\n",
                pThr->u32Id, (unsigned long)pThr->u64KstackCanary,
                (unsigned long)*pCan);
        for (;;) {
            __asm__ volatile("cli; hlt");
        }
    }
    if (*pMid != GJ_THR_KSTACK_CANARY_MID) {
        g_soft.u64CanaryFail++;
        kprintf("sched: KSTACK MID OVERFLOW thr=%u mid=0x%lx got=0x%lx - halt\n",
                pThr->u32Id, (unsigned long)GJ_THR_KSTACK_CANARY_MID,
                (unsigned long)*pMid);
        for (;;) {
            __asm__ volatile("cli; hlt");
        }
    }
    g_soft.u64CanaryMidOk++;
    g_soft.u64CanaryOk++;
    (void)thr_kstack_hwm_soft(pThr);
}

/*
 * Initial aStack frame for switch_context -> ret into trampoline.
 * Layout must match switch_context pop order (r15..rbp, then ret).
 */
static void
thr_build_initial_rsp(struct gj_thread *pThr)
{
    u64 *pSp;

    if (pThr == NULL) {
        return;
    }
    pSp = (u64 *)(void *)&pThr->aStack[GJ_THR_STACK_SIZE];
    pSp = (u64 *)((u64)pSp & ~0xfull);
    *(--pSp) = (u64)(gj_vaddr_t)thread_trampoline; /* ret addr */
    *(--pSp) = 0; /* rbp */
    *(--pSp) = 0; /* rbx */
    *(--pSp) = 0; /* r12 */
    *(--pSp) = 0; /* r13 */
    *(--pSp) = 0; /* r14 */
    *(--pSp) = 0; /* r15 */
    pThr->u64Rsp = (u64)(gj_vaddr_t)pSp;
}

static struct gj_thread *
thr_find_by_id(u32 u32Id)
{
    u32 iThr;

    if (u32Id == 0) {
        return NULL;
    }
    for (iThr = 0; iThr < GJ_MAX_THREADS; iThr++) {
        if (g_aThreads[iThr].u32Id == u32Id) {
            return &g_aThreads[iThr];
        }
    }
    return NULL;
}

static void
thread_trampoline(void)
{
    struct gj_thread *pThr = thread_current();

    if (pThr != NULL &&
        (pThr->u32Flags & (GJ_THR_F_USER_ENTRY | GJ_THR_F_USER32_ENTRY)) !=
            0) {
        int fUser32 = (pThr->u32Flags & GJ_THR_F_USER32_ENTRY) != 0;

        pThr->u32Flags &= ~(GJ_THR_F_USER_ENTRY | GJ_THR_F_USER32_ENTRY);
        /*
         * H3 residual lean belt: refuse ring-3 entry when the process AS is
         * already dead/destroyed. CLONE_VM / UDX multi-thr sibling can still
         * be RUNNABLE after parent process_death if thr_exit missed a race
         * (lab: thr enter user32 rip=0x58240013 -> #PF I=1 wild-rip class).
         * UDX host teardown: same refuse for work/IRQ thr on shared AS.
         * Companion: schedule() resume skip-user if !alive (mid-syscall).
         * Soft!=product. Grep: sched: thr skip user dead AS
         *   | thr_exit_before_as_destroy=1 | H3=death_residual
         *   | udx_host_teardown=1 | skip_user_if_not_alive=1
         */
        if (pThr->pProc == NULL || pThr->pProc->u32Alive == 0 ||
            pThr->pProc->u64Cr3 == 0) {
            const char *pszClass;

            g_u64SoftDeadAsSkip++;
            if (pThr->pProc == NULL) {
                g_u64SoftDeadAsSkipNull++;
                pszClass = "null_proc";
            } else if (pThr->pProc->u32Alive == 0) {
                g_u64SoftDeadAsSkipAlive++;
                pszClass = "alive0";
            } else {
                g_u64SoftDeadAsSkipCr3++;
                pszClass = "cr3_0";
            }
            kprintf("sched: thr %u skip user%s dead AS class=%s alive=%u "
                    "cr3=0x%lx rip=0x%lx H3=death_residual "
                    "thr_exit_before_as_destroy=1 udx_host_teardown=1 "
                    "skip_user_if_not_alive=1 soft_ne_product=1\n",
                    pThr->u32Id, fUser32 ? "32" : "", pszClass,
                    (pThr->pProc != NULL) ? pThr->pProc->u32Alive : 0u,
                    (unsigned long)((pThr->pProc != NULL)
                                        ? pThr->pProc->u64Cr3
                                        : 0ull),
                    (unsigned long)pThr->u64UserRip);
            /* Deep scrub before exit - no residual USER/CR3/sysuser. */
            pThr->u64UserRip = 0;
            pThr->u64UserRsp = 0;
            pThr->u64Cr3 = 0;
            pThr->u32SysUserValid = 0;
            pThr->u64SysUserRip = 0;
            pThr->u64SysUserRsp = 0;
            pThr->u64SysUserRflags = 0;
            pThr->pBlockObj = NULL;
            pThr->u32BlockTag = 0;
            pThr->pProc = NULL;
            thread_exit();
        }
        process_as_activate(pThr->pProc);
        /* First ring-3 entry: SYSCALL kstack ready; RSP0 stays IRQ stack. */
        thread_install_kstack(pThr);
        kprintf("sched: thr %u enter user%s rip=0x%lx rsp=0x%lx\n",
                pThr->u32Id, fUser32 ? "32" : "",
                (unsigned long)pThr->u64UserRip,
                (unsigned long)pThr->u64UserRsp);
        if (fUser32) {
            cpu_enter_user32(pThr->u64UserRip, pThr->u64UserRsp);
        } else {
            cpu_enter_user(pThr->u64UserRip, pThr->u64UserRsp);
        }
        /* not reached on success */
    }
    if (pThr != NULL && pThr->pfnEntry != NULL) {
        pThr->pfnEntry(pThr->pArg);
    }
    thread_exit();
}

void
thread_install_kstack(struct gj_thread *pThr)
{
    struct gj_cpu *pCpu = cpu_current();

    if (pCpu != NULL && pThr != NULL && pThr->u64KstackTop != 0) {
        /* SYSCALL path only (GS:0). Not TSS.RSP0. */
        pCpu->u64KernelRsp = pThr->u64KstackTop;
        g_u64SoftInstallKstk++;
    }
    /*
     * Ring-3 IRQs must not land on thr SYSCALL stacks. Always reassert the
     * dedicated IRQ stack here - even if pThr is NULL or kstack unset.
     */
    tss_use_irq_rsp0();
}

void
thread_init(void)
{
    u32 iThr;

    memset(g_aThreads, 0, sizeof(g_aThreads));
    memset(g_aThrCpu, 0, sizeof(g_aThrCpu)); /* all BSP-affine by default */
    memset(g_aThrQos, 0, sizeof(g_aThrQos));
    memset(g_aThrBoost, 0, sizeof(g_aThrBoost));
    memset(g_aThrLastPick, 0, sizeof(g_aThrLastPick));
    memset(&g_soft, 0, sizeof(g_soft));
    g_u32PickGen = 1;
    g_fSoftStatsOnce = 0;
    g_fSoftInvOnce = 0;
    g_fYieldReq = 0;
    g_u32SoftReadySnap = 0;
    g_u32SoftRunSnap = 0;
    g_u32SoftBlockedSnap = 0;
    g_u32SoftExitedSnap = 0;
    g_u32SoftUnusedSnap = 0;
    g_u32SoftLiveSnap = 0;
    g_u32SoftUserSnap = 0;
    g_u32SoftUser64Snap = 0;
    g_u32SoftUser32Snap = 0;
    g_u32SoftBoostSnap = 0;
    g_u32SoftBoostTicks = 0;
    g_u32SoftAffAnySnap = 0;
    g_u32SoftAffPinSnap = 0;
    g_u32SoftProcSnap = 0;
    g_u32SoftSysUserSnap = 0;
    g_u32SoftBlockObjSnap = 0;
    g_u32SoftIdleSnap = 0;
    g_u32SoftCr3Snap = 0;
    g_u32SoftReadyHwm = 0;
    g_u32SoftRunHwm = 0;
    g_u32SoftLiveHwm = 0;
    g_u32SoftBlockedHwm = 0;
    g_u32SoftExitedHwm = 0;
    g_u32SoftSysUserHwm = 0;
    g_u64SoftInvSamples = 0;
    g_u64SoftReadyTrans = 0;
    g_u64SoftRunTrans = 0;
    g_u64SoftBlockTrans = 0;
    g_u64SoftExitTrans = 0;
    g_u64SoftCreateOk = 0;
    g_u64SoftCreateFull = 0;
    g_u64SoftCreateUser = 0;
    g_u64SoftCreateUser32 = 0;
    g_u64SoftCreateReuse = 0;
    g_u64SoftCreateApIdle = 0;
    g_u64SoftCreateOnCpu = 0;
    g_u64SoftBlockN = 0;
    g_u64SoftWakeCalls = 0;
    g_u64SoftWakeThr = 0;
    g_u64SoftWakeNone = 0;
    g_u64SoftYieldN = 0;
    g_u64SoftYieldReq = 0;
    g_u64SoftYieldPendHit = 0;
    g_u64SoftSchedEnter = 0;
    g_u64SoftSchedSelf = 0;
    g_u64SoftSchedSwitch = 0;
    g_u64SoftSchedSpin = 0;
    g_u64SoftSchedAsAct = 0;
    g_u64SoftSchedCr3Own = 0;
    g_u64SoftExitN = 0;
    g_u64SoftSetCpuN = 0;
    g_u64SoftExecReplace = 0;
    g_u64SoftExecCalls = 0;
    g_u64SoftSysUserSave = 0;
    g_u64SoftSysUserRest = 0;
    g_u64SoftInstallKstk = 0;
    g_u64SoftEthPollN = 0; /* H1 residual lean: thr-stack poll tally */
    /* H3 residual lean deepen: thr_exit / UDX host teardown tallies */
    g_u64SoftThrExitProcCalls = 0;
    g_u64SoftThrExitProcN = 0;
    g_u64SoftThrExitNull = 0;
    g_u64SoftThrExitZero = 0;
    g_u64SoftThrExitMulti = 0;
    g_u64SoftThrExitDetach = 0;
    g_u64SoftThrExitRunnable = 0;
    g_u64SoftThrExitRunning = 0;
    g_u64SoftThrExitBlocked = 0;
    g_u64SoftThrExitUser = 0;
    g_u64SoftThrExitUser32 = 0;
    g_u64SoftThrExitSysUser = 0;
    g_u64SoftThrExitCr3 = 0;
    g_u64SoftThrExitBlockObj = 0;
    g_u64SoftThrExitDriver = 0;
    g_u64SoftDeadAsSkip = 0;
    g_u64SoftDeadAsSkipNull = 0;
    g_u64SoftDeadAsSkipAlive = 0;
    g_u64SoftDeadAsSkipCr3 = 0;
    g_u64SoftDeadAsSkipResume = 0;
    g_fThrExitResLeanOnce = 0;
    /* C3 thr soft tag residual (pe32_wow|sshd|init|ring3; Soft!=product).
     * STRONGER H3 sticky/restamp tallies reset with inventory init. */
    g_u64SoftTagH3StickyKeep = 0;
    g_u64SoftTagH3Restamp = 0;
    g_u64SoftTagH3Pe32Wow = 0;
    g_u32SoftTagPe32WowSnap = 0;
    g_u32SoftTagSshdSnap = 0;
    g_u32SoftTagInitSnap = 0;
    g_u32SoftTagRing3Snap = 0;
    g_u32SoftTagPe32U32Snap = 0;
    g_u32SoftTagIdleSnap = 0;
    g_u32SoftTagOtherSnap = 0;
    g_u32SoftTagEmptySnap = 0;
    g_u32SoftTagNamedSnap = 0;
    g_u64SoftTagSetN = 0;
    g_u64SoftTagSetPe32Wow = 0;
    g_u64SoftTagSetSshd = 0;
    g_u64SoftTagSetInit = 0;
    g_u64SoftTagSetRing3 = 0;
    g_u64SoftTagSetPe32U32 = 0;
    g_u64SoftTagSetIdle = 0;
    g_u64SoftTagSetOther = 0;
    g_fSoftTagResLeanOnce = 0;
    g_u32SoftLogN = 0;
    for (iThr = 0; iThr < 5u; iThr++) {
        g_aSoftReadyQos[iThr] = 0;
        g_aSoftRunQos[iThr] = 0;
    }
    /* Slot 0 = idle/bootstrap thread representing current execution */
    g_pIdle = &g_aThreads[0];
    g_pIdle->u32Id = g_u32NextId++;
    g_pIdle->u32State = GJ_THR_RUNNING;
    sched_soft_note_run();
    g_pIdle->pProc = NULL;
    {
        u32 iTag;

        for (iTag = 0; iTag < GJ_MAX_THREADS; iTag++) {
            g_aThrSoftTag[iTag][0] = '\0';
        }
        /* Slot 0 idle soft tag for panel honesty (Soft!=product). */
        g_aThrSoftTag[0][0] = 'i';
        g_aThrSoftTag[0][1] = 'd';
        g_aThrSoftTag[0][2] = 'l';
        g_aThrSoftTag[0][3] = 'e';
        g_aThrSoftTag[0][4] = '\0';
        thr_soft_tag_note_set("idle");
    }
    g_pIdle->u64Cr3 = cpu_read_cr3();
    g_pIdle->u64Rsp = 0;
    g_pIdle->u64KstackTop = thr_kstack_top(g_pIdle);
    thr_plant_kstack_canary(g_pIdle);
    g_pIdle->u32SysUserValid = 0;
    g_aThrCpu[0] = 0;
    g_aThrQos[0] = GJ_QOS_NORMAL;
    g_apIdle[0] = g_pIdle;
    cpu_set_current_thread(g_pIdle);
    for (iThr = 1; iThr < GJ_MAX_THREADS; iThr++) {
        g_aThreads[iThr].u32State = GJ_THR_UNUSED;
        g_aThrCpu[iThr] = 0;
        g_aThrQos[iThr] = GJ_QOS_NORMAL;
        g_aThrBoost[iThr] = 0;
    }
    for (iThr = 1; iThr < GJ_CPU_STATIC_MAX; iThr++) {
        g_apIdle[iThr] = NULL;
    }
    kprintf("sched: thread_init idle id=%u kstack=0x%lx\n", g_pIdle->u32Id,
            (unsigned long)g_pIdle->u64KstackTop);
    /* Soft product markers (greppable; prefix-stable). */
    kprintf("sched: soft qos classes=0..4 (norm/int/bg/util/drv) boost_cap=%u\n",
            GJ_QOS_BOOST_CAP);
    kprintf("sched: soft kstack canary base+mid hwm poison=0x%x mid_off=%u\n",
            (unsigned)GJ_THR_KSTACK_POISON, (unsigned)GJ_THR_KSTACK_MID);
    /* Baseline ready/run inventory after idle plant. */
    sched_soft_inventory_print();
}

int
thread_init_ap_idle(u32 u32Cpu)
{
    u32 iThr;
    struct gj_thread *pThr;

    if (u32Cpu == 0 || u32Cpu >= GJ_CPU_STATIC_MAX) {
        return -1;
    }
    if (g_apIdle[u32Cpu] != NULL) {
        return 0;
    }
    pThr = NULL;
    for (iThr = 1; iThr < GJ_MAX_THREADS; iThr++) {
        if (g_aThreads[iThr].u32State == GJ_THR_UNUSED ||
            g_aThreads[iThr].u32State == GJ_THR_EXITED) {
            pThr = &g_aThreads[iThr];
            break;
        }
    }
    if (pThr == NULL) {
        return -1;
    }
    memset(pThr, 0, sizeof(*pThr));
    g_aThrCpu[iThr] = (u8)u32Cpu;
    g_aThrQos[iThr] = GJ_QOS_NORMAL;
    g_aThrBoost[iThr] = 0;
    g_aThrLastPick[iThr] = 0;
    pThr->u32Id = g_u32NextId++;
    pThr->u32State = GJ_THR_RUNNABLE;
    sched_soft_note_ready();
    pThr->pProc = NULL;
    pThr->u64Cr3 = cpu_read_cr3();
    pThr->pfnEntry = NULL;
    pThr->u64KstackTop = thr_kstack_top(pThr);
    thr_plant_kstack_canary(pThr);
    thr_build_initial_rsp(pThr);
    g_apIdle[u32Cpu] = pThr;
    g_u64SoftCreateApIdle++;
    g_u64SoftCreateOk++;
    kprintf("sched: AP idle cpu=%u thr=%u\n", u32Cpu, pThr->u32Id);
    return 0;
}

struct gj_thread *
thread_current(void)
{
    return cpu_current_thread();
}

u32
thread_create(struct gj_process *pProc, void (*pfn)(void *), void *pArg)
{
    u32 iThr;
    struct gj_thread *pThr;

    /* pfn may be NULL for user-entry threads (GJ_THR_F_USER_ENTRY). */
    pThr = NULL;
    for (iThr = 1; iThr < GJ_MAX_THREADS; iThr++) {
        if (g_aThreads[iThr].u32State == GJ_THR_UNUSED ||
            g_aThreads[iThr].u32State == GJ_THR_EXITED) {
            if (g_aThreads[iThr].u32State == GJ_THR_EXITED) {
                g_u64SoftCreateReuse++;
            }
            pThr = &g_aThreads[iThr];
            break;
        }
    }
    if (pThr == NULL) {
        g_u64SoftCreateFull++;
        return 0;
    }
    memset(pThr, 0, sizeof(*pThr));
    g_aThrCpu[iThr] = 0; /* BSP until thread_set_cpu */
    g_aThrQos[iThr] = GJ_QOS_NORMAL;
    g_aThrBoost[iThr] = 0;
    g_aThrLastPick[iThr] = 0;
    g_aThrSoftTag[iThr][0] = '\0';
    pThr->u32Id = g_u32NextId++;
    pThr->u32State = GJ_THR_RUNNABLE;
    sched_soft_note_ready();
    pThr->pProc = pProc;
    pThr->u64Cr3 = pProc ? 0 : cpu_read_cr3(); /* 0 = inherit from process later */
    pThr->pfnEntry = pfn;
    pThr->pArg = pArg;
    pThr->u64KstackTop = thr_kstack_top(pThr);
    thr_plant_kstack_canary(pThr);
    /* u32SysUserValid left 0 - no mid-syscall state until first SYSCALL */
    thr_build_initial_rsp(pThr);
    g_u64SoftCreateOk++;
    kprintf("sched: create thr %u kstack=0x%lx sz=%u\n", pThr->u32Id,
            (unsigned long)pThr->u64KstackTop, GJ_THR_KSTACK_SIZE);
    return pThr->u32Id;
}

/*
 * Classify entry VA into soft tag class string (Soft!=product).
 * pe32 smokes stage at 0x52000000 / 0x51000000 / 0x55xxxxxx.
 * Ring3 smoke / init-like at 0x4000000..0x5000000.
 * Note: init.elf and sshd live also land in 0x04xxxxxx - auto class is
 * ring3 until main explicitly thread_soft_tag_set("init"|"sshd").
 * greppable: thread: soft tag | pe32_wow | ring3
 */
static const char *
thr_soft_tag_class_from_va(u64 u64Entry, int fUser32)
{
    if (u64Entry >= 0x52000000ull && u64Entry < 0x53000000ull) {
        return fUser32 ? "pe32_u32" : "pe32_wow";
    }
    if (u64Entry >= 0x51000000ull && u64Entry < 0x52000000ull) {
        return "pe32_img";
    }
    if (u64Entry >= 0x55000000ull && u64Entry < 0x56000000ull) {
        return "pe32_u32";
    }
    if (u64Entry >= 0x4000000ull && u64Entry < 0x5000000ull) {
        return "ring3";
    }
    if (u64Entry >= 0x10000000ull && u64Entry < 0x12000000ull) {
        return "user_elf";
    }
    return fUser32 ? "user32" : "user";
}

/*
 * Soft entry-class tag for no-COM1 STATUS kill panel.
 * greppable: thread: soft tag | pe32_wow | ring3
 * Soft!=product. Not product identity.
 */
static void
thr_soft_tag_from_entry(u32 u32ThrId, u64 u64Entry, int fUser32)
{
    thread_soft_tag_set(u32ThrId, thr_soft_tag_class_from_va(u64Entry, fUser32));
}

/*
 * STRONGER H3 thr soft tag sticky residual (Soft!=product; panel/no-COM1).
 * On exit_process sibling scrub: keep sticky tag; if empty, quiet-restamp
 * from entry VA class BEFORE USER_RIP clear so EXITED inventory + kill
 * panel retain pe32_wow|... named class. Never clears tags on thr_exit.
 * H3 thr_exit order stands (siblings before as_destroy). No stamp storm.
 */
static void
thr_soft_tag_h3_sticky_on_exit(u32 u32Idx, u64 u64Entry, u32 u32Flags)
{
    const char *sz;
    char *p;
    u32 i;
    int fUser32;

    if (u32Idx >= GJ_MAX_THREADS) {
        return;
    }
    p = g_aThrSoftTag[u32Idx];
    if (p[0] != '\0') {
        /* Sticky keep - do not scrub tag on thr_exit (kill panel residual). */
        g_u64SoftTagH3StickyKeep++;
        if (thr_soft_tag_eq(p, "pe32_wow") != 0) {
            g_u64SoftTagH3Pe32Wow++;
        }
        return;
    }
    /* Empty tag: quiet re-stamp from entry VA before USER_RIP clear. */
    if (u64Entry == 0ull) {
        return;
    }
    fUser32 = ((u32Flags & GJ_THR_F_USER32_ENTRY) != 0) ? 1 : 0;
    sz = thr_soft_tag_class_from_va(u64Entry, fUser32);
    for (i = 0; i < 11u && sz[i] != '\0'; i++) {
        p[i] = sz[i];
    }
    p[i] = '\0';
    thr_soft_tag_note_set(p);
    g_u64SoftTagH3Restamp++;
    if (thr_soft_tag_eq(p, "pe32_wow") != 0) {
        g_u64SoftTagH3Pe32Wow++;
    }
}

void
thread_soft_tag_set(u32 u32ThrId, const char *szTag)
{
    u32 iThr;
    u32 i;
    char *p;

    if (u32ThrId == 0 || szTag == NULL) {
        return;
    }
    for (iThr = 0; iThr < GJ_MAX_THREADS; iThr++) {
        if (g_aThreads[iThr].u32Id == u32ThrId) {
            p = g_aThrSoftTag[iThr];
            for (i = 0; i < 11u && szTag[i] != '\0'; i++) {
                p[i] = szTag[i];
            }
            p[i] = '\0';
            /* C3 soft tag residual: named-class set tallies (Soft!=product). */
            thr_soft_tag_note_set(p);
            kprintf("thread: soft tag thr=%u tag=%s "
                    "named=pe32_wow|sshd|init|ring3 soft_ne_product=1\n",
                    u32ThrId, p);
            return;
        }
    }
}

const char *
thread_soft_tag_get(u32 u32ThrId)
{
    u32 iThr;

    if (u32ThrId == 0) {
        return "";
    }
    for (iThr = 0; iThr < GJ_MAX_THREADS; iThr++) {
        if (g_aThreads[iThr].u32Id == u32ThrId) {
            return g_aThrSoftTag[iThr];
        }
    }
    return "";
}

u64
thread_user_entry_get(u32 u32ThrId)
{
    struct gj_thread *pThr = thr_find_by_id(u32ThrId);

    if (pThr == NULL) {
        return 0;
    }
    return pThr->u64UserRip;
}

u32
thread_flags_get(u32 u32ThrId)
{
    struct gj_thread *pThr = thr_find_by_id(u32ThrId);

    if (pThr == NULL) {
        return 0;
    }
    return pThr->u32Flags;
}

void
thread_set_clear_child_tid(u32 u32ThrId, u64 u64Ctid)
{
    struct gj_thread *pThr = thr_find_by_id(u32ThrId);

    if (pThr != NULL) {
        pThr->u64ClearChildTid = u64Ctid;
    }
}

void
thread_clear_child_tid_wake(struct gj_thread *pThr)
{
    u64 u64Ctid;
    struct gj_futex_key key;
    int fOk;

    if (pThr == NULL || pThr->u64ClearChildTid == 0) {
        return;
    }
    u64Ctid = pThr->u64ClearChildTid;
    pThr->u64ClearChildTid = 0;
    fOk = 0;
    if (user_range_ok(u64Ctid, sizeof(u32))) {
        if (user_range_mapped(u64Ctid, sizeof(u32)) &&
            user_store_u32(u64Ctid, 0) == GJ_OK) {
            fOk = 1;
        }
    } else {
        *(volatile u32 *)(gj_vaddr_t)u64Ctid = 0;
        fOk = 1;
    }
    if (fOk != 0 && futex_key_from_uaddr(&key, u64Ctid, 1) == GJ_OK) {
        (void)futex_wake(&key, 1);
    }
}

u32
thread_user_live_count(const struct gj_process *pProc)
{
    u32 iThr;
    u32 cLive;

    if (pProc == NULL) {
        return 0;
    }
    cLive = 0;
    for (iThr = 0; iThr < GJ_MAX_THREADS; iThr++) {
        struct gj_thread *pT = &g_aThreads[iThr];

        if (pT->pProc != pProc) {
            continue;
        }
        if (pT->u32State == GJ_THR_UNUSED || pT->u32State == GJ_THR_EXITED) {
            continue;
        }
        cLive++;
    }
    return cLive;
}

void
thread_set_fs_base(u32 u32ThrId, u64 u64FsBase)
{
    struct gj_thread *pThr = thr_find_by_id(u32ThrId);

    if (pThr != NULL) {
        pThr->u64FsBase = u64FsBase;
    }
}

u32
thread_create_user(struct gj_process *pProc, u64 u64Entry, u64 u64Stack)
{
    u32 u32Id;
    struct gj_thread *pThr;

    if (u64Entry == 0 || u64Stack == 0) {
        return 0;
    }
    u32Id = thread_create(pProc, NULL, NULL);
    if (u32Id == 0) {
        return 0;
    }
    pThr = thr_find_by_id(u32Id);
    if (pThr == NULL) {
        return 0;
    }
    pThr->u32Flags |= GJ_THR_F_USER_ENTRY;
    pThr->u64UserRip = u64Entry;
    pThr->u64UserRsp = u64Stack;
    pThr->pfnEntry = NULL;
    thr_soft_tag_from_entry(u32Id, u64Entry, 0);
    g_u64SoftCreateUser++;
    kprintf("sched: create user thr %u entry=0x%lx\n", u32Id,
            (unsigned long)u64Entry);
    return u32Id;
}

u32
thread_create_user32(struct gj_process *pProc, u64 u64Entry, u64 u64Stack)
{
    u32 u32Id;
    struct gj_thread *pThr;

    if (u64Entry == 0 || u64Stack == 0) {
        return 0;
    }
    u32Id = thread_create(pProc, NULL, NULL);
    if (u32Id == 0) {
        return 0;
    }
    pThr = thr_find_by_id(u32Id);
    if (pThr == NULL) {
        return 0;
    }
    pThr->u32Flags |= GJ_THR_F_USER32_ENTRY;
    pThr->u64UserRip = u64Entry;
    pThr->u64UserRsp = u64Stack;
    pThr->pfnEntry = NULL;
    thr_soft_tag_from_entry(u32Id, u64Entry, 1);
    g_u64SoftCreateUser32++;
    kprintf("sched: create user32 thr %u entry=0x%lx\n", u32Id,
            (unsigned long)u64Entry);
    return u32Id;
}

u32
thread_exec_replace(struct gj_process *pProc, u64 u64Entry, u64 u64Stack)
{
    u32 iThr;
    u32 cUpdated = 0;

    g_u64SoftExecCalls++;
    if (pProc == NULL || u64Entry == 0 || u64Stack == 0) {
        return 0;
    }
    for (iThr = 0; iThr < GJ_MAX_THREADS; iThr++) {
        struct gj_thread *pThr = &g_aThreads[iThr];

        if (pThr->u32State == GJ_THR_UNUSED || pThr->u32State == GJ_THR_EXITED) {
            continue;
        }
        if (pThr->pProc != pProc) {
            continue;
        }
        if ((pThr->u32Flags & (GJ_THR_F_USER_ENTRY | GJ_THR_F_USER32_ENTRY)) ==
            0) {
            continue;
        }
        pThr->u64UserRip = u64Entry;
        pThr->u64UserRsp = u64Stack;
        cUpdated++;
    }
    if (cUpdated > 0) {
        g_u64SoftExecReplace += (u64)cUpdated;
        kprintf("sched: exec_replace n=%u entry=0x%lx sp=0x%lx\n", cUpdated,
                (unsigned long)u64Entry, (unsigned long)u64Stack);
    }
    return cUpdated;
}

u32
thread_exit_process(struct gj_process *pProc)
{
    u32 iThr;
    u32 cExited = 0;
    u32 cUser = 0;
    u32 cUser32 = 0;
    u32 cRunnable = 0;
    u32 cRunning = 0;
    u32 cBlocked = 0;
    u32 cDriver = 0;
    struct gj_thread *pCur = thread_current();

    g_u64SoftThrExitProcCalls++;
    if (pProc == NULL) {
        g_u64SoftThrExitNull++;
        return 0;
    }
    for (iThr = 0; iThr < GJ_MAX_THREADS; iThr++) {
        struct gj_thread *pThr = &g_aThreads[iThr];
        u32 u32Idx;
        u32 u32St;
        u32 u32Flags;

        if (pThr == pCur) {
            continue; /* caller (exit/death) finishes itself */
        }
        if (pThr->u32State == GJ_THR_UNUSED || pThr->u32State == GJ_THR_EXITED) {
            continue;
        }
        if (pThr->pProc != pProc) {
            continue;
        }
        /*
         * H3 residual lean scrub (Soft!=product; UDX host teardown deepen):
         * Cancel first-entry so trampoline never iretq into freed maps.
         * Drop mid-syscall USER_*, block key, and thr CR3 so schedule cannot
         * resume into AS about to be destroyed. Detach pProc so resume path
         * cannot process_as_activate a dying PCB (skip user if !alive belt).
         * Soft residual != product multi-CPU thr-kill (RUNNING-on-remote soft
         * race still belt-only). UDX multi-thr hosts (rtl8168_udx / xhci_udx /
         * ddi_host): IRQ/work thr share one process AS - sibling drain is
         * thr-unit half of thr_exit_before_as_destroy=1.
         * STRONGER thr soft tags H3: sticky tag keep / entry re-stamp BEFORE
         * USER_RIP clear (kill panel + EXITED inventory retain pe32_wow).
         * Tags never scrubbed here - sticky_until_reuse only.
         */
        u32St = pThr->u32State;
        u32Flags = pThr->u32Flags;
        u32Idx = thr_index(pThr);
        if (u32St == GJ_THR_RUNNABLE) {
            g_u64SoftThrExitRunnable++;
            cRunnable++;
        } else if (u32St == GJ_THR_RUNNING) {
            g_u64SoftThrExitRunning++;
            cRunning++;
        } else if (u32St == GJ_THR_BLOCKED) {
            g_u64SoftThrExitBlocked++;
            cBlocked++;
        }
        if ((u32Flags & GJ_THR_F_USER_ENTRY) != 0) {
            g_u64SoftThrExitUser++;
            cUser++;
        }
        if ((u32Flags & GJ_THR_F_USER32_ENTRY) != 0) {
            g_u64SoftThrExitUser32++;
            cUser32++;
        }
        if (pThr->u32SysUserValid != 0) {
            g_u64SoftThrExitSysUser++;
        }
        if (pThr->u64Cr3 != 0) {
            g_u64SoftThrExitCr3++;
        }
        if (pThr->pBlockObj != NULL) {
            g_u64SoftThrExitBlockObj++;
        }
        if (u32Idx < GJ_MAX_THREADS && g_aThrQos[u32Idx] == GJ_QOS_DRIVER) {
            g_u64SoftThrExitDriver++;
            cDriver++;
        }
        thread_clear_child_tid_wake(pThr);
        /* STRONGER H3 tag sticky residual (before USER_RIP scrub). */
        thr_soft_tag_h3_sticky_on_exit(u32Idx, pThr->u64UserRip, u32Flags);
        pThr->u32Flags &= ~(GJ_THR_F_USER_ENTRY | GJ_THR_F_USER32_ENTRY);
        pThr->u64UserRip = 0;
        pThr->u64UserRsp = 0;
        pThr->u32SysUserValid = 0;
        pThr->u64SysUserRip = 0;
        pThr->u64SysUserRsp = 0;
        pThr->u64SysUserRflags = 0;
        pThr->pBlockObj = NULL;
        pThr->u32BlockTag = 0;
        pThr->u64Cr3 = 0;
        /* Detach dying PCB - skip-user residual (resume cannot as_activate). */
        pThr->pProc = NULL;
        g_u64SoftThrExitDetach++;
        pThr->u32State = GJ_THR_EXITED;
        pThr->pfnEntry = NULL;
        if (u32Idx < GJ_MAX_THREADS) {
            g_aThrBoost[u32Idx] = 0;
            /* Soft tag intentionally NOT cleared - sticky_until_reuse. */
        }
        door_on_thread_exit(pThr);
        sched_soft_note_exit();
        cExited++;
    }
    g_u64SoftThrExitProcN += (u64)cExited;
    if (cExited == 0) {
        /* Idempotent barrier / single-thr death - still thr_exit before AS. */
        g_u64SoftThrExitZero++;
    } else if (cExited >= 2u) {
        /* Multi-thr UDX host class (work + IRQ thr share AS). */
        g_u64SoftThrExitMulti++;
    }
    if (cExited > 0) {
        /*
         * H3 death residual - preserve (clone sibling / UDX host dead AS).
         * Caller (process_death) must keep thr_exit before as_destroy.
         * Grep: thread: exit_process | sched: exit_process
         * Grep: process: death thr_exit (caller) | thr skip user dead AS
         * Grep: thr_exit_before_as_destroy=1 | udx_host_teardown=1
         * Grep: hosts=rtl8168_udx|xhci_udx|ddi_host
         */
        kprintf("thread: exit_process n=%u runnable=%u running=%u blocked=%u "
                "user=%u user32=%u driver=%u "
                "(siblings; H3 death residual; "
                "thr_exit_before_as_destroy=1 udx_host_teardown=1; "
                "hosts=rtl8168_udx|xhci_udx|ddi_host; Soft!=product)\n",
                cExited, cRunnable, cRunning, cBlocked, cUser, cUser32,
                cDriver);
        kprintf("sched: exit_process n=%u H3=death_residual "
                "thr_exit_before_as_destroy=1 udx_host_teardown=1 "
                "multi_thr=%u soft_ne_product=1\n",
                cExited, (cExited >= 2u) ? 1u : 0u);
        /*
         * Grep: thread: soft thr_exit residual lean
         * First real sibling drain fires residual lean once (Soft!=product).
         * Inventory path also arms this; no stamp storms.
         */
        if (g_fThrExitResLeanOnce == 0) {
            g_fThrExitResLeanOnce = 1;
            kprintf("thread: soft thr_exit residual lean H3 "
                    "thr_exit_before_as_destroy=1 udx_host_teardown=1 "
                    "n=%u multi_thr=%u "
                    "scrub=user|user32|sysuser|cr3|block|detach "
                    "skip_user_if_not_alive=1 "
                    "hosts=rtl8168_udx|xhci_udx|ddi_host "
                    "product_multi_cpu_kill=0 product_udx_close=0 "
                    "Soft!=product dual=MIT/Apache wave=%u\n",
                    cExited, (cExited >= 2u) ? 1u : 0u,
                    (unsigned)THREAD_SOFT_DEEPEN_WAVE);
        }
    }
    return cExited;
}

u32
thread_get_state(u32 u32Id)
{
    struct gj_thread *pThr = thr_find_by_id(u32Id);

    if (pThr == NULL) {
        return GJ_THR_UNUSED;
    }
    return pThr->u32State;
}

void
thread_block(void *pBlockObj, u32 u32Tag)
{
    struct gj_thread *pThr = thread_current();

    if (pThr == NULL) {
        return;
    }
    pThr->pBlockObj = pBlockObj;
    pThr->u32BlockTag = u32Tag;
    pThr->u32State = GJ_THR_BLOCKED;
    g_u64SoftBlockN++;
    sched_soft_note_block();
}

u32
thread_wake(void *pBlockObj, u32 u32Tag, u32 u32Max)
{
    u32 iThr;
    u32 u32N = 0;

    g_u64SoftWakeCalls++;
    for (iThr = 0; iThr < GJ_MAX_THREADS && u32N < u32Max; iThr++) {
        struct gj_thread *pThr = &g_aThreads[iThr];

        if (pThr->u32State != GJ_THR_BLOCKED) {
            continue;
        }
        if (pThr->pBlockObj != pBlockObj) {
            continue;
        }
        if (u32Tag != 0 && pThr->u32BlockTag != u32Tag) {
            continue;
        }
        pThr->pBlockObj = NULL;
        pThr->u32BlockTag = 0;
        pThr->u32State = GJ_THR_RUNNABLE;
        sched_soft_note_ready();
        g_u64SoftWakeThr++;
        u32N++;
    }
    if (u32N == 0) {
        g_u64SoftWakeNone++;
    }
    return u32N;
}

static int
thr_ok_on_cpu(u32 u32Idx, u32 u32Cpu)
{
    u8 u8Aff = g_aThrCpu[u32Idx];

    /* 0xFF = any CPU; else must match affinity (default 0 = BSP only). */
    if (u8Aff == 0xFFu) {
        return 1;
    }
    return u8Aff == (u8)u32Cpu;
}

static struct gj_thread *
cpu_idle(u32 u32Cpu)
{
    if (u32Cpu < GJ_CPU_STATIC_MAX && g_apIdle[u32Cpu] != NULL) {
        return g_apIdle[u32Cpu];
    }
    if (u32Cpu == 0) {
        return g_pIdle;
    }
    return NULL;
}

/*
 * Base QoS -> rank (higher first). Historical 0/1/2 order preserved:
 * interactive > normal > background. Soft deepen adds utility + driver.
 */
static u8
qos_rank(u8 u8Qos)
{
    /* DRIVER(4) > INTERACTIVE(1) > NORMAL(0) > UTILITY(3) > BACKGROUND(2) */
    if (u8Qos == GJ_QOS_DRIVER) {
        return 4;
    }
    if (u8Qos == GJ_QOS_INTERACTIVE) {
        return 3;
    }
    if (u8Qos == GJ_QOS_UTILITY) {
        return 1;
    }
    if (u8Qos == GJ_QOS_BACKGROUND) {
        return 0;
    }
    /* NORMAL and unknown -> middle-high default */
    return 2;
}

static u8
qos_rank_eff(u32 u32Idx)
{
    u8 u8R;

    if (u32Idx >= GJ_MAX_THREADS) {
        return 0;
    }
    u8R = qos_rank(g_aThrQos[u32Idx]);
    /* Soft capped PI: residual boost lifts rank but never past driver. */
    if (g_aThrBoost[u32Idx] != 0) {
        u8 u8Cap = 4;

        if ((u16)u8R + (u16)g_aThrBoost[u32Idx] >= (u16)u8Cap) {
            u8R = u8Cap;
        } else {
            u8R = (u8)(u8R + g_aThrBoost[u32Idx]);
        }
    }
    return u8R;
}

static void
pick_note_class(u8 u8Qos)
{
    if (u8Qos == GJ_QOS_INTERACTIVE) {
        g_soft.u64PickInteractive++;
    } else if (u8Qos == GJ_QOS_BACKGROUND) {
        g_soft.u64PickBackground++;
    } else if (u8Qos == GJ_QOS_UTILITY) {
        g_soft.u64PickUtility++;
    } else if (u8Qos == GJ_QOS_DRIVER) {
        g_soft.u64PickDriver++;
    } else {
        g_soft.u64PickNormal++;
    }
}

static void
pick_soft_maybe_dump(void)
{
    /* One-shot warm soft dump after enough picks (greppable product depth). */
    if (g_fSoftStatsOnce == 0 && g_soft.u64PickTotal >= 64ull) {
        g_fSoftStatsOnce = 1;
        /* stats_print deepens inventory; avoid a second warm inventory line. */
        g_fSoftInvOnce = 1;
        (void)thread_sched_soft_stats_print();
    }
    /* Sample ready/run HWM periodically without kprintf spam. */
    if ((g_soft.u64PickTotal & 15ull) == 0ull) {
        sched_soft_inventory_scan();
    }
    if (g_fSoftInvOnce == 0 && g_soft.u64PickTotal >= 64ull) {
        g_fSoftInvOnce = 1;
        sched_soft_inventory_print();
    }
}

static struct gj_thread *
pick_next(void)
{
    static u32 g_aRobin[GJ_CPU_STATIC_MAX];
    u32 iThr;
    u32 u32Cpu = cpu_id();
    u32 u32Start;
    struct gj_thread *pIdle;
    struct gj_thread *pBest = NULL;
    struct gj_thread *pCur;
    u32 u32BestIdx = 0;
    u8 u8BestRank = 0;
    u32 u32BestWait = 0;

    if (u32Cpu >= GJ_CPU_STATIC_MAX) {
        u32Cpu = 0;
    }
    u32Start = g_aRobin[u32Cpu];

    for (iThr = 0; iThr < GJ_MAX_THREADS; iThr++) {
        u32 u32Idx = (u32Start + iThr) % GJ_MAX_THREADS;
        struct gj_thread *pThr = &g_aThreads[u32Idx];
        u8 u8R;
        u32 u32Wait;

        if (!thr_ok_on_cpu(u32Idx, u32Cpu)) {
            g_soft.u64PickAffSkip++;
            continue;
        }
        /* Idle threads only when nothing else is runnable */
        if (pThr == cpu_idle(u32Cpu)) {
            continue;
        }
        /*
         * Only RUNNABLE thr are candidates. A RUNNING current is not
         * re-picked here (self-switch is a no-op in schedule).
         */
        if (pThr->u32State != GJ_THR_RUNNABLE) {
            continue;
        }
        u8R = qos_rank_eff(u32Idx);
        /* Soft wait-age: gen delta since last pick (equal-rank fairness). */
        u32Wait = g_u32PickGen - g_aThrLastPick[u32Idx];
        /*
         * Odd pick gen: longest wait, ignore QoS. Two GJ_QOS_DRIVER hosts
         * (rtl8168_udx + xhci_udx) otherwise ping-pong on yield and starve
         * sshd (NORMAL). 0.1.154: 3WHS Connected, 0-byte ident — Gap C had
         * written the banner from the UDX syscall. Soft!=product.
         */
        if ((g_u32PickGen & 1u) != 0u) {
            if (pBest == NULL || u32Wait > u32BestWait) {
                pBest = pThr;
                u32BestIdx = u32Idx;
                u8BestRank = u8R;
                u32BestWait = u32Wait;
            }
        } else if (pBest == NULL || u8R > u8BestRank ||
                   (u8R == u8BestRank && u32Wait > u32BestWait)) {
            if (pBest != NULL && u8R == u8BestRank &&
                u32Wait > u32BestWait) {
                g_soft.u64PickEqualFair++;
            }
            pBest = pThr;
            u32BestIdx = u32Idx;
            u8BestRank = u8R;
            u32BestWait = u32Wait;
        }
    }
    if (pBest != NULL) {
        g_aRobin[u32Cpu] = (u32BestIdx + 1) % GJ_MAX_THREADS;
        g_aThrLastPick[u32BestIdx] = g_u32PickGen;
        g_u32PickGen++;
        g_soft.u64PickTotal++;
        pick_note_class(g_aThrQos[u32BestIdx]);
        pick_soft_maybe_dump();
        return pBest;
    }
    pIdle = cpu_idle(u32Cpu);
    if (pIdle != NULL && pIdle->u32State != GJ_THR_EXITED) {
        u32 u32IdleIdx;

        if (pIdle->u32State == GJ_THR_BLOCKED ||
            pIdle->u32State == GJ_THR_UNUSED) {
            pIdle->u32State = GJ_THR_RUNNABLE;
            sched_soft_note_ready();
        }
        u32IdleIdx = thr_index(pIdle);
        if (u32IdleIdx < GJ_MAX_THREADS) {
            g_aThrLastPick[u32IdleIdx] = g_u32PickGen;
        }
        g_u32PickGen++;
        g_soft.u64PickTotal++;
        g_soft.u64PickIdle++;
        pick_soft_maybe_dump();
        return pIdle;
    }
    pCur = thread_current();
    g_soft.u64PickTotal++;
    g_soft.u64PickSelf++;
    pick_soft_maybe_dump();
    return pCur;
}

void
thread_set_qos(u32 u32ThrId, u32 u32Qos)
{
    u32 iThr;

    for (iThr = 0; iThr < GJ_MAX_THREADS; iThr++) {
        if (g_aThreads[iThr].u32Id == u32ThrId &&
            g_aThreads[iThr].u32State != GJ_THR_UNUSED) {
            if (u32Qos <= GJ_QOS_CLASS_MAX) {
                g_aThrQos[iThr] = (u8)u32Qos;
            } else {
                g_aThrQos[iThr] = GJ_QOS_NORMAL;
                g_soft.u64QosClamp++;
            }
            g_soft.u64QosSet++;
            return;
        }
    }
}

u32
thread_get_qos(u32 u32ThrId)
{
    u32 iThr;

    for (iThr = 0; iThr < GJ_MAX_THREADS; iThr++) {
        if (g_aThreads[iThr].u32Id == u32ThrId) {
            return g_aThrQos[iThr];
        }
    }
    return GJ_QOS_NORMAL;
}

void
thread_qos_boost_soft(u32 u32ThrId, u32 u32Ticks)
{
    u32 iThr;
    u8 u8Add;

    if (u32Ticks == 0) {
        return;
    }
    u8Add = (u8)(u32Ticks > GJ_QOS_BOOST_CAP ? GJ_QOS_BOOST_CAP : u32Ticks);
    for (iThr = 0; iThr < GJ_MAX_THREADS; iThr++) {
        if (g_aThreads[iThr].u32Id == u32ThrId &&
            g_aThreads[iThr].u32State != GJ_THR_UNUSED) {
            u16 u16Sum = (u16)g_aThrBoost[iThr] + (u16)u8Add;

            if (u16Sum > GJ_QOS_BOOST_CAP) {
                g_aThrBoost[iThr] = (u8)GJ_QOS_BOOST_CAP;
            } else {
                g_aThrBoost[iThr] = (u8)u16Sum;
            }
            g_soft.u64QosBoostSoft++;
            return;
        }
    }
}

u32
thread_qos_effective_rank(u32 u32ThrId)
{
    u32 iThr;

    for (iThr = 0; iThr < GJ_MAX_THREADS; iThr++) {
        if (g_aThreads[iThr].u32Id == u32ThrId &&
            g_aThreads[iThr].u32State != GJ_THR_UNUSED) {
            return (u32)qos_rank_eff(iThr);
        }
    }
    return 0;
}

void
thread_sched_soft_stats_get(struct gj_sched_soft_stats *pOut)
{
    if (pOut == NULL) {
        return;
    }
    *pOut = g_soft;
}

u64
thread_sched_soft_stats_print(void)
{
    /* Grep: sched: soft stats */
    kprintf("sched: soft stats pick=%lu idle=%lu int=%lu norm=%lu bg=%lu "
            "util=%lu drv=%lu aff_skip=%lu eq_fair=%lu self=%lu "
            "qos_set=%lu qos_clamp=%lu boost=%lu decay=%lu "
            "can_plant=%lu can_chk=%lu can_ok=%lu can_mid=%lu can_fail=%lu "
            "hwm_max=%lu hwm_n=%lu wave=%u\n",
            (unsigned long)g_soft.u64PickTotal,
            (unsigned long)g_soft.u64PickIdle,
            (unsigned long)g_soft.u64PickInteractive,
            (unsigned long)g_soft.u64PickNormal,
            (unsigned long)g_soft.u64PickBackground,
            (unsigned long)g_soft.u64PickUtility,
            (unsigned long)g_soft.u64PickDriver,
            (unsigned long)g_soft.u64PickAffSkip,
            (unsigned long)g_soft.u64PickEqualFair,
            (unsigned long)g_soft.u64PickSelf,
            (unsigned long)g_soft.u64QosSet,
            (unsigned long)g_soft.u64QosClamp,
            (unsigned long)g_soft.u64QosBoostSoft,
            (unsigned long)g_soft.u64QosBoostDecay,
            (unsigned long)g_soft.u64CanaryPlant,
            (unsigned long)g_soft.u64CanaryCheck,
            (unsigned long)g_soft.u64CanaryOk,
            (unsigned long)g_soft.u64CanaryMidOk,
            (unsigned long)g_soft.u64CanaryFail,
            (unsigned long)g_soft.u64StackHwmMax,
            (unsigned long)g_soft.u64StackHwmSamples,
            (unsigned)THREAD_SOFT_DEEPEN_WAVE);
    /* Wave 15: full greppable thread: soft ... inventory alongside pick stats. */
    sched_soft_inventory_print();
    return g_soft.u64PickTotal;
}

void
thread_resched_cpu(u32 u32Cpu)
{
    if (u32Cpu == cpu_id()) {
        return;
    }
    apic_send_resched(u32Cpu);
}

void
thread_set_cpu(u32 u32ThrId, u32 u32Cpu)
{
    u32 iThr;

    for (iThr = 0; iThr < GJ_MAX_THREADS; iThr++) {
        if (g_aThreads[iThr].u32Id == u32ThrId &&
            g_aThreads[iThr].u32State != GJ_THR_UNUSED) {
            g_aThrCpu[iThr] = (u8)u32Cpu;
            g_u64SoftSetCpuN++;
            return;
        }
    }
}

u32
thread_create_on_cpu(struct gj_process *pProc, void (*pfn)(void *), void *pArg,
                     u32 u32Cpu)
{
    u32 u32Id = thread_create(pProc, pfn, pArg);

    if (u32Id != 0) {
        thread_set_cpu(u32Id, u32Cpu);
        g_u64SoftCreateOnCpu++;
    }
    return u32Id;
}

/*
 * Snapshot per-CPU GS USER_* onto the thr before switch. Without this,
 * thr B's SYSCALL overwrites GS and thr A's later sysretq uses B's return
 * target (wrong user RIP/RSP -> faults / #UD-class fallout).
 */
static void
thread_save_sys_user(struct gj_thread *pThr)
{
    struct gj_cpu *pCpu;

    if (pThr == NULL) {
        return;
    }
    pCpu = cpu_current();
    if (pCpu == NULL) {
        return;
    }
    pThr->u64SysUserRip = pCpu->u64UserRip;
    pThr->u64SysUserRsp = pCpu->u64UserRsp;
    pThr->u64SysUserRflags = pCpu->u64UserRflags;
    pThr->u32SysUserValid = 1;
    g_u64SoftSysUserSave++;
}

static void
thread_restore_sys_user(struct gj_thread *pThr)
{
    struct gj_cpu *pCpu;

    if (pThr == NULL || pThr->u32SysUserValid == 0) {
        return;
    }
    pCpu = cpu_current();
    if (pCpu == NULL) {
        return;
    }
    pCpu->u64UserRip = pThr->u64SysUserRip;
    pCpu->u64UserRsp = pThr->u64SysUserRsp;
    pCpu->u64UserRflags = pThr->u64SysUserRflags;
    g_u64SoftSysUserRest++;
}

void
schedule(void)
{
    struct gj_thread *pCur;
    struct gj_thread *pNext;
    struct gj_thread *pNow;
    u64 u64OldRsp;
    u64 u64KerCr3;

    g_u64SoftSchedEnter++;
    pCur = thread_current();
    /*
     * pick_next only considers GJ_THR_RUNNABLE, not RUNNING. If the
     * yielder stays RUNNING, OpenSSH ppoll's thread_yield always
     * selects idle and may never resume the listen walk after AcceptQ.
     * Park the current RUNNING thread first so it can be re-picked
     * (self-continue the ppoll loop). Dual DoD B OPEN.
     */
    if (pCur != NULL && pCur->u32State == GJ_THR_RUNNING) {
        pCur->u32State = GJ_THR_RUNNABLE;
        sched_soft_note_ready();
    }
    pNext = pick_next();
    if (pNext == NULL) {
        if (pCur != NULL && pCur->u32State == GJ_THR_RUNNABLE) {
            pCur->u32State = GJ_THR_RUNNING;
        }
        g_u64SoftSchedSelf++;
        return;
    }
    if (pCur == pNext) {
        if (pCur != NULL && pCur->u32State == GJ_THR_BLOCKED) {
            /* Spin briefly for a wake (cooperative; no IRQ-driven unblock). */
            g_u64SoftSchedSpin++;
            while (pick_next() == pCur && pCur->u32State == GJ_THR_BLOCKED) {
                __asm__ volatile ("pause");
            }
            pNext = pick_next();
            if (pNext == pCur) {
                g_u64SoftSchedSelf++;
                return;
            }
        } else {
            if (pCur != NULL && pCur->u32State == GJ_THR_RUNNABLE) {
                pCur->u32State = GJ_THR_RUNNING;
            }
            g_u64SoftSchedSelf++;
            return;
        }
    }

    if (pCur != NULL) {
        u32 u32CurIdx;

        thread_check_kstack(pCur);
        if (pCur->u32State == GJ_THR_RUNNING) {
            pCur->u32State = GJ_THR_RUNNABLE;
            sched_soft_note_ready();
        }
        /* Soft PI decay: one boost tick per leave (capped residual). */
        u32CurIdx = thr_index(pCur);
        if (u32CurIdx < GJ_MAX_THREADS && g_aThrBoost[u32CurIdx] != 0) {
            g_aThrBoost[u32CurIdx]--;
            g_soft.u64QosBoostDecay++;
        }
        /* Preserve SYSCALL return target across thr switch (per-CPU otherwise). */
        thread_save_sys_user(pCur);
    }
    pNext->u32State = GJ_THR_RUNNING;
    sched_soft_note_run();
    g_u64SoftSchedSwitch++;

    /*
     * Mark next current *before* switch so trampoline / thread_current()
     * see the right thr - but do NOT install SYSCALL kstack yet.
     * cpu_set_current_thread deliberately leaves u64KernelRsp alone;
     * thread_install_kstack runs only after switch_context returns here.
     *
     * Switch under kernel CR3 so thr stacks (kernel BSS) are always mapped.
     * Keep TSS.RSP0 on the dedicated IRQ stack so a parked thr's aKstack is
     * never the ring-3 interrupt stack of another thr.
     */
    cpu_set_current_thread(pNext);
    tss_use_irq_rsp0();
    u64KerCr3 = vmm_kernel_cr3() ? vmm_kernel_cr3() : cpu_read_cr3();
    cpu_load_cr3(u64KerCr3);
    vmm_set_anon_cursor(NULL);

    if (pCur == NULL) {
        u64OldRsp = 0;
        switch_context(&u64OldRsp, pNext->u64Rsp);
    } else {
        switch_context(&pCur->u64Rsp, pNext->u64Rsp);
    }

    /* Resumed on this thr's stack (locals are this schedule() invocation). */
    pNow = thread_current();
    if (pNow == NULL) {
        pNow = pNext;
        cpu_set_current_thread(pNow);
    }
    thread_check_kstack(pNow);
    cpu_set_fs_base(pNow->u64FsBase);
    /*
     * H3 residual belt (Soft!=product; thr_exit / skip user if !alive):
     * Trampoline covers first USER*_ENTRY. Mid-syscall resume must also
     * refuse sysuser restore + process_as_activate when the bound process
     * is already dead (u32Alive==0) or maps torn (u64Cr3==0). Else a
     * sibling that missed thr_exit scrub could sysret into AS about to be
     * / already freed (clone_vm / UDX multi-thr host class).
     * Kernel thr finishing process_death (no userish state) stay on ker
     * CR3 without force-exit. Soft residual != product multi-CPU thr-kill.
     * greppable: sched: thr skip user resume dead AS
     *            thr_exit_before_as_destroy=1 | H3=death_residual
     *            skip_user_if_not_alive=1 | udx_host_teardown=1
     */
    if (pNow->pProc != NULL &&
        (pNow->pProc->u32Alive == 0 || pNow->pProc->u64Cr3 == 0)) {
        int fUserish =
            (pNow->u32SysUserValid != 0) ||
            ((pNow->u32Flags &
              (GJ_THR_F_USER_ENTRY | GJ_THR_F_USER32_ENTRY)) != 0);
        const char *pszClass;

        g_u64SoftDeadAsSkip++;
        g_u64SoftDeadAsSkipResume++;
        if (pNow->pProc->u32Alive == 0) {
            g_u64SoftDeadAsSkipAlive++;
            pszClass = "alive0";
        } else {
            g_u64SoftDeadAsSkipCr3++;
            pszClass = "cr3_0";
        }
        pNow->u32Flags &= ~(GJ_THR_F_USER_ENTRY | GJ_THR_F_USER32_ENTRY);
        pNow->u32SysUserValid = 0;
        pNow->u64SysUserRip = 0;
        pNow->u64SysUserRsp = 0;
        pNow->u64SysUserRflags = 0;
        pNow->u64UserRip = 0;
        pNow->u64UserRsp = 0;
        pNow->u64Cr3 = 0;
        cpu_load_cr3(u64KerCr3);
        vmm_set_anon_cursor(NULL);
        thread_install_kstack(pNow);
        if (fUserish) {
            kprintf("sched: thr %u skip user resume dead AS class=%s "
                    "alive=%u cr3=0x%lx H3=death_residual "
                    "thr_exit_before_as_destroy=1 udx_host_teardown=1 "
                    "skip_user_if_not_alive=1 soft_ne_product=1\n",
                    pNow->u32Id, pszClass, pNow->pProc->u32Alive,
                    (unsigned long)pNow->pProc->u64Cr3);
            thread_exit();
            /* not reached */
        }
        /* Kernel thr on dead PCB (e.g. death path) - no user return. */
        return;
    }
    if (pNow->pProc != NULL && pNow->pProc->u64Cr3 != 0) {
        process_as_activate(pNow->pProc);
        g_u64SoftSchedAsAct++;
    } else if (pNow->u64Cr3 != 0) {
        cpu_load_cr3(pNow->u64Cr3);
        vmm_set_anon_cursor(NULL);
        g_u64SoftSchedCr3Own++;
    } else {
        cpu_load_cr3(u64KerCr3);
        vmm_set_anon_cursor(NULL);
    }
    /* Restore this thr's SYSCALL return target, then its SYSCALL kstack. */
    thread_restore_sys_user(pNow);
    thread_install_kstack(pNow);
}

void
thread_yield(void)
{
    g_u64SoftYieldN++;
    g_fYieldReq = 0;
    schedule();
}

void
thread_yield_request(void)
{
    g_u64SoftYieldReq++;
    g_fYieldReq = 1;
}

int
thread_yield_pending(void)
{
    int fPending = g_fYieldReq;

    if (fPending) {
        g_fYieldReq = 0;
        g_u64SoftYieldPendHit++;
    }
    return fPending;
}

void
thread_exit(void)
{
    struct gj_thread *pThr = thread_current();

    g_u64SoftExitN++;
    if (pThr != NULL) {
        thread_clear_child_tid_wake(pThr);
        pThr->u32State = GJ_THR_EXITED;
        pThr->pfnEntry = NULL;
        /* Defensive: do not restore stale USER_* if slot is reused later. */
        pThr->u32SysUserValid = 0;
        /* Drop cold-door roles so callers never CAS-hang on this thr. */
        door_on_thread_exit(pThr);
        sched_soft_note_exit();
    }
    schedule();
    for (;;) {
        __asm__ volatile ("hlt");
    }
}

/*
 * H1 residual lean (Soft!=product; G-AC-1):
 * Sole thr-stack call site for net_eth_poll in this unit. Invoked every
 * scheduler_run pass only. Never from timer/APIC/MSI-X/IRQ handlers, never
 * from scheduler_run_ap. Silent tally (wrap OK); no kprintf on this path
 * (stamp-storm / IRQ-stack-smash class).
 *
 * freestanding class SKIP; product UDX; poll ownership still thr-stack
 * for virtio/UDX later (H1 stands independent of freestanding class SKIP).
 *
 * Grep: net_eth_poll=run_loop_only | net_eth_irq=0 | owner=scheduler_run
 *       freestanding_class=SKIP | product=UDX | poll_own=thr_stack
 *       virtio_udx_later=1 | stack=thr
 */
static void
sched_thr_net_eth_poll(void)
{
    g_u64SoftEthPollN++;
    net_eth_poll();
}

void
scheduler_run(void)
{
    /*
     * H1 residual lean (Soft!=product; G-AC-1):
     * net_eth_poll only on scheduler_run thr stack - never timer/IRQ
     * (IRQ stack smash / #PF I=1 class). This unit owns thr-stack site.
     * freestanding class SKIP; product UDX; thr-stack for virtio/UDX later.
     * No stamp storms in-loop. Grep: sched: run loop
     *   | net_eth_poll=run_loop_only | net_eth_irq=0 | owner=scheduler_run
     *   | freestanding_class=SKIP | product=UDX | poll_own=thr_stack
     *   | virtio_udx_later=1
     */
    kprintf("sched: run loop (idle HLT when idle; net_eth_poll every pass "
            "on thr stack only; H1 residual lean; "
            "net_eth_poll=run_loop_only net_eth_irq=0 "
            "owner=scheduler_run stack=thr freestanding_class=SKIP "
            "product=UDX poll_own=thr_stack virtio_udx_later=1; "
            "not IRQ; Soft!=product; G-AC-1)\n");
    for (;;) {
        /* Full thr stack - H1 poll own; never IRQ. virtio/UDX later same. */
        sched_thr_net_eth_poll();
        session_input_poll(); /* fan-in virtio-input for session */
        /*
         * Product laptop attention / power (poll thr only; never IRQ):
         * Ctrl+Alt+Del → reboot; ACPI power button → S5 shutdown.
         * greppable: platform_power_poll | owner=scheduler_run
         */
        platform_power_poll();
        (void)thread_yield_pending(); /* clear soft-preempt flag */
        schedule();
        /* G-SCHED: idle with HLT when nothing runnable besides us */
        {
            struct gj_thread *pNext = pick_next();
            struct gj_thread *pIdle = cpu_idle(cpu_id());

            if (pNext == pIdle || pNext == thread_current()) {
                if (timer_ready()) {
                    __asm__ volatile ("sti; hlt" ::: "memory");
                } else {
                    __asm__ volatile ("pause");
                }
            } else {
                __asm__ volatile ("pause");
            }
        }
    }
}

void
scheduler_run_ap(void)
{
    u32 u32Cpu = cpu_id();
    struct gj_thread *pIdle = cpu_idle(u32Cpu);

    /*
     * AP idle + schedule only. No net_eth_poll here - H1 thr-stack poll
     * ownership is BSP scheduler_run only (Soft!=product; G-AC-1).
     * freestanding class SKIP; product UDX; virtio/UDX later still thr-stack.
     * Grep: sched: AP run loop
     */
    kprintf("sched: AP run loop cpu=%u idle=%u (no net_eth_poll; "
            "H1 poll=BSP scheduler_run thr only; freestanding_class=SKIP "
            "product=UDX poll_own=thr_stack virtio_udx_later=1; "
            "Soft!=product; G-AC-1)\n",
            u32Cpu, pIdle != NULL ? pIdle->u32Id : 0);
    if (pIdle != NULL) {
        pIdle->u32State = GJ_THR_RUNNING;
        sched_soft_note_run();
        cpu_set_current_thread(pIdle);
        /* SYSCALL stack for AP idle; RSP0 still dedicated IRQ (shared TSS). */
        thread_install_kstack(pIdle);
    }
    for (;;) {
        /* Deliberately no net_eth_poll - H1 thr-stack BSP owner only. */
        smp_ap_poll_work();
        schedule();
        {
            struct gj_thread *pNext = pick_next();

            if (pNext == pIdle || pNext == thread_current()) {
                __asm__ volatile ("hlt");
            } else {
                __asm__ volatile ("pause");
            }
        }
    }
}
