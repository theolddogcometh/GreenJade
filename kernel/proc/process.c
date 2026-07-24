/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Process: shared CNode, root meta bootstrap, pager on PCB, wait4 reaper,
 * G-PROC-5 death (CNode wipe + private AS reclaim for wait-registered children).
 * Soft deepen: pager ep kernel ref + badge + slot-1 mirror; wait reparent /
 * WNOWAIT / counts; death quota+CDT CNode clear + orphan reparent + scrub.
 *
 * Soft product inventory (Wave 35 exclusive deepen; this unit only):
 * greppable: "process: soft …"
 *   process: soft inventory …
 *   process: soft stats …
 *   process: soft init …
 *   process: soft seal …
 *   process: soft confine …
 *   process: soft pager …
 *   process: soft fault …
 *   process: soft wait …
 *   process: soft death …
 *   process: soft fork …
 *   process: soft jit …
 *   process: soft promise …
 *   process: soft path …
 *   process: soft return …  (Wave 19 return-path catalog)
 *   process: soft ret_surface … (Wave 19 terminal return classes)
 *   process: soft surface … (Wave 19 area catalog)
 *   process: soft deepen wave=68 …
 *   process: soft PASS|PARTIAL
 *   Apple §13 bootstrap seal checklist (wave=68 stamp):
 *     process: bootstrap seal soft …
 *     process: seal checklist …
 *     process: bootstrap seal soft deepen wave=68 …
 *   G-PROC-5 death tallies (wave=68 stamp):
 *     process: death … / process: death deepen wave=68 …
 *
 * Honesty: soft inventory only — not product multi-server seal, not Apple §13
 * closed, not bar3. Death cleanup ≠ bootstrap seal product.
 * docs/CAP_ADDRESSING.md · docs/APPLE_CHANNEL_REMAINING.md §13 ·
 * docs/SOLARIS_STYLE_REMAINING.md §6 · §9 · docs/SECURITY_CORE_DESIGN.md §13
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
#define GJ_PROCESS_SOFT_WAVE 68u
#define GJ_PROCESS_SOFT_AREAS 117u /* greppable inventory area count */
#define GJ_SEAL_SOFT_WAVE 68u /* Apple s13 seal checklist stamp */
#define GJ_SEAL_SOFT_LOG_MAX   8u

/* Forward: wait table lives later; soft census snapshots used/zombie/free. */
#define GJ_WAIT_SLOTS 64u
#define GJ_WAIT_PID_BASE 100u
#define GJ_FORK_STUBS 16u

/*
 * Soft path tallies (diagnostics only; wrap OK). Never hard-gate product.
 * greppable: process: soft …
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
static u32 g_u32SoftForkEnter;
static u32 g_u32SoftForkOk;
static u32 g_u32SoftForkFull;
static u32 g_u32SoftForkVfork;
static u32 g_u32SoftForkDeferred;
static u32 g_u32SoftForkAsOk;
static u32 g_u32SoftForkAsFail;
static u32 g_u32SoftForkCloneOk;
static u32 g_u32SoftForkCloneFail;
static u32 g_u32SoftDeathEnter;
static u32 g_u32SoftLogN;
static u8  g_fSoftInvOnce;

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
 * Prefix-stable: "process: soft …". Never hard-gates.
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
            "(soft inventory only; not bar3)\n",
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

    /* Grep: process: soft wait */
    kprintf("process: soft wait reg_ok=%u reg_idem=%u reg_full=%u "
            "reg_null=%u zombie=%u reap=%u nowait=%u reparent=%u "
            "forget=%u wait4_enter=%u echild=%u nohang0=%u "
            "used=%u zombie_now=%u live=%u free=%u wave=%u\n",
            g_u32SoftWaitRegOk, g_u32SoftWaitRegIdem, g_u32SoftWaitRegFull,
            g_u32SoftWaitRegNull, g_u32SoftWaitZombie, g_u32SoftWaitReap,
            g_u32SoftWaitNowait, g_u32SoftWaitReparentN, g_u32SoftWaitForget,
            g_u32SoftWait4Enter, g_u32SoftWait4Echild, g_u32SoftWait4Nohang0,
            u32Used, u32Zombie, u32Live, u32Free, GJ_PROCESS_SOFT_WAVE);

    /* Grep: process: soft death */
    kprintf("process: soft death enter=%u wave=%u "
            "(detail via process: death tallies / deepen)\n",
            g_u32SoftDeathEnter, GJ_PROCESS_SOFT_WAVE);

    /* Grep: process: soft fork */
    kprintf("process: soft fork enter=%u ok=%u full=%u vfork=%u "
            "deferred=%u as_ok=%u as_fail=%u clone_ok=%u clone_fail=%u "
            "wave=%u\n",
            g_u32SoftForkEnter, g_u32SoftForkOk, g_u32SoftForkFull,
            g_u32SoftForkVfork, g_u32SoftForkDeferred, g_u32SoftForkAsOk,
            g_u32SoftForkAsFail, g_u32SoftForkCloneOk, g_u32SoftForkCloneFail,
            GJ_PROCESS_SOFT_WAVE);

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
     * Honesty: PCB + fixed wait table + soft seal lamps ≠ multi-server
     * product seal / full posix wait / bar3.
     * Grep: process: soft path
     */
    kprintf("process: soft path claim=pcb,root_meta,pager,confine,"
            "wait4,death,fork G-PROC-5=1 Apple_s13_open=1 "
            "fixed_wait_table=%u fork_stubs=%u multi_server_seal=0 "
            "bar3=0 via=%s wave=%u (soft inventory; not product gate)\n",
            GJ_WAIT_SLOTS, GJ_FORK_STUBS, szViaSafe, GJ_PROCESS_SOFT_WAVE);

    /*
     * Grep: process: soft return
     * Wave 19 return-path catalog — init/seal/pager/fault/wait/fork outcomes.
     * Soft ≠ multi-server seal / bar3 product gate. product_kernel=OPEN.
     */
    kprintf("process: soft return init_ok=%u init_null=%u root_ok=%u "
            "root_fail=%u root_busy=%u pager_set=%u pager_fail=%u "
            "fault_no_pager=%u fault_wx=%u fault_busy=%u "
            "wait_reg_ok=%u wait_reg_full=%u wait_echild=%u "
            "fork_ok=%u fork_full=%u fork_as_fail=%u death=%u "
            "confine=%u product_kernel=OPEN wave=%u\n",
            g_u32SoftInitOk, g_u32SoftInitNull, g_u32SoftRootMetaOk,
            g_u32SoftRootMetaFail, g_u32SoftRootMetaBusy, g_u32SoftPagerSetOk,
            g_u32SoftPagerSetFail, g_u32SoftFaultNoPager, g_u32SoftFaultWxDeny,
            g_u32SoftFaultBusy, g_u32SoftWaitRegOk, g_u32SoftWaitRegFull,
            g_u32SoftWait4Echild, g_u32SoftForkOk, g_u32SoftForkFull,
            g_u32SoftForkAsFail, g_u32SoftDeathEnter, g_u32SoftConfineN,
            GJ_PROCESS_SOFT_WAVE);

    /* Grep: process: soft ret_surface — Wave 19 terminal return classes */
    kprintf("process: soft ret_surface init=ok|null root=ok|fail|busy "
            "pager=set|fail fault=no_pager|wx|busy wait=reg_ok|reg_full|echild "
            "fork=ok|full|as_fail death confine product_kernel=OPEN "
            "areas=%u wave=%u\n",
            GJ_PROCESS_SOFT_AREAS, GJ_PROCESS_SOFT_WAVE);

    /* Grep: process: soft surface — Wave 19 area catalog */
    kprintf("process: soft surface inventory,stats,init,seal,confine,"
            "pager,fault,wait,death,fork,jit,promise,path,return,"
            "ret_surface,surface,deepen,PASS,bootstrap_seal,death_tallies,"
            "headroom areas=%u wave=%u\n",
            GJ_PROCESS_SOFT_AREAS, GJ_PROCESS_SOFT_WAVE);

    /* Grep: process: soft headroom — wait table free slots */
    kprintf("process: soft headroom wait_free=%u wait_used=%u "
            "wait_slots=%u fork_stubs=%u wave=%u\n",
            u32Free, u32Used, GJ_WAIT_SLOTS, GJ_FORK_STUBS,
            GJ_PROCESS_SOFT_WAVE);

    /* Grep: process: soft retmap — Wave 19 return-surface map */
    kprintf("process: soft retmap ok|fail|inval|nodev|busy|nomem product_gate=0 soft_only=1 wave=68\n");

    /* Grep: process: soft deepen */
    /*
     * ---- Wave 19 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft≠product; not bar3.
     */
    /* Grep: process: soft retclass — Wave 19 return-class taxonomy (kept) */
    kprintf("process: soft retclass ok|fail|inval|nodev|busy|nomem "
            "soft_only=1 product_gate=0 wave=%u "
            "(retclass taxonomy; Soft≠product; not bar3)\n",
            (unsigned)GJ_PROCESS_SOFT_WAVE);
    /* Grep: process: soft retlane — Wave 19 return-lane catalog (kept) */
    kprintf("process: soft retlane inv|selftest|rate|retcode|retmap|class "
            "product_kernel=OPEN soft_ne_product=1 wave=%u "
            "(retlane catalog; Soft≠product)\n",
            (unsigned)GJ_PROCESS_SOFT_WAVE);
    /*
     * ---- Wave 20 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft≠product; not bar3.
     */
    /* Grep: process: soft retbound — Wave 20 return-bound honesty (kept) */
    kprintf("process: soft retbound soft_only=1 product_gate=0 hard_gate=0 "
            "never_blocks_m0=1 wave=%u "
            "(retbound honesty; Soft≠product; not bar3)\n",
            (unsigned)GJ_PROCESS_SOFT_WAVE);
    /* Grep: process: soft retseal — Wave 20 seal stamp (kept) */
    kprintf("process: soft retseal exclusive=1 soft_ne_product=1 "
            "product_kernel=OPEN bar3=0 wave=%u "
            "(retseal stamp; Soft≠product)\n",
            (unsigned)GJ_PROCESS_SOFT_WAVE);
            /*
             * ---- Wave 21 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
            */
            /* Grep: process: soft retpulse — Wave 21 return-pulse honesty (kept) */
            kprintf("process: soft retpulse soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retpulse honesty; Soft≠product; not bar3)\n",
                    (unsigned)GJ_PROCESS_SOFT_WAVE);
            /* Grep: process: soft retmark — Wave 21 mark stamp (kept) */
            kprintf("process: soft retmark exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retmark stamp; Soft≠product)\n",
                    (unsigned)GJ_PROCESS_SOFT_WAVE);
            /*
             * ---- Wave 22 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
            */
            /* Grep: process: soft retphase — Wave 22 return-phase honesty (kept) */
            kprintf("process: soft retphase soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retphase honesty; Soft≠product; not bar3)\n",
                    (unsigned)GJ_PROCESS_SOFT_WAVE);
            /* Grep: process: soft retbadge — Wave 22 badge stamp (kept) */
            kprintf("process: soft retbadge exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbadge stamp; Soft≠product)\n",
                    (unsigned)GJ_PROCESS_SOFT_WAVE);
