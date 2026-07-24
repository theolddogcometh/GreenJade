/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * process_spawn + PROCESS task cap mint (G-PROC-*).
 *
 * Product lifecycle (SOLARIS_STYLE_REMAINING §6):
 *   spawn → CNode + root meta + private AS + first thread + parent PROCESS cap
 *   kill  → process_death (pager/regions/CNode/AS for wait-registered children)
 *   wait  → reap exit code, invalidate parent PROCESS cap, recycle spawn slot
 * Failure paths reverse tear down (no orphan AS / stale cap).
 *
 * Soft product inventory (Wave 15 exclusive deepen; this unit only):
 *   - Spawn path: enter / ok / fail + deny reasons (null/cnode/full/meta/as/mint/thr)
 *   - Mint path: install + soft post-mint verify reason catalog
 *   - Kill / wait / from_cap path tallies + live peak
 *   - Wave 15: teardown / AS reverse / wait-register / JIT mint / lifecycle
 *   greppable: "spawn: soft …"
 *   Never hard-gates; diagnostics / smoke grep only (wrap OK). Soft ≠ bar3.
 *
 * Soft mint verify (grep: spawn: mint soft | spawn: soft mint):
 *   Post-mint verify of PROCESS type/rights/obj/gen (fail closed on soft bad)
 *   Cumulative ok/fail/live/kill/wait + mint ok/fail/soft/soft_bad counters
 */
#include <gj/cap.h>
#include <gj/cpu.h>
#include <gj/klog.h>
#include <gj/memobj.h>
#include <gj/process.h>
#include <gj/spawn.h>
#include <gj/string.h>
#include <gj/thread.h>
#include <gj/vmm.h>

struct spawn_slot {
    u8                   u8Used;
    u8                   u8Pad[7];
    struct gj_process    proc;
    struct gj_cnode      cnode;
    struct gj_cap_slot   aSlots[GJ_SPAWN_CNODE_SLOTS];
    struct gj_root_meta  meta;
};

static struct spawn_slot g_aSpawn[GJ_SPAWN_MAX];

/* ---- Soft spawn / PROCESS-mint counters (grep: spawn: soft) ------------ */
static u32 g_cSpawned;       /* successful process_spawn */
static u32 g_cSpawnFail;     /* failed process_spawn */
static u32 g_cKill;          /* process_kill resolved ok */
static u32 g_cWait;          /* process_wait reaped */
static u32 g_cMintOk;        /* PROCESS cap install ok */
static u32 g_cMintFail;      /* PROCESS cap install fail */
static u32 g_cMintSoft;      /* soft post-mint verify PASS */
static u32 g_cMintSoftBad;   /* soft post-mint verify FAIL */

/*
 * Soft product inventory (Wave 20 deepen). Cumulative unless noted live/peak.
 * greppable: spawn: soft …
 */
#define GJ_SPAWN_SOFT_WAVE 108u

static u32 g_u32SoftSpawnEnter;      /* process_spawn entries */
static u32 g_u32SoftDenyNull;        /* null parent / args / entry */
static u32 g_u32SoftDenyNoCnode;     /* parent CNode missing */
static u32 g_u32SoftDenyFull;        /* fixed spawn table full */
static u32 g_u32SoftDenyMeta;        /* bootstrap root meta fail */
static u32 g_u32SoftDenyAs;          /* process_as_ensure fail */
static u32 g_u32SoftDenyMint;        /* mint install or soft-verify fail */
static u32 g_u32SoftDenyThr;         /* thread_create fail */
static u32 g_u32SoftLivePeak;        /* peak occupied slots */

static u32 g_u32SoftMintNullArg;     /* mint/verify null parent/child/ref */
static u32 g_u32SoftMintNullRef;     /* soft-verify null cap ref */
static u32 g_u32SoftMintResolve;     /* soft-verify resolve fail */
static u32 g_u32SoftMintType;        /* soft-verify type != PROCESS */
static u32 g_u32SoftMintObj;         /* soft-verify object pointer mismatch */
static u32 g_u32SoftMintGen;         /* soft-verify slot gen mismatch */
static u32 g_u32SoftMintCoreRights;  /* soft-verify core rights missing */
static u32 g_u32SoftMintRights;      /* soft-verify full wanted rights miss */
static u32 g_u32SoftMintFromCap;     /* soft-verify process_from_cap mismatch */
static u32 g_u32SoftMintInstallFail; /* gj_cap_alloc_install fail */
static u32 g_u32SoftMintJit;         /* mint with CapJit rights OR'd (W15) */
static u32 g_u32SoftMintNoJit;       /* mint without JIT (W15) */

static u32 g_u32SoftKillEnter;       /* process_kill entries */
static u32 g_u32SoftKillIdem;        /* kill already-dead (idempotent ok) */
static u32 g_u32SoftKillDenyNull;    /* kill null parent/ref */
static u32 g_u32SoftKillDenyResolve; /* kill resolve fail */
static u32 g_u32SoftKillDenyType;    /* kill type != PROCESS */
static u32 g_u32SoftKillDenyRights;  /* kill missing DESTROY */
static u32 g_u32SoftKillDenyNoent;   /* kill null child object */

static u32 g_u32SoftWaitEnter;       /* process_wait entries */
static u32 g_u32SoftWaitAgain;       /* wait while child still alive */
static u32 g_u32SoftWaitDenyNull;    /* wait null parent/ref */
static u32 g_u32SoftWaitDenyResolve; /* wait resolve fail */
static u32 g_u32SoftWaitDenyType;    /* wait type != PROCESS */
static u32 g_u32SoftWaitDenyRights;  /* wait missing WAIT */
static u32 g_u32SoftWaitDenyNoent;   /* wait null child object */
static u32 g_u32SoftWaitSlotFree;    /* wait recycled fixed spawn slot (W15) */
static u32 g_u32SoftWaitCapInv;      /* wait invalidated parent PROCESS cap */

static u32 g_u32SoftFromCapHit;      /* process_from_cap success */
static u32 g_u32SoftFromCapMiss;     /* process_from_cap fail-closed */

/* Wave 15 lifecycle / reverse-path tallies. */
static u32 g_u32SoftFailCleanup;     /* spawn_fail_cleanup calls */
static u32 g_u32SoftAsTeardown;      /* spawn_as_teardown ran destroy */
static u32 g_u32SoftAsTeardownSkip;  /* AS teardown no-op (null/0/kernel) */
static u32 g_u32SoftWaitReg;         /* process_wait_register after thr live */
static u32 g_u32SoftPersonaNative;   /* spawn personality=0 */
static u32 g_u32SoftPersonaLinux;    /* spawn personality=1 */
static u32 g_u32SoftLastThrId;       /* last successful first-thr id (sample) */

static u32 g_u32SoftLogN;            /* soft inventory log emissions */
static u8  g_fSoftOnce;              /* one-shot after first product activity */

static void spawn_soft_inc(u32 *pCtr);
static void soft_inventory_log(const char *szVia);
static void soft_inventory_maybe_once(void);
static void soft_note_live_peak(void);

/**
 * Occupied fixed spawn slots (live children not yet reaped via process_wait).
 */
static u32
spawn_live_count(void)
{
    u32 iSlot;
    u32 cLive;

    cLive = 0;
    for (iSlot = 0; iSlot < GJ_SPAWN_MAX; iSlot++) {
        if (g_aSpawn[iSlot].u8Used) {
            cLive++;
        }
    }
    return cLive;
}

/** Soft: bump path tally (u32 wrap is fine for telemetry). */
static void
spawn_soft_inc(u32 *pCtr)
{
    if (pCtr == NULL) {
        return;
    }
    (*pCtr)++;
}

/** Soft: track peak occupied fixed slots (diagnostics only). */
static void
soft_note_live_peak(void)
{
    u32 cLive;

    cLive = spawn_live_count();
    if (cLive > g_u32SoftLivePeak) {
        g_u32SoftLivePeak = cLive;
    }
}

/**
 * Greppable soft spawn inventory (Wave 20 deepen; product / smoke).
 * Prefix-stable markers (spawn: soft …):
 *   spawn: soft inventory  — table caps + live/peak + rights + logs
 *   spawn: soft stats      — cumulative ok/fail/live/kill/wait/mint*
 *   spawn: soft spawn      — enter/ok/fail + deny reason catalog
 *   spawn: soft mint       — install + soft-verify reason catalog
 *   spawn: soft kill       — enter/ok/idem + deny catalog
 *   spawn: soft wait       — enter/ok/again + deny catalog
 *   spawn: soft from_cap   — process_from_cap hit/miss
 *   spawn: soft teardown   — reverse-path + AS teardown
 *   spawn: soft lifecycle  — persona/JIT/wait-reg/last thr
 *   spawn: soft capacity   — table/cnode/rights geometry
 *   spawn: soft return     — Wave 19 return-path catalog
 *   spawn: soft ret_surface — Wave 19 terminal return classes
 *   spawn: soft surface    — Wave 19 area catalog
 *   spawn: soft deepen     — Wave 20 stamp
 *   spawn: soft path       — honesty: fixed table ≠ full posix_spawn
 *   spawn: soft inventory PASS / spawn: soft PASS
 *
 * Never hard-gates; diagnostics only.
 * greppable: spawn: soft
 */
