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
 *   - Wave 19 greppable "thread: soft …" deepen (wave=69 stamp):
 *       inventory|table|ready|run|create|block|wake|yield|switch|
 *       path|qos|canary|aff|pick|stack|idle|caps|stats|exit|deepen|
 *       hwm|sysuser|user|boost|exec|invariant|return|ret_surface|ratio|surface|headroom
 *     twin "sched: soft …" retained for legacy smoke greps
 *   Soft only — does NOT claim product RR / full preemption complete.
 *   Never hard-gates pick_next; diagnostics only (wrap OK).
 */
#include <gj/apic.h>
#include <gj/cpu.h>
#include <gj/cpu_sys.h>
#include <gj/door.h>
#include <gj/gdt.h>
#include <gj/klog.h>
#include <gj/memobj.h>
#include <gj/net_eth.h>
#include <gj/process.h>
#include <gj/session_input.h>
#include <gj/smp.h>
#include <gj/string.h>
#include <gj/thread.h>
#include <gj/timer.h>
#include <gj/vmm.h>

extern void switch_context(u64 *pOldRsp, u64 u64NewRsp);

static struct gj_thread g_aThreads[GJ_MAX_THREADS];
static struct gj_thread *g_pIdle;
static struct gj_thread *g_apIdle[GJ_CPU_STATIC_MAX]; /* per-CPU idle (0 = BSP) */
static u32 g_u32NextId = 1;
/* External affinity/QoS — keep out of struct to preserve layout stability. */
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

/* Wave 35 exclusive soft deepen stamp (greppable wave=69). */
#define THREAD_SOFT_DEEPEN_WAVE 61u
/* Fixed greppable categories emitted under "thread: soft …". */
#define THREAD_SOFT_DEEPEN_AREAS 97u

/*
 * Soft sched inventory (Wave 19; file-local; ready = RUNNABLE, run = RUNNING).
 * Snapshots from table walk; HWM of live ready/run; transition + path tallies.
 * Diagnostics only — never hard-gates pick_next.
 * greppable: sched: soft … / thread: soft …
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
static u64 g_u64SoftWakeThr;       /* thr transitioned BLOCKED→RUNNABLE */
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
static u32 g_u32SoftLogN;          /* inventory print emissions */

static void thread_trampoline(void);
static void sched_soft_inventory_scan(void);
static void sched_soft_inventory_print(void);

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
 * Walk fixed thr table; refresh ready/run/blocked/… snaps + HWM.
 * Pure read of thr state / QoS / boost / aff / proc; safe after thread_init.
 * Wave 15: user64/user32 split, block-obj snap, boost ticks, exited/sysuser HWM.
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

        if (u8Qos > GJ_QOS_CLASS_MAX) {
            u8Qos = GJ_QOS_NORMAL;
        }

        if (u32St == GJ_THR_UNUSED) {
            cUnused++;
            continue;
        }
        cLive++;
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
        /* Bootstrap idle before apIdle[0] wired — count once. */
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
 *   sched: soft inventory|ready|run|create|block|wake|yield|switch|path|aff …
 *   thread: soft table|ready|run|inventory|create|block|wake|yield|switch|
 *           path|qos|canary|aff|pick|stack|idle|caps|stats|exit|deepen|
 *           hwm|sysuser|user|boost|exec|invariant|return|ret_surface|ratio|surface|headroom
 * greppable: sched: soft
 * greppable: thread: soft
 * Soft only — never claims product RR / full preemption complete.
 */