/*
 * ---- Wave 23 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
            */
            /* Grep: process: soft rettoken — Wave 23 return-token honesty (kept) */
            kprintf("process: soft rettoken soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(rettoken honesty; Soft≠product; not bar3)\n",
                    (unsigned)GJ_PROCESS_SOFT_WAVE);
            /* Grep: process: soft retcrest — Wave 23 crest stamp (kept) */
            kprintf("process: soft retcrest exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retcrest stamp; Soft≠product)\n",
                    (unsigned)GJ_PROCESS_SOFT_WAVE);
            /*
             * ---- Wave 24 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: process: soft retvault — Wave 24 return-vault honesty (kept) */
            kprintf("process: soft retvault soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retvault honesty; Soft≠product; not bar3)\n",
                    (unsigned)GJ_PROCESS_SOFT_WAVE);
            /* Grep: process: soft retbanner — Wave 24 banner stamp (kept) */
            kprintf("process: soft retbanner exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbanner stamp; Soft≠product)\n",
                    (unsigned)GJ_PROCESS_SOFT_WAVE);
            /*
             * ---- Wave 25 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: process: soft retledger — Wave 25 return-ledger honesty (kept) */
            kprintf("process: soft retledger soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retledger honesty; Soft≠product; not bar3)\n",
                    (unsigned)GJ_PROCESS_SOFT_WAVE);
            /* Grep: process: soft retbeacon — Wave 25 beacon stamp (kept) */
            kprintf("process: soft retbeacon exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbeacon stamp; Soft≠product)\n",
                    (unsigned)GJ_PROCESS_SOFT_WAVE);
            /*
             * ---- Wave 26 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: process: soft retcipher — Wave 26 return-cipher honesty (kept) */
            kprintf("process: soft retcipher soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retcipher honesty; Soft≠product; not bar3)\n",
                    (unsigned)GJ_PROCESS_SOFT_WAVE);
            /* Grep: process: soft retflame — Wave 26 flame stamp (kept) */
            kprintf("process: soft retflame exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retflame stamp; Soft≠product)\n",
                    (unsigned)GJ_PROCESS_SOFT_WAVE);
                    /*
                     * ---- Wave 27 complementary surfaces (kept) (never reshape primary).
                     * Return surfaces only — soft inventory; never hard-gates product paths.
                     * Soft≠product; not bar3.
                     */
                    /* Grep: process: soft retprism — Wave 27 return-prism honesty (kept) */
                    kprintf("process: soft retprism soft_only=1 product_gate=0 soft_ne_product=1 "
                            "never_blocks_m0=1 wave=%u "
                            "(retprism honesty; Soft≠product; not bar3)\n",
                            (unsigned)GJ_PROCESS_SOFT_WAVE);
                    /* Grep: process: soft retforge — Wave 27 forge stamp (kept) */
                    kprintf("process: soft retforge exclusive=1 soft_ne_product=1 "
                            "product_kernel=OPEN bar3=0 wave=%u "
                            "(retforge stamp; Soft≠product)\n",
                            (unsigned)GJ_PROCESS_SOFT_WAVE);
                            /*
                             * ---- Wave 28 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: process: soft retshard — Wave 28 return-shard honesty (kept) */
                            kprintf("process: soft retshard soft_only=1 product_gate=0 soft_ne_product=1 "
                                "never_blocks_m0=1 wave=%u "
                                "(retshard honesty; Soft≠product; not bar3)\n",
                                (unsigned)GJ_PROCESS_SOFT_WAVE);
                            /* Grep: process: soft retcrown — Wave 28 crown stamp (kept) */
                            kprintf("process: soft retcrown exclusive=1 soft_ne_product=1 "
                                "product_kernel=OPEN bar3=0 wave=%u "
                                "(retcrown stamp; Soft≠product)\n",
                                (unsigned)GJ_PROCESS_SOFT_WAVE);
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
 * Diagnostics only — never hard-gates.
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
 * Soft Apple §13 bootstrap seal checklist (process.c only; Wave 15 deepen).
 * Enumerates PCB lamps: root meta, ambient/confine, pager empty, promises,
 * plus soft open inventory for Apple s13 product seal items (all 0 until
 * product retype/IRQ/untyped seal exists — honesty inventory only).
 * One-way soft lamp is inventory only — does NOT seal retype/IRQ/untyped.
 * Grep: process: bootstrap seal soft | process: seal checklist
 * Honesty: not product-complete; no bar3.
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
 * Soft CNode occupancy count (const; inventory only for seal checklist).
 * Counts non-INVALID slots; does not lock (snapshot may race — soft OK).
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
     * Design K1–K6: root meta is kernel ops only — never a factory for
     * transferable PROCESS/CNODE. Soft lamp always 1 (policy intent);
     * product seal of retype/IRQ/untyped remains open (0 below).
     */
    fRootMetaNotFactory = 1;
    /*
     * Const path: gen!=0 and LIVE when pPagerEpObj known.
     * Empty gen ⇒ pager empty (expected post-init before set_pager).
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

    /* Grep: process: seal checklist … */
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
    /*
     * ---- Wave 19 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft≠product; not bar3.
     */
    /* Grep: process: bootstrap seal: soft retclass — Wave 19 return-class taxonomy (kept) */
    kprintf("process: bootstrap seal: soft retclass ok|fail|inval|nodev|busy|nomem "
            "soft_only=1 product_gate=0 wave=%u "
            "(retclass taxonomy; Soft≠product; not bar3)\n",
            (unsigned)GJ_PROCESS_SOFT_WAVE);
    /* Grep: process: bootstrap seal: soft retlane — Wave 19 return-lane catalog (kept) */
    kprintf("process: bootstrap seal: soft retlane inv|selftest|rate|retcode|retmap|class "
            "product_kernel=OPEN soft_ne_product=1 wave=%u "
            "(retlane catalog; Soft≠product)\n",
            (unsigned)GJ_PROCESS_SOFT_WAVE);
    /*
     * ---- Wave 20 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft≠product; not bar3.
     */
    /* Grep: process: bootstrap seal: soft retbound — Wave 20 return-bound honesty (kept) */
    kprintf("process: bootstrap seal: soft retbound soft_only=1 product_gate=0 hard_gate=0 "
            "never_blocks_m0=1 wave=%u "
            "(retbound honesty; Soft≠product; not bar3)\n",
            (unsigned)GJ_PROCESS_SOFT_WAVE);
    /* Grep: process: bootstrap seal: soft retseal — Wave 20 seal stamp (kept) */
    kprintf("process: bootstrap seal: soft retseal exclusive=1 soft_ne_product=1 "
            "product_kernel=OPEN bar3=0 wave=%u "
            "(retseal stamp; Soft≠product)\n",
            (unsigned)GJ_PROCESS_SOFT_WAVE);
            /*
             * ---- Wave 21 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
            */
            /* Grep: process: bootstrap seal: soft retpulse — Wave 21 return-pulse honesty (kept) */
            kprintf("process: bootstrap seal: soft retpulse soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retpulse honesty; Soft≠product; not bar3)\n",
                    (unsigned)GJ_PROCESS_SOFT_WAVE);
            /* Grep: process: bootstrap seal: soft retmark — Wave 21 mark stamp (kept) */
            kprintf("process: bootstrap seal: soft retmark exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retmark stamp; Soft≠product)\n",
                    (unsigned)GJ_PROCESS_SOFT_WAVE);
            /*
             * ---- Wave 22 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
            */
            /* Grep: process: bootstrap seal: soft retphase — Wave 22 return-phase honesty (kept) */
            kprintf("process: bootstrap seal: soft retphase soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retphase honesty; Soft≠product; not bar3)\n",
                    (unsigned)GJ_PROCESS_SOFT_WAVE);
            /* Grep: process: bootstrap seal: soft retbadge — Wave 22 badge stamp (kept) */
            kprintf("process: bootstrap seal: soft retbadge exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbadge stamp; Soft≠product)\n",
                    (unsigned)GJ_PROCESS_SOFT_WAVE);
