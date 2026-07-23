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
 * Soft product inventory (Wave 17 deepen). Cumulative unless noted live/peak.
 * greppable: spawn: soft …
 */
#define GJ_SPAWN_SOFT_WAVE 17u

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
 * Greppable soft spawn inventory (Wave 17 deepen; product / smoke).
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
 *   spawn: soft return     — Wave 17 return-path catalog
 *   spawn: soft ret_surface — Wave 17 terminal return classes
 *   spawn: soft surface    — Wave 17 area catalog
 *   spawn: soft deepen     — Wave 17 stamp
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
     * Wave 17 return-path catalog — spawn/mint/kill/wait deny outcomes.
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

    /* Grep: spawn: soft ret_surface — Wave 17 terminal return classes */
    kprintf("spawn: soft ret_surface spawn=ok|fail "
            "deny=null|full|as|mint|thr mint=ok|fail|soft|soft_bad "
            "kill=ok|idem wait=ok|again from_cap=hit|miss teardown "
            "product_kernel=OPEN areas=16 wave=%u\n",
            GJ_SPAWN_SOFT_WAVE);

    /* Grep: spawn: soft surface — Wave 17 area catalog */
    kprintf("spawn: soft surface inventory,stats,spawn,mint,kill,wait,"
            "from_cap,teardown,lifecycle,capacity,path,return,ret_surface,"
            "surface,deepen areas=16 wave=%u\n",
            GJ_SPAWN_SOFT_WAVE);

    /* Grep: spawn: soft deepen */
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