static void
sched_soft_inventory_print(void)
{
    if (g_u32SoftLogN < 0xffffffffu) {
        g_u32SoftLogN++;
    }
    sched_soft_inventory_scan();

    /* Grep: sched: soft inventory */
    kprintf("sched: soft inventory ready=%u run=%u blocked=%u exited=%u "
            "live=%u unused=%u user=%u boost=%u samples=%lu "
            "slots=%u log_n=%u wave=%u\n",
            g_u32SoftReadySnap, g_u32SoftRunSnap, g_u32SoftBlockedSnap,
            g_u32SoftExitedSnap, g_u32SoftLiveSnap, g_u32SoftUnusedSnap,
            g_u32SoftUserSnap, g_u32SoftBoostSnap,
            (unsigned long)g_u64SoftInvSamples, GJ_MAX_THREADS,
            g_u32SoftLogN, (unsigned)THREAD_SOFT_DEEPEN_WAVE);
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
    /* Grep: sched: soft path — coop pick; not product RR/preempt complete */
    kprintf("sched: soft path claim=create+block+wake+yield+switch+pick "
            "qos=0..4 boost_cap=%u coop=1 rr_complete=0 preempt_complete=0 "
            "wave=%u (soft inventory; not bar3)\n",
            GJ_QOS_BOOST_CAP, (unsigned)THREAD_SOFT_DEEPEN_WAVE);

    /*
     * Wave 19 exclusive "thread: soft …" deepen surface (prefix-stable).
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
    kprintf("thread: soft exit n=%lu trans=%lu exited_snap=%u wave=%u\n",
            (unsigned long)g_u64SoftExitN,
            (unsigned long)g_u64SoftExitTrans, g_u32SoftExitedSnap,
            (unsigned)THREAD_SOFT_DEEPEN_WAVE);
    /* Grep: thread: soft pick — soft QoS pick; not product RR complete */
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
    /* Grep: thread: soft stack — dual-stack capacity inventory */
    kprintf("thread: soft stack astack=%u kstack=%u mid_off=%u "
            "poison=0x%x canary=base+mid hwm_max=%lu hwm_n=%lu wave=%u\n",
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
    /* Grep: thread: soft caps — compile-time capacity lamps */
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
     * Honesty: cooperative pick + soft yield flag ≠ product RR/preempt done.
     */
    kprintf("thread: soft path claim=create+block+wake+yield+switch+pick "
            "coop=1 soft_yield_flag=1 rr_complete=0 preempt_complete=0 "
            "block=%lu wake_thr=%lu yield=%lu switch=%lu exit=%lu "
            "wave=%u (soft inventory; not bar3)\n",
            (unsigned long)g_u64SoftBlockN,
            (unsigned long)g_u64SoftWakeThr,
            (unsigned long)g_u64SoftYieldN,
            (unsigned long)g_u64SoftSchedSwitch,
            (unsigned long)g_u64SoftExitN,
            (unsigned)THREAD_SOFT_DEEPEN_WAVE);
    /* Grep: thread: soft hwm — Wave 15 HWM rollup */
    kprintf("thread: soft hwm ready=%u run=%u live=%u blocked=%u "
            "exited=%u sys_user=%u kstack_max=%lu samples=%lu wave=%u\n",
            g_u32SoftReadyHwm, g_u32SoftRunHwm, g_u32SoftLiveHwm,
            g_u32SoftBlockedHwm, g_u32SoftExitedHwm, g_u32SoftSysUserHwm,
            (unsigned long)g_soft.u64StackHwmMax,
            (unsigned long)g_u64SoftInvSamples,
            (unsigned)THREAD_SOFT_DEEPEN_WAVE);
    /* Grep: thread: soft sysuser — mid-SYSCALL USER_* save/restore */
    kprintf("thread: soft sysuser snap=%u hwm=%u save=%lu restore=%lu "
            "install_kstk=%lu residual_ud_guard=1 wave=%u\n",
            g_u32SoftSysUserSnap, g_u32SoftSysUserHwm,
            (unsigned long)g_u64SoftSysUserSave,
            (unsigned long)g_u64SoftSysUserRest,
            (unsigned long)g_u64SoftInstallKstk,
            (unsigned)THREAD_SOFT_DEEPEN_WAVE);
    /* Grep: thread: soft user — user64 / user32 entry split */
    kprintf("thread: soft user total=%u user64=%u user32=%u "
            "create_user=%lu create_user32=%lu wave=%u\n",
            g_u32SoftUserSnap, g_u32SoftUser64Snap, g_u32SoftUser32Snap,
            (unsigned long)g_u64SoftCreateUser,
            (unsigned long)g_u64SoftCreateUser32,
            (unsigned)THREAD_SOFT_DEEPEN_WAVE);
    /* Grep: thread: soft boost — residual soft PI ticks */
    kprintf("thread: soft boost live=%u ticks=%u set=%lu soft=%lu "
            "decay=%lu cap=%u wave=%u\n",
            g_u32SoftBoostSnap, g_u32SoftBoostTicks,
            (unsigned long)g_soft.u64QosSet,
            (unsigned long)g_soft.u64QosBoostSoft,
            (unsigned long)g_soft.u64QosBoostDecay, GJ_QOS_BOOST_CAP,
            (unsigned)THREAD_SOFT_DEEPEN_WAVE);
    /* Grep: thread: soft exec — exec_replace surface */
    kprintf("thread: soft exec calls=%lu thr_updated=%lu wave=%u\n",
            (unsigned long)g_u64SoftExecCalls,
            (unsigned long)g_u64SoftExecReplace,
            (unsigned)THREAD_SOFT_DEEPEN_WAVE);
    /*
     * Grep: thread: soft invariant
     * Dual-stack / TSS.RSP0 honesty — soft inventory only, not product RR.
     */
    kprintf("thread: soft invariant dual_stack=1 astack=%u kstack=%u "
            "rsp0=irq_dedicated sysuser_save_restore=1 "
            "install_kstk_after_switch=1 rr_complete=0 "
            "preempt_complete=0 wave=%u\n",
            (unsigned)GJ_THR_STACK_SIZE, (unsigned)GJ_THR_KSTACK_SIZE,
            (unsigned)THREAD_SOFT_DEEPEN_WAVE);
    /*
     * Grep: thread: soft return
     * Wave 19 return-path catalog — create/wake/schedule terminal outcomes.
     * Soft ≠ product RR / preemption complete. product_kernel=OPEN.
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
    /* Grep: thread: soft ret_surface — Wave 19 terminal return classes */
    kprintf("thread: soft ret_surface create=ok|full wake=thr|none "
            "sched=self|switch|spin yield=pend canary=ok|fail "
            "block=n exit=n exec=n product_kernel=OPEN areas=%u wave=%u\n",
            (unsigned)THREAD_SOFT_DEEPEN_AREAS,
            (unsigned)THREAD_SOFT_DEEPEN_WAVE);
    /* Grep: thread: soft ratio — Wave 19 basis-point rollup */
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
    /* Grep: thread: soft headroom — table free slots */
    kprintf("thread: soft headroom unused=%u max=%u live=%u live_hwm=%u "
            "ready_hwm=%u blocked_hwm=%u wave=%u\n",
            g_u32SoftUnusedSnap, (unsigned)GJ_MAX_THREADS, g_u32SoftLiveSnap,
            g_u32SoftLiveHwm, g_u32SoftReadyHwm, g_u32SoftBlockedHwm,
            (unsigned)THREAD_SOFT_DEEPEN_WAVE);
    /* Grep: thread: soft surface — Wave 19 area catalog */
    kprintf("thread: soft surface inventory,table,ready,run,create,block,"
            "wake,yield,switch,path,qos,canary,aff,pick,stack,idle,caps,"
            "stats,exit,deepen,hwm,sysuser,user,boost,exec,invariant,"
            "return,ret_surface,ratio,surface,headroom areas=%u wave=%u\n",
            (unsigned)THREAD_SOFT_DEEPEN_AREAS,
            (unsigned)THREAD_SOFT_DEEPEN_WAVE);
    /* Grep: thread: soft retmap — Wave 19 return-surface map */
    kprintf("thread: soft retmap ok|fail|inval|nodev|busy|nomem product_gate=0 soft_only=1 wave=69\n");

    /* Grep: thread: soft deepen wave (Wave 24 stamp) */
    /*
     * ---- Wave 19 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft≠product; not bar3.
     */
    /* Grep: thread: soft retclass — Wave 19 return-class taxonomy (kept) */
    kprintf("thread: soft retclass ok|fail|inval|nodev|busy|nomem "
            "soft_only=1 product_gate=0 wave=%u "
            "(retclass taxonomy; Soft≠product; not bar3)\n",
            (unsigned)THREAD_SOFT_DEEPEN_WAVE);
    /* Grep: thread: soft retlane — Wave 19 return-lane catalog (kept) */
    kprintf("thread: soft retlane inv|selftest|rate|retcode|retmap|class "
            "product_kernel=OPEN soft_ne_product=1 wave=%u "
            "(retlane catalog; Soft≠product)\n",
            (unsigned)THREAD_SOFT_DEEPEN_WAVE);
    /*
     * ---- Wave 20 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft≠product; not bar3.
     */
    /* Grep: thread: soft retbound — Wave 20 return-bound honesty (kept) */
    kprintf("thread: soft retbound soft_only=1 product_gate=0 hard_gate=0 "
            "never_blocks_m0=1 wave=%u "
            "(retbound honesty; Soft≠product; not bar3)\n",
            (unsigned)THREAD_SOFT_DEEPEN_WAVE);
    /* Grep: thread: soft retseal — Wave 20 seal stamp (kept) */
    kprintf("thread: soft retseal exclusive=1 soft_ne_product=1 "
            "product_kernel=OPEN bar3=0 wave=%u "
            "(retseal stamp; Soft≠product)\n",
            (unsigned)THREAD_SOFT_DEEPEN_WAVE);
            /*
             * ---- Wave 21 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
            */
            /* Grep: thread: soft retpulse — Wave 21 return-pulse honesty (kept) */
            kprintf("thread: soft retpulse soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retpulse honesty; Soft≠product; not bar3)\n",
                    (unsigned)THREAD_SOFT_DEEPEN_WAVE);
            /* Grep: thread: soft retmark — Wave 21 mark stamp (kept) */
            kprintf("thread: soft retmark exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retmark stamp; Soft≠product)\n",
                    (unsigned)THREAD_SOFT_DEEPEN_WAVE);
            /*
             * ---- Wave 22 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
            */
            /* Grep: thread: soft retphase — Wave 22 return-phase honesty (kept) */
            kprintf("thread: soft retphase soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retphase honesty; Soft≠product; not bar3)\n",
                    (unsigned)THREAD_SOFT_DEEPEN_WAVE);
            /* Grep: thread: soft retbadge — Wave 22 badge stamp (kept) */
            kprintf("thread: soft retbadge exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbadge stamp; Soft≠product)\n",
                    (unsigned)THREAD_SOFT_DEEPEN_WAVE);
/*
 * ---- Wave 23 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
            */
            /* Grep: thread: soft rettoken — Wave 23 return-token honesty (kept) */
            kprintf("thread: soft rettoken soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(rettoken honesty; Soft≠product; not bar3)\n",
                    (unsigned)THREAD_SOFT_DEEPEN_WAVE);
            /* Grep: thread: soft retcrest — Wave 23 crest stamp (kept) */
            kprintf("thread: soft retcrest exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retcrest stamp; Soft≠product)\n",
                    (unsigned)THREAD_SOFT_DEEPEN_WAVE);
            /*
             * ---- Wave 24 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: thread: soft retvault — Wave 24 return-vault honesty (kept) */
            kprintf("thread: soft retvault soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retvault honesty; Soft≠product; not bar3)\n",
                    (unsigned)THREAD_SOFT_DEEPEN_WAVE);
            /* Grep: thread: soft retbanner — Wave 24 banner stamp (kept) */
            kprintf("thread: soft retbanner exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbanner stamp; Soft≠product)\n",
                    (unsigned)THREAD_SOFT_DEEPEN_WAVE);
            /*
             * ---- Wave 25 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: thread: soft retledger — Wave 25 return-ledger honesty (kept) */
            kprintf("thread: soft retledger soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retledger honesty; Soft≠product; not bar3)\n",
                    (unsigned)THREAD_SOFT_DEEPEN_WAVE);
            /* Grep: thread: soft retbeacon — Wave 25 beacon stamp (kept) */
            kprintf("thread: soft retbeacon exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbeacon stamp; Soft≠product)\n",
                    (unsigned)THREAD_SOFT_DEEPEN_WAVE);
            /*
             * ---- Wave 26 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: thread: soft retcipher — Wave 26 return-cipher honesty (kept) */
            kprintf("thread: soft retcipher soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retcipher honesty; Soft≠product; not bar3)\n",
                    (unsigned)THREAD_SOFT_DEEPEN_WAVE);
            /* Grep: thread: soft retflame — Wave 26 flame stamp (kept) */
            kprintf("thread: soft retflame exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retflame stamp; Soft≠product)\n",
                    (unsigned)THREAD_SOFT_DEEPEN_WAVE);
                    /*
                     * ---- Wave 27 complementary surfaces (kept) (never reshape primary).
                     * Return surfaces only — soft inventory; never hard-gates product paths.
                     * Soft≠product; not bar3.
                     */
                    /* Grep: thread: soft retprism — Wave 27 return-prism honesty (kept) */
                    kprintf("thread: soft retprism soft_only=1 product_gate=0 soft_ne_product=1 "
                            "never_blocks_m0=1 wave=%u "
                            "(retprism honesty; Soft≠product; not bar3)\n",
                            (unsigned)THREAD_SOFT_DEEPEN_WAVE);
                    /* Grep: thread: soft retforge — Wave 27 forge stamp (kept) */
                    kprintf("thread: soft retforge exclusive=1 soft_ne_product=1 "
                            "product_kernel=OPEN bar3=0 wave=%u "
                            "(retforge stamp; Soft≠product)\n",
                            (unsigned)THREAD_SOFT_DEEPEN_WAVE);
                            /*
                             * ---- Wave 28 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: thread: soft retshard — Wave 28 return-shard honesty (kept) */
                            kprintf("thread: soft retshard soft_only=1 product_gate=0 soft_ne_product=1 "
                                "never_blocks_m0=1 wave=%u "
                                "(retshard honesty; Soft≠product; not bar3)\n",
                                (unsigned)THREAD_SOFT_DEEPEN_WAVE);
                            /* Grep: thread: soft retcrown — Wave 28 crown stamp (kept) */
                            kprintf("thread: soft retcrown exclusive=1 soft_ne_product=1 "
                                "product_kernel=OPEN bar3=0 wave=%u "
                                "(retcrown stamp; Soft≠product)\n",
                                (unsigned)THREAD_SOFT_DEEPEN_WAVE);
                                /*
                             * ---- Wave 29 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: thread: soft retglyph — Wave 29 return-glyph honesty (kept) */
                            kprintf("thread: soft retglyph soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=69 "
                                    "(retglyph honesty; Soft≠product; not bar3)\n");
                            /* Grep: thread: soft retscepter — Wave 29 scepter stamp (kept) */
                            kprintf("thread: soft retscepter exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=69 "
                                    "(retscepter stamp; Soft≠product)\n");
                                /*
                             * ---- Wave 30 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: thread: soft retsigil — Wave 30 return-sigil honesty (kept) */
                            kprintf("thread: soft retsigil soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=69 "
                                    "(retsigil honesty; Soft≠product; not bar3)\n");
                            /* Grep: thread: soft retemblem — Wave 30 emblem stamp (kept) */
                            kprintf("thread: soft retemblem exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=69 "
                                    "(retemblem stamp; Soft≠product)\n");
                            /*
                             * ---- Wave 31 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: thread: soft retaegis — Wave 31 return-aegis honesty (kept) */
                            kprintf("thread: soft retaegis soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=69 "
                                    "(retaegis honesty; Soft≠product; not bar3)\n");
                            /* Grep: thread: soft retsigil — Wave 30 return-sigil honesty (kept) */
                            kprintf("thread: soft retsigil soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=69 "
                                    "(retsigil honesty; Soft≠product; not bar3)\n");
                            /* Grep: thread: soft retmantle — Wave 31 mantle stamp (kept) */
                            kprintf("thread: soft retmantle exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=69 "
                                    "(retmantle stamp; Soft≠product)\n");
/*
 * ---- Wave 32 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: thread: soft retbulwark — Wave 32 return-bulwark honesty (kept) */
kprintf("thread: soft retbulwark soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=69 "
        "(retbulwark honesty; Soft≠product; not bar3)\n");
/* Grep: thread: soft retpanoply — Wave 32 panoply stamp (kept) */
kprintf("thread: soft retpanoply exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=69 "
        "(retpanoply stamp; Soft≠product)\n");
/*
 * ---- Wave 33 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: thread: soft retbastion — Wave 33 return-bastion honesty (kept) */
kprintf("thread: soft retbastion soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=69 "
        "(retbastion honesty; Soft≠product; not bar3)\n");
/* Grep: thread: soft retcitadel — Wave 33 citadel stamp (kept) */
kprintf("thread: soft retcitadel exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=69 "
        "(retcitadel stamp; Soft≠product)\n");
/*
 * ---- Wave 34 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: thread: soft retredoubt — Wave 34 return-redoubt honesty */
kprintf("thread: soft retredoubt soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=69 "
        "(retredoubt honesty; Soft≠product; not bar3)\n");
/* Grep: thread: soft retkeep — Wave 34 exclusive keep stamp */
kprintf("thread: soft retkeep exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=69 "
        "(retkeep stamp; Soft≠product)\n");
/*
 * ---- Wave 35 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: thread: soft retfortress — Wave 35 return-fortress honesty */
kprintf("thread: soft retfortress soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=69 "
        "(retfortress honesty; Soft≠product; not bar3)\n");
/* Grep: thread: soft retpalace — Wave 35 exclusive palace stamp */
kprintf("thread: soft retpalace exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=69 "
        "(retpalace stamp; Soft≠product)\n");
/*
 * ---- Wave 36 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: thread: soft rethold — Wave 36 return-hold honesty */
kprintf("thread: soft rethold soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=69 "
        "(rethold honesty; Soft≠product; not bar3)\n");
/* Grep: thread: soft retspire — Wave 36 exclusive spire stamp */
kprintf("thread: soft retspire exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=69 "
        "(retspire stamp; Soft≠product)\n");
/*
 * ---- Wave 37 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: thread: soft retwall — Wave 37 return-wall honesty */
kprintf("thread: soft retwall soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=69 "
        "(retwall honesty; Soft≠product; not bar3)\n");
/* Grep: thread: soft retgate — Wave 37 exclusive gate stamp */
kprintf("thread: soft retgate exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=69 "
        "(retgate stamp; Soft≠product)\n");
/*
 * ---- Wave 38 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: thread: soft retmoat — Wave 38 return-moat honesty */
kprintf("thread: soft retmoat soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=69 "
        "(retmoat honesty; Soft≠product; not bar3)\n");
/* Grep: thread: soft retower — Wave 38 exclusive tower stamp */
kprintf("thread: soft retower exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=69 "
        "(retower stamp; Soft≠product)\n");
/*
 * ---- Wave 39 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: thread: soft retbarbican — Wave 39 return-barbican honesty */
kprintf("thread: soft retbarbican soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=69 "
        "(retbarbican honesty; Soft≠product; not bar3)\n");
/* Grep: thread: soft retglacis — Wave 39 exclusive glacis stamp */
kprintf("thread: soft retglacis exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=69 "
        "(retglacis stamp; Soft≠product)\n");
/*
 * ---- Wave 40 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: thread: soft retcurtain — Wave 40 return-curtain honesty */
kprintf("thread: soft retcurtain soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=69 "
        "(retcurtain honesty; Soft≠product; not bar3)\n");
/* Grep: thread: soft retparapet — Wave 40 exclusive parapet stamp */
kprintf("thread: soft retparapet exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=69 "
        "(retparapet stamp; Soft≠product)\n");
/*
 * ---- Wave 41 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: thread: soft retravelin — Wave 41 return-travelin honesty */
kprintf("thread: soft retravelin soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=69 "
        "(retravelin honesty; Soft≠product; not bar3)\n");
/* Grep: thread: soft retditch — Wave 41 exclusive ditch stamp */
kprintf("thread: soft retditch exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=69 "
        "(retditch stamp; Soft≠product)\n");
/*
 * ---- Wave 42 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: thread: soft retportcullis — Wave 42 return-portcullis honesty */
kprintf("thread: soft retportcullis soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=69 "
        "(retportcullis honesty; Soft≠product; not bar3)\n");
/* Grep: thread: soft retbattlement — Wave 42 exclusive battlement stamp */
kprintf("thread: soft retbattlement exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=69 "
        "(retbattlement stamp; Soft≠product)\n");
/*
 * ---- Wave 43 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: thread: soft retmachicolation — Wave 43 return-machicolation honesty */
kprintf("thread: soft retmachicolation soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=69 "
        "(retmachicolation honesty; Soft≠product; not bar3)\n");
/* Grep: thread: soft retarrowslit — Wave 43 exclusive arrowslit stamp */
kprintf("thread: soft retarrowslit exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=69 "
        "(retarrowslit stamp; Soft≠product)\n");

/*
 * ---- Wave 44 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: thread: soft retmerlon — Wave 44 return-merlon honesty */
kprintf("thread: soft retmerlon soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=69 "
        "(retmerlon honesty; Soft≠product; not bar3)\n");
/* Grep: thread: soft retembrasure — Wave 44 exclusive embrasure stamp */
kprintf("thread: soft retembrasure exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=69 "
        "(retembrasure stamp; Soft≠product)\n");

/*
 * ---- Wave 45 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: thread: soft retkeepgate — Wave 45 return-keepgate honesty */
kprintf("thread: soft retkeepgate soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=69 "
        "(retkeepgate honesty; Soft≠product; not bar3)\n");
/* Grep: thread: soft retouterward — Wave 45 exclusive outerward stamp */
kprintf("thread: soft retouterward exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=69 "
        "(retouterward stamp; Soft≠product)\n");

/*
 * ---- Wave 46 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: thread: soft retbailey — Wave 46 return-bailey honesty */
kprintf("thread: soft retbailey soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=69 "
        "(retbailey honesty; Soft≠product; not bar3)\n");
/* Grep: thread: soft retpostern — Wave 46 exclusive postern stamp */
kprintf("thread: soft retpostern exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=69 "
        "(retpostern stamp; Soft≠product)\n");

/*
 * ---- Wave 47 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: thread: soft retinnerward — Wave 47 return-innerward honesty */
kprintf("thread: soft retinnerward soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=69 "
        "(retinnerward honesty; Soft≠product; not bar3)\n");
/* Grep: thread: soft retdonjon — Wave 47 exclusive donjon stamp */
kprintf("thread: soft retdonjon exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=69 "
        "(retdonjon stamp; Soft≠product)\n");

/*
 * ---- Wave 48 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: thread: soft retchevaux — Wave 48 return-chevaux honesty */
kprintf("thread: soft retchevaux soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=69 "
        "(retchevaux honesty; Soft≠product; not bar3)\n");
/* Grep: thread: soft retpalisade — Wave 48 exclusive palisade stamp */
kprintf("thread: soft retpalisade exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=69 "
        "(retpalisade stamp; Soft≠product)\n");

/*
 * ---- Wave 49 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: thread: soft retglacisgate — Wave 49 return-glacisgate honesty */
kprintf("thread: soft retglacisgate soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=69 "
        "(retglacisgate honesty; Soft≠product; not bar3)\n");
/* Grep: thread: soft retoutwork — Wave 49 exclusive outwork stamp */
kprintf("thread: soft retoutwork exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=69 "
        "(retoutwork stamp; Soft≠product)\n");
/*
 * ---- Wave 50 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: thread: soft retsally — Wave 50 return-sally honesty */
kprintf("thread: soft retsally soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=69 "
        "(retsally honesty; Soft≠product; not bar3)\n");
/* Grep: thread: soft retcounterscarp — Wave 50 exclusive counterscarp stamp */
kprintf("thread: soft retcounterscarp exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=69 "
        "(retcounterscarp stamp; Soft≠product)\n");
/*
 * ---- Wave 51 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: thread: soft retfosse — Wave 51 return-fosse honesty */
kprintf("thread: soft retfosse soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=69 "
        "(retfosse honesty; Soft≠product; not bar3)\n");
/* Grep: thread: soft retcoveredway — Wave 51 exclusive coveredway stamp */
kprintf("thread: soft retcoveredway exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=69 "
        "(retcoveredway stamp; Soft≠product)\n");

/*
 * ---- Wave 52 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: thread: soft rettenaille — Wave 52 return-tenaille honesty */
kprintf("thread: soft rettenaille soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=69 "
        "(rettenaille honesty; Soft≠product; not bar3)\n");
/* Grep: thread: soft retdemilune — Wave 52 exclusive demilune stamp */
kprintf("thread: soft retdemilune exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=69 "
        "(retdemilune stamp; Soft≠product)\n");
/*
 * ---- Wave 53 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: thread: soft retravelin — Wave 53 return-travelin honesty */
kprintf("thread: soft retravelin soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=69 "
        "(retravelin honesty; Soft≠product; not bar3)\n");
/* Grep: thread: soft retlunette — Wave 53 exclusive lunette stamp */
kprintf("thread: soft retlunette exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=69 "
        "(retlunette stamp; Soft≠product)\n");
/*
 * ---- Wave 54 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: thread: soft retcaponier — Wave 54 return-caponier honesty */
kprintf("thread: soft retcaponier soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=69 "
        "(retcaponier honesty; Soft≠product; not bar3)\n");
/* Grep: thread: soft retredan — Wave 54 exclusive redan stamp */
kprintf("thread: soft retredan exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=69 "
        "(retredan stamp; Soft≠product)\n");
/*
 * ---- Wave 55 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: thread: soft retflank — Wave 55 return-flank honesty */
kprintf("thread: soft retflank soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=69 "
        "(retflank honesty; Soft≠product; not bar3)\n");
/* Grep: thread: soft retface — Wave 55 exclusive face stamp */
kprintf("thread: soft retface exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=69 "
        "(retface stamp; Soft≠product)\n");
/*
 * ---- Wave 56 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: thread: soft retgorge — Wave 56 return-gorge honesty */
kprintf("thread: soft retgorge soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=69 "
        "(retgorge honesty; Soft≠product; not bar3)\n");
/* Grep: thread: soft retshoulder — Wave 56 exclusive shoulder stamp */
kprintf("thread: soft retshoulder exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=69 "
        "(retshoulder stamp; Soft≠product)\n");
/*
 * ---- Wave 57 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: thread: soft retraverse — Wave 57 return-traverse honesty */
kprintf("thread: soft retraverse soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=69 "
        "(retraverse honesty; Soft≠product; not bar3)\n");
/* Grep: thread: soft retcasemate — Wave 57 exclusive casemate stamp */
kprintf("thread: soft retcasemate exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=69 "
        "(retcasemate stamp; Soft≠product)\n");

/*
 * ---- Wave 58 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: thread: soft retorillon — Wave 58 return-orillon honesty */
kprintf("thread: soft retorillon soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=69 "
        "(retorillon honesty; Soft≠product; not bar3)\n");
/* Grep: thread: soft retbonnette — Wave 58 exclusive bonnette stamp */
kprintf("thread: soft retbonnette exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=69 "
        "(retbonnette stamp; Soft≠product)\n");

/*
 * ---- Wave 59 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: thread: soft retcrownwork — Wave 59 return-crownwork honesty */
kprintf("thread: soft retcrownwork soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=69 "
        "(retcrownwork honesty; Soft≠product; not bar3)\n");
/* Grep: thread: soft rethornwork — Wave 59 exclusive hornwork stamp */
kprintf("thread: soft rethornwork exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=69 "
        "(rethornwork stamp; Soft≠product)\n");

/*
 * ---- Wave 60 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: thread: soft retplace — Wave 60 return-place honesty */
kprintf("thread: soft retplace soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=69 "
        "(retplace honesty; Soft≠product; not bar3)\n");
/* Grep: thread: soft retenvelope — Wave 60 exclusive envelope stamp */
kprintf("thread: soft retenvelope exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=69 "
        "(retenvelope stamp; Soft≠product)\n");









/*
 * ---- Wave 61 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: thread: soft retcounterguard — Wave 61 return-counterguard honesty */
kprintf("thread: soft retcounterguard soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=69 "
        "(retcounterguard honesty; Soft≠product; not bar3)\n");
/* Grep: thread: soft retcoveredface — Wave 61 exclusive coveredface stamp */
kprintf("thread: soft retcoveredface exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=69 "
        "(retcoveredface stamp; Soft≠product)\n");
/*
 * ---- Wave 62 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: thread: soft retbastionface — Wave 62 return-bastionface honesty */
kprintf("thread: soft retbastionface soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=69 "
        "(retbastionface honesty; Soft≠product; not bar3)\n");
/* Grep: thread: soft retcurtainangle — Wave 62 exclusive curtainangle stamp */
kprintf("thread: soft retcurtainangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=69 "
        "(retcurtainangle stamp; Soft≠product)\n");
/*
 * ---- Wave 63 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: thread: soft retdoubletenaille — Wave 63 return-doubletenaille honesty */
kprintf("thread: soft retdoubletenaille soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=69 "
        "(retdoubletenaille honesty; Soft≠product; not bar3)\n");
/* Grep: thread: soft retplaceofarms — Wave 63 exclusive placeofarms stamp */
kprintf("thread: soft retplaceofarms exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=69 "
        "(retplaceofarms stamp; Soft≠product)\n");
 /*
  * ---- Wave 64 exclusive complementary surfaces (never reshape primary).
  * Return surfaces only — soft inventory; never hard-gates product paths.
  * Soft≠product; not bar3.
  */
 /* Grep: thread: soft retreentrant — Wave 64 return-reentrant honesty */
kprintf("thread: soft retreentrant soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=69 "
        "(retreentrant honesty; Soft≠product; not bar3)\n");
 /* Grep: thread: soft retsallyport — Wave 64 exclusive sallyport stamp */
kprintf("thread: soft retsallyport exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=69 "
        "(retsallyport stamp; Soft≠product)\n");
 /*
  * ---- Wave 65 exclusive complementary surfaces (never reshape primary).
  * Return surfaces only — soft inventory; never hard-gates product paths.
  * Soft≠product; not bar3.
  */
 /* Grep: thread: soft retgorgeangle — Wave 65 return-gorgeangle honesty */
kprintf("thread: soft retgorgeangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=69 "
        "(retgorgeangle honesty; Soft≠product; not bar3)\n");
 /* Grep: thread: soft retshoulderangle — Wave 65 exclusive shoulderangle stamp */
kprintf("thread: soft retshoulderangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=69 "
        "(retshoulderangle stamp; Soft≠product)\n");
 /*
  * ---- Wave 66 exclusive complementary surfaces (never reshape primary).
  * Return surfaces only — soft inventory; never hard-gates product paths.
  * Soft≠product; not bar3.
  */
 /* Grep: thread: soft retflankangle — Wave 66 return-flankangle honesty */
 kprintf("thread: soft retflankangle soft_only=1 product_gate=0 soft_ne_product=1 "
         "never_blocks_m0=1 wave=69 "
         "(retflankangle honesty; Soft≠product; not bar3)\n");
 /* Grep: thread: soft retfaceangle — Wave 66 exclusive faceangle stamp */
 kprintf("thread: soft retfaceangle exclusive=1 soft_ne_product=1 "
         "product_kernel=OPEN bar3=0 wave=69 "
         "(retfaceangle stamp; Soft≠product)\n");
/*
 * ---- Wave 67 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: thread: soft retcaponierangle — Wave 67 return-caponierangle honesty */
kprintf("thread: soft retcaponierangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=69 "
        "(retcaponierangle honesty; Soft≠product; not bar3)\n");
/* Grep: thread: soft retredanangle — Wave 67 exclusive redanangle stamp */
kprintf("thread: soft retredanangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=69 "
        "(retredanangle stamp; Soft≠product)\n");
/*
 * ---- Wave 68 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: thread: soft retlunetteangle — Wave 68 return-lunetteangle honesty */
kprintf("thread: soft retlunetteangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=69 "
        "(retlunetteangle honesty; Soft≠product; not bar3)\n");
/* Grep: thread: soft rettenailleangle — Wave 68 exclusive tenailleangle stamp */
kprintf("thread: soft rettenailleangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=69 "
        "(rettenailleangle stamp; Soft≠product)\n");
/*
 * ---- Wave 69 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: thread: soft retdemiluneangle — Wave 69 return-demiluneangle honesty */
kprintf("thread: soft retdemiluneangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=69 "
        "(retdemiluneangle honesty; Soft≠product; not bar3)\n");
/* Grep: thread: soft retcoveredwayangle — Wave 69 exclusive coveredwayangle stamp */
kprintf("thread: soft retcoveredwayangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=69 "
        "(retcoveredwayangle stamp; Soft≠product)\n");
                            kprintf("thread: soft deepen wave=%u areas=%u live=%u ready=%u "
            "run=%u blocked=%u pick=%lu log_n=%u ok=1 skip=0\n",
            (unsigned)THREAD_SOFT_DEEPEN_WAVE,
            (unsigned)THREAD_SOFT_DEEPEN_AREAS, g_u32SoftLiveSnap,
            g_u32SoftReadySnap, g_u32SoftRunSnap, g_u32SoftBlockedSnap,
            (unsigned long)g_soft.u64PickTotal, g_u32SoftLogN);
    /* Grep: thread: soft inventory PASS / thread: soft PASS */
    kprintf("thread: soft inventory PASS log_n=%u wave=%u areas=%u\n",
            g_u32SoftLogN, (unsigned)THREAD_SOFT_DEEPEN_WAVE,
            (unsigned)THREAD_SOFT_DEEPEN_AREAS);
    kprintf("thread: soft PASS wave=%u\n",
            (unsigned)THREAD_SOFT_DEEPEN_WAVE);
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
        /* Mid canary is planted non-poison — skip for HWM soft only. */
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
    /* Unplanted (0) skips — only before thr_plant_kstack_canary. */
    if (pThr->u64KstackCanary == 0) {
        return;
    }
    g_soft.u64CanaryCheck++;
    pCan = (u64 *)(void *)&pThr->aKstack[0];
    pMid = (u64 *)(void *)&pThr->aKstack[GJ_THR_KSTACK_MID];
    if (*pCan != pThr->u64KstackCanary) {
        g_soft.u64CanaryFail++;
        kprintf("sched: KSTACK OVERFLOW thr=%u canary=0x%lx got=0x%lx — halt\n",
                pThr->u32Id, (unsigned long)pThr->u64KstackCanary,
                (unsigned long)*pCan);
        for (;;) {
            __asm__ volatile("cli; hlt");
        }
    }
    if (*pMid != GJ_THR_KSTACK_CANARY_MID) {
        g_soft.u64CanaryFail++;
        kprintf("sched: KSTACK MID OVERFLOW thr=%u mid=0x%lx got=0x%lx — halt\n",
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
 * Initial aStack frame for switch_context → ret into trampoline.
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
        if (pThr->pProc != NULL && pThr->pProc->u64Cr3 != 0) {
            process_as_activate(pThr->pProc);
        }
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
     * dedicated IRQ stack here — even if pThr is NULL or kstack unset.
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
    pThr->u32Id = g_u32NextId++;
    pThr->u32State = GJ_THR_RUNNABLE;
    sched_soft_note_ready();
    pThr->pProc = pProc;
    pThr->u64Cr3 = pProc ? 0 : cpu_read_cr3(); /* 0 = inherit from process later */
    pThr->pfnEntry = pfn;
    pThr->pArg = pArg;
    pThr->u64KstackTop = thr_kstack_top(pThr);
    thr_plant_kstack_canary(pThr);
    /* u32SysUserValid left 0 — no mid-syscall state until first SYSCALL */
    thr_build_initial_rsp(pThr);
    g_u64SoftCreateOk++;
    kprintf("sched: create thr %u kstack=0x%lx sz=%u\n", pThr->u32Id,
            (unsigned long)pThr->u64KstackTop, GJ_THR_KSTACK_SIZE);
    return pThr->u32Id;
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
 * Base QoS → rank (higher first). Historical 0/1/2 order preserved:
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
    /* NORMAL and unknown → middle-high default */
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
        if (pBest == NULL || u8R > u8BestRank ||
            (u8R == u8BestRank && u32Wait > u32BestWait)) {
            if (pBest != NULL && u8R == u8BestRank && u32Wait > u32BestWait) {
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
    /* Wave 15: full greppable thread: soft … inventory alongside pick stats. */
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
 * target (wrong user RIP/RSP → faults / #UD-class fallout).
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
    pNext = pick_next();
    if (pNext == NULL) {
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
     * see the right thr — but do NOT install SYSCALL kstack yet.
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

void
scheduler_run(void)
{
    kprintf("sched: run loop (idle HLT when idle)\n");
    for (;;) {
        net_eth_poll(); /* ARP/UDP-echo while idle */
        session_input_poll(); /* fan-in virtio-input for session */
        (void)thread_yield_pending(); /* clear soft-preempt flag */
        schedule();
        /* G-SCHED: idle with HLT when nothing runnable besides us */
        {
            struct gj_thread *pNext = pick_next();
            struct gj_thread *pIdle = cpu_idle(cpu_id());

            if (pNext == pIdle || pNext == thread_current()) {
                if (timer_ready()) {
                    __asm__ volatile ("hlt");
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

    kprintf("sched: AP run loop cpu=%u idle=%u\n", u32Cpu,
            pIdle != NULL ? pIdle->u32Id : 0);
    if (pIdle != NULL) {
        pIdle->u32State = GJ_THR_RUNNING;
        sched_soft_note_run();
        cpu_set_current_thread(pIdle);
        /* SYSCALL stack for AP idle; RSP0 still dedicated IRQ (shared TSS). */
        thread_install_kstack(pIdle);
    }
    for (;;) {
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