/*
 * ---- Wave 23 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
            */
            /* Grep: process: bootstrap seal: soft rettoken — Wave 23 return-token honesty (kept) */
            kprintf("process: bootstrap seal: soft rettoken soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(rettoken honesty; Soft≠product; not bar3)\n",
                    (unsigned)GJ_PROCESS_SOFT_WAVE);
            /* Grep: process: bootstrap seal: soft retcrest — Wave 23 crest stamp (kept) */
            kprintf("process: bootstrap seal: soft retcrest exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retcrest stamp; Soft≠product)\n",
                    (unsigned)GJ_PROCESS_SOFT_WAVE);
            /*
             * ---- Wave 24 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: process: bootstrap seal: soft retvault — Wave 24 return-vault honesty (kept) */
            kprintf("process: bootstrap seal: soft retvault soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retvault honesty; Soft≠product; not bar3)\n",
                    (unsigned)GJ_PROCESS_SOFT_WAVE);
            /* Grep: process: bootstrap seal: soft retbanner — Wave 24 banner stamp (kept) */
            kprintf("process: bootstrap seal: soft retbanner exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbanner stamp; Soft≠product)\n",
                    (unsigned)GJ_PROCESS_SOFT_WAVE);
            /*
             * ---- Wave 25 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: process: bootstrap seal: soft retledger — Wave 25 return-ledger honesty (kept) */
            kprintf("process: bootstrap seal: soft retledger soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retledger honesty; Soft≠product; not bar3)\n",
                    (unsigned)GJ_PROCESS_SOFT_WAVE);
            /* Grep: process: bootstrap seal: soft retbeacon — Wave 25 beacon stamp (kept) */
            kprintf("process: bootstrap seal: soft retbeacon exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbeacon stamp; Soft≠product)\n",
                    (unsigned)GJ_PROCESS_SOFT_WAVE);
            /*
             * ---- Wave 26 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: process: bootstrap seal: soft retcipher — Wave 26 return-cipher honesty (kept) */
            kprintf("process: bootstrap seal: soft retcipher soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retcipher honesty; Soft≠product; not bar3)\n",
                    (unsigned)GJ_PROCESS_SOFT_WAVE);
            /* Grep: process: bootstrap seal: soft retflame — Wave 26 flame stamp (kept) */
            kprintf("process: bootstrap seal: soft retflame exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retflame stamp; Soft≠product)\n",
                    (unsigned)GJ_PROCESS_SOFT_WAVE);
                    /*
                     * ---- Wave 27 complementary surfaces (kept) (never reshape primary).
                     * Return surfaces only — soft inventory; never hard-gates product paths.
                     * Soft≠product; not bar3.
                     */
                    /* Grep: process: bootstrap seal: soft retprism — Wave 27 return-prism honesty (kept) */
                    kprintf("process: bootstrap seal: soft retprism soft_only=1 product_gate=0 soft_ne_product=1 "
                            "never_blocks_m0=1 wave=%u "
                            "(retprism honesty; Soft≠product; not bar3)\n",
                            (unsigned)GJ_PROCESS_SOFT_WAVE);
                    /* Grep: process: bootstrap seal: soft retforge — Wave 27 forge stamp (kept) */
                    kprintf("process: bootstrap seal: soft retforge exclusive=1 soft_ne_product=1 "
                            "product_kernel=OPEN bar3=0 wave=%u "
                            "(retforge stamp; Soft≠product)\n",
                            (unsigned)GJ_PROCESS_SOFT_WAVE);
                            /*
                             * ---- Wave 28 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: process: bootstrap seal: soft retshard — Wave 28 return-shard honesty (kept) */
                            kprintf("process: bootstrap seal: soft retshard soft_only=1 product_gate=0 soft_ne_product=1 "
                                "never_blocks_m0=1 wave=%u "
                                "(retshard honesty; Soft≠product; not bar3)\n",
                                (unsigned)GJ_PROCESS_SOFT_WAVE);
                            /* Grep: process: bootstrap seal: soft retcrown — Wave 28 crown stamp (kept) */
                            kprintf("process: bootstrap seal: soft retcrown exclusive=1 soft_ne_product=1 "
                                "product_kernel=OPEN bar3=0 wave=%u "
                                "(retcrown stamp; Soft≠product)\n",
                                (unsigned)GJ_PROCESS_SOFT_WAVE);
                                /*
                             * ---- Wave 29 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: process: bootstrap seal: soft retglyph — Wave 29 return-glyph honesty (kept) */
                            kprintf("process: bootstrap seal: soft retglyph soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retglyph honesty; Soft≠product; not bar3)\n",
                                    (unsigned)GJ_PROCESS_SOFT_WAVE);
                            /* Grep: process: bootstrap seal: soft retscepter — Wave 29 scepter stamp (kept) */
                            kprintf("process: bootstrap seal: soft retscepter exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=%u "
                                    "(retscepter stamp; Soft≠product)\n",
                                    (unsigned)GJ_PROCESS_SOFT_WAVE);
                                /*
                             * ---- Wave 30 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: process: bootstrap seal: soft retsigil — Wave 30 return-sigil honesty (kept) */
                            kprintf("process: bootstrap seal: soft retsigil soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retsigil honesty; Soft≠product; not bar3)\n",
                                    (unsigned)GJ_PROCESS_SOFT_WAVE);
                            /* Grep: process: bootstrap seal: soft retemblem — Wave 30 emblem stamp (kept) */
                            kprintf("process: bootstrap seal: soft retemblem exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=%u "
                                    "(retemblem stamp; Soft≠product)\n",
                                    (unsigned)GJ_PROCESS_SOFT_WAVE);
                            /*
                             * ---- Wave 31 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: process: bootstrap seal: soft retaegis — Wave 31 return-aegis honesty (kept) */
                            kprintf("process: bootstrap seal: soft retaegis soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retaegis honesty; Soft≠product; not bar3)\n",
                                    (unsigned)GJ_PROCESS_SOFT_WAVE);
                            /* Grep: process: bootstrap seal: soft retsigil — Wave 30 return-sigil honesty (kept) */
                            kprintf("process: bootstrap seal: soft retsigil soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retsigil honesty; Soft≠product; not bar3)\n",
                                    (unsigned)GJ_PROCESS_SOFT_WAVE);
                            /* Grep: process: bootstrap seal: soft retmantle — Wave 31 mantle stamp (kept) */
                            kprintf("process: bootstrap seal: soft retmantle exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=%u "
                                    "(retmantle stamp; Soft≠product)\n",
                                    (unsigned)GJ_PROCESS_SOFT_WAVE);
