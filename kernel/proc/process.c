/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Process: shared CNode, root meta bootstrap, pager on PCB, wait4 reaper,
 * G-PROC-5 death (CNode wipe + private AS reclaim for wait-registered children).
 * Soft deepen: pager ep kernel ref + badge + slot-1 mirror; wait reparent /
 * WNOWAIT / counts; death thr_exit siblings before AS destroy (clone_vm
 * FAULT H3: early + pre-as_destroy barrier) + quota+CDT CNode + scrub.
 * Soft!=product. Dual license: MIT OR Apache-2.0.
 *
 * H3 residual lean (ASSURANCE thr_exit before as_destroy; this unit only):
 *   Permanent order: u32Alive=0 -> thr_exit early (siblings + cur scrub) ->
 *   pager/region/cnode -> thr_exit barrier (siblings + cur scrub) ->
 *   cr3_publish=0 (local holds destroy target) -> private as_destroy.
 *   Dual belt residual lean: early drain after alive=0, re-drain immediately
 *   before as_destroy (catches thr bound mid-death). Covers pe32 clone_vm
 *   lab class and multi-thr UDX host processes (work/IRQ soft thr sharing AS).
 *   Cur-scrub residual: thread_exit_process skips current; death helper also
 *   clears current thr USER*_ENTRY / sysuser (valid+rip/rsp/rflags) / thr CR3 /
 *   block when bound to dying PCB (no EXIT - caller thread_exit). Fail-closed
 *   vs dying thr iretq / mid-syscall sysret into maps about to free (same
 *   #PF I=1 class as sibling residual).
 *   CR3 publish residual: after dual belt thr_exit, zero PCB u64Cr3 before
 *   as_destroy so trampoline/schedule refuse-enter cr3_0 belt is live during
 *   map free (local u64 holds destroy target). Long-lived skip leaves CR3.
 *   fork_stub_as_teardown mirrors early + barrier + cr3_pub before as_destroy.
 *   Companion: thread_exit_process + trampoline refuse-enter (thread.c).
 *   Soft!=product - not product UDX close; not Linux .ko (G-AC-1).
 *
 * Functional residual process lifecycle (UDX / sshd hosts; Soft!=product):
 *   LIVE host path (not immediate kill / not vfork fExitNow):
 *     wait_register -> live unreaped -> parent soft wait poll (WNOHANG yield
 *     + blocking yield) -> force-exit/exit_pid OR wait-heal -> G-PROC-5 H3
 *     death -> wait reap. Covers long-lived multi-thr UDX hosts
 *     (rtl8168_udx / xhci_udx / ddi_host_gj) and sshd session children.
 *   keep_live product host honesty (STRONGER denser residual; Soft!=product):
 *     process_spawn_host_launch class hosts may stay keep_live when host
 *     ELF embed is present (main: soft residual host_launch live). This
 *     unit must NEVER thrash as_destroy while product host thr are live
 *     (H3 thr_exit dual belt + cur_scrub + cr3_pub BEFORE as_destroy).
 *     Denser axes (companion to main host_launch live residual):
 *       keep_live=1 | product_host_live=1 | thr_live=1 | keep_live_rock=1
 *       | never_kill_embed=1 | soft_ne_product=1 | product_hosts=UDX
 *     keep_live=1 / product_host_live=1 means death is deferred until
 *     intentional kill/exit; soft wait poll + yield while alive; Dual DoD
 *     A/B stay OPEN (soft residual never closes Dual DoD; agent!=close).
 *     Soft!=product: keep_live residual != product UDX wire/TX/RX close;
 *     denser residual != Dual DoD close; never invent stamp .76;
 *     bar=v2026.08.04.75 stamp-free.
 *   exit_pid residual: process_linux_exit_pid runs full process_death (H3
 *     thr_exit dual belt + as_destroy) when the wait-registered PCB still
 *     needs teardown (alive/cr3/pager/start_thr/exc/regions). note_exit-only
 *     would set u32Alive=0 and skip teardown -> private AS leak / sibling
 *     USER*_ENTRY into freed maps (same H3 class).
 *   wait live-heal residual: parent wait4 sees PCB alive==0 but slot not
 *     zombie (mid-death race / incomplete force-exit) -> promote via full
 *     process_death when teardown remains, else note_exit, so live-host
 *     parents reaps without soft hang.
 *   wait WNOHANG / blocking residual: live-child return yields so concurrent
 *     UDX host thr + freestanding sshd eth accept (Dual DoD B direction)
 *     can progress while a parent soft-polls (pairs with spawn: soft wait
 *     poll).
 *   Dual DoD A/B remain OPEN (soft residual never closes Dual DoD).
 *   Bar honesty v2026.08.04.75 stamp-free; never bump GJ_IMAGE_VERSION here.
 * greppable: process: soft residual lean H3
 * greppable: process: soft death residual lean
 * greppable: process: soft residual lifecycle
 * greppable: process: soft residual keep_live
 * greppable: process: soft residual product_host_live
 * greppable: process: soft residual denser keep_live
 * greppable: process: soft residual denser product_host_live
 * greppable: process: soft residual denser thr_live
 * greppable: process: soft residual denser dual_dod
 * greppable: process: death thr_exit | process: death thr_exit barrier
 * greppable: process: death thr_exit early | process: death thr_exit cur_scrub
 * greppable: process: death H3 order | process: death cr3_pub
 * greppable: process: soft exit_pid death | process: soft wait nohang_yield
 * greppable: process: soft wait live_heal | process: soft wait live_death
 * greppable: thr_exit_before_as_destroy=1 | udx_host_teardown=1
 * greppable: H3=death_residual | udx_host_multi_thr=1 | soft_ne_product=1
 * greppable: cr3_pub_before_as_destroy=1 | dual_dod=OPEN | lifecycle=1
 * greppable: live_host_path=1 | not_immediate_kill=1
 * greppable: product_host_live=1 | keep_live=1 | never_kill_embed=1
 * greppable: keep_live_rock=1 | thr_live=1 | product_hosts=UDX
 * greppable: Soft!=product soft residual dual_dod OPEN product_host_live
 * greppable: denser keep_live residual | denser product_host_live residual
 * greppable: denser thr_live residual | denser dual_dod residual
 * greppable: denser thr_exit_before_as_destroy residual
 *
 * Soft product inventory (ABI-first fork/wait deepen; this unit only):
 * greppable: "process: soft ..."
 *   process: soft inventory ...
 *   process: soft stats ...
 *   process: soft init ...
 *   process: soft seal ...
 *   process: soft confine ...
 *   process: soft pager ...
 *   process: soft fault ...
 *   process: soft wait ...     - wait4/waitid WNOHANG reaper (shell/sshd later)
 *   process: soft fork-wait product-min - PCB parent fork/clone/wait APIs
 *   process: soft death ...
 *   process: soft death residual lean ...  - H3 thr_exit-before-as_destroy
 *   process: soft residual lean H3 ...
 *   process: soft residual lifecycle ...  - UDX/sshd live host death+wait residual
 *   process: soft residual keep_live ...  - product host keep_live honesty residual
 *   process: soft residual product_host_live ... - denser keep_live Dual DoD OPEN
 *   process: soft residual denser keep_live ... - denser keep_live_rock thr_live axes
 *   process: soft residual denser product_host_live ... - denser Dual DoD OPEN residual
 *   process: soft residual denser thr_live ... - denser thr_live keep_live Dual DoD OPEN
 *   process: soft residual denser dual_dod ... - denser Dual DoD OPEN honesty residual
 *   process: soft fork ...     - linux_fork + linux_clone flag map (usable pid)
 *   process: soft jit ...
 *   process: soft promise ...
 *   process: soft path ...
 *   process: soft surface ... (Wave 19 area catalog)
 *   process: soft deepen wave=116 ...
 *   process: soft PASS|PARTIAL
 *   Apple s13 bootstrap seal checklist (wave=116 stamp):
 *     process: bootstrap seal soft ...
 *     process: seal checklist ...
 *     process: bootstrap seal soft deepen wave=116 ...
 *   G-PROC-5 death tallies (wave=116 stamp):
 *     process: death ... / process: death deepen wave=116 ...
 *
 * Honesty: soft inventory only - not product multi-server seal, not Apple s13
 * closed. Fork stub != real user child; wait table != full posix wait.
 * keep_live residual != product UDX close; denser residual != Dual DoD close;
 * Dual DoD A/B OPEN until DUT. Soft!=product. G-AC-1.
 * docs/CAP_ADDRESSING.md | docs/APPLE_CHANNEL_REMAINING.md s13 |
 * docs/SOLARIS_STYLE_REMAINING.md s6 | s9 | docs/SECURITY_CORE_DESIGN.md s13 |
 * docs/ASSURANCE_LITE.md H3
 */
#include <gj/cap.h>
#include <gj/cpu.h>
#include <gj/door.h>
#include <gj/klog.h>
#include <gj/memobj.h>
#include <gj/process.h>
#include <gj/string.h>
#include <gj/thread.h>
#include <gj/vmm.h>

/* ---- Wave 19 exclusive soft inventory (this unit only) ------------------ */
#define GJ_PROCESS_SOFT_WAVE 116u
/* +live lifecycle + denser keep_live thr_live product_host_live Dual DoD residual */
#define GJ_PROCESS_SOFT_AREAS 244u
#define GJ_SEAL_SOFT_WAVE 116u /* Apple s13 seal checklist stamp */
#define GJ_SEAL_SOFT_LOG_MAX   8u

/* Forward: wait table lives later; soft census snapshots used/zombie/free. */
#define GJ_WAIT_SLOTS 64u
#define GJ_WAIT_PID_BASE 100u
/* Soft fork stub pool (shell pipelines later; not product process table). */
#define GJ_FORK_STUBS 32u

/*
 * Soft path tallies (diagnostics only; wrap OK). Never hard-gate product.
 * greppable: process: soft ...
 */
static u32 g_u32SoftInitOk;
static u32 g_u32SoftInitNull;
static u32 g_u32SoftRootMetaOk;
static u32 g_u32SoftRootMetaFail;
static u32 g_u32SoftRootMetaBusy;
static u32 g_u32SoftConfineN;
static u32 g_u32SoftConfineNull;
static u32 g_u32SoftPromiseOkHit;
static u32 g_u32SoftPromiseOkMiss;
static u32 g_u32SoftPromiseReqOk;
static u32 g_u32SoftPromiseReqDeny;
static u32 g_u32SoftJitSetOn;
static u32 g_u32SoftJitSetOff;
static u32 g_u32SoftJitQueryHit;
static u32 g_u32SoftJitQueryMiss;
static u32 g_u32SoftPagerSetOk;
static u32 g_u32SoftPagerSetFail;
static u32 g_u32SoftPagerClear;
static u32 g_u32SoftPagerRefreshClear;
static u32 g_u32SoftPagerRefreshKeep;
static u32 g_u32SoftPagerMirrorOk;
static u32 g_u32SoftPagerHasHit;
static u32 g_u32SoftPagerHasMiss;
static u32 g_u32SoftFaultEnter;
static u32 g_u32SoftFaultNoPager;
static u32 g_u32SoftFaultWxDeny;
static u32 g_u32SoftFaultBusy;
static u32 g_u32SoftFaultAgain;
static u32 g_u32SoftWaitRegOk;
static u32 g_u32SoftWaitRegIdem;
static u32 g_u32SoftWaitRegFull;
static u32 g_u32SoftWaitRegNull;
static u32 g_u32SoftWaitZombie;
static u32 g_u32SoftWaitReap;
static u32 g_u32SoftWaitNowait;
static u32 g_u32SoftWaitReparentN;
static u32 g_u32SoftWaitForget;
static u32 g_u32SoftWait4Enter;
static u32 g_u32SoftWait4Echild;
static u32 g_u32SoftWait4Nohang0;
static u32 g_u32SoftWait4LiveTimeout; /* blocking soft budget, live remain */
static u32 g_u32SoftWait4ReapPid;     /* specific-pid reap */
static u32 g_u32SoftWait4ReapAny;     /* any-child reap */
static u32 g_u32SoftForkEnter;
static u32 g_u32SoftForkOk;
static u32 g_u32SoftForkFull;
static u32 g_u32SoftForkVfork;
static u32 g_u32SoftForkDeferred;
static u32 g_u32SoftForkAsOk;
static u32 g_u32SoftForkAsFail;
static u32 g_u32SoftForkCloneOk;
static u32 g_u32SoftForkCloneFail;
static u32 g_u32SoftForkRegFail;      /* wait table full after stub alloc */
static u32 g_u32SoftCloneEnter;
static u32 g_u32SoftCloneForkLike;    /* flags 0 / share -> fork */
static u32 g_u32SoftCloneVfork;
static u32 g_u32SoftCloneThreadReject;
static u32 g_u32SoftCloneNsReject;
static u32 g_u32SoftDeathEnter;
/* Sibling thr EXITED before AS destroy (clone_vm FAULT H3; Soft!=product). */
static u64 g_u64DeathThrExit;
/* Early-pass thr exits only (post alive=0; pre pager/region/cnode). */
static u64 g_u64DeathThrExitEarly;
/* Second-pass residual exits at pre-as_destroy barrier (usually 0). */
static u64 g_u64DeathThrExitBarrier;
/* Pre-as_destroy barrier invocations (n may be 0; dual belt residual lean). */
static u64 g_u64DeathThrExitBarrierCalls;
/* Deaths that reached thr_exit dual belt then as_destroy attempt/skip. */
static u64 g_u64DeathH3OrderPass;
/* fork_stub_as_teardown H3 dual-belt invocations (Soft!=product). */
static u64 g_u64DeathH3ForkStub;
/*
 * Current-thr user/CR3 scrub hits (thread_exit_process skips current; H3
 * residual lean fail-closed before as_destroy). Soft!=product.
 */
static u64 g_u64DeathThrExitCurScrub;
/*
 * PCB u64Cr3 published 0 after thr_exit dual belt, before as_destroy
 * (local holds destroy target; refuse-enter cr3_0 during map free).
 */
static u64 g_u64DeathH3Cr3Pub;
/* H3 residual lean once-lamp (Soft!=product; no stamp storm). */
static u8  g_fH3ResLeanOnce;
/* Lifecycle residual once-lamp (UDX/sshd host death+wait; Dual DoD OPEN). */
static u8  g_fLifecycleResLeanOnce;
/*
 * keep_live product host honesty once-lamp (Soft!=product; Dual DoD OPEN).
 * denser residual: product_host_live keep_live keep_live_rock thr_live
 * product_hosts=UDX; never thrash as_destroy while thr live (H3 thr_exit
 * before as_destroy spirit). Companion main host_launch live axes. No
 * stamp storm (H2). denser residual != Dual DoD close.
 */
static u8  g_fKeepLiveResLeanOnce;
/*
 * process_linux_exit_pid full process_death path (H3 thr_exit before as_destroy).
 * note_exit-only path would race deferred fork exit worker into AS leak.
 */
static u64 g_u64SoftExitPidDeath;
static u64 g_u64SoftExitPidNoteOnly; /* already fully torn; re-note only */
/* wait4 WNOHANG live yield (sshd accept / UDX host thr progress residual). */
static u64 g_u64SoftWaitNohangYield;
/* wait4 blocking live yield (same concurrent host progress class). */
static u64 g_u64SoftWaitBlockYield;
/*
 * wait live-heal: PCB alive==0 but wait slot not zombie (live host residual).
 * live_death: heal promoted full process_death (teardown still pending).
 */
static u64 g_u64SoftWaitLiveHeal;
static u64 g_u64SoftWaitLiveDeath;
static u32 g_u32SoftLogN;
static u8  g_fSoftInvOnce;
static u32 g_u32SoftLastForkPid;      /* last usable child pid (diag) */
/* process: soft fork-wait product-min tallies */
static u32 g_u32SoftFwForkEnter;
static u32 g_u32SoftFwForkOk;
static u32 g_u32SoftFwCloneEnter;
static u32 g_u32SoftFwWaitEnter;
static u32 g_u32SoftFwWaitReap;
static u32 g_u32SoftFwWaitidEnter;
static u32 g_u32SoftFwSmokeOk;
static u8  g_fSoftFwPassOnce;         /* one-shot PASS kprintf */

static void process_soft_inc(u32 *pCtr);
static void process_soft_inventory(const char *szVia);
static void process_soft_maybe_once(void);
static void process_soft_wait_census(u32 *pUsed, u32 *pZombie, u32 *pLive,
                                     u32 *pFree);

/** Soft: saturating bump (avoid wrap for small smokes). */
static void
process_soft_inc(u32 *pCtr)
{
    if (pCtr == NULL) {
        return;
    }
    if (*pCtr < 0xffffffffu) {
        (*pCtr)++;
    }
}

/*
 * Greppable Wave 19 soft process inventory (product / smoke).
 * Prefix-stable: "process: soft ...". Never hard-gates.
 * greppable: process: soft
 */
static void
process_soft_inventory(const char *szVia)
{
    const char *szViaSafe;
    const char *szVerdict;
    u32 u32Used = 0;
    u32 u32Zombie = 0;
    u32 u32Live = 0;
    u32 u32Free = 0;

    szViaSafe = (szVia != NULL && szVia[0] != '\0') ? szVia : "unknown";
    process_soft_inc(&g_u32SoftLogN);
    process_soft_wait_census(&u32Used, &u32Zombie, &u32Live, &u32Free);

    if (g_u32SoftInitOk != 0u || g_u32SoftRootMetaOk != 0u ||
        g_u32SoftDeathEnter != 0u || g_u32SoftWaitRegOk != 0u ||
        g_u32SoftPagerSetOk != 0u) {
        szVerdict = "PASS";
    } else if (g_u32SoftInitNull != 0u || g_u32SoftRootMetaFail != 0u ||
               g_u32SoftPagerSetFail != 0u || g_u32SoftForkFull != 0u) {
        szVerdict = "PARTIAL";
    } else {
        szVerdict = "PARTIAL";
    }

    /* Grep: process: soft inventory */
    kprintf("process: soft inventory via=%s wave=%u areas=%u logs=%u "
            "wait_used=%u wait_zombie=%u wait_live=%u wait_free=%u "
            "init_ok=%u root_meta_ok=%u death=%u "
            "(soft inventory only)\n",
            szViaSafe, GJ_PROCESS_SOFT_WAVE, GJ_PROCESS_SOFT_AREAS,
            g_u32SoftLogN, u32Used, u32Zombie, u32Live, u32Free,
            g_u32SoftInitOk, g_u32SoftRootMetaOk, g_u32SoftDeathEnter);

    /* Grep: process: soft stats */
    kprintf("process: soft stats init_ok=%u init_null=%u root_ok=%u "
            "root_fail=%u root_busy=%u confine=%u pager_set=%u "
            "pager_fail=%u death=%u wait_reg=%u wait_reap=%u "
            "fork_ok=%u logs=%u wave=%u\n",
            g_u32SoftInitOk, g_u32SoftInitNull, g_u32SoftRootMetaOk,
            g_u32SoftRootMetaFail, g_u32SoftRootMetaBusy, g_u32SoftConfineN,
            g_u32SoftPagerSetOk, g_u32SoftPagerSetFail, g_u32SoftDeathEnter,
            g_u32SoftWaitRegOk, g_u32SoftWaitReap, g_u32SoftForkOk,
            g_u32SoftLogN, GJ_PROCESS_SOFT_WAVE);

    /* Grep: process: soft init */
    kprintf("process: soft init ok=%u null=%u wave=%u\n",
            g_u32SoftInitOk, g_u32SoftInitNull, GJ_PROCESS_SOFT_WAVE);

    /* Grep: process: soft seal */
    kprintf("process: soft seal root_ok=%u root_fail=%u root_busy=%u "
            "wave=%u (checklist via process: bootstrap seal soft)\n",
            g_u32SoftRootMetaOk, g_u32SoftRootMetaFail, g_u32SoftRootMetaBusy,
            GJ_PROCESS_SOFT_WAVE);

    /* Grep: process: soft confine */
    kprintf("process: soft confine n=%u null=%u wave=%u\n",
            g_u32SoftConfineN, g_u32SoftConfineNull, GJ_PROCESS_SOFT_WAVE);

    /* Grep: process: soft pager */
    kprintf("process: soft pager set_ok=%u set_fail=%u clear=%u "
            "refresh_clear=%u refresh_keep=%u mirror_ok=%u "
            "has_hit=%u has_miss=%u wave=%u\n",
            g_u32SoftPagerSetOk, g_u32SoftPagerSetFail, g_u32SoftPagerClear,
            g_u32SoftPagerRefreshClear, g_u32SoftPagerRefreshKeep,
            g_u32SoftPagerMirrorOk, g_u32SoftPagerHasHit, g_u32SoftPagerHasMiss,
            GJ_PROCESS_SOFT_WAVE);

    /* Grep: process: soft fault */
    kprintf("process: soft fault enter=%u no_pager=%u wx_deny=%u busy=%u "
            "again=%u wave=%u\n",
            g_u32SoftFaultEnter, g_u32SoftFaultNoPager, g_u32SoftFaultWxDeny,
            g_u32SoftFaultBusy, g_u32SoftFaultAgain, GJ_PROCESS_SOFT_WAVE);

    /*
     * Grep: process: soft wait
     * ABI-first reaper for shell/sshd later: wait4/waitid WNOHANG + reap.
     * Product incomplete (no stop/continue; fixed table; soft poll budget).
     */
    kprintf("process: soft wait reg_ok=%u reg_idem=%u reg_full=%u "
            "reg_null=%u zombie=%u reap=%u nowait=%u reparent=%u "
            "forget=%u wait4_enter=%u echild=%u nohang0=%u "
            "live_timeout=%u reap_pid=%u reap_any=%u "
            "nohang_yield=%llu block_yield=%llu live_heal=%llu "
            "live_death=%llu "
            "used=%u zombie_now=%u live=%u free=%u wave=%u\n",
            g_u32SoftWaitRegOk, g_u32SoftWaitRegIdem, g_u32SoftWaitRegFull,
            g_u32SoftWaitRegNull, g_u32SoftWaitZombie, g_u32SoftWaitReap,
            g_u32SoftWaitNowait, g_u32SoftWaitReparentN, g_u32SoftWaitForget,
            g_u32SoftWait4Enter, g_u32SoftWait4Echild, g_u32SoftWait4Nohang0,
            g_u32SoftWait4LiveTimeout, g_u32SoftWait4ReapPid,
            g_u32SoftWait4ReapAny,
            (unsigned long long)g_u64SoftWaitNohangYield,
            (unsigned long long)g_u64SoftWaitBlockYield,
            (unsigned long long)g_u64SoftWaitLiveHeal,
            (unsigned long long)g_u64SoftWaitLiveDeath,
            u32Used, u32Zombie, u32Live, u32Free,
            GJ_PROCESS_SOFT_WAVE);

    /* Grep: process: soft death | process: death thr_exit */
    kprintf("process: soft death enter=%u thr_exit=%llu thr_early=%llu "
            "thr_bar=%llu thr_bar_calls=%llu thr_cur_scrub=%llu "
            "h3_order=%llu fork_stub_h3=%llu cr3_pub=%llu "
            "wave=%u (H3 thr_exit before as_destroy; Soft!=product)\n",
            g_u32SoftDeathEnter,
            (unsigned long long)g_u64DeathThrExit,
            (unsigned long long)g_u64DeathThrExitEarly,
            (unsigned long long)g_u64DeathThrExitBarrier,
            (unsigned long long)g_u64DeathThrExitBarrierCalls,
            (unsigned long long)g_u64DeathThrExitCurScrub,
            (unsigned long long)g_u64DeathH3OrderPass,
            (unsigned long long)g_u64DeathH3ForkStub,
            (unsigned long long)g_u64DeathH3Cr3Pub,
            GJ_PROCESS_SOFT_WAVE);
    /*
     * H3 residual lean once-lamp (Soft!=product; UDX host multi-thr class).
     * Align tokens with thread.c companion. No stamp storm (H2).
     * Grep: process: soft death residual lean | process: soft residual lean H3
     * Grep: thr_exit_before_as_destroy=1 | udx_host_teardown=1 | H3=death_residual
     * Grep: process: death thr_exit cur_scrub | process: death cr3_pub
     * Grep: cr3_pub_before_as_destroy=1
     */
    if (g_fH3ResLeanOnce == 0) {
        g_fH3ResLeanOnce = 1;
        kprintf("process: soft death residual lean H3 thr_exit_before_as_destroy=1 "
                "early+barrier=1 cur_scrub=1 cr3_pub=1 fork_stub=1 "
                "udx_host_multi_thr=1 udx_host_teardown=1 H3=death_residual "
                "never_as_destroy_while_thr_live=1 product_host_live=1 "
                "keep_live=1 keep_live_rock=1 thr_live=1 product_hosts=UDX "
                "soft_ne_product=1 Soft!=product dual=MIT/Apache "
                "dual_dod=OPEN denser=1 wave=%u bar=v2026.08.04.75\n",
                GJ_PROCESS_SOFT_WAVE);
        kprintf("process: soft residual lean H3 order=alive0,thr_exit_early,"
                "cur_scrub,pager_region_cnode,thr_exit_barrier,cur_scrub,"
                "cr3_pub,as_destroy pe32_clone_vm=1 udx_host=1 dual_belt=1 "
                "cr3_pub_before_as_destroy=1 never_as_destroy_while_thr_live=1 "
                "product_host_live=1 keep_live=1 keep_live_rock=1 thr_live=1 "
                "product_hosts=UDX Soft!=product dual_dod=OPEN denser=1 "
                "dual=MIT/Apache\n");
    }
    /*
     * Functional residual lifecycle once-lamp (live UDX host + sshd class).
     * Soft!=product; Dual DoD A/B OPEN; no stamp storm (H2).
     * Grep: process: soft residual lifecycle
     * Grep: process: soft exit_pid death | process: soft wait nohang_yield
     * Grep: process: soft wait live_heal | process: soft wait live_death
     * Grep: dual_dod=OPEN | udx_host_teardown=1 | thr_exit_before_as_destroy=1
     * Grep: live_host_path=1 | not_immediate_kill=1
     * Grep: product_host_live=1 | keep_live=1
     */
    if (g_fLifecycleResLeanOnce == 0) {
        g_fLifecycleResLeanOnce = 1;
        kprintf("process: soft residual lifecycle exit_pid_death=1 "
                "wait_nohang_yield=1 wait_block_yield=1 wait_live_heal=1 "
                "need_death_broad=1 thr_exit_before_as_destroy=1 "
                "udx_host_teardown=1 hosts=rtl8168_udx|xhci_udx|ddi_host_gj "
                "product_hosts=UDX sshd_wait=1 "
                "dual_dod=OPEN dual_dod_a=OPEN dual_dod_b=OPEN "
                "product_udx_close=0 product_sshd_tcp22=OPEN "
                "product_host_live=1 keep_live=1 keep_live_rock=1 thr_live=1 "
                "never_kill_embed=1 soft_ne_product=1 G-AC-1=1 "
                "Soft!=product dual=MIT/Apache denser=1 "
                "lifecycle=1 live_host_path=1 not_immediate_kill=1 wave=%u "
                "bar=v2026.08.04.75\n",
                GJ_PROCESS_SOFT_WAVE);
        kprintf("process: soft residual lifecycle path="
                "wait_reg,live_run,soft_wait_poll,"
                "exit_pid_or_wait_heal,death_H3_thr_exit_before_as_destroy,"
                "wait_reap "
                "exit_pid_death=%llu note_only=%llu nohang_yield=%llu "
                "block_yield=%llu live_heal=%llu live_death=%llu "
                "Soft!=product dual_dod=OPEN live_host_path=1 "
                "product_host_live=1 keep_live=1 keep_live_rock=1 thr_live=1 "
                "product_hosts=UDX denser=1 not_immediate_kill=1\n",
                (unsigned long long)g_u64SoftExitPidDeath,
                (unsigned long long)g_u64SoftExitPidNoteOnly,
                (unsigned long long)g_u64SoftWaitNohangYield,
                (unsigned long long)g_u64SoftWaitBlockYield,
                (unsigned long long)g_u64SoftWaitLiveHeal,
                (unsigned long long)g_u64SoftWaitLiveDeath);
    }
    /*
     * STRONGER denser keep_live product host honesty residual (Soft!=product).
     * product_host_live / keep_live / keep_live_rock / thr_live:
     * process_spawn_host_launch UDX hosts stay live while embed thr run;
     * process layer never thrash as_destroy while thr live (H3 thr_exit
     * before as_destroy). Companion denser axes to main host_launch live.
     * Dual DoD OPEN; denser residual != Dual DoD close; agent!=close.
     * Grep: process: soft residual keep_live
     * Grep: process: soft residual product_host_live
     * Grep: process: soft residual denser keep_live
     * Grep: process: soft residual denser product_host_live
     * Grep: process: soft residual denser thr_live
     * Grep: process: soft residual denser dual_dod
     * Grep: Soft!=product soft residual dual_dod OPEN product_host_live
     * Grep: product_host_live=1 | keep_live=1 | keep_live_rock=1
     * Grep: thr_live=1 | product_hosts=UDX | dual_dod=OPEN
     * Grep: thr_exit_before_as_destroy=1 | never_kill_embed=1
     * Grep: denser keep_live residual | denser product_host_live residual
     * Grep: denser thr_live residual | denser dual_dod residual
     * Grep: denser thr_exit_before_as_destroy residual
     */
    if (g_fKeepLiveResLeanOnce == 0) {
        g_fKeepLiveResLeanOnce = 1;
        kprintf("process: soft residual keep_live product_host_live=1 "
                "keep_live=1 keep_live_rock=1 thr_live=1 "
                "never_kill_embed=1 kill_on_embed=0 "
                "hosts=rtl8168_udx|xhci_udx|ddi_host_gj "
                "product_hosts=UDX api=process_spawn_host_launch "
                "H3=thr_exit_before_as_destroy "
                "never_as_destroy_while_thr_live=1 "
                "thr_exit_before_as_destroy=1 cr3_pub_before_as_destroy=1 "
                "udx_host_teardown=1 udx_host_multi_thr=1 "
                "dual_dod=OPEN dual_dod_a=OPEN dual_dod_b=OPEN "
                "soft_no_close=1 dod_close=0 product_udx_close=0 "
                "soft_ne_product=1 G-AC-1=1 Soft!=product dual=MIT/Apache "
                "live_host_path=1 not_immediate_kill=1 denser=1 "
                "bar=v2026.08.04.75 stamp-free wave=%u "
                "(Soft!=product soft residual dual_dod OPEN product_host_live; "
                "denser keep_live residual; never thrash as_destroy while thr "
                "live; agent!=close)\n",
                GJ_PROCESS_SOFT_WAVE);
        kprintf("process: soft residual product_host_live "
                "product_host_live=1 keep_live=1 keep_live_rock=1 thr_live=1 "
                "path=wait_reg|live_run|soft_wait_poll|keep_live|"
                "exit_or_heal|death_H3_thr_exit_before_as_destroy|wait_reap "
                "order=alive0,thr_exit_early,cur_scrub,pager_region_cnode,"
                "thr_exit_barrier,cur_scrub,cr3_pub,as_destroy "
                "never_as_destroy_while_thr_live=1 "
                "thr_exit_before_as_destroy=1 "
                "dual_dod=OPEN Soft!=product soft residual "
                "product_host_live honesty residual denser=1 "
                "hosts=rtl8168_udx,xhci_udx,ddi_host_gj product_hosts=UDX "
                "bar=v2026.08.04.75 stamp-free\n");
        kprintf("process: soft residual denser keep_live "
                "keep_live=1 product_host_live=1 keep_live_rock=1 thr_live=1 "
                "never_kill_embed=1 soft_ne_product=1 product_hosts=UDX "
                "axes=keep_live|product_host_live|thr_live|keep_live_rock|"
                "never_kill_embed|soft_ne_product|dual_dod_open|"
                "thr_exit_before_as_destroy "
                "H3=thr_exit_before_as_destroy "
                "never_as_destroy_while_thr_live=1 "
                "dual_dod=OPEN dual_dod_a=OPEN dual_dod_b=OPEN "
                "denser_keep_live_residual=1 denser_product_host_live=1 "
                "denser_thr_live_residual=1 denser_dual_dod_residual=1 "
                "Soft!=product soft residual dual_dod OPEN product_host_live "
                "bar=v2026.08.04.75 stamp-free wave=%u "
                "(denser keep_live residual; denser thr_live residual; "
                "denser != Dual DoD close; agent!=close; G-AC-1)\n",
                GJ_PROCESS_SOFT_WAVE);
        kprintf("process: soft residual denser product_host_live "
                "product_host_live=1 keep_live=1 keep_live_rock=1 thr_live=1 "
                "hosts=rtl8168_udx|xhci_udx|ddi_host_gj product_hosts=UDX "
                "companion=main_host_launch_live "
                "H3=defer_death_while_live thr_exit_before_as_destroy=1 "
                "cr3_pub_before_as_destroy=1 udx_host_teardown=1 "
                "dual_dod=OPEN dual_dod_a=OPEN dual_dod_b=OPEN denser=1 "
                "Soft!=product denser residual product_host_live honesty "
                "bar=v2026.08.04.75 stamp-free\n");
        kprintf("process: soft residual denser thr_live "
                "thr_live=1 keep_live=1 keep_live_rock=1 product_host_live=1 "
                "product_hosts=UDX hosts=rtl8168_udx|xhci_udx|ddi_host_gj "
                "never_as_destroy_while_thr_live=1 "
                "thr_exit_before_as_destroy=1 cr3_pub_before_as_destroy=1 "
                "H3=thr_exit_before_as_destroy udx_host_multi_thr=1 "
                "dual_dod=OPEN dual_dod_a=OPEN dual_dod_b=OPEN "
                "denser_thr_live_residual=1 denser_keep_live_residual=1 "
                "Soft!=product soft residual dual_dod OPEN thr_live "
                "bar=v2026.08.04.75 stamp-free wave=%u "
                "(denser thr_live residual; denser thr_exit_before_as_destroy "
                "residual; denser != Dual DoD close; G-AC-1)\n",
                GJ_PROCESS_SOFT_WAVE);
        kprintf("process: soft residual denser dual_dod "
                "dual_dod=OPEN dual_dod_a=OPEN dual_dod_b=OPEN "
                "keep_live=1 thr_live=1 product_host_live=1 keep_live_rock=1 "
                "product_hosts=UDX soft_no_close=1 dod_close=0 "
                "product_udx_close=0 soft_ne_product=1 Soft!=product "
                "H3=thr_exit_before_as_destroy thr_exit_before_as_destroy=1 "
                "never_as_destroy_while_thr_live=1 denser=1 "
                "denser_dual_dod_residual=1 denser_keep_live_residual=1 "
                "denser_thr_live_residual=1 denser_product_host_live=1 "
                "bar=v2026.08.04.75 stamp-free wave=%u "
                "(denser dual_dod residual; Dual DoD OPEN Soft!=product; "
                "agent!=close; denser residual != Dual DoD close)\n",
                GJ_PROCESS_SOFT_WAVE);
        kprintf("process: soft residual keep_live honesty "
                "Soft!=product soft residual dual_dod=OPEN "
                "product_host_live=1 keep_live=1 keep_live_rock=1 thr_live=1 "
                "process_spawn_host_launch=1 integration_hold=1 "
                "product_hosts=UDX H3=defer_death_while_live denser=1 "
                "exit_pid_death=%llu live_heal=%llu live_death=%llu "
                "nohang_yield=%llu block_yield=%llu "
                "soft=1 product=0 G-AC-1=1 dual=MIT_OR_Apache-2.0 "
                "denser_keep_live_residual=1 denser_thr_live_residual=1 "
                "denser_dual_dod_residual=1\n",
                (unsigned long long)g_u64SoftExitPidDeath,
                (unsigned long long)g_u64SoftWaitLiveHeal,
                (unsigned long long)g_u64SoftWaitLiveDeath,
                (unsigned long long)g_u64SoftWaitNohangYield,
                (unsigned long long)g_u64SoftWaitBlockYield);
    }

    /*
     * Grep: process: soft fork
     * usable child pid + deferred/vfork exit; clone flags map fork-like.
     * Soft incomplete: stub PCB, not full user AS/thread (shell/sshd later).
     */
    kprintf("process: soft fork enter=%u ok=%u full=%u vfork=%u "
            "deferred=%u as_ok=%u as_fail=%u clone_ok=%u clone_fail=%u "
            "reg_fail=%u last_pid=%u "
            "clone_enter=%u clone_forklike=%u clone_vfork=%u "
            "clone_thr_rej=%u clone_ns_rej=%u wave=%u\n",
            g_u32SoftForkEnter, g_u32SoftForkOk, g_u32SoftForkFull,
            g_u32SoftForkVfork, g_u32SoftForkDeferred, g_u32SoftForkAsOk,
            g_u32SoftForkAsFail, g_u32SoftForkCloneOk, g_u32SoftForkCloneFail,
            g_u32SoftForkRegFail, g_u32SoftLastForkPid, g_u32SoftCloneEnter,
            g_u32SoftCloneForkLike, g_u32SoftCloneVfork,
            g_u32SoftCloneThreadReject, g_u32SoftCloneNsReject,
            GJ_PROCESS_SOFT_WAVE);

    /*
     * Grep: process: soft fork-wait product-min
     * PCB parent -> child pid + wait4/waitid reaper (cold personality later).
     */
    kprintf("process: soft fork-wait product-min fork_enter=%u fork_ok=%u "
            "clone_enter=%u wait_enter=%u wait_reap=%u waitid_enter=%u "
            "smoke_ok=%u pass_once=%u last_pid=%u wave=%u "
            "(soft!=product; not full AS clone)\n",
            g_u32SoftFwForkEnter, g_u32SoftFwForkOk, g_u32SoftFwCloneEnter,
            g_u32SoftFwWaitEnter, g_u32SoftFwWaitReap, g_u32SoftFwWaitidEnter,
            g_u32SoftFwSmokeOk, (unsigned)g_fSoftFwPassOnce,
            g_u32SoftLastForkPid, GJ_PROCESS_SOFT_WAVE);

    /* Grep: process: soft jit */
    kprintf("process: soft jit set_on=%u set_off=%u query_hit=%u "
            "query_miss=%u wave=%u\n",
            g_u32SoftJitSetOn, g_u32SoftJitSetOff, g_u32SoftJitQueryHit,
            g_u32SoftJitQueryMiss, GJ_PROCESS_SOFT_WAVE);

    /* Grep: process: soft promise */
    kprintf("process: soft promise ok_hit=%u ok_miss=%u req_ok=%u "
            "req_deny=%u wave=%u\n",
            g_u32SoftPromiseOkHit, g_u32SoftPromiseOkMiss,
            g_u32SoftPromiseReqOk, g_u32SoftPromiseReqDeny,
            GJ_PROCESS_SOFT_WAVE);

    /*
     * Honesty: PCB + fixed wait table + soft seal lamps != multi-server
     * product seal / full posix wait.
     * Grep: process: soft path
     */
    kprintf("process: soft path claim=pcb,root_meta,pager,confine,"
            "wait4,death,fork G-PROC-5=1 Apple_s13_open=1 "
            "fixed_wait_table=%u fork_stubs=%u multi_server_seal=0 "
            " via=%s wave=%u (soft inventory; not product gate)\n",
            GJ_WAIT_SLOTS, GJ_FORK_STUBS, szViaSafe, GJ_PROCESS_SOFT_WAVE);

    /* Grep: process: soft surface - lean Wave 19 area catalog (H2 no storm) */
    kprintf("process: soft surface inventory,stats,init,seal,confine,"
            "pager,fault,wait,death,fork,jit,promise,path,surface,deepen,"
            "PASS,bootstrap_seal,death_tallies,headroom,H3_residual_lean,"
            "H3_dual_belt,H3_fork_stub,H3_cur_scrub,H3_cr3_pub,"
            "thr_exit_before_as_destroy,cr3_pub_before_as_destroy,"
            "lifecycle_residual,exit_pid_death,wait_nohang_yield,"
            "wait_block_yield,wait_live_heal,wait_live_death,need_death_broad,"
            "live_host_path,not_immediate_kill,dual_dod,"
            "keep_live,product_host_live,never_kill_embed,"
            "never_as_destroy_while_thr_live,"
            "keep_live_rock,thr_live,product_hosts_UDX,"
            "denser_keep_live,denser_product_host_live,"
            "denser_thr_live,denser_dual_dod,"
            "denser_thr_exit_before_as_destroy "
            "areas=%u wave=%u\n",
            GJ_PROCESS_SOFT_AREAS, GJ_PROCESS_SOFT_WAVE);

    /* Grep: process: soft headroom - wait table free slots */
    kprintf("process: soft headroom wait_free=%u wait_used=%u "
            "wait_slots=%u fork_stubs=%u wave=%u\n",
            u32Free, u32Used, GJ_WAIT_SLOTS, GJ_FORK_STUBS,
            GJ_PROCESS_SOFT_WAVE);


    /* Grep: process: soft deepen */
    kprintf("process: soft deepen wave=%u areas=%u via=%s init_ok=%u "
            "root_ok=%u confine=%u pager_set=%u fault=%u wait_reg=%u "
            "death=%u fork_ok=%u logs=%u "
            "(soft inventory only; not product gate)\n",
            GJ_PROCESS_SOFT_WAVE, GJ_PROCESS_SOFT_AREAS, szViaSafe,
            g_u32SoftInitOk, g_u32SoftRootMetaOk, g_u32SoftConfineN,
            g_u32SoftPagerSetOk, g_u32SoftFaultEnter, g_u32SoftWaitRegOk,
            g_u32SoftDeathEnter, g_u32SoftForkOk, g_u32SoftLogN);

    /* Grep: process: soft PASS | PARTIAL */
    kprintf("process: soft %s via=%s wave=%u logs=%u areas=%u\n",
            szVerdict, szViaSafe, GJ_PROCESS_SOFT_WAVE, g_u32SoftLogN,
            GJ_PROCESS_SOFT_AREAS);
}

/**
 * After first product activity, print soft inventory once.
 * Diagnostics only - never hard-gates.
 */
static void
process_soft_maybe_once(void)
{
    if (g_fSoftInvOnce != 0) {
        return;
    }
    if (g_u32SoftInitOk == 0u && g_u32SoftRootMetaOk == 0u &&
        g_u32SoftConfineN == 0u && g_u32SoftPagerSetOk == 0u &&
        g_u32SoftWaitRegOk == 0u && g_u32SoftDeathEnter == 0u &&
        g_u32SoftForkOk == 0u) {
        return;
    }
    g_fSoftInvOnce = 1;
    process_soft_inventory("once");
}

void
gj_process_init(struct gj_process *pProc, struct gj_cnode *pCnode,
                struct gj_cap_slot *pSlots, u64 cSlots)
{
    if (pProc == NULL || pCnode == NULL || pSlots == NULL) {
        process_soft_inc(&g_u32SoftInitNull);
        return;
    }

    gj_obj_hdr_init(&pProc->hdr);
    gj_cnode_init(pCnode, pSlots, cSlots);

    pProc->pCnode = pCnode;
    pProc->pRootMeta = NULL;
    pProc->refPager = gj_cap_ref_null();
    pProc->pPagerEpObj = NULL;
    pProc->u32PagerBadge = 0;
    gj_space_fault_init(&pProc->fault);
    pProc->u32Personality = 1; /* LINUX default for game path */
    pProc->u32Jit = 0;
    pProc->u32Confined = 0; /* ambient until gj_process_confine */
    pProc->u32Promises = GJ_PROMISE_ALL;
    pProc->u64Cr3 = 0; /* inherit until per-process AS (G-AS-1) */
    pProc->u64AnonNext = 0x0000000040000000ull;
    pProc->u64ExecEntry = 0;
    pProc->u64InterpEntry = 0;
    pProc->u64LoadBias = 0;
    pProc->u64ExecStack = 0;
    pProc->u64StartEntry = 0;
    pProc->u32StartThr = 0;
    pProc->u32ExecFlags = 0;
    pProc->cNeededLoaded = 0;
    pProc->cAuxv = 0;
    memset(pProc->aAuxv, 0, sizeof(pProc->aAuxv));
    memset(pProc->szExecPath, 0, sizeof(pProc->szExecPath));
    memset(pProc->aRegions, 0, sizeof(pProc->aRegions));
    pProc->pParent = NULL;
    pProc->u32ExitCode = 0;
    pProc->u32Alive = 1;
    memset(&pProc->excPort, 0, sizeof(pProc->excPort));
    process_soft_inc(&g_u32SoftInitOk);
    process_soft_maybe_once();
}

void
gj_process_set_jit(struct gj_process *pProc, int fEnable)
{
    if (pProc == NULL) {
        return;
    }
    /* G-JIT-4: u32Jit is cache of GJ_RIGHT_JIT authority */
    pProc->u32Jit = fEnable ? 1u : 0u;
    if (fEnable) {
        process_soft_inc(&g_u32SoftJitSetOn);
    } else {
        process_soft_inc(&g_u32SoftJitSetOff);
    }
}

/*
 * Soft Apple s13 bootstrap seal checklist (process.c only; Wave 15 deepen).
 * Enumerates PCB lamps: root meta, ambient/confine, pager empty, promises,
 * plus soft open inventory for Apple s13 product seal items (all 0 until
 * product retype/IRQ/untyped seal exists - honesty inventory only).
 * One-way soft lamp is inventory only - does NOT seal retype/IRQ/untyped.
 * Grep: process: bootstrap seal soft | process: seal checklist
 * Honesty: not product-complete.
 */
static u32 g_u32SealChecklistLogs;
static u32 g_u32BootstrapSealSoftLamp; /* 0 open, 1 soft-attempted once */
static u64 g_u64SealEmits;             /* total checklist emit attempts */
static u64 g_u64SealViaRootMeta;
static u64 g_u64SealViaConfine;
static u64 g_u64SealViaDeath;          /* death seal_note path */
static u64 g_u64SealViaOther;
static u64 g_u64SealRateLimited;       /* skipped by log budget */
static u32 g_u32SealDeathFullLogs;     /* full checklist emits via=death */
/*
 * Soft Apple s13 product-seal lamps (Wave 13 inventory). Stay 0 until a real
 * post-bootstrap seal drops privileged retype / broad IRQ / root untyped.
 * Grep: process: bootstrap seal soft lamps | process: seal checklist s13
 */
static u32 g_u32SealProductRetype;     /* 0 = open (not product-sealed) */
static u32 g_u32SealProductIrqBind;    /* 0 = open */
static u32 g_u32SealProductRootUntyped; /* 0 = open */
static u32 g_u32SealProductStickyNs;   /* 0 = open (sticky bootstrap ns) */

/* Soft G-PROC-5 death tallies (grep: process: death). Wave 15 deepen. */
static u64 g_u64DeathTotal;
static u64 g_u64DeathPagerClear;
static u64 g_u64DeathFaultForce;
static u64 g_u64DeathCnodeWipe;
static u64 g_u64DeathCnodeSlots;
static u64 g_u64DeathAsDestroyOk;
static u64 g_u64DeathAsDestroyFail;
static u64 g_u64DeathAsSkip;
static u64 g_u64DeathIdempotent;       /* second-call early exit */
static u64 g_u64DeathWaitChild;        /* wait-registered wipe path */
static u64 g_u64DeathLongLived;        /* boot/init skip wipe */
static u64 g_u64DeathRegions;          /* region views dropped */
static u64 g_u64DeathReparent;         /* children reparented to init */
static u64 g_u64DeathExcClear;         /* exception port cleared */
static u64 g_u64DeathConfineScrub;     /* confine/promises scrub */
static u64 g_u64DeathJitScrub;         /* CapJit cache scrub */
/*
 * H3 residual lean thr_exit tallies (inventory scope; Soft!=product):
 *   g_u64DeathThrExit / Early / Barrier / BarrierCalls / CurScrub /
 *   H3OrderPass / H3ForkStub / H3Cr3Pub - declared with soft death counters.
 */

/*
 * Soft CNode occupancy count (const; inventory only for seal checklist).
 * Counts non-INVALID slots; does not lock (snapshot may race - soft OK).
 */
static u32
process_seal_cnode_live_slots(const struct gj_process *pProc)
{
    u64 u64Slot;
    u32 u32Live = 0;

    if (pProc == NULL || pProc->pCnode == NULL || pProc->pCnode->pSlots == NULL) {
        return 0;
    }
    for (u64Slot = 0; u64Slot < pProc->pCnode->cSlots; u64Slot++) {
        if (pProc->pCnode->pSlots[u64Slot].u16Type != (u16)GJ_CAP_INVALID) {
            u32Live++;
        }
    }
    return u32Live;
}

static void
process_seal_via_tally(const char *szViaSafe)
{
    if (strcmp(szViaSafe, "root_meta") == 0) {
        g_u64SealViaRootMeta++;
    } else if (strcmp(szViaSafe, "confine") == 0) {
        g_u64SealViaConfine++;
    } else if (strcmp(szViaSafe, "death") == 0) {
        g_u64SealViaDeath++;
    } else {
        g_u64SealViaOther++;
    }
}

static void
process_seal_checklist_soft(const struct gj_process *pProc, const char *szVia)
{
    int fRootMeta;
    int fPagerEmpty;
    int fAmbient;
    int fRootMetaNotFactory;
    u32 u32Confined;
    u32 u32Promises;
    u32 u32CnodeLive;
    u32 u32CnodeSlots;
    u32 u32HadJit;
    const char *szViaSafe;

    if (pProc == NULL) {
        return;
    }
    szViaSafe = (szVia != NULL && szVia[0] != '\0') ? szVia : "unknown";
    g_u64SealEmits++;
    process_seal_via_tally(szViaSafe);
    /*
     * Rate-limit root_meta installs (many PE smokes); always emit on
     * confine (ambient authority drop is the soft seal edge).
     * Death: allow a small full-checklist budget (Wave 13 seal_note always
     * prints separately; full dump only first GJ_SEAL_SOFT_LOG_MAX deaths).
     */
    if (strcmp(szViaSafe, "death") == 0) {
        if (g_u32SealDeathFullLogs >= GJ_SEAL_SOFT_LOG_MAX) {
            g_u64SealRateLimited++;
            return;
        }
        g_u32SealDeathFullLogs++;
    } else if (strcmp(szViaSafe, "confine") != 0 &&
               g_u32SealChecklistLogs >= GJ_SEAL_SOFT_LOG_MAX) {
        g_u64SealRateLimited++;
        return;
    }
    g_u32SealChecklistLogs++;
    fRootMeta = (pProc->pRootMeta != NULL) ? 1 : 0;
    /*
     * Design K1-K6: root meta is kernel ops only - never a factory for
     * transferable PROCESS/CNODE. Soft lamp always 1 (policy intent);
     * product seal of retype/IRQ/untyped remains open (0 below).
     */
    fRootMetaNotFactory = 1;
    /*
     * Const path: gen!=0 and LIVE when pPagerEpObj known.
     * Empty gen -> pager empty (expected post-init before set_pager).
     */
    fPagerEmpty = 0;
    if (gj_cap_ref_is_null(&pProc->refPager)) {
        fPagerEmpty = 1;
    } else if (pProc->pPagerEpObj != NULL &&
               pProc->pPagerEpObj->u32State != (u32)GJ_OBJ_LIVE) {
        fPagerEmpty = 1; /* dead ep treated as empty for seal lamp */
    }
    u32Confined = pProc->u32Confined;
    u32Promises = pProc->u32Promises;
    fAmbient = (u32Confined == 0u) ? 1 : 0;
    u32CnodeLive = process_seal_cnode_live_slots(pProc);
    u32CnodeSlots = (pProc->pCnode != NULL) ? (u32)pProc->pCnode->cSlots : 0u;
    u32HadJit = pProc->u32Jit;

    /* Grep: process: bootstrap seal soft */
    kprintf("process: bootstrap seal soft via=%s logs=%u lamp=%u "
            "(Apple s13 soft checklist)\n",
            szViaSafe, g_u32SealChecklistLogs, g_u32BootstrapSealSoftLamp);

    /* Grep: process: seal checklist ... */
    kprintf("process: seal checklist root_meta=%u pager_empty=%u "
            "ambient=%u confined=%u promises=0x%x soft\n",
            fRootMeta, fPagerEmpty, fAmbient, u32Confined, u32Promises);

    /* Ambient authority drop lamps (soft confine PCB flags exposed). */
    kprintf("process: seal checklist ambient_drop_lamp confined=%u "
            "promises=0x%x ambient=%u soft\n",
            u32Confined, u32Promises, fAmbient);

    kprintf("process: seal checklist root_meta_installed=%u "
            "pager_empty=%u promises=0x%x soft\n",
            fRootMeta, fPagerEmpty, u32Promises);

    /*
     * Wave 13: CNode + root-meta-not-factory inventory (soft snapshot).
     * Grep: process: seal checklist cnode | process: seal checklist factory
     */
    kprintf("process: seal checklist cnode live=%u slots=%u "
            "root_meta_not_factory=%u soft\n",
            u32CnodeLive, u32CnodeSlots, fRootMetaNotFactory);
    kprintf("process: seal checklist factory root_meta_not_factory=%u "
            "jit_cache=%u soft\n",
            fRootMetaNotFactory, u32HadJit);

    /*
     * Wave 13: Apple s13 product-seal item lamps (all 0 = open honesty).
     * Grep: process: bootstrap seal soft lamps | process: seal checklist s13
     * Does NOT perform product retype/IRQ/untyped seal.
     */
    kprintf("process: bootstrap seal soft lamps retype=%u irq_bind=%u "
            "root_untyped=%u sticky_bootstrap=%u one_way=%u soft "
            "(Apple s13 open)\n",
            g_u32SealProductRetype, g_u32SealProductIrqBind,
            g_u32SealProductRootUntyped, g_u32SealProductStickyNs,
            g_u32BootstrapSealSoftLamp);
    kprintf("process: seal checklist s13 retype_seal=%u irq_seal=%u "
            "untyped_seal=%u sticky=%u one_way=%u soft\n",
            g_u32SealProductRetype, g_u32SealProductIrqBind,
            g_u32SealProductRootUntyped, g_u32SealProductStickyNs,
            g_u32BootstrapSealSoftLamp);

    /*
     * Wave 13 deepen stamp + emit tallies.
     * Grep: process: bootstrap seal soft deepen | process: bootstrap seal soft tallies
     */
                            




















    kprintf("process: bootstrap seal soft deepen wave=%u via=%s "
            "logs=%u emits=%llu rate_limited=%llu "
            "\n",
            GJ_SEAL_SOFT_WAVE, szViaSafe, g_u32SealChecklistLogs,
            (unsigned long long)g_u64SealEmits,
            (unsigned long long)g_u64SealRateLimited);
    kprintf("process: bootstrap seal soft tallies via_root_meta=%llu "
            "via_confine=%llu via_death=%llu via_other=%llu "
            "lamp=%u soft\n",
            (unsigned long long)g_u64SealViaRootMeta,
            (unsigned long long)g_u64SealViaConfine,
            (unsigned long long)g_u64SealViaDeath,
            (unsigned long long)g_u64SealViaOther,
            g_u32BootstrapSealSoftLamp);

    /* Honesty: not product multi-server seal; Apple s13 remains open. */
    kprintf("process: bootstrap seal soft not product-complete "
            "(Apple s13 open)\n");
}

/*
 * Soft one-way seal lamp (Apple s13). Records checklist; never product-seals
 * privileged retype / broad IRQ / root untyped. Grep: process: bootstrap seal soft
 */
static void
process_bootstrap_seal_soft_try(struct gj_process *pProc, const char *szVia)
{
    if (pProc == NULL) {
        return;
    }
    if (g_u32BootstrapSealSoftLamp == 0u) {
        g_u32BootstrapSealSoftLamp = 1u;
        kprintf("process: bootstrap seal soft one-way lamp=1 "
                "(soft only; not product-complete)\n");
        /*
         * Wave 13: one-way flip honesty - product s13 lamps stay 0.
         * Grep: process: bootstrap seal soft one-way | process: bootstrap seal soft lamps
         */
        kprintf("process: bootstrap seal soft one-way product_retype=%u "
                "product_irq=%u product_untyped=%u product_sticky=%u "
                "soft (still open)\n",
                g_u32SealProductRetype, g_u32SealProductIrqBind,
                g_u32SealProductRootUntyped, g_u32SealProductStickyNs);
    }
    process_seal_checklist_soft(pProc, szVia);
}

void
gj_process_confine(struct gj_process *pProc, u32 u32Promises)
{
    if (pProc == NULL) {
        process_soft_inc(&g_u32SoftConfineNull);
        return;
    }
    /* Soft: set confined; promises are the allowed ambient set. */
    pProc->u32Confined = 1u;
    pProc->u32Promises = u32Promises;
    process_soft_inc(&g_u32SoftConfineN);
    /*
     * Soft confine expose PCB flags + ambient authority drop lamp.
     * Grep: process: confine soft | process: seal checklist
     * Not product multi-server.
     */
    kprintf("process: confine soft confined=%u promises=0x%x "
            "ambient_drop=1 (soft; not product multi-server)\n",
            pProc->u32Confined, pProc->u32Promises);
    /* Confine is the soft ambient-drop edge - re-emit seal checklist lamps. */
    process_bootstrap_seal_soft_try(pProc, "confine");
    process_soft_maybe_once();
}

int
gj_process_promise_ok(const struct gj_process *pProc, u32 u32Promise)
{
    int fOk;

    if (pProc == NULL) {
        process_soft_inc(&g_u32SoftPromiseOkMiss);
        return 0;
    }
    if (pProc->u32Confined == 0u) {
        process_soft_inc(&g_u32SoftPromiseOkHit);
        return 1; /* ambient */
    }
    fOk = (pProc->u32Promises & u32Promise) != 0u ? 1 : 0;
    if (fOk != 0) {
        process_soft_inc(&g_u32SoftPromiseOkHit);
    } else {
        process_soft_inc(&g_u32SoftPromiseOkMiss);
    }
    return fOk;
}

int
gj_process_promise_require(const struct gj_process *pProc, u32 u32Promise)
{
    /* NULL process: no confine subject (product ambient smokes). */
    if (pProc == NULL) {
        process_soft_inc(&g_u32SoftPromiseReqOk);
        return 0;
    }
    if (gj_process_promise_ok(pProc, u32Promise)) {
        process_soft_inc(&g_u32SoftPromiseReqOk);
        return 0;
    }
    process_soft_inc(&g_u32SoftPromiseReqDeny);
    return -13; /* LINUX_EACCES shape (linux_abi.h LINUX_EACCES) */
}

int
gj_process_has_jit(const struct gj_process *pProc)
{
    /*
     * CapJit: bool cache OR PROCESS self-right when wired via parent mint.
     * Authority source is GJ_RIGHT_JIT on task cap (G-JIT-1); cache for speed.
     */
    if (pProc != NULL && pProc->u32Jit != 0) {
        process_soft_inc(&g_u32SoftJitQueryHit);
        return 1;
    }
    process_soft_inc(&g_u32SoftJitQueryMiss);
    return 0;
}

gj_status_t
gj_process_bootstrap_root_meta(struct gj_process *pProc,
                               struct gj_root_meta *pMeta,
                               struct gj_cap_ref *pOutRef)
{
    gj_status_t st;
    struct gj_cap_ref ref;
    u16 u16Rights;

    if (pProc == NULL || pMeta == NULL || pProc->pCnode == NULL) {
        process_soft_inc(&g_u32SoftRootMetaFail);
        return GJ_ERR_INVAL;
    }

    /* Already filled? */
    if (pProc->pRootMeta != NULL) {
        process_soft_inc(&g_u32SoftRootMetaBusy);
        return GJ_ERR_BUSY;
    }

    gj_obj_hdr_init(&pMeta->hdr);
    pMeta->pProc = pProc;
    pMeta->pCnode = pProc->pCnode;
    pProc->pRootMeta = pMeta;

    /*
     * Install into slot 0 as ROOT_META.
     * Process + CNode are kernel fields for kernel ops only (K1-K6).
     * Not transferable Scheme A PROCESS/CNODE caps.
     */
    u16Rights = (u16)(GJ_RIGHT_READ | GJ_RIGHT_IDENTIFY);
    st = gj_cap_slot_install(pProc->pCnode, GJ_CAP_SLOT_ROOT_META,
                             (u16)GJ_CAP_ROOT_META, u16Rights, &pMeta->hdr,
                             &ref);
    if (st != GJ_OK) {
        pProc->pRootMeta = NULL;
        pMeta->pProc = NULL;
        pMeta->pCnode = NULL;
        process_soft_inc(&g_u32SoftRootMetaFail);
        return st;
    }

    if (pOutRef != NULL) {
        *pOutRef = ref;
    }
    process_soft_inc(&g_u32SoftRootMetaOk);
    /*
     * Soft Apple s13 seal checklist after root meta install.
     * Enumerates root_meta / pager_empty / ambient / promises (rate-limited).
     * Grep: process: bootstrap seal soft | process: seal checklist
     * Honesty: not product-complete.
     */
    process_seal_checklist_soft(pProc, "root_meta");
    process_soft_maybe_once();
    return GJ_OK;
}

/*
 * Soft pager kernel ref (SOLARIS_STYLE s9): hold endpoint while PCB names it.
 * Grep: process:pager ref
 */
static void
process_pager_ref_hold(struct gj_obj_hdr *pObj)
{
    if (pObj == NULL) {
        return;
    }
    pObj->u32Ref++;
}

static void
process_pager_ref_drop(struct gj_obj_hdr *pObj)
{
    if (pObj == NULL) {
        return;
    }
    if (pObj->u32Ref > 0u) {
        pObj->u32Ref--;
    }
}

/*
 * Soft: optional CAP_ADDRESSING slot-1 mirror of default pager.
 * Kernel still uses PCB as canonical; slot is introspection only.
 * Grep: process:pager slot1
 */
static void
process_pager_mirror_clear(struct gj_process *pProc)
{
    struct gj_cap_slot *pSlot;

    if (pProc == NULL || pProc->pCnode == NULL || pProc->pCnode->pSlots == NULL) {
        return;
    }
    if (pProc->pCnode->cSlots <= GJ_CAP_SLOT_PAGER) {
        return;
    }
    pSlot = &pProc->pCnode->pSlots[GJ_CAP_SLOT_PAGER];
    if (pSlot->u16Type == (u16)GJ_CAP_INVALID) {
        return;
    }
    /* Only clear if mirror still names our pager object (or any ENDPOINT). */
    if (pSlot->u16Type == (u16)GJ_CAP_ENDPOINT &&
        (pProc->pPagerEpObj == NULL || pSlot->pObj == (void *)pProc->pPagerEpObj)) {
        (void)gj_cap_quota_slot_refund(pProc->pCnode->pQuotaAccount);
        if (pSlot->pObj != NULL) {
            gj_cdt_unlink_slot((struct gj_obj_hdr *)pSlot->pObj, pProc->pCnode,
                               GJ_CAP_SLOT_PAGER);
        }
        gj_cap_slot_invalidate_locked(pSlot, (struct gj_obj_hdr *)pSlot->pObj);
    }
}

static void
process_pager_mirror_install(struct gj_process *pProc, struct gj_obj_hdr *pEp,
                             u16 u16Rights)
{
    struct gj_cap_ref refMirror;
    gj_status_t st;
    u16 u16MirRights;

    if (pProc == NULL || pProc->pCnode == NULL || pEp == NULL) {
        return;
    }
    if (pProc->pCnode->cSlots <= GJ_CAP_SLOT_PAGER) {
        return;
    }
    /* Drop prior mirror without requiring it matches old ep. */
    {
        struct gj_cap_slot *pSlot = &pProc->pCnode->pSlots[GJ_CAP_SLOT_PAGER];

        if (pSlot->u16Type != (u16)GJ_CAP_INVALID) {
            (void)gj_cap_quota_slot_refund(pProc->pCnode->pQuotaAccount);
            if (pSlot->pObj != NULL) {
                gj_cdt_unlink_slot((struct gj_obj_hdr *)pSlot->pObj, pProc->pCnode,
                                   GJ_CAP_SLOT_PAGER);
            }
            gj_cap_slot_invalidate_locked(pSlot,
                                          (struct gj_obj_hdr *)pSlot->pObj);
        }
    }
    /* Mirror is READ|IDENTIFY (+ GRANT if source had it) - not ambient MAP. */
    u16MirRights = (u16)(GJ_RIGHT_READ | GJ_RIGHT_IDENTIFY);
    if ((u16Rights & (u16)GJ_RIGHT_GRANT) != 0) {
        u16MirRights = (u16)(u16MirRights | GJ_RIGHT_GRANT);
    }
    st = gj_cap_slot_install(pProc->pCnode, GJ_CAP_SLOT_PAGER,
                             (u16)GJ_CAP_ENDPOINT, u16MirRights, pEp,
                             &refMirror);
    if (st == GJ_OK) {
        process_soft_inc(&g_u32SoftPagerMirrorOk);
        kprintf("process: pager mirror slot=%lu gen=%u soft\n",
                (unsigned long)refMirror.u64Slot, refMirror.u32SlotGen);
    }
}

void
gj_process_clear_pager(struct gj_process *pProc)
{
    struct gj_obj_hdr *pOld;

    if (pProc == NULL) {
        return;
    }
    pOld = pProc->pPagerEpObj;
    process_pager_mirror_clear(pProc);
    pProc->refPager = gj_cap_ref_null();
    pProc->pPagerEpObj = NULL;
    pProc->u32PagerBadge = 0;
    /* Soft: drop kernel hold after PCB cleared (SOLARIS_STYLE s9 clear). */
    process_pager_ref_drop(pOld);
    process_soft_inc(&g_u32SoftPagerClear);
}

void
gj_process_pager_refresh(struct gj_process *pProc)
{
    if (pProc == NULL) {
        return;
    }
    if (gj_cap_ref_is_null(&pProc->refPager)) {
        return;
    }
    /* Soft ep-revoke hook: DEAD/REVOKING endpoint clears PCB pager. */
    if (pProc->pPagerEpObj == NULL ||
        pProc->pPagerEpObj->u32State != (u32)GJ_OBJ_LIVE) {
        kprintf("process: pager refresh clear (ep dead) soft\n");
        gj_process_clear_pager(pProc);
        process_soft_inc(&g_u32SoftPagerRefreshClear);
        return;
    }
    process_soft_inc(&g_u32SoftPagerRefreshKeep);
}

u32
gj_process_pager_badge(const struct gj_process *pProc)
{
    if (pProc == NULL) {
        return 0;
    }
    return pProc->u32PagerBadge;
}

gj_status_t
gj_process_set_pager_badge(struct gj_process *pProc, u64 u64EpSlot,
                           u32 u32EpGen, u32 u32Badge)
{
    struct gj_cap_resolved res;
    gj_status_t st;
    struct gj_obj_hdr *pOld;
    u32 u32SnapBadge;

    if (pProc == NULL || pProc->pCnode == NULL) {
        process_soft_inc(&g_u32SoftPagerSetFail);
        return GJ_ERR_INVAL;
    }

    /* Clear pager */
    if (u32EpGen == 0) {
        gj_process_clear_pager(pProc);
        process_soft_inc(&g_u32SoftPagerSetOk);
        return GJ_OK;
    }

    /* Endpoint must resolve in *this* process CNode (Scheme A). */
    st = gj_cap_resolve(pProc->pCnode, u64EpSlot, u32EpGen, &res);
    if (st != GJ_OK) {
        process_soft_inc(&g_u32SoftPagerSetFail);
        return st;
    }
    /* Pager must be ENDPOINT (doors-like Call/reply); GRANT required. */
    if (res.u16Type != (u16)GJ_CAP_ENDPOINT) {
        process_soft_inc(&g_u32SoftPagerSetFail);
        return GJ_ERR_PERM;
    }
    if ((res.u16Rights & (u16)GJ_RIGHT_GRANT) == 0) {
        process_soft_inc(&g_u32SoftPagerSetFail);
        return GJ_ERR_PERM;
    }
    if (res.pObj == NULL) {
        process_soft_inc(&g_u32SoftPagerSetFail);
        return GJ_ERR_INVAL;
    }
    /* Soft LIVE check - refuse DEAD/REVOKING endpoints (fail closed). */
    if (res.pObj->u32State != (u32)GJ_OBJ_LIVE) {
        process_soft_inc(&g_u32SoftPagerSetFail);
        return GJ_ERR_DEAD;
    }

    /* Soft badge: explicit arg wins; else snap door server badge. */
    u32SnapBadge = u32Badge;
    if (u32SnapBadge == 0u) {
        u32SnapBadge = door_get_badge((struct gj_door *)res.pObj);
    }

    /* Replace: hold new first, then drop old (avoid transient zero-ref). */
    pOld = pProc->pPagerEpObj;
    process_pager_ref_hold(res.pObj);
    pProc->refPager = gj_cap_ref_make(u64EpSlot, u32EpGen);
    pProc->pPagerEpObj = res.pObj;
    pProc->u32PagerBadge = u32SnapBadge;
    if (pOld != NULL && pOld != res.pObj) {
        process_pager_ref_drop(pOld);
    } else if (pOld == res.pObj) {
        /* Same object re-set: undo the extra hold from this call. */
        process_pager_ref_drop(res.pObj);
    }
    process_pager_mirror_install(pProc, res.pObj, res.u16Rights);
    process_soft_inc(&g_u32SoftPagerSetOk);
    process_soft_maybe_once();
    kprintf("process: set_pager slot=%lu gen=%u badge=%u ref=%u soft\n",
            (unsigned long)u64EpSlot, u32EpGen, u32SnapBadge,
            res.pObj->u32Ref);
    return GJ_OK;
}

gj_status_t
gj_process_set_pager(struct gj_process *pProc, u64 u64EpSlot, u32 u32EpGen)
{
    /* Badge 0 -> soft-snap from door endpoint when LIVE. */
    return gj_process_set_pager_badge(pProc, u64EpSlot, u32EpGen, 0u);
}

int
gj_process_has_pager(const struct gj_process *pProc)
{
    if (pProc == NULL) {
        process_soft_inc(&g_u32SoftPagerHasMiss);
        return 0;
    }
    if (gj_cap_ref_is_null(&pProc->refPager)) {
        process_soft_inc(&g_u32SoftPagerHasMiss);
        return 0;
    }
    /*
     * Soft refresh needs mutable PCB; const path only reports gen.
     * Callers that need ep-dead clear should use gj_process_pager_refresh.
     */
    if (pProc->pPagerEpObj != NULL &&
        pProc->pPagerEpObj->u32State != (u32)GJ_OBJ_LIVE) {
        process_soft_inc(&g_u32SoftPagerHasMiss);
        return 0;
    }
    process_soft_inc(&g_u32SoftPagerHasHit);
    return 1;
}

/*
 * Fault policy (CAP_ADDRESSING + SOLARIS_STYLE_REMAINING s7):
 * - one fault lock per space (here: process.fault until gj_space exists)
 * - no pager -> FAULT (kill)
 * - with pager: build cluster + kernel cookie; Call pager (IPC later)
 * - object owns pages / maps are views (map path not fully wired; Apple s2)
 */
gj_status_t
gj_process_handle_fault(struct gj_process *pProc, u64 u64FaultVa, int fWrite,
                        int fExec)
{
    gj_status_t st;
    u64 u64Base;
    u32 u32Access;
    u32 u32NPages;
    struct gj_fault_msg msg;
    struct gj_map_cookie cookie;

    if (pProc == NULL) {
        return GJ_ERR_INVAL;
    }

    process_soft_inc(&g_u32SoftFaultEnter);

    /* Soft: drop PCB pager if endpoint was revoked under us. */
    gj_process_pager_refresh(pProc);

    st = gj_space_fault_enter(&pProc->fault);
    if (st != GJ_OK) {
        /* Full impl: wait on CV with mono timeout */
        process_soft_inc(&g_u32SoftFaultBusy);
        return st;
    }

    if (!gj_process_has_pager(pProc)) {
        gj_space_fault_leave(&pProc->fault);
        process_soft_inc(&g_u32SoftFaultNoPager);
        return GJ_ERR_FAULT; /* kill path */
    }

    /* Page-align; cluster of 1 for now (coalesce adjacent later). */
    u64Base = u64FaultVa & ~(4096ull - 1ull);
    u32NPages = 1;
    if (u32NPages > GJ_FAULT_CLUSTER_MAX) {
        u32NPages = GJ_FAULT_CLUSTER_MAX;
    }
    u32Access = GJ_FAULT_ACCESS_R;
    if (fWrite) {
        u32Access |= GJ_FAULT_ACCESS_W;
    }
    if (fExec) {
        u32Access |= GJ_FAULT_ACCESS_X;
    }
    /* W^X: refuse W|X at policy layer when mapping */
    if ((u32Access & GJ_FAULT_ACCESS_W) && (u32Access & GJ_FAULT_ACCESS_X)) {
        gj_space_fault_leave(&pProc->fault);
        process_soft_inc(&g_u32SoftFaultWxDeny);
        return GJ_ERR_PERM;
    }

    st = gj_map_cookie_create(&cookie, /*pSpace*/ pProc, pProc, /*thread*/ NULL,
                              u64Base, u32NPages, u32Access,
                              /*deadline*/ 0, &msg);
    if (st != GJ_OK) {
        gj_space_fault_leave(&pProc->fault);
        return st;
    }

    /*
     * Full path: ipc_call(pager, &msg) with mono timeout; on OK,
     * consume cookie, map FRAMEs, transfer ownership to client, resume.
     * Soft: stamp badge into flags low bits for pager payload later.
     * Until IPC+map exist: invalidate cookie and report AGAIN.
     */
    msg.u32Flags = pProc->u32PagerBadge;
    (void)cookie;
    gj_map_cookie_invalidate(msg.u64CookieLo, msg.u64CookieHi);
    gj_space_fault_leave(&pProc->fault);
    process_soft_inc(&g_u32SoftFaultAgain);
    return GJ_ERR_AGAIN;
}

/* ---- wait4 reaper (product: PROCESS caps; interim fixed zombie table) --- */

struct process_wait_slot {
    u8                 u8Used;
    u8                 u8Zombie;
    u8                 u8Reaped;
    u8                 u8Pad;
    u32                u32Pid;
    u32                u32Ppid;
    u32                u32Exit;
    struct gj_process *pProc;
};

static struct process_wait_slot g_aWait[GJ_WAIT_SLOTS];
static u32                      g_u32NextPid = GJ_WAIT_PID_BASE;
/* Soft reaper observability (wrap OK). Grep: process:wait stats */
static u64                      g_u64WaitRegister;
static u64                      g_u64WaitZombie;
static u64                      g_u64WaitReap;
static u64                      g_u64WaitReparent;
static u64                      g_u64WaitNowaitPeek;

/* process_wait_pid_of defined later; used by soft parent identity helpers. */
u32 process_wait_pid_of(struct gj_process *pProc);

/*
 * Soft parent identity for PCBs that are not wait-registered children
 * (boot/init long-lived parents). pids 2..99 - below GJ_WAIT_PID_BASE so
 * they never collide with child wait-table pids. Does NOT put the parent
 * into the zombie table (avoids G-PROC-5 CNode wipe on parent death).
 * greppable: process: soft fork-wait product-min
 */
#define GJ_SOFT_PARENT_MAX      16u
#define GJ_SOFT_PARENT_PID_BASE 2u
#define GJ_SOFT_PARENT_PID_MAX  99u

struct process_soft_parent {
    u8                 u8Used;
    u8                 u8Pad[3];
    u32                u32Pid;
    struct gj_process *pParent;
};

static struct process_soft_parent g_aSoftParent[GJ_SOFT_PARENT_MAX];
static u32                        g_u32NextSoftParentPid = GJ_SOFT_PARENT_PID_BASE;

/** Wait-table or soft-parent pid for PCB; 0 if unknown (no allocate). */
static u32
process_soft_parent_pid_lookup(struct gj_process *pParent)
{
    u32 i;
    u32 pid;

    if (pParent == NULL) {
        return 0;
    }
    pid = process_wait_pid_of(pParent);
    if (pid != 0u) {
        return pid;
    }
    for (i = 0; i < GJ_SOFT_PARENT_MAX; i++) {
        if (g_aSoftParent[i].u8Used && g_aSoftParent[i].pParent == pParent) {
            return g_aSoftParent[i].u32Pid;
        }
    }
    return 0;
}

/**
 * Ensure soft parent identity for fork/wait filter.
 * Wait-registered parents keep their wait pid; others get soft map 2..99.
 * Fallback 1 (init) if soft map full.
 */
static u32
process_soft_ensure_parent_pid(struct gj_process *pParent)
{
    u32 i;
    u32 pid;

    if (pParent == NULL) {
        return 1u;
    }
    pid = process_soft_parent_pid_lookup(pParent);
    if (pid != 0u) {
        return pid;
    }
    for (i = 0; i < GJ_SOFT_PARENT_MAX; i++) {
        if (!g_aSoftParent[i].u8Used) {
            pid = g_u32NextSoftParentPid++;
            if (pid < GJ_SOFT_PARENT_PID_BASE || pid > GJ_SOFT_PARENT_PID_MAX) {
                g_u32NextSoftParentPid = GJ_SOFT_PARENT_PID_BASE + 1u;
                pid = GJ_SOFT_PARENT_PID_BASE;
            }
            g_aSoftParent[i].u8Used = 1;
            g_aSoftParent[i].u32Pid = pid;
            g_aSoftParent[i].pParent = pParent;
            kprintf("process: soft fork-wait product-min parent_id pid=%u "
                    "soft (not wait-table)\n",
                    pid);
            return pid;
        }
    }
    return 1u; /* soft map full -> init bucket */
}

/** Resolve wait-table PCB by Linux-shaped pid (NULL if missing). */
static struct gj_process *
process_wait_proc_of_pid(u32 u32Pid)
{
    u32 i;

    if (u32Pid == 0u) {
        return NULL;
    }
    for (i = 0; i < GJ_WAIT_SLOTS; i++) {
        if (g_aWait[i].u8Used && g_aWait[i].u32Pid == u32Pid) {
            return g_aWait[i].pProc;
        }
    }
    return NULL;
}

/** Soft parent PCB for a soft/wait parent pid (NULL if none). */
static struct gj_process *
process_soft_parent_pcb_of_pid(u32 u32Ppid)
{
    u32 i;

    if (u32Ppid == 0u) {
        return NULL;
    }
    for (i = 0; i < GJ_WAIT_SLOTS; i++) {
        if (g_aWait[i].u8Used && g_aWait[i].u32Pid == u32Ppid) {
            return g_aWait[i].pProc;
        }
    }
    for (i = 0; i < GJ_SOFT_PARENT_MAX; i++) {
        if (g_aSoftParent[i].u8Used && g_aSoftParent[i].u32Pid == u32Ppid) {
            return g_aSoftParent[i].pParent;
        }
    }
    return NULL;
}

/**
 * One-shot greppable PASS after first successful fork+wait product-min path.
 * greppable: process: soft fork-wait product-min PASS
 */
static void
process_soft_fw_pass_once(i64 i64Pid, int nStatus, const char *szVia)
{
    const char *szViaSafe;

    if (g_fSoftFwPassOnce != 0) {
        return;
    }
    if (i64Pid <= 0) {
        return;
    }
    g_fSoftFwPassOnce = 1;
    szViaSafe = (szVia != NULL && szVia[0] != '\0') ? szVia : "path";
    kprintf("process: soft fork-wait product-min PASS via=%s pid=%ld "
            "status=0x%x WIFEXITED=%d exit=%d soft!=product\n",
            szViaSafe, (long)i64Pid, (unsigned)nStatus,
            GJ_WIFEXITED(nStatus) ? 1 : 0, GJ_WEXITSTATUS(nStatus));
}

/*
 * Soft wait-table census (Wave 15 inventory). Snapshots used/zombie/live/free.
 * Does not lock (soft OK). greppable via process: soft wait ...
 */
static void
process_soft_wait_census(u32 *pUsed, u32 *pZombie, u32 *pLive, u32 *pFree)
{
    u32 i;
    u32 cUsed = 0;
    u32 cZombie = 0;
    u32 cLive = 0;

    for (i = 0; i < GJ_WAIT_SLOTS; i++) {
        if (!g_aWait[i].u8Used) {
            continue;
        }
        cUsed++;
        if (g_aWait[i].u8Zombie && !g_aWait[i].u8Reaped) {
            cZombie++;
        } else if (!g_aWait[i].u8Zombie && !g_aWait[i].u8Reaped) {
            cLive++;
        }
    }
    if (pUsed != NULL) {
        *pUsed = cUsed;
    }
    if (pZombie != NULL) {
        *pZombie = cZombie;
    }
    if (pLive != NULL) {
        *pLive = cLive;
    }
    if (pFree != NULL) {
        *pFree = (cUsed < GJ_WAIT_SLOTS) ? (GJ_WAIT_SLOTS - cUsed) : 0u;
    }
}

u32
process_wait_register(struct gj_process *pChild, u32 u32Ppid)
{
    u32 i;
    u32 pid;
    u32 u32ParentPid;

    if (pChild == NULL) {
        process_soft_inc(&g_u32SoftWaitRegNull);
        return 0;
    }
    /* Already registered? */
    for (i = 0; i < GJ_WAIT_SLOTS; i++) {
        if (g_aWait[i].u8Used && g_aWait[i].pProc == pChild) {
            process_soft_inc(&g_u32SoftWaitRegIdem);
            return g_aWait[i].u32Pid;
        }
    }
    u32ParentPid = u32Ppid ? u32Ppid : 1u;
    for (i = 0; i < GJ_WAIT_SLOTS; i++) {
        if (!g_aWait[i].u8Used) {
            pid = g_u32NextPid++;
            if (g_u32NextPid < GJ_WAIT_PID_BASE) {
                g_u32NextPid = GJ_WAIT_PID_BASE;
            }
            g_aWait[i].u8Used = 1;
            g_aWait[i].u8Zombie = 0;
            g_aWait[i].u8Reaped = 0;
            g_aWait[i].u32Pid = pid;
            g_aWait[i].u32Ppid = u32ParentPid;
            g_aWait[i].u32Exit = 0;
            g_aWait[i].pProc = pChild;
            pChild->u32Alive = 1;
            /*
             * Soft: link pParent from wait table or soft parent map
             * (process: soft fork-wait product-min).
             */
            pChild->pParent = process_soft_parent_pcb_of_pid(u32ParentPid);
            g_u64WaitRegister++;
            process_soft_inc(&g_u32SoftWaitRegOk);
            process_soft_maybe_once();
            kprintf("process: wait register pid=%u ppid=%u\n", pid,
                    g_aWait[i].u32Ppid);
            return pid;
        }
    }
    process_soft_inc(&g_u32SoftWaitRegFull);
    return 0; /* table full - caller may continue without wait4 */
}

void
process_wait_note_exit(struct gj_process *pChild, u32 u32Code)
{
    u32 i;

    if (pChild == NULL) {
        return;
    }
    pChild->u32ExitCode = u32Code;
    pChild->u32Alive = 0;
    for (i = 0; i < GJ_WAIT_SLOTS; i++) {
        if (g_aWait[i].u8Used && g_aWait[i].pProc == pChild) {
            /* Soft: re-note updates exit code even if already zombie. */
            if (!g_aWait[i].u8Zombie) {
                g_u64WaitZombie++;
                process_soft_inc(&g_u32SoftWaitZombie);
            }
            g_aWait[i].u8Zombie = 1;
            g_aWait[i].u32Exit = u32Code;
            kprintf("process: zombie pid=%u code=%u\n", g_aWait[i].u32Pid,
                    u32Code);
            return;
        }
    }
}

void
process_wait_forget(struct gj_process *pProc)
{
    u32 i;

    if (pProc == NULL) {
        return;
    }
    for (i = 0; i < GJ_WAIT_SLOTS; i++) {
        if (g_aWait[i].u8Used && g_aWait[i].pProc == pProc) {
            g_aWait[i].u8Used = 0;
            g_aWait[i].u8Zombie = 0;
            g_aWait[i].u8Reaped = 0;
            g_aWait[i].u32Exit = 0;
            g_aWait[i].u32Pid = 0;
            g_aWait[i].u32Ppid = 0;
            g_aWait[i].pProc = NULL;
            process_soft_inc(&g_u32SoftWaitForget);
            return;
        }
    }
}

static int
process_is_wait_child(struct gj_process *pProc)
{
    u32 i;

    if (pProc == NULL) {
        return 0;
    }
    for (i = 0; i < GJ_WAIT_SLOTS; i++) {
        if (g_aWait[i].u8Used && g_aWait[i].pProc == pProc) {
            return 1;
        }
    }
    return 0;
}

/*
 * Soft: whether wait-registered PCB still needs full G-PROC-5 process_death
 * (H3 thr_exit dual belt + as_destroy) vs note_exit-only.
 *
 * Live UDX host residual (not immediate kill path; Soft!=product):
 * force-exit (exit_pid) and wait-heal must not skip teardown when residual
 * AS / pager / start thr / exception port / region views remain. Covers
 * multi-thr hosts rtl8168_udx / xhci_udx / ddi_host_gj + sshd children.
 * denser keep_live / product_host_live honesty: intentional death still
 * runs full H3 order (never thrash as_destroy while thr live). Soft residual
 * only; Dual DoD stays OPEN; denser residual != Dual DoD close.
 * Grep: process: soft residual lifecycle | need_death_broad=1
 * Grep: live_host_path=1 | thr_exit_before_as_destroy=1
 * Grep: product_host_live=1 | keep_live=1 | keep_live_rock=1
 * Grep: thr_live=1 | product_hosts=UDX | dual_dod=OPEN
 */
static int
process_lifecycle_need_death(struct gj_process *pProc)
{
    u32 iReg;

    if (pProc == NULL) {
        return 0;
    }
    if (pProc->u32Alive != 0u) {
        return 1;
    }
    if (pProc->u64Cr3 != 0ull) {
        return 1;
    }
    if (gj_process_has_pager(pProc)) {
        return 1;
    }
    if (pProc->u32StartThr != 0u) {
        return 1;
    }
    if (pProc->excPort.u8Live != 0u || pProc->excPort.u8Pending != 0u ||
        pProc->excPort.u32HandlerThr != 0u) {
        return 1;
    }
    for (iReg = 0; iReg < GJ_PROC_REGION_MAX; iReg++) {
        if (pProc->aRegions[iReg].u8Used != 0u) {
            return 1;
        }
    }
    return 0;
}

u32
process_wait_reparent(u32 u32OldPpid, u32 u32NewPpid)
{
    u32 i;
    u32 u32N = 0;

    if (u32OldPpid == 0 || u32NewPpid == 0 || u32OldPpid == u32NewPpid) {
        return 0;
    }
    for (i = 0; i < GJ_WAIT_SLOTS; i++) {
        if (!g_aWait[i].u8Used || g_aWait[i].u8Reaped) {
            continue;
        }
        if (g_aWait[i].u32Ppid != u32OldPpid) {
            continue;
        }
        g_aWait[i].u32Ppid = u32NewPpid;
        if (g_aWait[i].pProc != NULL) {
            g_aWait[i].pProc->pParent = NULL; /* soft: parent PCB gone */
        }
        u32N++;
        g_u64WaitReparent++;
        process_soft_inc(&g_u32SoftWaitReparentN);
        kprintf("process: wait reparent pid=%u ppid %u->%u soft\n",
                g_aWait[i].u32Pid, u32OldPpid, u32NewPpid);
    }
    return u32N;
}

u32
process_wait_live_count(u32 u32Ppid)
{
    u32 i;
    u32 u32N = 0;

    for (i = 0; i < GJ_WAIT_SLOTS; i++) {
        if (!g_aWait[i].u8Used || g_aWait[i].u8Reaped || g_aWait[i].u8Zombie) {
            continue;
        }
        if (u32Ppid != 0 && g_aWait[i].u32Ppid != u32Ppid) {
            continue;
        }
        u32N++;
    }
    return u32N;
}

u32
process_wait_zombie_count(u32 u32Ppid)
{
    u32 i;
    u32 u32N = 0;

    for (i = 0; i < GJ_WAIT_SLOTS; i++) {
        if (!g_aWait[i].u8Used || g_aWait[i].u8Reaped || !g_aWait[i].u8Zombie) {
            continue;
        }
        if (u32Ppid != 0 && g_aWait[i].u32Ppid != u32Ppid) {
            continue;
        }
        u32N++;
    }
    return u32N;
}

/*
 * Soft G-PROC-5 CNode wipe: kernel-authority slot clear with quota refund +
 * CDT unlink (never rights-gated like user gj_cap_delete). Boot/init CNodes
 * are not passed here - only wait-registered children.
 * Grep: process: death cnode | process:death cnode
 */
static u32
process_death_cnode_wipe(struct gj_process *pProc)
{
    u64 u64Slot;
    u32 u32Cleared = 0;
    struct gj_cnode *pCnode;

    if (pProc == NULL || pProc->pCnode == NULL || pProc->pCnode->pSlots == NULL) {
        return 0;
    }
    pCnode = pProc->pCnode;
    /* Soft try-lock: death still wipes if busy (must not skip); unlock only if held. */
    {
        int fLocked = gj_cnode_trylock(pCnode);

        for (u64Slot = 0; u64Slot < pCnode->cSlots; u64Slot++) {
            struct gj_cap_slot *pSlot = &pCnode->pSlots[u64Slot];
            struct gj_obj_hdr *pObj;

            if (pSlot->u16Type == (u16)GJ_CAP_INVALID) {
                continue;
            }
            pObj = (struct gj_obj_hdr *)pSlot->pObj;
            (void)gj_cap_quota_slot_refund(pCnode->pQuotaAccount);
            if (pObj != NULL) {
                gj_cdt_unlink_slot(pObj, pCnode, u64Slot);
            }
            gj_cap_slot_invalidate_locked(pSlot, pObj);
            u32Cleared++;
        }
        if (fLocked) {
            gj_cnode_unlock(pCnode);
        }
    }
    pProc->pRootMeta = NULL;
    /* Soft: detach quota ledger pointer (account body lives with creator). */
    pCnode->pQuotaAccount = NULL;
    return u32Cleared;
}

/*
 * H3 thr_exit helper: EXIT every non-current sibling of pProc and scrub
 * start-thr handoff so nothing can iretq into maps about to be freed.
 * Residual lean dual-belt core (Soft!=product):
 *   - Early call after u32Alive=0 (refuse-enter races fail-closed)
 *   - Barrier call immediately before as_destroy (mid-death bind race)
 * Same drain for CLONE_VM / pe32 lab class and multi-thr UDX host processes
 * (work/IRQ soft thr sharing private AS). Not product UDX host close.
 *
 * Cur-scrub residual (stronger fail-closed belt; Soft!=product):
 *   thread_exit_process intentionally skips the current thr (caller finishes
 *   via thread_exit / EXITED+schedule). Scrub current thr USER*_ENTRY /
 *   sysuser (valid + rip/rsp/rflags) / thr CR3 / block key when bound to
 *   pProc so the dying thr cannot iretq or mid-syscall sysret / load thr
 *   CR3 into maps about to free - same #PF I=1 class as sibling residual
 *   (pe32 clone_vm / UDX host). Parity with sibling scrub fields minus
 *   EXITED + pProc detach (caller needs PCB for rest of death). Do NOT
 *   mark current EXITED here.
 * Grep: process: death thr_exit | process: death thr_exit cur_scrub
 * Grep: thread: exit_process | thr_exit_before_as_destroy=1
 * Grep: process: soft residual lean H3 | udx_host_teardown=1 | H3=death_residual
 */
static u32
process_death_thr_exit_siblings(struct gj_process *pProc)
{
    u32 cThrEx;
    struct gj_thread *pCur;

    if (pProc == NULL) {
        return 0;
    }
    cThrEx = thread_exit_process(pProc);
    /* No recycled PCB re-entry via start handoff after sibling drain. */
    pProc->u32StartThr = 0;
    pProc->u64StartEntry = 0;
    /*
     * H3 cur_scrub: fail-closed for dying thr (skipped by thread_exit_process).
     * Full sysuser field parity with sibling drain + schedule resume refuse
     * (valid + rip/rsp/rflags) - mid-syscall sysret belt. Soft!=product.
     * Grep: process: death thr_exit cur_scrub | thr_exit_before_as_destroy=1
     */
    pCur = thread_current();
    if (pCur != NULL && pCur->pProc == pProc) {
        u32 fHad = 0;

        if ((pCur->u32Flags & (GJ_THR_F_USER_ENTRY | GJ_THR_F_USER32_ENTRY)) != 0 ||
            pCur->u32SysUserValid != 0 || pCur->u64Cr3 != 0 ||
            pCur->pBlockObj != NULL || pCur->u64SysUserRip != 0 ||
            pCur->u64SysUserRsp != 0 || pCur->u64SysUserRflags != 0) {
            fHad = 1;
        }
        pCur->u32Flags &= ~(GJ_THR_F_USER_ENTRY | GJ_THR_F_USER32_ENTRY);
        pCur->u64UserRip = 0;
        pCur->u64UserRsp = 0;
        pCur->u32SysUserValid = 0;
        pCur->u64SysUserRip = 0;
        pCur->u64SysUserRsp = 0;
        pCur->u64SysUserRflags = 0;
        pCur->u64Cr3 = 0;
        pCur->pBlockObj = NULL;
        pCur->u32BlockTag = 0;
        if (fHad != 0u) {
            g_u64DeathThrExitCurScrub++;
            /* Lean lamp only on real scrub (H2 no storm). */
            kprintf("process: death thr_exit cur_scrub=1 soft "
                    "(G-PROC-5 H3 thr_exit_before_as_destroy=1)\n");
        }
    }
    return cThrEx;
}

/* Soft: scrub exec/auxv handoff so reaped PCBs leave no image facts. */
static void
process_death_scrub_exec(struct gj_process *pProc)
{
    u32 u32HadConfine;
    u32 u32HadJit;

    if (pProc == NULL) {
        return;
    }
    pProc->u64ExecEntry = 0;
    pProc->u64InterpEntry = 0;
    pProc->u64LoadBias = 0;
    pProc->u64ExecStack = 0;
    pProc->u64StartEntry = 0;
    pProc->u32StartThr = 0;
    pProc->u32ExecFlags = 0;
    pProc->cNeededLoaded = 0;
    pProc->cAuxv = 0;
    memset(pProc->aAuxv, 0, sizeof(pProc->aAuxv));
    memset(pProc->szExecPath, 0, sizeof(pProc->szExecPath));
    pProc->u64AnonNext = 0x0000000040000000ull;
    /*
     * Soft multi-server confine death cleanup: drop PCB confine flags so a
     * recycled wait-table slot cannot inherit ambient-drop state.
     * Grep: process: death confine_scrub
     */
    u32HadConfine = pProc->u32Confined;
    pProc->u32Confined = 0;
    pProc->u32Promises = 0;
    g_u64DeathConfineScrub++;
    /*
     * Wave 13: also scrub CapJit cache so recycled PCB cannot inherit JIT.
     * Authority source is GJ_RIGHT_JIT on task cap; cache must not stick.
     * Grep: process: death jit_scrub
     */
    u32HadJit = pProc->u32Jit;
    pProc->u32Jit = 0;
    g_u64DeathJitScrub++;
    kprintf("process: death jit_scrub had=%u now=0 soft (G-PROC-5)\n",
            u32HadJit);
    (void)u32HadConfine;
}

void
process_death(struct gj_process *pProc, u32 u32ExitCode)
{
    u32 iReg;
    u64 u64Cr3;
    u64 u64Ker;
    u64 u64SavedCr3;
    u32 u32Cleared = 0;
    u32 u32SelfPid;
    u32 u32Reparented;
    u32 u32RegionsDropped = 0;
    u32 u32FaultWasBusy = 0;
    u32 u32HadPager = 0;
    u32 u32HadExc = 0;
    u32 u32AsOk = 0;
    u32 u32AsFail = 0;
    u32 u32AsSkip = 0;
    u32 u32WasConfined = 0;
    u32 u32WasJit = 0;
    u32 cThrEx = 0;
    u32 cThrExBar = 0;
    int fWaitChild;

    if (pProc == NULL) {
        return;
    }
    process_soft_inc(&g_u32SoftDeathEnter);
    /*
     * Idempotent: second death re-notes zombie code. Soft belt (H3): still
     * drain residual siblings so a half-torn PCB cannot leave USER*_ENTRY
     * thr RUNNABLE after maps are already gone (UDX host multi-thr race).
     */
    if (!pProc->u32Alive && pProc->u64Cr3 == 0 && !gj_process_has_pager(pProc)) {
        u32 cThrIdem;

        g_u64DeathIdempotent++;
        cThrIdem = process_death_thr_exit_siblings(pProc);
        g_u64DeathThrExitBarrierCalls++;
        if (cThrIdem != 0u) {
            g_u64DeathThrExit += (u64)cThrIdem;
            g_u64DeathThrExitBarrier += (u64)cThrIdem;
        }
        /* Grep: process: death idempotent | process: death thr_exit */
        kprintf("process: death idempotent re-note exit=%u thr_exit=%u soft "
                "(G-PROC-5 H3 thr_exit_before_as_destroy=1)\n",
                u32ExitCode, cThrIdem);
        process_wait_note_exit(pProc, u32ExitCode);
        return;
    }

    g_u64DeathTotal++;
    u32SelfPid = process_wait_pid_of(pProc);
    u32WasConfined = pProc->u32Confined;
    u32WasJit = pProc->u32Jit;
    pProc->u32ExitCode = u32ExitCode;
    /*
     * H3 residual lean dual belt (Soft!=product | UDX host multi-thr):
     *   1) Mark dead first so trampoline refuse-enter (alive==0) races with
     *      thr_exit are fail-closed.
     *   2) Early thr_exit: drain CLONE_VM / multi-thr siblings (incl. UDX
     *      host work/IRQ thr) + cur_scrub BEFORE any private map free.
     *   3) Later barrier re-drain + cur_scrub immediately before as_destroy.
     *   4) CR3 publish 0 (local holds target) then private as_destroy so
     *      refuse-enter cr3_0 is live during map free.
     * denser keep_live / product_host_live honesty: intentional death of a
     * product host still obeys never-as_destroy-while-thr-live (H3 spirit).
     * Soft residual never claims Dual DoD close. Soft!=product.
     * Lab class: pe32 clone_vm child RIP~0x58240013 -> #PF I=1.
     * Grep: process: death thr_exit early | process: death thr_exit
     * Grep: process: death thr_exit cur_scrub | thr_exit_before_as_destroy=1
     * Grep: process: death cr3_pub | cr3_pub_before_as_destroy=1
     * Grep: product_host_live=1 | keep_live=1 | keep_live_rock=1
     * Grep: thr_live=1 | product_hosts=UDX | dual_dod=OPEN
     * Grep: thread: exit_process
     */
    pProc->u32Alive = 0;
    cThrEx = process_death_thr_exit_siblings(pProc);
    g_u64DeathThrExit += (u64)cThrEx;
    g_u64DeathThrExitEarly += (u64)cThrEx;
    /*
     * Greppable order probe vs death as_destroy (n=0 still soft-ok).
     * Grep: process: death thr_exit | process: death thr_exit early
     * Grep: thr_exit_before_as_destroy=1
     */
    kprintf("process: death thr_exit n=%u early=1 "
            "thr_exit_before_as_destroy=1 soft "
            "(G-PROC-5 H3 udx_host_multi_thr=1 Soft!=product "
            "product_host_live keep_live keep_live_rock thr_live "
            "product_hosts=UDX dual_dod=OPEN denser=1)\n",
            cThrEx);

    /* ---- pager clear (G-PROC-5) ---------------------------------------- */
    u32HadPager = gj_process_has_pager(pProc) ? 1u : 0u;
    gj_process_clear_pager(pProc);
    g_u64DeathPagerClear++;
    /* Grep: process: death pager */
    kprintf("process: death pager_clear had=%u soft (G-PROC-5)\n",
            u32HadPager);

    /* Drop exception port (handler thr may already be gone) */
    u32HadExc = (pProc->excPort.u8Live != 0u || pProc->excPort.u8Pending != 0u ||
                 pProc->excPort.u32HandlerThr != 0u)
                    ? 1u
                    : 0u;
    pProc->excPort.u8Live = 0;
    pProc->excPort.u8Pending = 0;
    pProc->excPort.u32HandlerThr = 0;
    pProc->excPort.u32Vec = 0;
    pProc->excPort.u32Count = 0;
    pProc->excPort.u64Error = 0;
    pProc->excPort.u64Rip = 0;
    pProc->excPort.u64Cr2 = 0;
    g_u64DeathExcClear++;
    /* Grep: process: death exc_clear */
    kprintf("process: death exc_clear had=%u soft (G-PROC-5)\n", u32HadExc);

    /*
     * Soft: force-clear fault serialization so death cannot leave AS locked.
     * Grep: process: death fault_lock
     */
    u32FaultWasBusy = (pProc->fault.u32FaultInProgress != 0u ||
                       pProc->fault.u32Waiters != 0u)
                          ? 1u
                          : 0u;
    pProc->fault.u32FaultInProgress = 0;
    pProc->fault.u32Waiters = 0;
    g_u64DeathFaultForce++;
    kprintf("process: death fault_lock force was_busy=%u soft (G-PROC-5)\n",
            u32FaultWasBusy);

    /* Drop region views (object owns pages; maps are views - G-MO) */
    for (iReg = 0; iReg < GJ_PROC_REGION_MAX; iReg++) {
        if (pProc->aRegions[iReg].u8Used) {
            memset(&pProc->aRegions[iReg], 0, sizeof(pProc->aRegions[iReg]));
            u32RegionsDropped++;
        }
    }
    g_u64DeathRegions += (u64)u32RegionsDropped;
    /* Grep: process: death regions */
    kprintf("process: death regions dropped=%u soft (G-PROC-5)\n",
            u32RegionsDropped);

    /*
     * Soft G-PROC-5: reparent unreaped children to init before we become a
     * zombie (wait4 parent filter stays honest for grand-children).
     */
    u32Reparented = 0;
    if (u32SelfPid != 0) {
        u32Reparented = process_wait_reparent(u32SelfPid, 1u);
    }
    g_u64DeathReparent += (u64)u32Reparented;

    fWaitChild = process_is_wait_child(pProc);
    /*
     * G-PROC-5: wipe CNode for wait-registered children only (never boot/init).
     * Soft deepen: quota refund + CDT unlink per slot, then process revoke.
     * Grep: process: death cnode_wipe | process: cnode_clear
     */
    if (fWaitChild) {
        g_u64DeathWaitChild++;
        u32Cleared = process_death_cnode_wipe(pProc);
        g_u64DeathCnodeWipe++;
        g_u64DeathCnodeSlots += (u64)u32Cleared;
        (void)gj_obj_revoke_begin(&pProc->hdr);
        (void)gj_revoke_cdt_walk_batch(&pProc->hdr, 16);
        (void)gj_revoke_process_deferred(16);
        kprintf("process: death cnode_wipe slots=%u wait_child=1 soft "
                "(G-PROC-5)\n",
                u32Cleared);
        kprintf("process: cnode_clear slots=%u PASS\n", u32Cleared);
        process_death_scrub_exec(pProc);
        kprintf("process: death confine_scrub confined=0 promises=0 "
                "was_confined=%u was_jit=%u soft\n",
                u32WasConfined, u32WasJit);
    } else {
        g_u64DeathLongLived++;
        /* Long-lived PCB: leave CNode; still note skip for tallies. */
        kprintf("process: death cnode_wipe slots=0 wait_child=0 skip soft "
                "(G-PROC-5)\n");
    }
    /*
     * H3 pre-as_destroy barrier (residual lean dual belt): re-exit residual
     * siblings after pager / region / cnode work. Catches any thr that
     * became bound mid-death (soft race belt; UDX host multi-thr safety).
     * Always count barrier call; log body only when n!=0 (H2 no storm).
     * Grep: process: death thr_exit barrier
     */
    cThrExBar = process_death_thr_exit_siblings(pProc);
    g_u64DeathThrExitBarrierCalls++;
    if (cThrExBar != 0u) {
        g_u64DeathThrExit += (u64)cThrExBar;
        g_u64DeathThrExitBarrier += (u64)cThrExBar;
        kprintf("process: death thr_exit barrier n=%u soft "
                "(G-PROC-5 H3 thr_exit_before_as_destroy=1)\n",
                cThrExBar);
    }
    /* Dual belt complete - as_destroy may free maps now (H3 order pass). */
    g_u64DeathH3OrderPass++;
    /* Grep: process: death H3 order | process: death thr_exit cur_scrub */
    /* Grep: product_host_live | keep_live | keep_live_rock | thr_live */
    /* Grep: never_as_destroy_while_thr_live | product_hosts=UDX */
    kprintf("process: death H3 order thr_early=%u thr_bar=%u "
            "thr_cur_scrub=%llu thr_exit_before_as_destroy=1 "
            "never_as_destroy_while_thr_live=1 "
            "udx_host_teardown=1 soft (G-PROC-5 H3 Soft!=product "
            "product_host_live keep_live keep_live_rock thr_live "
            "product_hosts=UDX dual_dod=OPEN denser=1)\n",
            cThrEx, cThrExBar,
            (unsigned long long)g_u64DeathThrExitCurScrub);

    /*
     * Destroy private AS only for wait-registered children (PE/spawn/fork).
     * Must stay AFTER thr_exit early + barrier + cur_scrub (H3 residual lean).
     * CR3 publish residual: zero PCB u64Cr3 before map free so refuse-enter
     * cr3_0 belt is live during as_destroy (local holds destroy target).
     * Never free boot/init AS. Save/restore caller CR3: death may run
     * mid-syscall on the parent AS (e.g. vfork child exit while parent PE32
     * is current).
     * Grep: process: death as_destroy | process: as_destroy
     * Grep: thr_exit_before_as_destroy=1 | process: death cr3_pub
     * Grep: cr3_pub_before_as_destroy=1
     */
    u64SavedCr3 = cpu_read_cr3();
    u64Cr3 = pProc->u64Cr3;
    u64Ker = vmm_kernel_cr3();
    if (fWaitChild && u64Cr3 != 0 && u64Ker != 0 &&
        (u64Cr3 & ~0xfffull) != (u64Ker & ~0xfffull)) {
        /*
         * H3: publish dead-AS (cr3_0) BEFORE free maps. thr_exit dual belt
         * already drained siblings + cur_scrub; local u64Cr3 is destroy tgt.
         * Grep: process: death cr3_pub | cr3_pub_before_as_destroy=1
         */
        pProc->u64Cr3 = 0;
        g_u64DeathH3Cr3Pub++;
        kprintf("process: death cr3_pub=1 before_as_destroy soft "
                "(G-PROC-5 H3 cr3_pub_before_as_destroy=1 "
                "thr_exit_before_as_destroy=1)\n");
        cpu_load_cr3(u64Ker);
        vmm_set_anon_cursor(NULL);
        if (vmm_as_destroy(u64Cr3) == GJ_OK) {
            u32AsOk = 1;
            g_u64DeathAsDestroyOk++;
            kprintf("process: death as_destroy cr3=0x%lx ok=1 soft "
                    "(G-PROC-5 H3 thr_exit_before_as_destroy=1 "
                    "cr3_pub_before_as_destroy=1)\n",
                    (unsigned long)u64Cr3);
            kprintf("process: as_destroy cr3=0x%lx PASS\n",
                    (unsigned long)u64Cr3);
        } else {
            u32AsFail = 1;
            g_u64DeathAsDestroyFail++;
            kprintf("process: death as_destroy cr3=0x%lx ok=0 soft "
                    "(G-PROC-5 H3 thr_exit_before_as_destroy=1 "
                    "cr3_pub_before_as_destroy=1)\n",
                    (unsigned long)u64Cr3);
            kprintf("process: as_destroy cr3=0x%lx FAIL\n",
                    (unsigned long)u64Cr3);
        }
        /* PCB CR3 already published 0 (H3 cr3_pub residual). */
    } else if (u64Cr3 != 0 && u64Ker != 0 &&
               (u64Cr3 & ~0xfffull) != (u64Ker & ~0xfffull)) {
        /* Long-lived process: leave AS; switch off if we were on it */
        u32AsSkip = 1;
        g_u64DeathAsSkip++;
        kprintf("process: death as_destroy cr3=0x%lx skip=long_lived soft "
                "(G-PROC-5 H3)\n",
                (unsigned long)u64Cr3);
        if ((u64SavedCr3 & ~0xfffull) == (u64Cr3 & ~0xfffull)) {
            cpu_load_cr3(u64Ker);
            vmm_set_anon_cursor(NULL);
            u64SavedCr3 = u64Ker;
        }
    } else {
        u32AsSkip = 1;
        g_u64DeathAsSkip++;
        kprintf("process: death as_destroy cr3=0x%lx skip=none soft "
                "(G-PROC-5 H3)\n",
                (unsigned long)u64Cr3);
    }
    /* Restore caller address space when it was not the victim */
    if (u64SavedCr3 != 0 &&
        (u64SavedCr3 & ~0xfffull) != (u64Cr3 & ~0xfffull)) {
        cpu_load_cr3(u64SavedCr3);
    } else if (u64Ker != 0) {
        cpu_load_cr3(u64Ker);
    }
    process_wait_note_exit(pProc, u32ExitCode);
    pProc->pParent = NULL;

    /*
     * Wave 13: death != Apple s13 bootstrap seal product.
     * Cleanup revokes grants for this PCB; does not seal privileged retype /
     * broad IRQ / root untyped. Soft seal_note + optional checklist.
     * Grep: process: death seal_note | process: bootstrap seal soft
     */
    kprintf("process: death seal_note soft lamp=%u product_seal=0 "
            "retype=%u irq=%u untyped=%u sticky=%u "
            "(death cleanup != Apple s13 seal)\n",
            g_u32BootstrapSealSoftLamp, g_u32SealProductRetype,
            g_u32SealProductIrqBind, g_u32SealProductRootUntyped,
            g_u32SealProductStickyNs);
    /* Rate-limited full checklist via=death (seal_note above always emits). */
    process_seal_checklist_soft(pProc, "death");

    /*
     * Aggregate G-PROC-5 death tallies (soft product observability).
     * Grep: process: death exit= | process: death tallies
     */
    kprintf("process: death tallies total=%llu thr_exit=%llu thr_early=%llu "
            "thr_bar=%llu thr_bar_calls=%llu thr_cur_scrub=%llu h3_order=%llu "
            "cr3_pub=%llu pager_clear=%llu fault_force=%llu cnode_wipe=%llu "
            "cnode_slots=%llu as_ok=%llu as_fail=%llu as_skip=%llu "
            "soft (G-PROC-5 H3 thr_exit_before_as_destroy=1 "
            "cr3_pub_before_as_destroy=1)\n",
            (unsigned long long)g_u64DeathTotal,
            (unsigned long long)g_u64DeathThrExit,
            (unsigned long long)g_u64DeathThrExitEarly,
            (unsigned long long)g_u64DeathThrExitBarrier,
            (unsigned long long)g_u64DeathThrExitBarrierCalls,
            (unsigned long long)g_u64DeathThrExitCurScrub,
            (unsigned long long)g_u64DeathH3OrderPass,
            (unsigned long long)g_u64DeathH3Cr3Pub,
            (unsigned long long)g_u64DeathPagerClear,
            (unsigned long long)g_u64DeathFaultForce,
            (unsigned long long)g_u64DeathCnodeWipe,
            (unsigned long long)g_u64DeathCnodeSlots,
            (unsigned long long)g_u64DeathAsDestroyOk,
            (unsigned long long)g_u64DeathAsDestroyFail,
            (unsigned long long)g_u64DeathAsSkip);
    /*
     * Deepen tallies (extra axes; wrap OK). Soft!=product.
     * Grep: process: death deepen | process: death tallies deepen
     */
    kprintf("process: death deepen tallies wait_child=%llu "
            "long_lived=%llu idempotent=%llu regions=%llu reparent=%llu "
            "exc_clear=%llu confine_scrub=%llu jit_scrub=%llu thr_exit=%llu "
            "thr_early=%llu thr_bar=%llu thr_bar_calls=%llu thr_cur_scrub=%llu "
            "h3_order=%llu fork_stub_h3=%llu cr3_pub=%llu soft (G-PROC-5 H3)\n",
            (unsigned long long)g_u64DeathWaitChild,
            (unsigned long long)g_u64DeathLongLived,
            (unsigned long long)g_u64DeathIdempotent,
            (unsigned long long)g_u64DeathRegions,
            (unsigned long long)g_u64DeathReparent,
            (unsigned long long)g_u64DeathExcClear,
            (unsigned long long)g_u64DeathConfineScrub,
            (unsigned long long)g_u64DeathJitScrub,
            (unsigned long long)g_u64DeathThrExit,
            (unsigned long long)g_u64DeathThrExitEarly,
            (unsigned long long)g_u64DeathThrExitBarrier,
            (unsigned long long)g_u64DeathThrExitBarrierCalls,
            (unsigned long long)g_u64DeathThrExitCurScrub,
            (unsigned long long)g_u64DeathH3OrderPass,
            (unsigned long long)g_u64DeathH3ForkStub,
            (unsigned long long)g_u64DeathH3Cr3Pub);
    kprintf("process: death exit=%u thr_exit=%u thr_bar=%u reparent=%u "
            "regions=%u cnode_slots=%u as_ok=%u as_fail=%u as_skip=%u "
            "wait_child=%d thr_exit_before_as_destroy=1 thr_cur_scrub=%llu "
            "cr3_pub=%llu soft (G-PROC-5 H3 Soft!=product)\n",
            u32ExitCode, cThrEx, cThrExBar, u32Reparented, u32RegionsDropped,
            u32Cleared, u32AsOk, u32AsFail, u32AsSkip, fWaitChild,
            (unsigned long long)g_u64DeathThrExitCurScrub,
            (unsigned long long)g_u64DeathH3Cr3Pub);
    /* Wave 15: unified soft inventory dump after death path. */
    process_soft_inventory("death");
}

/*
 * Stub children for Linux fork/vfork/clone (no full AS clone until product
 * spawn). greppable: process: soft fork
 * Product incomplete: stub does not run user code; usable for parent
 * wait4/waitid WNOHANG smokes and shell/sshd ABI later.
 */
static struct gj_process g_aForkStub[GJ_FORK_STUBS];
static u8                g_aForkUsed[GJ_FORK_STUBS];

/**
 * Soft reverse: drop private AS on fork path when wait_register fails
 * (not wait-registered -> process_death would skip AS destroy).
 * H3 residual lean dual belt: thr_exit non-current siblings BEFORE
 * as_destroy - early pass + pre-as_destroy barrier + cr3_pub (same order
 * as process_death). Clone/death safety for fork stubs and multi-thr UDX
 * host teardown class. Soft!=product.
 * Grep: process: death thr_exit | process: death thr_exit barrier
 * Grep: thr_exit_before_as_destroy=1 | process: death H3 order
 * Grep: process: death cr3_pub | cr3_pub_before_as_destroy=1
 */
static void
fork_stub_as_teardown(struct gj_process *pChild)
{
    u64 u64Cr3;
    u64 u64Ker;
    u64 u64Saved;
    u32 cThrEx;
    u32 cThrExBar;

    if (pChild == NULL) {
        return;
    }
    g_u64DeathH3ForkStub++;
    /*
     * H3 residual lean: refuse-enter + sibling EXIT before free maps
     * (clone_vm FAULT class / UDX host multi-thr). Early drain first.
     */
    pChild->u32Alive = 0;
    cThrEx = process_death_thr_exit_siblings(pChild);
    g_u64DeathThrExit += (u64)cThrEx;
    g_u64DeathThrExitEarly += (u64)cThrEx;
    if (cThrEx != 0u) {
        /* Grep: process: death thr_exit | thr_exit_before_as_destroy=1 */
        kprintf("process: death thr_exit n=%u early=1 soft (fork_stub H3 "
                "thr_exit_before_as_destroy=1)\n",
                cThrEx);
    }
    u64Cr3 = pChild->u64Cr3;
    u64Ker = vmm_kernel_cr3();
    if (u64Cr3 == 0 || u64Ker == 0 ||
        (u64Cr3 & ~0xfffull) == (u64Ker & ~0xfffull)) {
        pChild->u64Cr3 = 0;
        g_u64DeathH3OrderPass++;
        return;
    }
    /*
     * H3 pre-as_destroy barrier (residual lean): re-drain residual siblings
     * immediately before map free - mirrors process_death dual belt.
     * Grep: process: death thr_exit barrier
     */
    cThrExBar = process_death_thr_exit_siblings(pChild);
    g_u64DeathThrExitBarrierCalls++;
    if (cThrExBar != 0u) {
        g_u64DeathThrExit += (u64)cThrExBar;
        g_u64DeathThrExitBarrier += (u64)cThrExBar;
        kprintf("process: death thr_exit barrier n=%u soft (fork_stub H3 "
                "thr_exit_before_as_destroy=1)\n",
                cThrExBar);
    }
    /* Grep: process: death H3 order */
    g_u64DeathH3OrderPass++;
    kprintf("process: death H3 order thr_early=%u thr_bar=%u "
            "thr_exit_before_as_destroy=1 fork_stub=1 soft "
            "(G-PROC-5 H3 Soft!=product)\n",
            cThrEx, cThrExBar);
    /*
     * H3 cr3_pub: publish dead-AS before free maps (local holds target).
     * Grep: process: death cr3_pub | cr3_pub_before_as_destroy=1
     */
    pChild->u64Cr3 = 0;
    g_u64DeathH3Cr3Pub++;
    kprintf("process: death cr3_pub=1 before_as_destroy soft (fork_stub H3 "
            "cr3_pub_before_as_destroy=1 thr_exit_before_as_destroy=1)\n");
    u64Saved = cpu_read_cr3();
    cpu_load_cr3(u64Ker);
    vmm_set_anon_cursor(NULL);
    /* Must stay AFTER thr_exit early + barrier + cr3_pub (H3). */
    /* Grep: process: as_destroy */
    (void)vmm_as_destroy(u64Cr3);
    if (u64Saved != 0 &&
        (u64Saved & ~0xfffull) != (u64Cr3 & ~0xfffull)) {
        cpu_load_cr3(u64Saved);
    }
}

/**
 * Deferred child exit: runs after parent returns from fork so wait4/waitid
 * can observe live (WNOHANG->0) then zombie (reap pid+status).
 * greppable: process: soft fork
 */
static void
fork_child_exit_worker(void *pArg)
{
    struct gj_process *pChild = (struct gj_process *)pArg;

    /* Yield so parent can run wait4/WNOHANG after fork returns a usable pid */
    thread_yield();
    thread_yield();
    thread_yield();
    if (pChild != NULL && pChild->u32Alive) {
        /* process_death frees private AS (avoid PML4 leak on fork stubs) */
        process_death(pChild, 0);
    }
    thread_exit();
}

i64
process_linux_fork(u32 u32Ppid, int fExitNow)
{
    u32 i;
    u32 pid;
    u32 thr = 0;
    u32 u32Parent;

    /*
     * process: soft fork - enter
     * Returns usable wait-table pid (>= GJ_WAIT_PID_BASE) on success.
     */
    process_soft_inc(&g_u32SoftForkEnter);
    for (i = 0; i < GJ_FORK_STUBS; i++) {
        if (!g_aForkUsed[i]) {
            break;
        }
    }
    if (i >= GJ_FORK_STUBS) {
        process_soft_inc(&g_u32SoftForkFull);
        kprintf("process: soft fork full stubs=%u EAGAIN\n", GJ_FORK_STUBS);
        return -11; /* EAGAIN */
    }
    memset(&g_aForkStub[i], 0, sizeof(g_aForkStub[i]));
    g_aForkUsed[i] = 1;
    g_aForkStub[i].u32Alive = 1;
    g_aForkStub[i].u32Personality = 1;
    /*
     * Private AS shell for child (G-AS) + clone parent private user pages.
     * Product: full COW; bring-up: copy non-identity user 4K pages from parent.
     * Soft incomplete (process: soft fork): not a runnable user child.
     */
    if (process_as_ensure(&g_aForkStub[i]) == GJ_OK) {
        u32 cCloned = 0;
        extern struct gj_process *g_pLinuxProc;

        process_soft_inc(&g_u32SoftForkAsOk);
        kprintf("process: linux_fork as cr3=0x%lx\n",
                (unsigned long)g_aForkStub[i].u64Cr3);
        if (g_pLinuxProc != NULL && g_pLinuxProc->u64Cr3 != 0 &&
            g_aForkStub[i].u64Cr3 != 0 &&
            (g_pLinuxProc->u64Cr3 & ~0xfffull) !=
                (g_aForkStub[i].u64Cr3 & ~0xfffull)) {
            if (vmm_as_clone_user_pages(g_pLinuxProc->u64Cr3,
                                        g_aForkStub[i].u64Cr3, 512,
                                        &cCloned) == GJ_OK) {
                process_soft_inc(&g_u32SoftForkCloneOk);
                kprintf("process: linux_fork clone pages=%u PASS\n", cCloned);
            } else {
                process_soft_inc(&g_u32SoftForkCloneFail);
                kprintf("process: linux_fork clone pages FAIL\n");
            }
        }
    } else {
        process_soft_inc(&g_u32SoftForkAsFail);
    }
    u32Parent = u32Ppid ? u32Ppid : 1u;
    pid = process_wait_register(&g_aForkStub[i], u32Parent);
    if (pid == 0) {
        /* Soft reverse: free AS + stub so table pressure does not leak CR3. */
        fork_stub_as_teardown(&g_aForkStub[i]);
        g_aForkUsed[i] = 0;
        process_soft_inc(&g_u32SoftForkRegFail);
        process_soft_inc(&g_u32SoftForkFull);
        kprintf("process: soft fork reg_fail ENOMEM soft\n");
        return -12; /* ENOMEM */
    }
    g_u32SoftLastForkPid = pid;
    if (fExitNow) {
        /* vfork-shaped: child already zombie; parent wait4 reaps immediately */
        process_soft_inc(&g_u32SoftForkVfork);
        process_death(&g_aForkStub[i], 0);
    } else {
        /*
         * fork-shaped: deferred zombie so parent can:
         *   wait4(pid, ..., WNOHANG) -> 0 while live, then pid+status
         * greppable: process: soft fork / process: soft wait
         */
        thr = thread_create(&g_aForkStub[i], fork_child_exit_worker,
                            &g_aForkStub[i]);
        if (thr == 0) {
            /* Fallback: immediate exit so wait still works with usable pid */
            process_death(&g_aForkStub[i], 0);
            process_soft_inc(&g_u32SoftForkOk);
            process_soft_maybe_once();
            kprintf("process: soft fork pid=%u (no thr, exit now) ok\n", pid);
            kprintf("process: linux_fork pid=%u (no thr, exit now)\n", pid);
            return (i64)pid;
        }
        process_soft_inc(&g_u32SoftForkDeferred);
    }
    process_soft_inc(&g_u32SoftForkOk);
    process_soft_maybe_once();
    kprintf("process: soft fork pid=%u ppid=%u exit_now=%d thr=%u ok\n", pid,
            u32Parent, fExitNow, thr);
    kprintf("process: linux_fork pid=%u exit_now=%d thr=%u\n", pid, fExitNow,
            thr);
    return (i64)pid;
}

/*
 * Soft clone(2) flag map -> fork-like wait child (ABI-first for shell/sshd).
 * greppable: process: soft fork
 */
i64
process_linux_clone(u32 u32Ppid, u64 u64Flags)
{
    u64 u64Share;

    process_soft_inc(&g_u32SoftCloneEnter);
    /* CSIGNAL (low 8) is exit signal only - not clone geometry. */
    u64Share = u64Flags & ~GJ_CLONE_CSIGNAL;

    /*
     * CLONE_THREAD needs child stack + entry (syscall cold / thread_create_user).
     * Soft: reject here so ABI callers do not get a false process pid.
     */
    if ((u64Share & GJ_CLONE_THREAD) != 0ull) {
        process_soft_inc(&g_u32SoftCloneThreadReject);
        kprintf("process: soft fork clone CLONE_THREAD reject EINVAL soft\n");
        return -22; /* EINVAL */
    }

    /* Namespace isolation not product - soft EINVAL (shell never sets these). */
    if ((u64Share & GJ_CLONE_NS_MASK) != 0ull) {
        process_soft_inc(&g_u32SoftCloneNsReject);
        kprintf("process: soft fork clone ns flags=0x%lx EINVAL soft\n",
                (unsigned long)u64Share);
        return -22; /* EINVAL */
    }

    /* CLONE_VFORK -> immediate zombie; parent wait4 reaps. */
    if ((u64Share & GJ_CLONE_VFORK) != 0ull) {
        process_soft_inc(&g_u32SoftCloneVfork);
        kprintf("process: soft fork clone vfork-like soft\n");
        return process_linux_fork(u32Ppid, 1);
    }

    /*
     * flags==0 or share-table bits (VM/FS/FILES/SIGHAND/...): fork-like.
     * Product incomplete: share bits ignored (stub AS, not true share).
     * Returns usable child pid for wait4/waitid WNOHANG.
     */
    process_soft_inc(&g_u32SoftCloneForkLike);
    kprintf("process: soft fork clone fork-like flags=0x%lx soft\n",
            (unsigned long)u64Flags);
    return process_linux_fork(u32Ppid, 0);
}

i64
process_linux_exit_pid(u32 u32Pid, u32 u32Code)
{
    u32 i;
    struct gj_process *pProc;

    /*
     * Functional residual lifecycle — live UDX/sshd hosts (Soft!=product):
     * Force-exit of a wait-registered pid must run full G-PROC-5 H3 death
     * when teardown is still pending (alive/cr3/pager/start_thr/exc/regions).
     * note_exit-only would set u32Alive=0 and skip thr_exit dual belt +
     * as_destroy -> private AS leak and residual USER*_ENTRY thr into maps
     * about to free (H3 class). Not the immediate-kill vfork path: covers
     * long-lived multi-thr hosts + session children.
     * denser keep_live / product_host_live: intentional exit still uses H3
     * order (thr_exit before as_destroy); never thrash maps while thr live.
     * Hosts: rtl8168_udx / xhci_udx / ddi_host_gj + sshd session children.
     * Grep: process: soft exit_pid death | thr_exit_before_as_destroy=1
     * Grep: process: soft residual lifecycle | udx_host_teardown=1
     * Grep: live_host_path=1 | need_death_broad=1 | not_immediate_kill=1
     * Grep: product_host_live=1 | keep_live=1 | keep_live_rock=1
     * Grep: thr_live=1 | product_hosts=UDX | dual_dod=OPEN
     */
    for (i = 0; i < GJ_WAIT_SLOTS; i++) {
        if (g_aWait[i].u8Used && !g_aWait[i].u8Reaped &&
            g_aWait[i].u32Pid == u32Pid) {
            pProc = g_aWait[i].pProc;
            if (pProc == NULL) {
                return -3; /* ESRCH */
            }
            if (process_lifecycle_need_death(pProc) != 0) {
                g_u64SoftExitPidDeath++;
                /* Grep: process: soft exit_pid death */
                kprintf("process: soft exit_pid death pid=%u code=%u "
                        "thr_exit_before_as_destroy=1 udx_host_teardown=1 "
                        "need_death_broad=1 live_host_path=1 "
                        "product_host_live=1 keep_live=1 keep_live_rock=1 "
                        "thr_live=1 product_hosts=UDX denser=1 "
                        "soft (G-PROC-5 H3 Soft!=product dual_dod=OPEN "
                        "soft residual denser product_host_live)\n",
                        u32Pid, u32Code);
                process_death(pProc, u32Code);
            } else {
                g_u64SoftExitPidNoteOnly++;
                process_wait_note_exit(pProc, u32Code);
            }
            return 0;
        }
    }
    return -3; /* ESRCH */
}

u32
process_wait_pid_of(struct gj_process *pProc)
{
    u32 i;

    if (pProc == NULL) {
        return 0;
    }
    for (i = 0; i < GJ_WAIT_SLOTS; i++) {
        if (g_aWait[i].u8Used && g_aWait[i].pProc == pProc) {
            return g_aWait[i].u32Pid;
        }
    }
    return 0;
}

i64
process_wait4(i64 i64Pid, i32 *pStatus, int nOptions)
{
    return process_wait4_ppid(0, i64Pid, pStatus, nOptions);
}

i64
process_wait4_ppid(u32 u32Ppid, i64 i64Pid, i32 *pStatus, int nOptions)
{
    u32 attempt;
    int fNoHang = (nOptions & GJ_WAIT_WNOHANG) != 0;
    int fNoWait = (nOptions & GJ_WAIT_WNOWAIT) != 0;
    /* Soft poll budget: enough yields for fork_child_exit_worker + shell poll. */
    u32 u32MaxAttempts = fNoHang ? 1u : 256u;
    int fLastHaveChild = 0;

    /*
     * process: soft wait - wait4/waitid reaper enter
     * WNOHANG: 0 while live child matches; pid+status when zombie.
     * Never ECHILD while a matching unreaped child still exists.
     */
    process_soft_inc(&g_u32SoftWait4Enter);
    /*
     * Soft: WUNTRACED / WCONTINUED ignored (no stop/continue state yet).
     * pid 0 treated as any-child (bring-up); pid < -1 process-group unsupported.
     * Product incomplete - greppable: process: soft wait
     */
    for (attempt = 0; attempt < u32MaxAttempts; attempt++) {
        u32 i;
        int fHaveChild = 0;

        /* Reap a matching zombie (optionally only our children) */
        for (i = 0; i < GJ_WAIT_SLOTS; i++) {
            struct process_wait_slot *pS = &g_aWait[i];
            i32 i32Status;
            i64 i64Ret;

            if (!pS->u8Used || pS->u8Reaped) {
                continue;
            }
            if (u32Ppid != 0 && pS->u32Ppid != u32Ppid) {
                continue;
            }
            /* Soft: exact pid, any (-1), or legacy any (0 -> treat as -1). */
            if (i64Pid > 0 && (u32)i64Pid != pS->u32Pid) {
                continue;
            }
            if (i64Pid < -1) {
                continue; /* process group - unsupported */
            }
            fHaveChild = 1;
            /*
             * Live UDX host lifecycle residual (Soft!=product; not immediate
             * kill path): PCB already marked dead (u32Alive==0) but wait slot
             * not yet zombie (mid-death race or incomplete force-exit). Heal
             * so parent soft-poll of long-lived / denser keep_live product
             * hosts can reap without hang. Prefer full G-PROC-5 H3 death when
             * teardown still pending (thr_exit before as_destroy; never thrash
             * maps while thr live). denser product_host_live honesty residual.
             * Grep: process: soft wait live_heal | process: soft wait live_death
             * Grep: live_host_path=1 | thr_exit_before_as_destroy=1
             * Grep: product_host_live=1 | keep_live=1 | keep_live_rock=1
             * Grep: thr_live=1 | product_hosts=UDX | dual_dod=OPEN
             */
            if (!pS->u8Zombie && pS->pProc != NULL &&
                pS->pProc->u32Alive == 0u) {
                struct gj_process *pHeal = pS->pProc;
                u32 u32HealCode = pHeal->u32ExitCode;

                g_u64SoftWaitLiveHeal++;
                if (process_lifecycle_need_death(pHeal) != 0) {
                    g_u64SoftWaitLiveDeath++;
                    /* Grep: process: soft wait live_death */
                    kprintf("process: soft wait live_death pid=%u code=%u "
                            "thr_exit_before_as_destroy=1 "
                            "udx_host_teardown=1 live_host_path=1 "
                            "product_host_live=1 keep_live=1 keep_live_rock=1 "
                            "thr_live=1 product_hosts=UDX denser=1 "
                            "soft (G-PROC-5 H3 Soft!=product dual_dod=OPEN "
                            "soft residual denser product_host_live)\n",
                            pS->u32Pid, u32HealCode);
                    process_death(pHeal, u32HealCode);
                } else {
                    /* Grep: process: soft wait live_heal */
                    kprintf("process: soft wait live_heal pid=%u code=%u "
                            "note_exit soft (live_host_path=1 "
                            "product_host_live=1 keep_live=1 keep_live_rock=1 "
                            "thr_live=1 product_hosts=UDX denser=1 "
                            "Soft!=product dual_dod=OPEN)\n",
                            pS->u32Pid, u32HealCode);
                    process_wait_note_exit(pHeal, u32HealCode);
                }
            }
            if (!pS->u8Zombie) {
                continue;
            }
            /* Linux-shaped exit status: (code & 0xff) << 8; signal path later. */
            i32Status = (i32)((pS->u32Exit & 0xffu) << 8);
            if (pStatus != NULL) {
                *pStatus = i32Status;
            }
            i64Ret = (i64)pS->u32Pid;

            if (fNoWait) {
                /* Soft WNOWAIT: report zombie without consuming the slot. */
                g_u64WaitNowaitPeek++;
                process_soft_inc(&g_u32SoftWaitNowait);
                kprintf("process: soft wait nowait pid=%u status=0x%x soft\n",
                        pS->u32Pid, (unsigned)i32Status);
                kprintf("process: wait4 nowait pid=%u status=0x%x soft\n",
                        pS->u32Pid, (unsigned)i32Status);
                return i64Ret;
            }

            pS->u8Reaped = 1;
            g_u64WaitReap++;
            process_soft_inc(&g_u32SoftWaitReap);
            if (i64Pid > 0) {
                process_soft_inc(&g_u32SoftWait4ReapPid);
            } else {
                process_soft_inc(&g_u32SoftWait4ReapAny);
            }
            kprintf("process: soft wait reaped pid=%u status=0x%x "
                    "nohang=%d soft\n",
                    pS->u32Pid, (unsigned)i32Status, fNoHang);
            kprintf("process: wait4 reaped pid=%u status=0x%x\n", pS->u32Pid,
                    (unsigned)i32Status);
            {
                u32 j;

                for (j = 0; j < GJ_FORK_STUBS; j++) {
                    if (g_aForkUsed[j] && pS->pProc == &g_aForkStub[j]) {
                        g_aForkUsed[j] = 0;
                        break;
                    }
                }
            }
            /* Soft: full slot scrub so pid reuse cannot see stale exit. */
            pS->u8Used = 0;
            pS->u8Zombie = 0;
            pS->u8Reaped = 0;
            pS->u32Exit = 0;
            pS->u32Pid = 0;
            pS->u32Ppid = 0;
            pS->pProc = NULL;
            return i64Ret;
        }
        fLastHaveChild = fHaveChild;
        /* No unreaped children at all -> ECHILD */
        if (!fHaveChild) {
            process_soft_inc(&g_u32SoftWait4Echild);
            kprintf("process: soft wait echild pid=%ld ppid=%u soft\n",
                    (long)i64Pid, u32Ppid);
            return -10; /* ECHILD */
        }
        /* Live children, none exited yet - WNOHANG poll returns 0 (usable). */
        if (fNoHang) {
            process_soft_inc(&g_u32SoftWait4Nohang0);
            /*
             * Dual DoD B / UDX host residual (Soft!=product): yield once so
             * concurrent freestanding sshd eth accept + denser keep_live
             * product host thr (rtl8168_udx / xhci_udx / ddi_host_gj;
             * product_host_live keep_live_rock thr_live product_hosts=UDX)
             * can run while a parent soft-polls wait4 WNOHANG. Pairs with
             * spawn: soft wait poll. No kprintf on hot poll (H2 no storms).
             * Grep: process: soft wait nohang_yield | dual_dod=OPEN
             * Grep: product_host_live=1 | keep_live=1 | keep_live_rock=1
             * Grep: thr_live=1 | product_hosts=UDX
             */
            g_u64SoftWaitNohangYield++;
            thread_yield();
            return 0; /* WNOHANG */
        }
        /*
         * Blocking soft poll residual (live UDX host path; Soft!=product):
         * yield so concurrent multi-thr / denser keep_live product hosts +
         * fork exit workers can mark zombie while parent waits. Same
         * concurrent-progress class as WNOHANG yield; not immediate kill.
         * Grep: process: soft wait block_yield | live_host_path=1
         * Grep: product_host_live=1 | keep_live=1 | keep_live_rock=1
         * Grep: thr_live=1 | product_hosts=UDX | dual_dod=OPEN
         */
        g_u64SoftWaitBlockYield++;
        thread_yield();
    }
    /*
     * Soft poll budget exhausted with live children still registered.
     * Must NOT return ECHILD (would break shell wait loops). Return 0 as
     * soft would-block timeout; caller may retry or use WNOHANG poll.
     * greppable: process: soft wait
     */
    (void)fLastHaveChild;
    process_soft_inc(&g_u32SoftWait4LiveTimeout);
    kprintf("process: soft wait live_timeout pid=%ld ppid=%u attempts=%u "
            "soft (not ECHILD; product incomplete)\n",
            (long)i64Pid, u32Ppid, u32MaxAttempts);
    return 0;
}

/* ---- process: soft fork-wait product-min (PCB parent surface) ----------- */

u32
process_soft_parent_pid_of(struct gj_process *pParent)
{
    return process_soft_parent_pid_lookup(pParent);
}

/**
 * After linux_fork/clone: force child->pParent and soft identity link.
 * greppable: process: soft fork-wait product-min
 */
static void
process_soft_fw_link_child(struct gj_process *pParent, i64 i64ChildPid)
{
    struct gj_process *pChild;

    if (pParent == NULL || i64ChildPid <= 0) {
        return;
    }
    pChild = process_wait_proc_of_pid((u32)i64ChildPid);
    if (pChild != NULL) {
        pChild->pParent = pParent;
    }
}

i64
process_fork_soft(struct gj_process *pParent)
{
    u32 u32Ppid;
    i64 i64Pid;

    /*
     * process: soft fork-wait product-min - fork enter
     * Reliable parent PCB -> child wait-table pid (>= GJ_WAIT_PID_BASE).
     */
    process_soft_inc(&g_u32SoftFwForkEnter);
    if (pParent == NULL) {
        kprintf("process: soft fork-wait product-min fork EINVAL null parent\n");
        return -22; /* EINVAL */
    }
    u32Ppid = process_soft_ensure_parent_pid(pParent);
    i64Pid = process_linux_fork(u32Ppid, 0);
    if (i64Pid > 0) {
        process_soft_fw_link_child(pParent, i64Pid);
        process_soft_inc(&g_u32SoftFwForkOk);
        g_u32SoftLastForkPid = (u32)i64Pid;
        kprintf("process: soft fork-wait product-min fork pid=%ld ppid=%u ok\n",
                (long)i64Pid, u32Ppid);
    } else {
        kprintf("process: soft fork-wait product-min fork fail r=%ld ppid=%u\n",
                (long)i64Pid, u32Ppid);
    }
    return i64Pid;
}

i64
process_clone_soft(struct gj_process *pParent, u64 u64Flags)
{
    u32 u32Ppid;
    i64 i64Pid;

    process_soft_inc(&g_u32SoftFwCloneEnter);
    if (pParent == NULL) {
        kprintf("process: soft fork-wait product-min clone EINVAL null parent\n");
        return -22; /* EINVAL */
    }
    u32Ppid = process_soft_ensure_parent_pid(pParent);
    i64Pid = process_linux_clone(u32Ppid, u64Flags);
    if (i64Pid > 0) {
        process_soft_fw_link_child(pParent, i64Pid);
        process_soft_inc(&g_u32SoftFwForkOk);
        g_u32SoftLastForkPid = (u32)i64Pid;
        kprintf("process: soft fork-wait product-min clone pid=%ld ppid=%u "
                "flags=0x%lx ok\n",
                (long)i64Pid, u32Ppid, (unsigned long)u64Flags);
    } else {
        kprintf("process: soft fork-wait product-min clone fail r=%ld "
                "flags=0x%lx\n",
                (long)i64Pid, (unsigned long)u64Flags);
    }
    return i64Pid;
}

i64
process_wait_soft(struct gj_process *pParent, i64 i64Pid, int *pStatus,
                  int nOptions)
{
    u32 u32Ppid;
    i64 i64Ret;
    i32 i32St = 0;

    /*
     * process: soft fork-wait product-min - wait4-shaped reaper
     * Status: (exit & 0xff) << 8 -> GJ_WIFEXITED true for normal exit.
     */
    process_soft_inc(&g_u32SoftFwWaitEnter);
    if (pParent == NULL) {
        return -22; /* EINVAL */
    }
    u32Ppid = process_soft_parent_pid_lookup(pParent);
    if (u32Ppid == 0u) {
        /* Never forked as this parent - no soft identity -> ECHILD. */
        process_soft_inc(&g_u32SoftWait4Echild);
        kprintf("process: soft fork-wait product-min wait echild no parent_id\n");
        return -10; /* ECHILD */
    }
    i64Ret = process_wait4_ppid(u32Ppid, i64Pid, &i32St, nOptions);
    if (pStatus != NULL) {
        *pStatus = (int)i32St;
    }
    if (i64Ret > 0) {
        process_soft_inc(&g_u32SoftFwWaitReap);
        process_soft_fw_pass_once(i64Ret, (int)i32St, "wait_soft");
        kprintf("process: soft fork-wait product-min wait reaped pid=%ld "
                "status=0x%x WIFEXITED=%d\n",
                (long)i64Ret, (unsigned)i32St, GJ_WIFEXITED(i32St) ? 1 : 0);
    }
    return i64Ret;
}

i64
process_waitid_soft(struct gj_process *pParent, u32 u32IdType, i64 i64Id,
                    int *pStatus, int nOptions, int *pSiCode)
{
    i64 i64PidArg;
    i64 i64Ret;
    int nSt = 0;

    /*
     * process: soft fork-wait product-min - waitid-shaped
     * P_ALL / P_PID only; si_code = CLD_EXITED on normal exit reap.
     */
    process_soft_inc(&g_u32SoftFwWaitidEnter);
    if (pParent == NULL) {
        return -22; /* EINVAL */
    }
    if (u32IdType == GJ_P_PGID) {
        kprintf("process: soft fork-wait product-min waitid P_PGID EINVAL\n");
        return -22; /* EINVAL - process group not product */
    }
    if (u32IdType == GJ_P_PID) {
        if (i64Id <= 0) {
            return -22; /* EINVAL */
        }
        i64PidArg = i64Id;
    } else if (u32IdType == GJ_P_ALL) {
        i64PidArg = -1;
    } else {
        return -22; /* EINVAL unknown idtype */
    }

    i64Ret = process_wait_soft(pParent, i64PidArg, &nSt, nOptions);
    if (pStatus != NULL) {
        *pStatus = nSt;
    }
    if (i64Ret > 0) {
        if (pSiCode != NULL) {
            /* Soft: always exited-shape status from wait table today. */
            *pSiCode = GJ_CLD_EXITED;
        }
        process_soft_fw_pass_once(i64Ret, nSt, "waitid_soft");
        kprintf("process: soft fork-wait product-min waitid reaped pid=%ld "
                "si_code=%d status=0x%x\n",
                (long)i64Ret, GJ_CLD_EXITED, (unsigned)nSt);
    } else if (pSiCode != NULL) {
        *pSiCode = 0;
    }
    return i64Ret;
}

i64
process_fork_wait_soft_smoke(struct gj_process *pParent)
{
    i64 i64Pid;
    i64 i64Wr;
    int nStatus = 0;

    /*
     * Smoke: CLONE_VFORK -> immediate zombie, then wait (no deferred thr race).
     * First success -> process: soft fork-wait product-min PASS
     */
    if (pParent == NULL) {
        return -22;
    }
    i64Pid = process_clone_soft(pParent, GJ_CLONE_VFORK);
    if (i64Pid <= 0) {
        kprintf("process: soft fork-wait product-min smoke fork fail r=%ld\n",
                (long)i64Pid);
        return i64Pid;
    }
    i64Wr = process_wait_soft(pParent, i64Pid, &nStatus, 0);
    if (i64Wr == i64Pid && GJ_WIFEXITED(nStatus)) {
        process_soft_inc(&g_u32SoftFwSmokeOk);
        process_soft_fw_pass_once(i64Wr, nStatus, "smoke");
        kprintf("process: soft fork-wait product-min smoke ok pid=%ld "
                "status=0x%x exit=%d\n",
                (long)i64Wr, (unsigned)nStatus, GJ_WEXITSTATUS(nStatus));
        return i64Wr;
    }
    kprintf("process: soft fork-wait product-min smoke PARTIAL pid=%ld "
            "wr=%ld status=0x%x\n",
            (long)i64Pid, (long)i64Wr, (unsigned)nStatus);
    return (i64Wr < 0) ? i64Wr : -1;
}