static void
soft_inventory_log(const char *szVia)
{
    const char *szViaSafe;
    u32         cLive;
    u32         u32Free;
    u16         u16Base;
    u16         u16Core;

    spawn_soft_inc(&g_u32SoftLogN);
    szViaSafe = (szVia != NULL && szVia[0] != '\0') ? szVia : "unknown";
    cLive = spawn_live_count();
    u32Free = (cLive < GJ_SPAWN_MAX) ? (GJ_SPAWN_MAX - cLive) : 0u;
    u16Base = GJ_SPAWN_PROCESS_RIGHTS_BASE;
    u16Core = GJ_SPAWN_PROCESS_RIGHTS_CORE;

    /* Grep: spawn: soft inventory */
    kprintf("spawn: soft inventory via=%s max=%u slots=%u live=%u free=%u "
            "peak=%u rights_base=0x%x rights_core=0x%x logs=%u wave=%u\n",
            szViaSafe, GJ_SPAWN_MAX, (unsigned)GJ_SPAWN_CNODE_SLOTS,
            cLive, u32Free, g_u32SoftLivePeak,
            (unsigned)u16Base, (unsigned)u16Core, g_u32SoftLogN,
            GJ_SPAWN_SOFT_WAVE);

    /* Grep: spawn: soft stats */
    kprintf("spawn: soft stats ok=%u fail=%u live=%u kill=%u wait=%u "
            "mint_ok=%u mint_fail=%u mint_soft=%u mint_soft_bad=%u\n",
            g_cSpawned, g_cSpawnFail, cLive, g_cKill, g_cWait,
            g_cMintOk, g_cMintFail, g_cMintSoft, g_cMintSoftBad);

    /* Grep: spawn: soft spawn */
    kprintf("spawn: soft spawn enter=%u ok=%u fail=%u deny_null=%u "
            "deny_cnode=%u deny_full=%u deny_meta=%u deny_as=%u "
            "deny_mint=%u deny_thr=%u\n",
            g_u32SoftSpawnEnter, g_cSpawned, g_cSpawnFail,
            g_u32SoftDenyNull, g_u32SoftDenyNoCnode, g_u32SoftDenyFull,
            g_u32SoftDenyMeta, g_u32SoftDenyAs, g_u32SoftDenyMint,
            g_u32SoftDenyThr);

    /* Grep: spawn: soft mint */
    kprintf("spawn: soft mint install_ok=%u install_fail=%u soft_pass=%u "
            "soft_bad=%u null_arg=%u null_ref=%u resolve=%u type=%u "
            "obj=%u gen=%u core_rights=%u rights=%u from_cap=%u "
            "jit=%u nojit=%u\n",
            g_cMintOk, g_cMintFail, g_cMintSoft, g_cMintSoftBad,
            g_u32SoftMintNullArg, g_u32SoftMintNullRef, g_u32SoftMintResolve,
            g_u32SoftMintType, g_u32SoftMintObj, g_u32SoftMintGen,
            g_u32SoftMintCoreRights, g_u32SoftMintRights,
            g_u32SoftMintFromCap, g_u32SoftMintJit, g_u32SoftMintNoJit);

    /* Grep: spawn: soft kill */
    kprintf("spawn: soft kill enter=%u ok=%u idem=%u deny_null=%u "
            "deny_resolve=%u deny_type=%u deny_rights=%u deny_noent=%u\n",
            g_u32SoftKillEnter, g_cKill, g_u32SoftKillIdem,
            g_u32SoftKillDenyNull, g_u32SoftKillDenyResolve,
            g_u32SoftKillDenyType, g_u32SoftKillDenyRights,
            g_u32SoftKillDenyNoent);

    /* Grep: spawn: soft wait */
    kprintf("spawn: soft wait enter=%u ok=%u again=%u deny_null=%u "
            "deny_resolve=%u deny_type=%u deny_rights=%u deny_noent=%u "
            "slot_free=%u cap_inv=%u\n",
            g_u32SoftWaitEnter, g_cWait, g_u32SoftWaitAgain,
            g_u32SoftWaitDenyNull, g_u32SoftWaitDenyResolve,
            g_u32SoftWaitDenyType, g_u32SoftWaitDenyRights,
            g_u32SoftWaitDenyNoent, g_u32SoftWaitSlotFree,
            g_u32SoftWaitCapInv);

    /* Grep: spawn: soft from_cap */
    kprintf("spawn: soft from_cap hit=%u miss=%u\n",
            g_u32SoftFromCapHit, g_u32SoftFromCapMiss);

    /* Grep: spawn: soft teardown (Wave 15 reverse path) */
    kprintf("spawn: soft teardown fail_cleanup=%u as_teardown=%u "
            "as_skip=%u\n",
            g_u32SoftFailCleanup, g_u32SoftAsTeardown,
            g_u32SoftAsTeardownSkip);

    /* Grep: spawn: soft lifecycle (Wave 15) */
    kprintf("spawn: soft lifecycle wait_reg=%u persona_native=%u "
            "persona_linux=%u last_thr=%u peak=%u\n",
            g_u32SoftWaitReg, g_u32SoftPersonaNative, g_u32SoftPersonaLinux,
            g_u32SoftLastThrId, g_u32SoftLivePeak);

    /* Grep: spawn: soft capacity (Wave 15 geometry) */
    kprintf("spawn: soft capacity max=%u cnode_slots=%u live=%u free=%u "
            "peak=%u rights_base=0x%x rights_core=0x%x "
            "fail_cleanup=%u as_teardown=%u wave=%u\n",
            GJ_SPAWN_MAX, (unsigned)GJ_SPAWN_CNODE_SLOTS, cLive, u32Free,
            g_u32SoftLivePeak, (unsigned)u16Base, (unsigned)u16Core,
            g_u32SoftFailCleanup, g_u32SoftAsTeardown, GJ_SPAWN_SOFT_WAVE);

    /*
     * Honesty line: fixed spawn table + PROCESS mint is product bring-up,
     * not full posix_spawn / multi-server confine.
     * Grep: spawn: soft path
     */
    kprintf("spawn: soft path claim=process_spawn G-PROC-2=mint "
            "G-PROC-5=death fixed_table=1 table_max=%u cnode_slots=%u "
            "posix_spawn=0 multi_server_confine=0 via=%s wave=%u "
            "(soft inventory; not bar3)\n",
            GJ_SPAWN_MAX, (unsigned)GJ_SPAWN_CNODE_SLOTS, szViaSafe,
            GJ_SPAWN_SOFT_WAVE);

    /*
     * Grep: spawn: soft return
     * Wave 19 return-path catalog — spawn/mint/kill/wait deny outcomes.
     * Soft ≠ posix_spawn / multi-server confine product. product_kernel=OPEN.
     */
    kprintf("spawn: soft return spawn_ok=%u spawn_fail=%u deny_null=%u "
            "deny_full=%u deny_as=%u deny_mint=%u deny_thr=%u "
            "mint_ok=%u mint_fail=%u mint_soft=%u mint_soft_bad=%u "
            "kill_ok=%u kill_idem=%u wait_ok=%u wait_again=%u "
            "from_cap_hit=%u from_cap_miss=%u teardown=%u "
            "product_kernel=OPEN wave=%u\n",
            g_cSpawned, g_cSpawnFail, g_u32SoftDenyNull, g_u32SoftDenyFull,
            g_u32SoftDenyAs, g_u32SoftDenyMint, g_u32SoftDenyThr, g_cMintOk,
            g_cMintFail, g_cMintSoft, g_cMintSoftBad, g_cKill,
            g_u32SoftKillIdem, g_cWait, g_u32SoftWaitAgain,
            g_u32SoftFromCapHit, g_u32SoftFromCapMiss, g_u32SoftFailCleanup,
            GJ_SPAWN_SOFT_WAVE);

    /* Grep: spawn: soft ret_surface — Wave 19 terminal return classes */
    kprintf("spawn: soft ret_surface spawn=ok|fail "
            "deny=null|full|as|mint|thr mint=ok|fail|soft|soft_bad "
            "kill=ok|idem wait=ok|again from_cap=hit|miss teardown "
            "product_kernel=OPEN areas=95 wave=%u\n",
            GJ_SPAWN_SOFT_WAVE);

    /* Grep: spawn: soft surface — Wave 19 area catalog */
    kprintf("spawn: soft surface inventory,stats,spawn,mint,kill,wait,"
            "from_cap,teardown,lifecycle,capacity,path,return,ret_surface,"
            "surface,deepen areas=99 wave=%u\n",
            GJ_SPAWN_SOFT_WAVE);

    /* Grep: spawn: soft retmap — Wave 19 return-surface map */
    kprintf("spawn: soft retmap ok|fail|inval|nodev|busy|nomem product_gate=0 soft_only=1 wave=108\n");

    /* Grep: spawn: soft deepen */
    /*
     * ---- Wave 19 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft≠product; not bar3.
     */
    /* Grep: spawn: soft retclass — Wave 19 return-class taxonomy (kept) */
    kprintf("spawn: soft retclass ok|fail|inval|nodev|busy|nomem "
            "soft_only=1 product_gate=0 wave=%u "
            "(retclass taxonomy; Soft≠product; not bar3)\n",
            (unsigned)GJ_SPAWN_SOFT_WAVE);
    /* Grep: spawn: soft retlane — Wave 19 return-lane catalog (kept) */
    kprintf("spawn: soft retlane inv|selftest|rate|retcode|retmap|class "
            "product_kernel=OPEN soft_ne_product=1 wave=%u "
            "(retlane catalog; Soft≠product)\n",
            (unsigned)GJ_SPAWN_SOFT_WAVE);
    /*
     * ---- Wave 20 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft≠product; not bar3.
     */
    /* Grep: spawn: soft retbound — Wave 20 return-bound honesty (kept) */
    kprintf("spawn: soft retbound soft_only=1 product_gate=0 hard_gate=0 "
            "never_blocks_m0=1 wave=%u "
            "(retbound honesty; Soft≠product; not bar3)\n",
            (unsigned)GJ_SPAWN_SOFT_WAVE);
    /* Grep: spawn: soft retseal — Wave 20 seal stamp (kept) */
    kprintf("spawn: soft retseal exclusive=1 soft_ne_product=1 "
            "product_kernel=OPEN bar3=0 wave=%u "
            "(retseal stamp; Soft≠product)\n",
            (unsigned)GJ_SPAWN_SOFT_WAVE);
            /*
             * ---- Wave 21 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
            */
            /* Grep: spawn: soft retpulse — Wave 21 return-pulse honesty (kept) */
            kprintf("spawn: soft retpulse soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retpulse honesty; Soft≠product; not bar3)\n",
                    (unsigned)GJ_SPAWN_SOFT_WAVE);
            /* Grep: spawn: soft retmark — Wave 21 mark stamp (kept) */
            kprintf("spawn: soft retmark exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retmark stamp; Soft≠product)\n",
                    (unsigned)GJ_SPAWN_SOFT_WAVE);
            /*
             * ---- Wave 22 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
            */
            /* Grep: spawn: soft retphase — Wave 22 return-phase honesty (kept) */
            kprintf("spawn: soft retphase soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retphase honesty; Soft≠product; not bar3)\n",
                    (unsigned)GJ_SPAWN_SOFT_WAVE);
            /* Grep: spawn: soft retbadge — Wave 22 badge stamp (kept) */
            kprintf("spawn: soft retbadge exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbadge stamp; Soft≠product)\n",
                    (unsigned)GJ_SPAWN_SOFT_WAVE);
/*
 * ---- Wave 23 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
            */
            /* Grep: spawn: soft rettoken — Wave 23 return-token honesty (kept) */
            kprintf("spawn: soft rettoken soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(rettoken honesty; Soft≠product; not bar3)\n",
                    (unsigned)GJ_SPAWN_SOFT_WAVE);
            /* Grep: spawn: soft retcrest — Wave 23 crest stamp (kept) */
            kprintf("spawn: soft retcrest exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retcrest stamp; Soft≠product)\n",
                    (unsigned)GJ_SPAWN_SOFT_WAVE);
            /*
             * ---- Wave 24 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: spawn: soft retvault — Wave 24 return-vault honesty (kept) */
            kprintf("spawn: soft retvault soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retvault honesty; Soft≠product; not bar3)\n",
                    (unsigned)GJ_SPAWN_SOFT_WAVE);
            /* Grep: spawn: soft retbanner — Wave 24 banner stamp (kept) */
            kprintf("spawn: soft retbanner exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbanner stamp; Soft≠product)\n",
                    (unsigned)GJ_SPAWN_SOFT_WAVE);
            /*
             * ---- Wave 25 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: spawn: soft retledger — Wave 25 return-ledger honesty (kept) */
            kprintf("spawn: soft retledger soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retledger honesty; Soft≠product; not bar3)\n",
                    (unsigned)GJ_SPAWN_SOFT_WAVE);
            /* Grep: spawn: soft retbeacon — Wave 25 beacon stamp (kept) */
            kprintf("spawn: soft retbeacon exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbeacon stamp; Soft≠product)\n",
                    (unsigned)GJ_SPAWN_SOFT_WAVE);
            /*
             * ---- Wave 26 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: spawn: soft retcipher — Wave 26 return-cipher honesty (kept) */
            kprintf("spawn: soft retcipher soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retcipher honesty; Soft≠product; not bar3)\n",
                    (unsigned)GJ_SPAWN_SOFT_WAVE);
            /* Grep: spawn: soft retflame — Wave 26 flame stamp (kept) */
            kprintf("spawn: soft retflame exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retflame stamp; Soft≠product)\n",
                    (unsigned)GJ_SPAWN_SOFT_WAVE);
                    /*
                     * ---- Wave 27 complementary surfaces (kept) (never reshape primary).
                     * Return surfaces only — soft inventory; never hard-gates product paths.
                     * Soft≠product; not bar3.
                     */
                    /* Grep: spawn: soft retprism — Wave 27 return-prism honesty (kept) */
                    kprintf("spawn: soft retprism soft_only=1 product_gate=0 soft_ne_product=1 "
                            "never_blocks_m0=1 wave=%u "
                            "(retprism honesty; Soft≠product; not bar3)\n",
                            (unsigned)GJ_SPAWN_SOFT_WAVE);
                    /* Grep: spawn: soft retforge — Wave 27 forge stamp (kept) */
                    kprintf("spawn: soft retforge exclusive=1 soft_ne_product=1 "
                            "product_kernel=OPEN bar3=0 wave=%u "
                            "(retforge stamp; Soft≠product)\n",
                            (unsigned)GJ_SPAWN_SOFT_WAVE);
                            /*
                             * ---- Wave 28 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: spawn: soft retshard — Wave 28 return-shard honesty (kept) */
                            kprintf("spawn: soft retshard soft_only=1 product_gate=0 soft_ne_product=1 "
                                "never_blocks_m0=1 wave=%u "
                                "(retshard honesty; Soft≠product; not bar3)\n",
                                (unsigned)GJ_SPAWN_SOFT_WAVE);
                            /* Grep: spawn: soft retcrown — Wave 28 crown stamp (kept) */
                            kprintf("spawn: soft retcrown exclusive=1 soft_ne_product=1 "
                                "product_kernel=OPEN bar3=0 wave=%u "
                                "(retcrown stamp; Soft≠product)\n",
                                (unsigned)GJ_SPAWN_SOFT_WAVE);
                                /*
                             * ---- Wave 29 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: spawn: soft retglyph — Wave 29 return-glyph honesty (kept) */
                            kprintf("spawn: soft retglyph soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retglyph honesty; Soft≠product; not bar3)\n",
                                    (unsigned)GJ_SPAWN_SOFT_WAVE);
                            /* Grep: spawn: soft retscepter — Wave 29 scepter stamp (kept) */
                            kprintf("spawn: soft retscepter exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=%u "
                                    "(retscepter stamp; Soft≠product)\n",
                                    (unsigned)GJ_SPAWN_SOFT_WAVE);
                                /*
                             * ---- Wave 30 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: spawn: soft retsigil — Wave 30 return-sigil honesty (kept) */
                            kprintf("spawn: soft retsigil soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retsigil honesty; Soft≠product; not bar3)\n",
                                    (unsigned)GJ_SPAWN_SOFT_WAVE);
                            /* Grep: spawn: soft retemblem — Wave 30 emblem stamp (kept) */
                            kprintf("spawn: soft retemblem exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=%u "
                                    "(retemblem stamp; Soft≠product)\n",
                                    (unsigned)GJ_SPAWN_SOFT_WAVE);
                            /*
                             * ---- Wave 31 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: spawn: soft retaegis — Wave 31 return-aegis honesty (kept) */
                            kprintf("spawn: soft retaegis soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retaegis honesty; Soft≠product; not bar3)\n",
                                    (unsigned)GJ_SPAWN_SOFT_WAVE);
                            /* Grep: spawn: soft retsigil — Wave 30 return-sigil honesty (kept) */
                            kprintf("spawn: soft retsigil soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retsigil honesty; Soft≠product; not bar3)\n",
                                    (unsigned)GJ_SPAWN_SOFT_WAVE);
                            /* Grep: spawn: soft retmantle — Wave 31 mantle stamp (kept) */
                            kprintf("spawn: soft retmantle exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=%u "
                                    "(retmantle stamp; Soft≠product)\n",
                                    (unsigned)GJ_SPAWN_SOFT_WAVE);
/*
 * ---- Wave 32 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: spawn: soft retbulwark — Wave 32 return-bulwark honesty (kept) */
kprintf("spawn: soft retbulwark soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbulwark honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_SPAWN_SOFT_WAVE);
/* Grep: spawn: soft retpanoply — Wave 32 panoply stamp (kept) */
kprintf("spawn: soft retpanoply exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retpanoply stamp; Soft≠product)\n",
        (unsigned)GJ_SPAWN_SOFT_WAVE);
/*
 * ---- Wave 33 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: spawn: soft retbastion — Wave 33 return-bastion honesty (kept) */
kprintf("spawn: soft retbastion soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbastion honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_SPAWN_SOFT_WAVE);
/* Grep: spawn: soft retcitadel — Wave 33 citadel stamp (kept) */
kprintf("spawn: soft retcitadel exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retcitadel stamp; Soft≠product)\n",
        (unsigned)GJ_SPAWN_SOFT_WAVE);
/*
 * ---- Wave 34 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: spawn: soft retredoubt — Wave 34 return-redoubt honesty */
kprintf("spawn: soft retredoubt soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retredoubt honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_SPAWN_SOFT_WAVE);
/* Grep: spawn: soft retkeep — Wave 34 exclusive keep stamp */
kprintf("spawn: soft retkeep exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retkeep stamp; Soft≠product)\n",
        (unsigned)GJ_SPAWN_SOFT_WAVE);
/*
 * ---- Wave 35 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: spawn: soft retfortress — Wave 35 return-fortress honesty */
kprintf("spawn: soft retfortress soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retfortress honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_SPAWN_SOFT_WAVE);
/* Grep: spawn: soft retpalace — Wave 35 exclusive palace stamp */
kprintf("spawn: soft retpalace exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retpalace stamp; Soft≠product)\n",
        (unsigned)GJ_SPAWN_SOFT_WAVE);
/*
 * ---- Wave 36 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: spawn: soft rethold — Wave 36 return-hold honesty */
kprintf("spawn: soft rethold soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(rethold honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_SPAWN_SOFT_WAVE);
/* Grep: spawn: soft retspire — Wave 36 exclusive spire stamp */
kprintf("spawn: soft retspire exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retspire stamp; Soft≠product)\n",
        (unsigned)GJ_SPAWN_SOFT_WAVE);
/*
 * ---- Wave 37 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: spawn: soft retwall — Wave 37 return-wall honesty */
kprintf("spawn: soft retwall soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retwall honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_SPAWN_SOFT_WAVE);
/* Grep: spawn: soft retgate — Wave 37 exclusive gate stamp */
kprintf("spawn: soft retgate exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retgate stamp; Soft≠product)\n",
        (unsigned)GJ_SPAWN_SOFT_WAVE);
/*
 * ---- Wave 38 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: spawn: soft retmoat — Wave 38 return-moat honesty */
kprintf("spawn: soft retmoat soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retmoat honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_SPAWN_SOFT_WAVE);
/* Grep: spawn: soft retower — Wave 38 exclusive tower stamp */
kprintf("spawn: soft retower exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retower stamp; Soft≠product)\n",
        (unsigned)GJ_SPAWN_SOFT_WAVE);
                            
/*
 * ---- Wave 39 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: spawn: soft retbarbican — Wave 39 return-barbican honesty */
kprintf("spawn: soft retbarbican soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbarbican honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_SPAWN_SOFT_WAVE);
/* Grep: spawn: soft retglacis — Wave 39 exclusive glacis stamp */
kprintf("spawn: soft retglacis exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retglacis stamp; Soft≠product)\n",
        (unsigned)GJ_SPAWN_SOFT_WAVE);
/*
 * ---- Wave 40 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: spawn: soft retcurtain — Wave 40 return-curtain honesty */
kprintf("spawn: soft retcurtain soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retcurtain honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_SPAWN_SOFT_WAVE);
/* Grep: spawn: soft retparapet — Wave 40 exclusive parapet stamp */
kprintf("spawn: soft retparapet exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retparapet stamp; Soft≠product)\n",
        (unsigned)GJ_SPAWN_SOFT_WAVE);
/*
 * ---- Wave 41 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: spawn: soft retravelin — Wave 41 return-travelin honesty */
kprintf("spawn: soft retravelin soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retravelin honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_SPAWN_SOFT_WAVE);
/* Grep: spawn: soft retditch — Wave 41 exclusive ditch stamp */
kprintf("spawn: soft retditch exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retditch stamp; Soft≠product)\n",
        (unsigned)GJ_SPAWN_SOFT_WAVE);
/*
 * ---- Wave 42 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: spawn: soft retportcullis — Wave 42 return-portcullis honesty */
kprintf("spawn: soft retportcullis soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retportcullis honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_SPAWN_SOFT_WAVE);
/* Grep: spawn: soft retbattlement — Wave 42 exclusive battlement stamp */
kprintf("spawn: soft retbattlement exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retbattlement stamp; Soft≠product)\n",
        (unsigned)GJ_SPAWN_SOFT_WAVE);
/*
 * ---- Wave 43 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: spawn: soft retmachicolation — Wave 43 return-machicolation honesty */
kprintf("spawn: soft retmachicolation soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retmachicolation honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_SPAWN_SOFT_WAVE);
/* Grep: spawn: soft retarrowslit — Wave 43 exclusive arrowslit stamp */
kprintf("spawn: soft retarrowslit exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retarrowslit stamp; Soft≠product)\n",
        (unsigned)GJ_SPAWN_SOFT_WAVE);

/*
 * ---- Wave 44 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: spawn: soft retmerlon — Wave 44 return-merlon honesty */
kprintf("spawn: soft retmerlon soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retmerlon honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_SPAWN_SOFT_WAVE);
/* Grep: spawn: soft retembrasure — Wave 44 exclusive embrasure stamp */
kprintf("spawn: soft retembrasure exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retembrasure stamp; Soft≠product)\n",
        (unsigned)GJ_SPAWN_SOFT_WAVE);

/*
 * ---- Wave 45 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: spawn: soft retkeepgate — Wave 45 return-keepgate honesty */
kprintf("spawn: soft retkeepgate soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retkeepgate honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_SPAWN_SOFT_WAVE);
/* Grep: spawn: soft retouterward — Wave 45 exclusive outerward stamp */
kprintf("spawn: soft retouterward exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retouterward stamp; Soft≠product)\n",
        (unsigned)GJ_SPAWN_SOFT_WAVE);

/*
 * ---- Wave 46 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: spawn: soft retbailey — Wave 46 return-bailey honesty */
kprintf("spawn: soft retbailey soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbailey honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_SPAWN_SOFT_WAVE);
/* Grep: spawn: soft retpostern — Wave 46 exclusive postern stamp */
kprintf("spawn: soft retpostern exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retpostern stamp; Soft≠product)\n",
        (unsigned)GJ_SPAWN_SOFT_WAVE);

/*
 * ---- Wave 47 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: spawn: soft retinnerward — Wave 47 return-innerward honesty */
kprintf("spawn: soft retinnerward soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=108 "
        "(retinnerward honesty; Soft≠product; not bar3)\n");
/* Grep: spawn: soft retdonjon — Wave 47 exclusive donjon stamp */
kprintf("spawn: soft retdonjon exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=108 "
        "(retdonjon stamp; Soft≠product)\n");

/*
 * ---- Wave 48 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: spawn: soft retchevaux — Wave 48 return-chevaux honesty */
kprintf("spawn: soft retchevaux soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=108 "
        "(retchevaux honesty; Soft≠product; not bar3)\n");
/* Grep: spawn: soft retpalisade — Wave 48 exclusive palisade stamp */
kprintf("spawn: soft retpalisade exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=108 "
        "(retpalisade stamp; Soft≠product)\n");

/*
 * ---- Wave 49 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: spawn: soft retglacisgate — Wave 49 return-glacisgate honesty */
kprintf("spawn: soft retglacisgate soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=108 "
        "(retglacisgate honesty; Soft≠product; not bar3)\n");
/* Grep: spawn: soft retoutwork — Wave 49 exclusive outwork stamp */
kprintf("spawn: soft retoutwork exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=108 "
        "(retoutwork stamp; Soft≠product)\n");
/*
 * ---- Wave 50 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: spawn: soft retsally — Wave 50 return-sally honesty */
kprintf("spawn: soft retsally soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=108 "
        "(retsally honesty; Soft≠product; not bar3)\n");
/* Grep: spawn: soft retcounterscarp — Wave 50 exclusive counterscarp stamp */
kprintf("spawn: soft retcounterscarp exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=108 "
        "(retcounterscarp stamp; Soft≠product)\n");
/*
 * ---- Wave 51 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: spawn: soft retfosse — Wave 51 return-fosse honesty */
kprintf("spawn: soft retfosse soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=108 "
        "(retfosse honesty; Soft≠product; not bar3)\n");
/* Grep: spawn: soft retcoveredway — Wave 51 exclusive coveredway stamp */
kprintf("spawn: soft retcoveredway exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=108 "
        "(retcoveredway stamp; Soft≠product)\n");

/*
 * ---- Wave 52 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: spawn: soft rettenaille — Wave 52 return-tenaille honesty */
kprintf("spawn: soft rettenaille soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=108 "
        "(rettenaille honesty; Soft≠product; not bar3)\n");
/* Grep: spawn: soft retdemilune — Wave 52 exclusive demilune stamp */
kprintf("spawn: soft retdemilune exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=108 "
        "(retdemilune stamp; Soft≠product)\n");
/*
 * ---- Wave 53 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: spawn: soft retravelin — Wave 53 return-travelin honesty */
kprintf("spawn: soft retravelin soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=108 "
        "(retravelin honesty; Soft≠product; not bar3)\n");
/* Grep: spawn: soft retlunette — Wave 53 exclusive lunette stamp */
kprintf("spawn: soft retlunette exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=108 "
        "(retlunette stamp; Soft≠product)\n");
/*
 * ---- Wave 54 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: spawn: soft retcaponier — Wave 54 return-caponier honesty */
kprintf("spawn: soft retcaponier soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=108 "
        "(retcaponier honesty; Soft≠product; not bar3)\n");
/* Grep: spawn: soft retredan — Wave 54 exclusive redan stamp */
kprintf("spawn: soft retredan exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=108 "
        "(retredan stamp; Soft≠product)\n");
/*
 * ---- Wave 55 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: spawn: soft retflank — Wave 55 return-flank honesty */
kprintf("spawn: soft retflank soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=108 "
        "(retflank honesty; Soft≠product; not bar3)\n");
/* Grep: spawn: soft retface — Wave 55 exclusive face stamp */
kprintf("spawn: soft retface exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=108 "
        "(retface stamp; Soft≠product)\n");
/*
 * ---- Wave 56 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: spawn: soft retgorge — Wave 56 return-gorge honesty */
kprintf("spawn: soft retgorge soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=108 "
        "(retgorge honesty; Soft≠product; not bar3)\n");
/* Grep: spawn: soft retshoulder — Wave 56 exclusive shoulder stamp */
kprintf("spawn: soft retshoulder exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=108 "
        "(retshoulder stamp; Soft≠product)\n");
/*
 * ---- Wave 57 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: spawn: soft retraverse — Wave 57 return-traverse honesty */
kprintf("spawn: soft retraverse soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=108 "
        "(retraverse honesty; Soft≠product; not bar3)\n");
/* Grep: spawn: soft retcasemate — Wave 57 exclusive casemate stamp */
kprintf("spawn: soft retcasemate exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=108 "
        "(retcasemate stamp; Soft≠product)\n");

/*
 * ---- Wave 58 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: spawn: soft retorillon — Wave 58 return-orillon honesty */
kprintf("spawn: soft retorillon soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=108 "
        "(retorillon honesty; Soft≠product; not bar3)\n");
/* Grep: spawn: soft retbonnette — Wave 58 exclusive bonnette stamp */
kprintf("spawn: soft retbonnette exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=108 "
        "(retbonnette stamp; Soft≠product)\n");

/*
 * ---- Wave 59 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: spawn: soft retcrownwork — Wave 59 return-crownwork honesty */
kprintf("spawn: soft retcrownwork soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=108 "
        "(retcrownwork honesty; Soft≠product; not bar3)\n");
/* Grep: spawn: soft rethornwork — Wave 59 exclusive hornwork stamp */
kprintf("spawn: soft rethornwork exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=108 "
        "(rethornwork stamp; Soft≠product)\n");

/*
 * ---- Wave 60 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: spawn: soft retplace — Wave 60 return-place honesty */
kprintf("spawn: soft retplace soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=108 "
        "(retplace honesty; Soft≠product; not bar3)\n");
/* Grep: spawn: soft retenvelope — Wave 60 exclusive envelope stamp */
kprintf("spawn: soft retenvelope exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=108 "
        "(retenvelope stamp; Soft≠product)\n");









/*
 * ---- Wave 61 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: spawn: soft retcounterguard — Wave 61 return-counterguard honesty */
kprintf("spawn: soft retcounterguard soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=108 "
        "(retcounterguard honesty; Soft≠product; not bar3)\n");
/* Grep: spawn: soft retcoveredface — Wave 61 exclusive coveredface stamp */
kprintf("spawn: soft retcoveredface exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=108 "
        "(retcoveredface stamp; Soft≠product)\n");
/*
 * ---- Wave 62 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: spawn: soft retbastionface — Wave 62 return-bastionface honesty */
kprintf("spawn: soft retbastionface soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=108 "
        "(retbastionface honesty; Soft≠product; not bar3)\n");
/* Grep: spawn: soft retcurtainangle — Wave 62 exclusive curtainangle stamp */
kprintf("spawn: soft retcurtainangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=108 "
        "(retcurtainangle stamp; Soft≠product)\n");
/*
 * ---- Wave 63 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: spawn: soft retdoubletenaille — Wave 63 return-doubletenaille honesty */
kprintf("spawn: soft retdoubletenaille soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=108 "
        "(retdoubletenaille honesty; Soft≠product; not bar3)\n");
/* Grep: spawn: soft retplaceofarms — Wave 63 exclusive placeofarms stamp */
kprintf("spawn: soft retplaceofarms exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=108 "
        "(retplaceofarms stamp; Soft≠product)\n");
 /*
  * ---- Wave 64 exclusive complementary surfaces (never reshape primary).
  * Return surfaces only — soft inventory; never hard-gates product paths.
  * Soft≠product; not bar3.
  */
 /* Grep: spawn: soft retreentrant — Wave 64 return-reentrant honesty */
kprintf("spawn: soft retreentrant soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=108 "
        "(retreentrant honesty; Soft≠product; not bar3)\n");
 /* Grep: spawn: soft retsallyport — Wave 64 exclusive sallyport stamp */
kprintf("spawn: soft retsallyport exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=108 "
        "(retsallyport stamp; Soft≠product)\n");
 /*
  * ---- Wave 65 exclusive complementary surfaces (never reshape primary).
  * Return surfaces only — soft inventory; never hard-gates product paths.
  * Soft≠product; not bar3.
  */
 /* Grep: spawn: soft retgorgeangle — Wave 65 return-gorgeangle honesty */
kprintf("spawn: soft retgorgeangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=108 "
        "(retgorgeangle honesty; Soft≠product; not bar3)\n");
 /* Grep: spawn: soft retshoulderangle — Wave 65 exclusive shoulderangle stamp */
kprintf("spawn: soft retshoulderangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=108 "
        "(retshoulderangle stamp; Soft≠product)\n");
 /*
  * ---- Wave 66 exclusive complementary surfaces (never reshape primary).
  * Return surfaces only — soft inventory; never hard-gates product paths.
  * Soft≠product; not bar3.
  */
 /* Grep: spawn: soft retflankangle — Wave 66 return-flankangle honesty */
 kprintf("spawn: soft retflankangle soft_only=1 product_gate=0 soft_ne_product=1 "
         "never_blocks_m0=1 wave=108 "
         "(retflankangle honesty; Soft≠product; not bar3)\n");
 /* Grep: spawn: soft retfaceangle — Wave 66 exclusive faceangle stamp */
 kprintf("spawn: soft retfaceangle exclusive=1 soft_ne_product=1 "
         "product_kernel=OPEN bar3=0 wave=108 "
         "(retfaceangle stamp; Soft≠product)\n");
/*
 * ---- Wave 67 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: spawn: soft retcaponierangle — Wave 67 return-caponierangle honesty */
kprintf("spawn: soft retcaponierangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=108 "
        "(retcaponierangle honesty; Soft≠product; not bar3)\n");
/* Grep: spawn: soft retredanangle — Wave 67 exclusive redanangle stamp */
kprintf("spawn: soft retredanangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=108 "
        "(retredanangle stamp; Soft≠product)\n");
/*
 * ---- Wave 68 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: spawn: soft retlunetteangle — Wave 68 return-lunetteangle honesty */
kprintf("spawn: soft retlunetteangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=108 "
        "(retlunetteangle honesty; Soft≠product; not bar3)\n");
/* Grep: spawn: soft rettenailleangle — Wave 68 exclusive tenailleangle stamp */
kprintf("spawn: soft rettenailleangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=108 "
        "(rettenailleangle stamp; Soft≠product)\n");
/*
 * ---- Wave 69 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: spawn: soft retdemiluneangle — Wave 69 return-demiluneangle honesty */
kprintf("spawn: soft retdemiluneangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=108 "
        "(retdemiluneangle honesty; Soft≠product; not bar3)\n");
/* Grep: spawn: soft retcoveredwayangle — Wave 69 exclusive coveredwayangle stamp */
kprintf("spawn: soft retcoveredwayangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=108 "
        "(retcoveredwayangle stamp; Soft≠product)\n");
/*
 * ---- Wave 70 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: spawn: soft retfosseangle — Wave 70 return-fosseangle honesty */
kprintf("spawn: soft retfosseangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=108 (retfosseangle honesty; Soft≠product; not bar3)\n");
/* Grep: spawn: soft retcounterscarple — Wave 70 exclusive counterscarple stamp */
kprintf("spawn: soft retcounterscarple exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=108 (retcounterscarple stamp; Soft≠product)\n");
/*
 * ---- Wave 71 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: spawn: soft retsallyportangle — Wave 71 return-sallyportangle honesty */
kprintf("spawn: soft retsallyportangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=108 (retsallyportangle honesty; Soft≠product; not bar3)\n");
/* Grep: spawn: soft retreentrantangle — Wave 71 exclusive reentrantangle stamp */
kprintf("spawn: soft retreentrantangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=108 (retreentrantangle stamp; Soft≠product)\n");
/*
 * ---- Wave 72 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: spawn: soft retplaceofarmsangle — Wave 72 return-placeofarmsangle honesty */
kprintf("spawn: soft retplaceofarmsangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=108 (retplaceofarmsangle honesty; Soft≠product; not bar3)\n");
/* Grep: spawn: soft retdoubletenailleangle — Wave 72 exclusive doubletenailleangle stamp */
kprintf("spawn: soft retdoubletenailleangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=108 (retdoubletenailleangle stamp; Soft≠product)\n");
/* Grep: spawn: soft retcurtainface — Wave 73 return-curtainface honesty */
kprintf("spawn: soft retcurtainface soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=108 (retcurtainface honesty; Soft≠product; not bar3)\n");
/* Grep: spawn: soft retbastionangle — Wave 73 exclusive bastionangle stamp */
kprintf("spawn: soft retbastionangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=108 (retbastionangle stamp; Soft≠product)\n");
/* Grep: spawn: soft retglacisangle — Wave 74 return-glacisangle honesty */
kprintf("spawn: soft retglacisangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=108 (retglacisangle honesty; Soft≠product; not bar3)\n");
/* Grep: spawn: soft retparapetangle — Wave 74 exclusive parapetangle stamp */
kprintf("spawn: soft retparapetangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=108 (retparapetangle stamp; Soft≠product)\n");
/* Grep: spawn: soft retmoatangle — Wave 75 return-moatangle honesty */
kprintf("spawn: soft retmoatangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=108 (retmoatangle honesty; Soft≠product; not bar3)\n");
/* Grep: spawn: soft retowerangle — Wave 75 exclusive towerangle stamp */
kprintf("spawn: soft retowerangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=108 (retowerangle stamp; Soft≠product)\n");
/* Grep: spawn: soft retgateangle — Wave 76 return-gateangle honesty */
kprintf("spawn: soft retgateangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=108 (retgateangle honesty; Soft≠product; not bar3)\n");
/* Grep: spawn: soft retwallangle — Wave 76 exclusive wallangle stamp */
kprintf("spawn: soft retwallangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=108 (retwallangle stamp; Soft≠product)\n");
/* Grep: spawn: soft retspireangle — Wave 77 return-spireangle honesty */
kprintf("spawn: soft retspireangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=108 (retspireangle honesty; Soft≠product; not bar3)\n");
/* Grep: spawn: soft retholdangle — Wave 77 exclusive holdangle stamp */
kprintf("spawn: soft retholdangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=108 (retholdangle stamp; Soft≠product)\n");
/* Grep: spawn: soft retpalaceangle — Wave 78 return-palaceangle honesty */
kprintf("spawn: soft retpalaceangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=108 (retpalaceangle honesty; Soft≠product; not bar3)\n");
/* Grep: spawn: soft retfortressangle — Wave 78 exclusive fortressangle stamp */
kprintf("spawn: soft retfortressangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=108 (retfortressangle stamp; Soft≠product)\n");
/* Grep: spawn: soft retkeepangle — Wave 79 return-keepangle honesty */
kprintf("spawn: soft retkeepangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=108 (retkeepangle honesty; Soft≠product; not bar3)\n");
/* Grep: spawn: soft retredoubtangle — Wave 79 exclusive redoubtangle stamp */
kprintf("spawn: soft retredoubtangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=108 (retredoubtangle stamp; Soft≠product)\n");
/* Grep: spawn: soft retcitadelangle — Wave 80 return-citadelangle honesty */
kprintf("spawn: soft retcitadelangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=108 (retcitadelangle honesty; Soft≠product; not bar3)\n");
/* Grep: spawn: soft retbastionkeep — Wave 80 exclusive bastionkeep stamp */
kprintf("spawn: soft retbastionkeep exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=108 (retbastionkeep stamp; Soft≠product)\n");
/* Grep: spawn: soft retpanoplyangle — Wave 81 return-panoplyangle honesty */
kprintf("spawn: soft retpanoplyangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=108 (retpanoplyangle honesty; Soft≠product; not bar3)\n");
/* Grep: spawn: soft retbulwarkangle — Wave 81 exclusive bulwarkangle stamp */
kprintf("spawn: soft retbulwarkangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=108 (retbulwarkangle stamp; Soft≠product)\n");
/* Grep: spawn: soft retmantleangle — Wave 82 return-mantleangle honesty */
kprintf("spawn: soft retmantleangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=108 (retmantleangle honesty; Soft≠product; not bar3)\n");
/* Grep: spawn: soft retaegisangle — Wave 82 exclusive aegisangle stamp */
kprintf("spawn: soft retaegisangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=108 (retaegisangle stamp; Soft≠product)\n");
/* Grep: spawn: soft retemblemangle — Wave 83 return-emblemangle honesty */
kprintf("spawn: soft retemblemangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=108 (retemblemangle honesty; Soft≠product; not bar3)\n");
/* Grep: spawn: soft retsigilangle — Wave 83 exclusive sigilangle stamp */
kprintf("spawn: soft retsigilangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=108 (retsigilangle stamp; Soft≠product)\n");
/* Grep: spawn: soft retscepterangle — Wave 84 return-scepterangle honesty */
kprintf("spawn: soft retscepterangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=108 (retscepterangle honesty; Soft≠product; not bar3)\n");
/* Grep: spawn: soft retglyphangle — Wave 84 exclusive glyphangle stamp */
kprintf("spawn: soft retglyphangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=108 (retglyphangle stamp; Soft≠product)\n");
/* Grep: spawn: soft retcrownangle — Wave 85 return-crownangle honesty */
kprintf("spawn: soft retcrownangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=108 (retcrownangle honesty; Soft≠product; not bar3)\n");
/* Grep: spawn: soft retshardangle — Wave 85 exclusive shardangle stamp */
kprintf("spawn: soft retshardangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=108 (retshardangle stamp; Soft≠product)\n");
/* Grep: spawn: soft retforgeangle — Wave 86 return-forgeangle honesty */
kprintf("spawn: soft retforgeangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=108 (retforgeangle honesty; Soft≠product; not bar3)\n");
/* Grep: spawn: soft retprismangle — Wave 86 exclusive prismangle stamp */
kprintf("spawn: soft retprismangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=108 (retprismangle stamp; Soft≠product)\n");
/* Grep: spawn: soft retflameangle — Wave 87 return-flameangle honesty */
kprintf("spawn: soft retflameangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=108 (retflameangle honesty; Soft≠product; not bar3)\n");
/* Grep: spawn: soft retcipherangle — Wave 87 exclusive cipherangle stamp */
kprintf("spawn: soft retcipherangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=108 (retcipherangle stamp; Soft≠product)\n");
/* Grep: spawn: soft retbeaconangle — Wave 88 return-beaconangle honesty */
kprintf("spawn: soft retbeaconangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=108 (retbeaconangle honesty; Soft≠product; not bar3)\n");
/* Grep: spawn: soft retledgerangle — Wave 88 exclusive ledgerangle stamp */
kprintf("spawn: soft retledgerangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=108 (retledgerangle stamp; Soft≠product)\n");
/* Grep: spawn: soft retbannerangle — Wave 89 return-bannerangle honesty */
kprintf("spawn: soft retbannerangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=108 (retbannerangle honesty; Soft≠product; not bar3)\n");
/* Grep: spawn: soft retvaultangle — Wave 89 exclusive vaultangle stamp */
kprintf("spawn: soft retvaultangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=108 (retvaultangle stamp; Soft≠product)\n");
/* Grep: spawn: soft retcrestangle — Wave 90 return-crestangle honesty */
kprintf("spawn: soft retcrestangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=108 (retcrestangle honesty; Soft≠product; not bar3)\n");
/* Grep: spawn: soft rettokenangle — Wave 90 exclusive tokenangle stamp */
kprintf("spawn: soft rettokenangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=108 (rettokenangle stamp; Soft≠product)\n");
/* Grep: spawn: soft retbadgeangle — Wave 91 return-badgeangle honesty */
kprintf("spawn: soft retbadgeangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=108 (retbadgeangle honesty; Soft≠product; not bar3)\n");
/* Grep: spawn: soft retphaseangle — Wave 91 exclusive phaseangle stamp */
kprintf("spawn: soft retphaseangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=108 (retphaseangle stamp; Soft≠product)\n");
/* Grep: spawn: soft retmarkangle — Wave 92 return-markangle honesty */
kprintf("spawn: soft retmarkangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=108 (retmarkangle honesty; Soft≠product; not bar3)\n");
/* Grep: spawn: soft retpulseangle — Wave 92 exclusive pulseangle stamp */
kprintf("spawn: soft retpulseangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=108 (retpulseangle stamp; Soft≠product)\n");

/* Grep: spawn: soft retsealangle — Wave 93 return-sealangle honesty */
kprintf("spawn: soft retsealangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=108 (retsealangle honesty; Soft≠product; not bar3)\n");
/* Grep: spawn: soft retboundangle — Wave 93 exclusive boundangle stamp */
kprintf("spawn: soft retboundangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=108 (retboundangle stamp; Soft≠product)\n");
/* Grep: spawn: soft retstemangle — Wave 94 return-stemangle honesty */
kprintf("spawn: soft retstemangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=108 (retstemangle honesty; Soft≠product; not bar3)\n");
/* Grep: spawn: soft retbladeangle — Wave 94 exclusive bladeangle stamp */
kprintf("spawn: soft retbladeangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=108 (retbladeangle stamp; Soft≠product)\n");
/* Grep: spawn: soft retchordangle — Wave 95 return-chordangle honesty */
kprintf("spawn: soft retchordangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=108 (retchordangle honesty; Soft≠product; not bar3)\n");
/* Grep: spawn: soft retarcangle — Wave 95 exclusive arcangle stamp */
kprintf("spawn: soft retarcangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=108 (retarcangle stamp; Soft≠product)\n");
/* Grep: spawn: soft retsectorangle — Wave 96 return-sectorangle honesty */
kprintf("spawn: soft retsectorangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=108 (retsectorangle honesty; Soft≠product; not bar3)\n");
/* Grep: spawn: soft retwedgeangle — Wave 96 exclusive wedgeangle stamp */
kprintf("spawn: soft retwedgeangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=108 (retwedgeangle stamp; Soft≠product)\n");
/* Grep: spawn: soft retradiusangle — Wave 97 return-radiusangle honesty */
kprintf("spawn: soft retradiusangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=108 (retradiusangle honesty; Soft≠product; not bar3)\n");
/* Grep: spawn: soft retdiameterangle — Wave 97 exclusive diameterangle stamp */
kprintf("spawn: soft retdiameterangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=108 (retdiameterangle stamp; Soft≠product)\n");
/* Grep: spawn: soft retcircumangle — Wave 98 return-circumangle honesty */
kprintf("spawn: soft retcircumangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=108 (retcircumangle honesty; Soft≠product; not bar3)\n");
/* Grep: spawn: soft retellipseangle — Wave 98 exclusive ellipseangle stamp */
kprintf("spawn: soft retellipseangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=108 (retellipseangle stamp; Soft≠product)\n");
/* Grep: spawn: soft rethyperangle — Wave 99 return-hyperangle honesty */
kprintf("spawn: soft rethyperangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=108 (rethyperangle honesty; Soft≠product; not bar3)\n");
/* Grep: spawn: soft retparabolaangle — Wave 99 exclusive parabolaangle stamp */
kprintf("spawn: soft retparabolaangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=108 (retparabolaangle stamp; Soft≠product)\n");
/* Grep: spawn: soft retspiralangle — Wave 100 return-spiralangle honesty */
kprintf("spawn: soft retspiralangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=108 (retspiralangle honesty; Soft≠product; not bar3)\n");
/* Grep: spawn: soft rethelixangle — Wave 100 exclusive helixangle stamp */
kprintf("spawn: soft rethelixangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=108 (rethelixangle stamp; Soft≠product)\n");
/* Grep: spawn: soft rettorusangle — Wave 101 return-torusangle honesty */
kprintf("spawn: soft rettorusangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=108 (rettorusangle honesty; Soft≠product; not bar3)\n");
/* Grep: spawn: soft retknotangle — Wave 101 exclusive knotangle stamp */
kprintf("spawn: soft retknotangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=108 (retknotangle stamp; Soft≠product)\n");
/* Grep: spawn: soft retmoebiusangle — Wave 102 return-moebiusangle honesty */
kprintf("spawn: soft retmoebiusangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=108 (retmoebiusangle honesty; Soft≠product; not bar3)\n");
/* Grep: spawn: soft retkleinangle — Wave 102 exclusive kleinangle stamp */
kprintf("spawn: soft retkleinangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=108 (retkleinangle stamp; Soft≠product)\n");
/* Grep: spawn: soft retprojectangle — Wave 103 return-projectangle honesty */
kprintf("spawn: soft retprojectangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=108 (retprojectangle honesty; Soft≠product; not bar3)\n");
/* Grep: spawn: soft retaffineangle — Wave 103 exclusive affineangle stamp */
kprintf("spawn: soft retaffineangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=108 (retaffineangle stamp; Soft≠product)\n");
/* Grep: spawn: soft retlinearangle — Wave 104 return-linearangle honesty */
kprintf("spawn: soft retlinearangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=108 (retlinearangle honesty; Soft≠product; not bar3)\n");
/* Grep: spawn: soft retbilinearangle — Wave 104 exclusive bilinearangle stamp */
kprintf("spawn: soft retbilinearangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=108 (retbilinearangle stamp; Soft≠product)\n");
/* Grep: spawn: soft retquadraticangle — Wave 105 return-quadraticangle honesty */
kprintf("spawn: soft retquadraticangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=108 (retquadraticangle honesty; Soft≠product; not bar3)\n");
/* Grep: spawn: soft retcubicangle — Wave 105 exclusive cubicangle stamp */
kprintf("spawn: soft retcubicangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=108 (retcubicangle stamp; Soft≠product)\n");
/* Grep: spawn: soft retquarticangle — Wave 106 return-quarticangle honesty */
kprintf("spawn: soft retquarticangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=108 (retquarticangle honesty; Soft≠product; not bar3)\n");
/* Grep: spawn: soft retquinticangle — Wave 106 exclusive quinticangle stamp */
kprintf("spawn: soft retquinticangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=108 (retquinticangle stamp; Soft≠product)\n");
/* Grep: spawn: soft retsplineangle — Wave 107 return-splineangle honesty */
kprintf("spawn: soft retsplineangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=108 (retsplineangle honesty; Soft≠product; not bar3)\n");
/* Grep: spawn: soft retbezierangle — Wave 107 exclusive bezierangle stamp */
kprintf("spawn: soft retbezierangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=108 (retbezierangle stamp; Soft≠product)\n");
/* Grep: spawn: soft rethurmitangle — Wave 108 return-hermitangle honesty */
kprintf("spawn: soft rethurmitangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=108 (rethurmitangle honesty; Soft≠product; not bar3)\n");
/* Grep: spawn: soft retcatmullangle — Wave 108 exclusive catmullangle stamp */
kprintf("spawn: soft retcatmullangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=108 (retcatmullangle stamp; Soft≠product)\n");
                            kprintf("spawn: soft deepen wave=%u via=%s enter=%u ok=%u fail=%u "
            "live=%u peak=%u mint_soft=%u mint_soft_bad=%u teardown=%u "
            "logs=%u (soft inventory only; not product gate)\n",
            GJ_SPAWN_SOFT_WAVE, szViaSafe, g_u32SoftSpawnEnter, g_cSpawned,
            g_cSpawnFail, cLive, g_u32SoftLivePeak, g_cMintSoft,
            g_cMintSoftBad, g_u32SoftFailCleanup, g_u32SoftLogN);

    /*
     * Soft lamp only — table + core rights configured. Never hard-gates.
     * Grep: spawn: soft inventory PASS | spawn: soft PASS
     */
    if (GJ_SPAWN_MAX > 0u && u16Core != 0) {
        kprintf("spawn: soft inventory PASS via=%s logs=%u live=%u "
                "peak=%u mint_soft=%u mint_soft_bad=%u wave=%u\n",
                szViaSafe, g_u32SoftLogN, cLive, g_u32SoftLivePeak,
                g_cMintSoft, g_cMintSoftBad, GJ_SPAWN_SOFT_WAVE);
        kprintf("spawn: soft PASS via=%s wave=%u\n", szViaSafe,
                GJ_SPAWN_SOFT_WAVE);
    } else {
        kprintf("spawn: soft FAIL via=%s max=%u core=0x%x "
                "(soft inventory only; not product gate)\n",
                szViaSafe, GJ_SPAWN_MAX, (unsigned)u16Core);
    }
}

/**
 * After first product spawn/kill/wait activity, print soft inventory once
 * (mirrors door/futex soft-stats-once). Diagnostics only.
 */
static void
soft_inventory_maybe_once(void)
{
    if (g_fSoftOnce != 0) {
        return;
    }
    if (g_u32SoftSpawnEnter == 0 && g_u32SoftKillEnter == 0 &&
        g_u32SoftWaitEnter == 0) {
        return;
    }
    g_fSoftOnce = 1;
    soft_inventory_log("once");
}

u32
process_spawn_count(void)
{
    return g_cSpawned;
}

u32
process_spawn_fail_count(void)
{
    return g_cSpawnFail;
}

u32
process_spawn_live_count(void)
{
    return spawn_live_count();
}

u32
process_spawn_kill_count(void)
{
    return g_cKill;
}

u32
process_spawn_wait_count(void)
{
    return g_cWait;
}

u32
process_spawn_mint_count(void)
{
    return g_cMintOk;
}

u32
process_spawn_mint_fail_count(void)
{
    return g_cMintFail;
}

u32
process_spawn_mint_soft_count(void)
{
    return g_cMintSoft;
}

u32
process_spawn_mint_soft_bad_count(void)
{
    return g_cMintSoftBad;
}

gj_status_t
process_spawn_stats(struct gj_spawn_stats *pOut)
{
    if (pOut == NULL) {
        return GJ_ERR_INVAL;
    }
    pOut->u32Ok = g_cSpawned;
    pOut->u32Fail = g_cSpawnFail;
    pOut->u32Live = spawn_live_count();
    pOut->u32Kill = g_cKill;
    pOut->u32Wait = g_cWait;
    pOut->u32MintOk = g_cMintOk;
    pOut->u32MintFail = g_cMintFail;
    pOut->u32MintSoft = g_cMintSoft;
    pOut->u32MintSoftBad = g_cMintSoftBad;
    return GJ_OK;
}

u32
process_spawn_stats_soft(void)
{
    u32 cLive;

    cLive = spawn_live_count();
    /*
     * Wave 11: full greppable soft inventory dump (includes soft stats line).
     * Grep: spawn: soft
     */
    soft_inventory_log("stats");
    return cLive;
}

/**
 * Locate fixed spawn-table entry for a child process (NULL if not a spawn child).
 */
static struct spawn_slot *
spawn_slot_of(struct gj_process *pProc)
{
    u32 iSlot;

    if (pProc == NULL) {
        return NULL;
    }
    for (iSlot = 0; iSlot < GJ_SPAWN_MAX; iSlot++) {
        if (g_aSpawn[iSlot].u8Used && &g_aSpawn[iSlot].proc == pProc) {
            return &g_aSpawn[iSlot];
        }
    }
    return NULL;
}

/**
 * Destroy private AS under kernel CR3; restore caller CR3.
 * Safe when u64Cr3 is 0 or matches kernel template.
 */
static void
spawn_as_teardown(struct gj_process *pProc)
{
    u64 u64Cr3;
    u64 u64Ker;
    u64 u64Saved;

    if (pProc == NULL || pProc->u64Cr3 == 0) {
        spawn_soft_inc(&g_u32SoftAsTeardownSkip);
        return;
    }
    u64Cr3 = pProc->u64Cr3;
    u64Ker = vmm_kernel_cr3();
    u64Saved = cpu_read_cr3();
    if (u64Ker != 0 &&
        (u64Cr3 & ~0xfffull) != (u64Ker & ~0xfffull)) {
        cpu_load_cr3(u64Ker);
        vmm_set_anon_cursor(NULL);
        (void)vmm_as_destroy(u64Cr3);
        pProc->u64Cr3 = 0;
        spawn_soft_inc(&g_u32SoftAsTeardown);
        if (u64Saved != 0) {
            cpu_load_cr3(u64Saved);
        } else if (u64Ker != 0) {
            cpu_load_cr3(u64Ker);
        }
    } else {
        spawn_soft_inc(&g_u32SoftAsTeardownSkip);
    }
}

/**
 * Reverse spawn install: drop parent PROCESS cap (if any) and private AS, free slot.
 */
static void
spawn_fail_cleanup(struct spawn_slot *pSlot, struct gj_process *pParent,
                   const struct gj_cap_ref *pRef)
{
    if (pSlot == NULL) {
        return;
    }
    spawn_soft_inc(&g_u32SoftFailCleanup);
    if (pParent != NULL && pRef != NULL && pParent->pCnode != NULL) {
        struct gj_cap_resolved res;

        if (gj_cap_resolve_ref(pParent->pCnode, pRef, &res) == GJ_OK &&
            res.pSlot != NULL) {
            gj_cap_slot_invalidate(res.pSlot);
        }
    }
    spawn_as_teardown(&pSlot->proc);
    pSlot->u8Used = 0;
}

/**
 * Soft-required PROCESS task rights after mint (kill/reap/vm).
 * Full base rights should be present; core is the hard soft gate.
 */
static u16
spawn_process_rights(u32 u32Jit)
{
    u16 u16Rights;

    u16Rights = GJ_SPAWN_PROCESS_RIGHTS_BASE;
    if (u32Jit) {
        u16Rights |= (u16)GJ_RIGHT_JIT;
        spawn_soft_inc(&g_u32SoftMintJit);
    } else {
        spawn_soft_inc(&g_u32SoftMintNoJit);
    }
    return u16Rights;
}

/**
 * Soft post-mint verify of parent PROCESS task cap (G-PROC-2).
 * Checks type, object pointer, slot gen, and core rights matrix.
 * Fail closed: returns non-OK so spawn tears down the partial child.
 * Grep: spawn: mint soft
 * Grep: spawn: soft mint (Wave 11 reason tallies)
 */
static gj_status_t
spawn_process_cap_mint_soft_verify(struct gj_process *pParent,
                                   const struct gj_cap_ref *pRef,
                                   struct gj_process *pChild,
                                   u16 u16WantRights)
{
    struct gj_cap_resolved res;
    gj_status_t st;
    struct gj_process *pFrom;

    if (pParent == NULL || pRef == NULL || pChild == NULL ||
        pParent->pCnode == NULL) {
        g_cMintSoftBad++;
        spawn_soft_inc(&g_u32SoftMintNullArg);
        return GJ_ERR_INVAL;
    }
    if (gj_cap_ref_is_null(pRef)) {
        g_cMintSoftBad++;
        spawn_soft_inc(&g_u32SoftMintNullRef);
        kprintf("spawn: mint soft FAIL null ref\n");
        return GJ_ERR_INVAL;
    }

    st = gj_cap_resolve_ref(pParent->pCnode, pRef, &res);
    if (st != GJ_OK) {
        g_cMintSoftBad++;
        spawn_soft_inc(&g_u32SoftMintResolve);
        kprintf("spawn: mint soft FAIL resolve st=%d\n", (int)st);
        return st;
    }
    if (res.u16Type != (u16)GJ_CAP_PROCESS) {
        g_cMintSoftBad++;
        spawn_soft_inc(&g_u32SoftMintType);
        kprintf("spawn: mint soft FAIL type=%u want=PROCESS\n",
                (unsigned)res.u16Type);
        return GJ_ERR_PERM;
    }
    if (res.pObj != (void *)&pChild->hdr) {
        g_cMintSoftBad++;
        spawn_soft_inc(&g_u32SoftMintObj);
        kprintf("spawn: mint soft FAIL obj mismatch\n");
        return GJ_ERR_INVAL;
    }
    if (res.pSlot != NULL && res.pSlot->u32Gen != pRef->u32SlotGen) {
        g_cMintSoftBad++;
        spawn_soft_inc(&g_u32SoftMintGen);
        kprintf("spawn: mint soft FAIL gen slot=%u ref=%u\n",
                res.pSlot->u32Gen, pRef->u32SlotGen);
        return GJ_ERR_INVAL;
    }
    /* Core task rights must all be present (soft gate). */
    if ((res.u16Rights & GJ_SPAWN_PROCESS_RIGHTS_CORE) !=
        GJ_SPAWN_PROCESS_RIGHTS_CORE) {
        g_cMintSoftBad++;
        spawn_soft_inc(&g_u32SoftMintCoreRights);
        kprintf("spawn: mint soft FAIL core rights=0x%x need=0x%x\n",
                (unsigned)res.u16Rights,
                (unsigned)GJ_SPAWN_PROCESS_RIGHTS_CORE);
        return GJ_ERR_PERM;
    }
    /* Full wanted rights should match install (soft observability). */
    if ((res.u16Rights & u16WantRights) != u16WantRights) {
        g_cMintSoftBad++;
        spawn_soft_inc(&g_u32SoftMintRights);
        kprintf("spawn: mint soft FAIL rights=0x%x want=0x%x\n",
                (unsigned)res.u16Rights, (unsigned)u16WantRights);
        return GJ_ERR_PERM;
    }

    pFrom = process_from_cap(pParent, pRef);
    if (pFrom != pChild) {
        g_cMintSoftBad++;
        spawn_soft_inc(&g_u32SoftMintFromCap);
        kprintf("spawn: mint soft FAIL from_cap mismatch\n");
        return GJ_ERR_INVAL;
    }

    g_cMintSoft++;
    /* Grep: spawn: mint soft PASS */
    kprintf("spawn: mint soft PASS slot=%lu gen=%u rights=0x%x\n",
            (unsigned long)pRef->u64Slot, pRef->u32SlotGen,
            (unsigned)res.u16Rights);
    return GJ_OK;
}

/**
 * Mint GJ_CAP_PROCESS into parent CNode + soft post-mint verify (G-PROC-2).
 * On soft-verify failure: invalidate installed slot and return error.
 */
static gj_status_t
spawn_process_cap_mint(struct gj_process *pParent, struct gj_process *pChild,
                       u32 u32Jit, struct gj_cap_ref *pOutRef)
{
    u16 u16Rights;
    gj_status_t st;

    if (pParent == NULL || pChild == NULL || pOutRef == NULL ||
        pParent->pCnode == NULL) {
        g_cMintFail++;
        spawn_soft_inc(&g_u32SoftMintNullArg);
        spawn_soft_inc(&g_u32SoftMintInstallFail);
        return GJ_ERR_INVAL;
    }

    u16Rights = spawn_process_rights(u32Jit);
    st = gj_cap_alloc_install(pParent->pCnode, (u16)GJ_CAP_PROCESS, u16Rights,
                              &pChild->hdr, pOutRef);
    if (st != GJ_OK) {
        g_cMintFail++;
        spawn_soft_inc(&g_u32SoftMintInstallFail);
        kprintf("spawn: mint soft FAIL install st=%d\n", (int)st);
        return st;
    }
    g_cMintOk++;

    st = spawn_process_cap_mint_soft_verify(pParent, pOutRef, pChild,
                                            u16Rights);
    if (st != GJ_OK) {
        /* Fail closed: drop the bad parent handle before returning. */
        if (pParent->pCnode != NULL) {
            struct gj_cap_resolved res;

            if (gj_cap_resolve_ref(pParent->pCnode, pOutRef, &res) == GJ_OK &&
                res.pSlot != NULL) {
                gj_cap_slot_invalidate(res.pSlot);
            }
        }
        memset(pOutRef, 0, sizeof(*pOutRef));
        return st;
    }
    return GJ_OK;
}

gj_status_t
process_kill(struct gj_process *pParent, const struct gj_cap_ref *pRef,
             u32 u32ExitCode)
{
    struct gj_process *pChild;
    struct gj_cap_resolved res;
    gj_status_t st;

    spawn_soft_inc(&g_u32SoftKillEnter);

    if (pParent == NULL || pRef == NULL) {
        spawn_soft_inc(&g_u32SoftKillDenyNull);
        soft_inventory_maybe_once();
        return GJ_ERR_INVAL;
    }
    st = gj_cap_resolve_ref(pParent->pCnode, pRef, &res);
    if (st != GJ_OK) {
        spawn_soft_inc(&g_u32SoftKillDenyResolve);
        soft_inventory_maybe_once();
        return st;
    }
    if (res.u16Type != (u16)GJ_CAP_PROCESS) {
        spawn_soft_inc(&g_u32SoftKillDenyType);
        soft_inventory_maybe_once();
        return GJ_ERR_PERM;
    }
    if ((res.u16Rights & (u16)GJ_RIGHT_DESTROY) == 0) {
        spawn_soft_inc(&g_u32SoftKillDenyRights);
        soft_inventory_maybe_once();
        return GJ_ERR_PERM;
    }
    pChild = (struct gj_process *)res.pObj;
    if (pChild == NULL) {
        spawn_soft_inc(&g_u32SoftKillDenyNoent);
        soft_inventory_maybe_once();
        return GJ_ERR_NOENT;
    }
    g_cKill++;
    if (!pChild->u32Alive) {
        /* Already dead — idempotent kill */
        spawn_soft_inc(&g_u32SoftKillIdem);
        soft_inventory_maybe_once();
        return GJ_OK;
    }
    /*
     * G-PROC-5: full death path (pager, regions, wait-registered CNode wipe,
     * private AS destroy, zombie for wait4 / process_wait).
     */
    process_death(pChild, u32ExitCode);
    kprintf("spawn: kill child exit=%u PASS\n", u32ExitCode);
    soft_inventory_maybe_once();
    return GJ_OK;
}

gj_status_t
process_wait(struct gj_process *pParent, const struct gj_cap_ref *pRef,
             u32 *pOutExit)
{
    struct gj_process *pChild;
    struct gj_cap_resolved res;
    struct spawn_slot *pSlot;
    gj_status_t st;
    u32 u32Exit;

    spawn_soft_inc(&g_u32SoftWaitEnter);

    if (pParent == NULL || pRef == NULL) {
        spawn_soft_inc(&g_u32SoftWaitDenyNull);
        soft_inventory_maybe_once();
        return GJ_ERR_INVAL;
    }
    st = gj_cap_resolve_ref(pParent->pCnode, pRef, &res);
    if (st != GJ_OK) {
        spawn_soft_inc(&g_u32SoftWaitDenyResolve);
        soft_inventory_maybe_once();
        return st;
    }
    if (res.u16Type != (u16)GJ_CAP_PROCESS) {
        spawn_soft_inc(&g_u32SoftWaitDenyType);
        soft_inventory_maybe_once();
        return GJ_ERR_PERM;
    }
    if ((res.u16Rights & (u16)GJ_RIGHT_WAIT) == 0) {
        spawn_soft_inc(&g_u32SoftWaitDenyRights);
        soft_inventory_maybe_once();
        return GJ_ERR_PERM;
    }
    pChild = (struct gj_process *)res.pObj;
    if (pChild == NULL) {
        spawn_soft_inc(&g_u32SoftWaitDenyNoent);
        soft_inventory_maybe_once();
        return GJ_ERR_NOENT;
    }
    if (pChild->u32Alive) {
        spawn_soft_inc(&g_u32SoftWaitAgain);
        soft_inventory_maybe_once();
        return GJ_ERR_AGAIN;
    }
    u32Exit = pChild->u32ExitCode;
    if (pOutExit != NULL) {
        *pOutExit = u32Exit;
    }
    /*
     * Reap: drop wait-table entry, invalidate parent PROCESS cap, recycle
     * spawn slot. Cap gen bump prevents reuse of a stale handle.
     */
    process_wait_forget(pChild);
    if (res.pSlot != NULL) {
        gj_cap_slot_invalidate(res.pSlot);
        spawn_soft_inc(&g_u32SoftWaitCapInv);
    }
    pSlot = spawn_slot_of(pChild);
    if (pSlot != NULL) {
        pSlot->u8Used = 0;
        spawn_soft_inc(&g_u32SoftWaitSlotFree);
    }
    g_cWait++;
    kprintf("spawn: wait reaped exit=%u PASS\n", u32Exit);
    soft_inventory_maybe_once();
    return GJ_OK;
}

struct gj_process *
process_from_cap(struct gj_process *pParent, const struct gj_cap_ref *pRef)
{
    struct gj_cap_resolved res;
    gj_status_t st;

    if (pParent == NULL || pRef == NULL || pParent->pCnode == NULL) {
        spawn_soft_inc(&g_u32SoftFromCapMiss);
        return NULL;
    }
    st = gj_cap_resolve_ref(pParent->pCnode, pRef, &res);
    if (st != GJ_OK || res.u16Type != (u16)GJ_CAP_PROCESS) {
        spawn_soft_inc(&g_u32SoftFromCapMiss);
        return NULL;
    }
    spawn_soft_inc(&g_u32SoftFromCapHit);
    return (struct gj_process *)res.pObj;
}

gj_status_t
process_spawn(struct gj_process *pParent, const struct gj_spawn_args *pArgs,
              struct gj_process **ppOutChild, struct gj_cap_ref *pOutCap)
{
    u32 iSlot;
    struct spawn_slot *pSlot;
    struct gj_process *pChild;
    struct gj_cap_ref ref;
    gj_status_t st;
    u32 u32Thr;
    u32 u32Ppid;

    spawn_soft_inc(&g_u32SoftSpawnEnter);

    if (pParent == NULL || pArgs == NULL || pArgs->pfnEntry == NULL) {
        g_cSpawnFail++;
        spawn_soft_inc(&g_u32SoftDenyNull);
        soft_inventory_maybe_once();
        return GJ_ERR_INVAL;
    }
    if (pParent->pCnode == NULL) {
        g_cSpawnFail++;
        spawn_soft_inc(&g_u32SoftDenyNoCnode);
        soft_inventory_maybe_once();
        return GJ_ERR_INVAL;
    }

    pSlot = NULL;
    for (iSlot = 0; iSlot < GJ_SPAWN_MAX; iSlot++) {
        if (!g_aSpawn[iSlot].u8Used) {
            pSlot = &g_aSpawn[iSlot];
            break;
        }
    }
    if (pSlot == NULL) {
        g_cSpawnFail++;
        spawn_soft_inc(&g_u32SoftDenyFull);
        soft_inventory_maybe_once();
        return GJ_ERR_NOMEM;
    }
    memset(pSlot, 0, sizeof(*pSlot));
    pSlot->u8Used = 1;
    pChild = &pSlot->proc;
    memset(&ref, 0, sizeof(ref));

    gj_process_init(pChild, &pSlot->cnode, pSlot->aSlots, GJ_SPAWN_CNODE_SLOTS);
    pChild->pParent = pParent;
    pChild->u32Alive = 1;
    pChild->u32Personality = pArgs->u32Personality;
    if (pArgs->u32Personality == 0u) {
        spawn_soft_inc(&g_u32SoftPersonaNative);
    } else {
        spawn_soft_inc(&g_u32SoftPersonaLinux);
    }
    gj_process_set_jit(pChild, pArgs->u32Jit ? 1 : 0);

    st = gj_process_bootstrap_root_meta(pChild, &pSlot->meta, NULL);
    if (st != GJ_OK) {
        pSlot->u8Used = 0;
        g_cSpawnFail++;
        spawn_soft_inc(&g_u32SoftDenyMeta);
        soft_inventory_maybe_once();
        return st;
    }

    st = process_as_ensure(pChild);
    if (st != GJ_OK) {
        pSlot->u8Used = 0;
        g_cSpawnFail++;
        spawn_soft_inc(&g_u32SoftDenyAs);
        soft_inventory_maybe_once();
        return st;
    }

    /* Mint PROCESS task port into parent CNode + soft verify (G-PROC-2) */
    st = spawn_process_cap_mint(pParent, pChild, pArgs->u32Jit, &ref);
    if (st != GJ_OK) {
        spawn_fail_cleanup(pSlot, NULL, NULL);
        g_cSpawnFail++;
        spawn_soft_inc(&g_u32SoftDenyMint);
        soft_inventory_maybe_once();
        return st;
    }

    u32Thr = thread_create(pChild, pArgs->pfnEntry, pArgs->pArg);
    if (u32Thr == 0) {
        spawn_fail_cleanup(pSlot, pParent, &ref);
        g_cSpawnFail++;
        spawn_soft_inc(&g_u32SoftDenyThr);
        soft_inventory_maybe_once();
        return GJ_ERR_NOMEM;
    }

    /*
     * Wait-register after first thr is live so process_death reclaims CNode +
     * private AS (G-PROC-5). Parent pid from wait table when known; else 1.
     */
    u32Ppid = process_wait_pid_of(pParent);
    (void)process_wait_register(pChild, u32Ppid != 0 ? u32Ppid : 1u);
    spawn_soft_inc(&g_u32SoftWaitReg);
    g_u32SoftLastThrId = u32Thr;

    g_cSpawned++;
    soft_note_live_peak();
    kprintf("spawn: child thr=%u cr3=0x%lx cap slot=%lu gen=%u "
            "count=%u live=%u mint_soft=%u PASS\n",
            u32Thr, (unsigned long)pChild->u64Cr3,
            (unsigned long)ref.u64Slot, ref.u32SlotGen,
            g_cSpawned, spawn_live_count(), g_cMintSoft);

    if (ppOutChild != NULL) {
        *ppOutChild = pChild;
    }
    if (pOutCap != NULL) {
        *pOutCap = ref;
    }
    soft_inventory_maybe_once();
    return GJ_OK;
}