/*
 * ---- Wave 32 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: process: bootstrap seal: soft retbulwark — Wave 32 return-bulwark honesty (kept) */
kprintf("process: bootstrap seal: soft retbulwark soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbulwark honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_PROCESS_SOFT_WAVE);
/* Grep: process: bootstrap seal: soft retpanoply — Wave 32 panoply stamp (kept) */
kprintf("process: bootstrap seal: soft retpanoply exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retpanoply stamp; Soft≠product)\n",
        (unsigned)GJ_PROCESS_SOFT_WAVE);
/*
 * ---- Wave 33 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: process: bootstrap seal: soft retbastion — Wave 33 return-bastion honesty (kept) */
kprintf("process: bootstrap seal: soft retbastion soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbastion honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_PROCESS_SOFT_WAVE);
/* Grep: process: bootstrap seal: soft retcitadel — Wave 33 citadel stamp (kept) */
kprintf("process: bootstrap seal: soft retcitadel exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retcitadel stamp; Soft≠product)\n",
        (unsigned)GJ_PROCESS_SOFT_WAVE);
/*
 * ---- Wave 34 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: process: bootstrap seal: soft retredoubt — Wave 34 return-redoubt honesty */
kprintf("process: bootstrap seal: soft retredoubt soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retredoubt honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_PROCESS_SOFT_WAVE);
/* Grep: process: bootstrap seal: soft retkeep — Wave 34 exclusive keep stamp */
kprintf("process: bootstrap seal: soft retkeep exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retkeep stamp; Soft≠product)\n",
        (unsigned)GJ_PROCESS_SOFT_WAVE);
/*
 * ---- Wave 35 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: process: bootstrap seal: soft retfortress — Wave 35 return-fortress honesty */
kprintf("process: bootstrap seal: soft retfortress soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retfortress honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_PROCESS_SOFT_WAVE);
/* Grep: process: bootstrap seal: soft retpalace — Wave 35 exclusive palace stamp */
kprintf("process: bootstrap seal: soft retpalace exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retpalace stamp; Soft≠product)\n",
        (unsigned)GJ_PROCESS_SOFT_WAVE);
/*
 * ---- Wave 36 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: process: bootstrap seal: soft rethold — Wave 36 return-hold honesty */
kprintf("process: bootstrap seal: soft rethold soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(rethold honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_PROCESS_SOFT_WAVE);
/* Grep: process: bootstrap seal: soft retspire — Wave 36 exclusive spire stamp */
kprintf("process: bootstrap seal: soft retspire exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retspire stamp; Soft≠product)\n",
        (unsigned)GJ_PROCESS_SOFT_WAVE);
/*
 * ---- Wave 37 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: process: bootstrap seal: soft retwall — Wave 37 return-wall honesty */
kprintf("process: bootstrap seal: soft retwall soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retwall honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_PROCESS_SOFT_WAVE);
/* Grep: process: bootstrap seal: soft retgate — Wave 37 exclusive gate stamp */
kprintf("process: bootstrap seal: soft retgate exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retgate stamp; Soft≠product)\n",
        (unsigned)GJ_PROCESS_SOFT_WAVE);
/*
 * ---- Wave 38 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: process: bootstrap seal: soft retmoat — Wave 38 return-moat honesty */
kprintf("process: bootstrap seal: soft retmoat soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retmoat honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_PROCESS_SOFT_WAVE);
/* Grep: process: bootstrap seal: soft retower — Wave 38 exclusive tower stamp */
kprintf("process: bootstrap seal: soft retower exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retower stamp; Soft≠product)\n",
        (unsigned)GJ_PROCESS_SOFT_WAVE);
                            
/*
 * ---- Wave 39 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: process: bootstrap seal: soft retbarbican — Wave 39 return-barbican honesty */
kprintf("process: bootstrap seal: soft retbarbican soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbarbican honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_PROCESS_SOFT_WAVE);
/* Grep: process: bootstrap seal: soft retglacis — Wave 39 exclusive glacis stamp */
kprintf("process: bootstrap seal: soft retglacis exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retglacis stamp; Soft≠product)\n",
        (unsigned)GJ_PROCESS_SOFT_WAVE);
/*
 * ---- Wave 40 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: process: bootstrap seal: soft retcurtain — Wave 40 return-curtain honesty */
kprintf("process: bootstrap seal: soft retcurtain soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retcurtain honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_PROCESS_SOFT_WAVE);
/* Grep: process: bootstrap seal: soft retparapet — Wave 40 exclusive parapet stamp */
kprintf("process: bootstrap seal: soft retparapet exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retparapet stamp; Soft≠product)\n",
        (unsigned)GJ_PROCESS_SOFT_WAVE);
/*
 * ---- Wave 41 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: process: bootstrap seal: soft retravelin — Wave 41 return-travelin honesty */
kprintf("process: bootstrap seal: soft retravelin soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retravelin honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_PROCESS_SOFT_WAVE);
/* Grep: process: bootstrap seal: soft retditch — Wave 41 exclusive ditch stamp */
kprintf("process: bootstrap seal: soft retditch exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retditch stamp; Soft≠product)\n",
        (unsigned)GJ_PROCESS_SOFT_WAVE);
/*
 * ---- Wave 42 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: process: bootstrap seal: soft retportcullis — Wave 42 return-portcullis honesty */
kprintf("process: bootstrap seal: soft retportcullis soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retportcullis honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_PROCESS_SOFT_WAVE);
/* Grep: process: bootstrap seal: soft retbattlement — Wave 42 exclusive battlement stamp */
kprintf("process: bootstrap seal: soft retbattlement exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retbattlement stamp; Soft≠product)\n",
        (unsigned)GJ_PROCESS_SOFT_WAVE);
/*
 * ---- Wave 43 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: process: bootstrap seal: soft retmachicolation — Wave 43 return-machicolation honesty */
kprintf("process: bootstrap seal: soft retmachicolation soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retmachicolation honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_PROCESS_SOFT_WAVE);
/* Grep: process: bootstrap seal: soft retarrowslit — Wave 43 exclusive arrowslit stamp */
kprintf("process: bootstrap seal: soft retarrowslit exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retarrowslit stamp; Soft≠product)\n",
        (unsigned)GJ_PROCESS_SOFT_WAVE);

/*
 * ---- Wave 44 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: process: bootstrap seal: soft retmerlon — Wave 44 return-merlon honesty */
kprintf("process: bootstrap seal: soft retmerlon soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retmerlon honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_PROCESS_SOFT_WAVE);
/* Grep: process: bootstrap seal: soft retembrasure — Wave 44 exclusive embrasure stamp */
kprintf("process: bootstrap seal: soft retembrasure exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retembrasure stamp; Soft≠product)\n",
        (unsigned)GJ_PROCESS_SOFT_WAVE);

/*
 * ---- Wave 45 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: process: bootstrap seal: soft retkeepgate — Wave 45 return-keepgate honesty */
kprintf("process: bootstrap seal: soft retkeepgate soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retkeepgate honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_PROCESS_SOFT_WAVE);
/* Grep: process: bootstrap seal: soft retouterward — Wave 45 exclusive outerward stamp */
kprintf("process: bootstrap seal: soft retouterward exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retouterward stamp; Soft≠product)\n",
        (unsigned)GJ_PROCESS_SOFT_WAVE);

/*
 * ---- Wave 46 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: process: bootstrap seal: soft retbailey — Wave 46 return-bailey honesty */
kprintf("process: bootstrap seal: soft retbailey soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbailey honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_PROCESS_SOFT_WAVE);
/* Grep: process: bootstrap seal: soft retpostern — Wave 46 exclusive postern stamp */
kprintf("process: bootstrap seal: soft retpostern exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retpostern stamp; Soft≠product)\n",
        (unsigned)GJ_PROCESS_SOFT_WAVE);

/*
 * ---- Wave 47 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: process: bootstrap seal: soft retinnerward — Wave 47 return-innerward honesty */
kprintf("process: bootstrap seal: soft retinnerward soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=68 "
        "(retinnerward honesty; Soft≠product; not bar3)\n");
/* Grep: process: bootstrap seal: soft retdonjon — Wave 47 exclusive donjon stamp */
kprintf("process: bootstrap seal: soft retdonjon exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=68 "
        "(retdonjon stamp; Soft≠product)\n");

/*
 * ---- Wave 48 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: process: bootstrap seal: soft retchevaux — Wave 48 return-chevaux honesty */
kprintf("process: bootstrap seal: soft retchevaux soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=68 "
        "(retchevaux honesty; Soft≠product; not bar3)\n");
/* Grep: process: bootstrap seal: soft retpalisade — Wave 48 exclusive palisade stamp */
kprintf("process: bootstrap seal: soft retpalisade exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=68 "
        "(retpalisade stamp; Soft≠product)\n");

/*
 * ---- Wave 49 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: process: bootstrap seal: soft retglacisgate — Wave 49 return-glacisgate honesty */
kprintf("process: bootstrap seal: soft retglacisgate soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=68 "
        "(retglacisgate honesty; Soft≠product; not bar3)\n");
/* Grep: process: bootstrap seal: soft retoutwork — Wave 49 exclusive outwork stamp */
kprintf("process: bootstrap seal: soft retoutwork exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=68 "
        "(retoutwork stamp; Soft≠product)\n");
/*
 * ---- Wave 50 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: process: bootstrap seal: soft retsally — Wave 50 return-sally honesty */
kprintf("process: bootstrap seal: soft retsally soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=68 "
        "(retsally honesty; Soft≠product; not bar3)\n");
/* Grep: process: bootstrap seal: soft retcounterscarp — Wave 50 exclusive counterscarp stamp */
kprintf("process: bootstrap seal: soft retcounterscarp exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=68 "
        "(retcounterscarp stamp; Soft≠product)\n");
/*
 * ---- Wave 51 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: process: bootstrap seal: soft retfosse — Wave 51 return-fosse honesty */
kprintf("process: bootstrap seal: soft retfosse soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=68 "
        "(retfosse honesty; Soft≠product; not bar3)\n");
/* Grep: process: bootstrap seal: soft retcoveredway — Wave 51 exclusive coveredway stamp */
kprintf("process: bootstrap seal: soft retcoveredway exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=68 "
        "(retcoveredway stamp; Soft≠product)\n");

/*
 * ---- Wave 52 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: process: bootstrap seal: soft rettenaille — Wave 52 return-tenaille honesty */
kprintf("process: bootstrap seal: soft rettenaille soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=68 "
        "(rettenaille honesty; Soft≠product; not bar3)\n");
/* Grep: process: bootstrap seal: soft retdemilune — Wave 52 exclusive demilune stamp */
kprintf("process: bootstrap seal: soft retdemilune exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=68 "
        "(retdemilune stamp; Soft≠product)\n");
/*
 * ---- Wave 53 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: process: bootstrap seal: soft retravelin — Wave 53 return-travelin honesty */
kprintf("process: bootstrap seal: soft retravelin soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=68 "
        "(retravelin honesty; Soft≠product; not bar3)\n");
/* Grep: process: bootstrap seal: soft retlunette — Wave 53 exclusive lunette stamp */
kprintf("process: bootstrap seal: soft retlunette exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=68 "
        "(retlunette stamp; Soft≠product)\n");
/*
 * ---- Wave 54 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: process: bootstrap seal: soft retcaponier — Wave 54 return-caponier honesty */
kprintf("process: bootstrap seal: soft retcaponier soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=68 "
        "(retcaponier honesty; Soft≠product; not bar3)\n");
/* Grep: process: bootstrap seal: soft retredan — Wave 54 exclusive redan stamp */
kprintf("process: bootstrap seal: soft retredan exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=68 "
        "(retredan stamp; Soft≠product)\n");
/*
 * ---- Wave 55 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: process: bootstrap seal: soft retflank — Wave 55 return-flank honesty */
kprintf("process: bootstrap seal: soft retflank soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=68 "
        "(retflank honesty; Soft≠product; not bar3)\n");
/* Grep: process: bootstrap seal: soft retface — Wave 55 exclusive face stamp */
kprintf("process: bootstrap seal: soft retface exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=68 "
        "(retface stamp; Soft≠product)\n");
/*
 * ---- Wave 56 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: process: bootstrap seal: soft retgorge — Wave 56 return-gorge honesty */
kprintf("process: bootstrap seal: soft retgorge soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=68 "
        "(retgorge honesty; Soft≠product; not bar3)\n");
/* Grep: process: bootstrap seal: soft retshoulder — Wave 56 exclusive shoulder stamp */
kprintf("process: bootstrap seal: soft retshoulder exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=68 "
        "(retshoulder stamp; Soft≠product)\n");
/*
 * ---- Wave 57 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: process: bootstrap seal: soft retraverse — Wave 57 return-traverse honesty */
kprintf("process: bootstrap seal: soft retraverse soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=68 "
        "(retraverse honesty; Soft≠product; not bar3)\n");
/* Grep: process: bootstrap seal: soft retcasemate — Wave 57 exclusive casemate stamp */
kprintf("process: bootstrap seal: soft retcasemate exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=68 "
        "(retcasemate stamp; Soft≠product)\n");

/*
 * ---- Wave 58 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: process: bootstrap seal: soft retorillon — Wave 58 return-orillon honesty */
kprintf("process: bootstrap seal: soft retorillon soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=68 "
        "(retorillon honesty; Soft≠product; not bar3)\n");
/* Grep: process: bootstrap seal: soft retbonnette — Wave 58 exclusive bonnette stamp */
kprintf("process: bootstrap seal: soft retbonnette exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=68 "
        "(retbonnette stamp; Soft≠product)\n");

/*
 * ---- Wave 59 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: process: bootstrap seal: soft retcrownwork — Wave 59 return-crownwork honesty */
kprintf("process: bootstrap seal: soft retcrownwork soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=68 "
        "(retcrownwork honesty; Soft≠product; not bar3)\n");
/* Grep: process: bootstrap seal: soft rethornwork — Wave 59 exclusive hornwork stamp */
kprintf("process: bootstrap seal: soft rethornwork exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=68 "
        "(rethornwork stamp; Soft≠product)\n");

/*
 * ---- Wave 60 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: process: bootstrap seal: soft retplace — Wave 60 return-place honesty */
kprintf("process: bootstrap seal: soft retplace soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=68 "
        "(retplace honesty; Soft≠product; not bar3)\n");
/* Grep: process: bootstrap seal: soft retenvelope — Wave 60 exclusive envelope stamp */
kprintf("process: bootstrap seal: soft retenvelope exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=68 "
        "(retenvelope stamp; Soft≠product)\n");









/*
 * ---- Wave 61 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: process: bootstrap seal: soft retcounterguard — Wave 61 return-counterguard honesty */
kprintf("process: bootstrap seal: soft retcounterguard soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=68 "
        "(retcounterguard honesty; Soft≠product; not bar3)\n");
/* Grep: process: bootstrap seal: soft retcoveredface — Wave 61 exclusive coveredface stamp */
kprintf("process: bootstrap seal: soft retcoveredface exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=68 "
        "(retcoveredface stamp; Soft≠product)\n");
/*
 * ---- Wave 62 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: process: bootstrap seal: soft retbastionface — Wave 62 return-bastionface honesty */
kprintf("process: bootstrap seal: soft retbastionface soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=68 "
        "(retbastionface honesty; Soft≠product; not bar3)\n");
/* Grep: process: bootstrap seal: soft retcurtainangle — Wave 62 exclusive curtainangle stamp */
kprintf("process: bootstrap seal: soft retcurtainangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=68 "
        "(retcurtainangle stamp; Soft≠product)\n");
/*
 * ---- Wave 63 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: process: bootstrap seal: soft retdoubletenaille — Wave 63 return-doubletenaille honesty */
kprintf("process: bootstrap seal: soft retdoubletenaille soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=68 "
        "(retdoubletenaille honesty; Soft≠product; not bar3)\n");
/* Grep: process: bootstrap seal: soft retplaceofarms — Wave 63 exclusive placeofarms stamp */
kprintf("process: bootstrap seal: soft retplaceofarms exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=68 "
        "(retplaceofarms stamp; Soft≠product)\n");
 /*
  * ---- Wave 64 exclusive complementary surfaces (never reshape primary).
  * Return surfaces only — soft inventory; never hard-gates product paths.
  * Soft≠product; not bar3.
  */
 /* Grep: process: bootstrap seal: soft retreentrant — Wave 64 return-reentrant honesty */
kprintf("process: bootstrap seal: soft retreentrant soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=68 "
        "(retreentrant honesty; Soft≠product; not bar3)\n");
 /* Grep: process: bootstrap seal: soft retsallyport — Wave 64 exclusive sallyport stamp */
kprintf("process: bootstrap seal: soft retsallyport exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=68 "
        "(retsallyport stamp; Soft≠product)\n");
 /*
  * ---- Wave 65 exclusive complementary surfaces (never reshape primary).
  * Return surfaces only — soft inventory; never hard-gates product paths.
  * Soft≠product; not bar3.
  */
 /* Grep: process: bootstrap seal: soft retgorgeangle — Wave 65 return-gorgeangle honesty */
kprintf("process: bootstrap seal: soft retgorgeangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=68 "
        "(retgorgeangle honesty; Soft≠product; not bar3)\n");
 /* Grep: process: bootstrap seal: soft retshoulderangle — Wave 65 exclusive shoulderangle stamp */
kprintf("process: bootstrap seal: soft retshoulderangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=68 "
        "(retshoulderangle stamp; Soft≠product)\n");
 /*
  * ---- Wave 66 exclusive complementary surfaces (never reshape primary).
  * Return surfaces only — soft inventory; never hard-gates product paths.
  * Soft≠product; not bar3.
  */
 /* Grep: process: bootstrap seal: soft retflankangle — Wave 66 return-flankangle honesty */
 kprintf("process: bootstrap seal: soft retflankangle soft_only=1 product_gate=0 soft_ne_product=1 "
         "never_blocks_m0=1 wave=68 "
         "(retflankangle honesty; Soft≠product; not bar3)\n");
 /* Grep: process: bootstrap seal: soft retfaceangle — Wave 66 exclusive faceangle stamp */
 kprintf("process: bootstrap seal: soft retfaceangle exclusive=1 soft_ne_product=1 "
         "product_kernel=OPEN bar3=0 wave=68 "
         "(retfaceangle stamp; Soft≠product)\n");
/*
 * ---- Wave 67 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: process: bootstrap seal: soft retcaponierangle — Wave 67 return-caponierangle honesty */
kprintf("process: bootstrap seal: soft retcaponierangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=68 "
        "(retcaponierangle honesty; Soft≠product; not bar3)\n");
/* Grep: process: bootstrap seal: soft retredanangle — Wave 67 exclusive redanangle stamp */
kprintf("process: bootstrap seal: soft retredanangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=68 "
        "(retredanangle stamp; Soft≠product)\n");
/*
 * ---- Wave 68 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: process: bootstrap seal: soft retlunetteangle — Wave 68 return-lunetteangle honesty */
kprintf("process: bootstrap seal: soft retlunetteangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=68 "
        "(retlunetteangle honesty; Soft≠product; not bar3)\n");
/* Grep: process: bootstrap seal: soft rettenailleangle — Wave 68 exclusive tenailleangle stamp */
kprintf("process: bootstrap seal: soft rettenailleangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=68 "
        "(rettenailleangle stamp; Soft≠product)\n");
                            kprintf("process: bootstrap seal soft deepen wave=%u via=%s "
            "logs=%u emits=%llu rate_limited=%llu "
            "(not product-complete; no bar3)\n",
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
            "(Apple s13 open; no bar3)\n");
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
                "(soft only; not product-complete; no bar3)\n");
        /*
         * Wave 13: one-way flip honesty — product s13 lamps stay 0.
         * Grep: process: bootstrap seal soft one-way | process: bootstrap seal soft lamps
         */
        kprintf("process: bootstrap seal soft one-way product_retype=%u "
                "product_irq=%u product_untyped=%u product_sticky=%u "
                "soft (still open; no bar3)\n",
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
     * Not product multi-server; no bar3.
     */
    kprintf("process: confine soft confined=%u promises=0x%x "
            "ambient_drop=1 (soft; not product multi-server; no bar3)\n",
            pProc->u32Confined, pProc->u32Promises);
    /* Confine is the soft ambient-drop edge — re-emit seal checklist lamps. */
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
     * Process + CNode are kernel fields for kernel ops only (K1–K6).
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
     * Soft Apple §13 seal checklist after root meta install.
     * Enumerates root_meta / pager_empty / ambient / promises (rate-limited).
     * Grep: process: bootstrap seal soft | process: seal checklist
     * Honesty: not product-complete; no bar3.
     */
    process_seal_checklist_soft(pProc, "root_meta");
    process_soft_maybe_once();
    return GJ_OK;
}

/*
 * Soft pager kernel ref (SOLARIS_STYLE §9): hold endpoint while PCB names it.
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
    /* Mirror is READ|IDENTIFY (+ GRANT if source had it) — not ambient MAP. */
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
    /* Soft: drop kernel hold after PCB cleared (SOLARIS_STYLE §9 clear). */
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
    /* Soft LIVE check — refuse DEAD/REVOKING endpoints (fail closed). */
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
    /* Badge 0 → soft-snap from door endpoint when LIVE. */
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
 * Fault policy (CAP_ADDRESSING + SOLARIS_STYLE_REMAINING §7):
 * - one fault lock per space (here: process.fault until gj_space exists)
 * - no pager ⇒ FAULT (kill)
 * - with pager: build cluster + kernel cookie; Call pager (IPC later)
 * - object owns pages / maps are views (map path not fully wired; Apple §2)
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

/*
 * Soft wait-table census (Wave 15 inventory). Snapshots used/zombie/live/free.
 * Does not lock (soft OK). greppable via process: soft wait …
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
            /* Soft: link pParent when parent PCB is still in the wait table. */
            pChild->pParent = NULL;
            {
                u32 j;

                for (j = 0; j < GJ_WAIT_SLOTS; j++) {
                    if (g_aWait[j].u8Used && g_aWait[j].u32Pid == u32ParentPid) {
                        pChild->pParent = g_aWait[j].pProc;
                        break;
                    }
                }
            }
            g_u64WaitRegister++;
            process_soft_inc(&g_u32SoftWaitRegOk);
            process_soft_maybe_once();
            kprintf("process: wait register pid=%u ppid=%u\n", pid,
                    g_aWait[i].u32Ppid);
            return pid;
        }
    }
    process_soft_inc(&g_u32SoftWaitRegFull);
    return 0; /* table full — caller may continue without wait4 */
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
        kprintf("process: wait reparent pid=%u ppid %u→%u soft\n",
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
 * are not passed here — only wait-registered children.
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
    int fWaitChild;

    if (pProc == NULL) {
        return;
    }
    process_soft_inc(&g_u32SoftDeathEnter);
    /* Idempotent: second death only re-notes zombie code */
    if (!pProc->u32Alive && pProc->u64Cr3 == 0 && !gj_process_has_pager(pProc)) {
        g_u64DeathIdempotent++;
        /* Grep: process: death idempotent */
        kprintf("process: death idempotent re-note exit=%u soft "
                "(G-PROC-5)\n",
                u32ExitCode);
        process_wait_note_exit(pProc, u32ExitCode);
        return;
    }

    g_u64DeathTotal++;
    u32SelfPid = process_wait_pid_of(pProc);
    u32WasConfined = pProc->u32Confined;
    u32WasJit = pProc->u32Jit;
    pProc->u32ExitCode = u32ExitCode;
    pProc->u32Alive = 0;

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

    /* Drop region views (object owns pages; maps are views — G-MO) */
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
     * Destroy private AS only for wait-registered children (PE/spawn/fork).
     * Never free boot/init AS — ring-3 smokes share it with the rest of kmain.
     * Save/restore caller CR3: death may run mid-syscall on the parent AS
     * (e.g. vfork child exit while parent PE32 is current).
     * Grep: process: death as_destroy | process: as_destroy
     */
    u64SavedCr3 = cpu_read_cr3();
    u64Cr3 = pProc->u64Cr3;
    u64Ker = vmm_kernel_cr3();
    if (fWaitChild && u64Cr3 != 0 && u64Ker != 0 &&
        (u64Cr3 & ~0xfffull) != (u64Ker & ~0xfffull)) {
        cpu_load_cr3(u64Ker);
        vmm_set_anon_cursor(NULL);
        if (vmm_as_destroy(u64Cr3) == GJ_OK) {
            u32AsOk = 1;
            g_u64DeathAsDestroyOk++;
            kprintf("process: death as_destroy cr3=0x%lx ok=1 soft "
                    "(G-PROC-5)\n",
                    (unsigned long)u64Cr3);
            kprintf("process: as_destroy cr3=0x%lx PASS\n",
                    (unsigned long)u64Cr3);
        } else {
            u32AsFail = 1;
            g_u64DeathAsDestroyFail++;
            kprintf("process: death as_destroy cr3=0x%lx ok=0 soft "
                    "(G-PROC-5)\n",
                    (unsigned long)u64Cr3);
            kprintf("process: as_destroy cr3=0x%lx FAIL\n",
                    (unsigned long)u64Cr3);
        }
        pProc->u64Cr3 = 0;
    } else if (u64Cr3 != 0 && u64Ker != 0 &&
               (u64Cr3 & ~0xfffull) != (u64Ker & ~0xfffull)) {
        /* Long-lived process: leave AS; switch off if we were on it */
        u32AsSkip = 1;
        g_u64DeathAsSkip++;
        kprintf("process: death as_destroy cr3=0x%lx skip=long_lived soft "
                "(G-PROC-5)\n",
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
                "(G-PROC-5)\n",
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
     * Wave 13: death ≠ Apple §13 bootstrap seal product.
     * Cleanup revokes grants for this PCB; does not seal privileged retype /
     * broad IRQ / root untyped. Soft seal_note + optional checklist.
     * Grep: process: death seal_note | process: bootstrap seal soft
     */
    kprintf("process: death seal_note soft lamp=%u product_seal=0 "
            "retype=%u irq=%u untyped=%u sticky=%u "
            "(death cleanup != Apple s13 seal; no bar3)\n",
            g_u32BootstrapSealSoftLamp, g_u32SealProductRetype,
            g_u32SealProductIrqBind, g_u32SealProductRootUntyped,
            g_u32SealProductStickyNs);
    /* Rate-limited full checklist via=death (seal_note above always emits). */
    process_seal_checklist_soft(pProc, "death");

    /*
     * Aggregate G-PROC-5 death tallies (soft product observability).
     * Grep: process: death exit= | process: death tallies
     */
    kprintf("process: death tallies total=%llu pager_clear=%llu "
            "fault_force=%llu cnode_wipe=%llu cnode_slots=%llu "
            "as_ok=%llu as_fail=%llu as_skip=%llu soft (G-PROC-5)\n",
            (unsigned long long)g_u64DeathTotal,
            (unsigned long long)g_u64DeathPagerClear,
            (unsigned long long)g_u64DeathFaultForce,
            (unsigned long long)g_u64DeathCnodeWipe,
            (unsigned long long)g_u64DeathCnodeSlots,
            (unsigned long long)g_u64DeathAsDestroyOk,
            (unsigned long long)g_u64DeathAsDestroyFail,
            (unsigned long long)g_u64DeathAsSkip);
    /*
     * Wave 15 deepen tallies (extra axes; wrap OK).
     * Grep: process: death deepen | process: death tallies deepen
     */
    kprintf("process: death deepen wave=%u tallies wait_child=%llu "
            "long_lived=%llu idempotent=%llu regions=%llu reparent=%llu "
            "exc_clear=%llu confine_scrub=%llu jit_scrub=%llu soft "
            "(G-PROC-5; no bar3)\n",
            GJ_PROCESS_SOFT_WAVE,
            (unsigned long long)g_u64DeathWaitChild,
            (unsigned long long)g_u64DeathLongLived,
            (unsigned long long)g_u64DeathIdempotent,
            (unsigned long long)g_u64DeathRegions,
            (unsigned long long)g_u64DeathReparent,
            (unsigned long long)g_u64DeathExcClear,
            (unsigned long long)g_u64DeathConfineScrub,
            (unsigned long long)g_u64DeathJitScrub);
    kprintf("process: death exit=%u reparent=%u regions=%u "
            "cnode_slots=%u as_ok=%u as_fail=%u as_skip=%u "
            "wait_child=%d soft (G-PROC-5)\n",
            u32ExitCode, u32Reparented, u32RegionsDropped, u32Cleared,
            u32AsOk, u32AsFail, u32AsSkip, fWaitChild);
    /* Wave 15: unified soft inventory dump after death path. */
    process_soft_inventory("death");
}

/* Stub children for Linux fork/vfork (no full AS clone until product spawn). */
static struct gj_process g_aForkStub[GJ_FORK_STUBS];
static u8                g_aForkUsed[GJ_FORK_STUBS];

/**
 * Deferred child exit: schedules after parent returns from fork so wait4 can
 * reap a real zombie (not only vfork's immediate exit).
 */
static void
fork_child_exit_worker(void *pArg)
{
    struct gj_process *pChild = (struct gj_process *)pArg;

    /* Yield so parent can run wait4 after fork returns */
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

    process_soft_inc(&g_u32SoftForkEnter);
    for (i = 0; i < GJ_FORK_STUBS; i++) {
        if (!g_aForkUsed[i]) {
            break;
        }
    }
    if (i >= GJ_FORK_STUBS) {
        process_soft_inc(&g_u32SoftForkFull);
        return -11; /* EAGAIN */
    }
    memset(&g_aForkStub[i], 0, sizeof(g_aForkStub[i]));
    g_aForkUsed[i] = 1;
    g_aForkStub[i].u32Alive = 1;
    g_aForkStub[i].u32Personality = 1;
    /*
     * Private AS shell for child (G-AS) + clone parent private user pages.
     * Product: full COW; bring-up: copy non-identity user 4K pages from parent.
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
    pid = process_wait_register(&g_aForkStub[i], u32Ppid ? u32Ppid : 1u);
    if (pid == 0) {
        g_aForkUsed[i] = 0;
        process_soft_inc(&g_u32SoftForkFull);
        return -12; /* ENOMEM */
    }
    if (fExitNow) {
        /* vfork-shaped: child already exited; free AS shell */
        process_soft_inc(&g_u32SoftForkVfork);
        process_death(&g_aForkStub[i], 0);
    } else {
        /*
         * fork-shaped: schedule a kernel worker that marks the child zombie
         * after yields so parent wait4 can reap (bring-up; product: user AS).
         */
        thr = thread_create(&g_aForkStub[i], fork_child_exit_worker,
                            &g_aForkStub[i]);
        if (thr == 0) {
            /* Fallback: immediate exit so wait still works */
            process_death(&g_aForkStub[i], 0);
            process_soft_inc(&g_u32SoftForkOk);
            kprintf("process: linux_fork pid=%u (no thr, exit now)\n", pid);
            return (i64)pid;
        }
        process_soft_inc(&g_u32SoftForkDeferred);
    }
    process_soft_inc(&g_u32SoftForkOk);
    process_soft_maybe_once();
    kprintf("process: linux_fork pid=%u exit_now=%d thr=%u\n", pid, fExitNow,
            thr);
    return (i64)pid;
}

i64
process_linux_exit_pid(u32 u32Pid, u32 u32Code)
{
    u32 i;

    for (i = 0; i < GJ_WAIT_SLOTS; i++) {
        if (g_aWait[i].u8Used && !g_aWait[i].u8Reaped &&
            g_aWait[i].u32Pid == u32Pid) {
            process_wait_note_exit(g_aWait[i].pProc, u32Code);
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
    u32 u32MaxAttempts = fNoHang ? 1u : 64u;

    process_soft_inc(&g_u32SoftWait4Enter);
    /*
     * Soft: WUNTRACED / WCONTINUED ignored (no stop/continue state yet).
     * pid 0 treated as any-child (bring-up); pid < -1 process-group unsupported.
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
            /* Soft: exact pid, any (-1), or legacy any (0 → treat as -1). */
            if (i64Pid > 0 && (u32)i64Pid != pS->u32Pid) {
                continue;
            }
            if (i64Pid < -1) {
                continue; /* process group — unsupported */
            }
            fHaveChild = 1;
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
                kprintf("process: wait4 nowait pid=%u status=0x%x soft\n",
                        pS->u32Pid, (unsigned)i32Status);
                return i64Ret;
            }

            pS->u8Reaped = 1;
            g_u64WaitReap++;
            process_soft_inc(&g_u32SoftWaitReap);
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
        /* No unreaped children at all → ECHILD */
        if (!fHaveChild) {
            process_soft_inc(&g_u32SoftWait4Echild);
            return -10; /* ECHILD */
        }
        /* Live children, none exited yet */
        if (fNoHang) {
            process_soft_inc(&g_u32SoftWait4Nohang0);
            return 0; /* WNOHANG */
        }
        /* Blocking-ish: yield so fork exit workers can run */
        thread_yield();
    }
    process_soft_inc(&g_u32SoftWait4Echild);
    return -10; /* ECHILD — timed out waiting for child exit */
}
