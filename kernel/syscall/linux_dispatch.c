/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Hybrid Option C: hot table / cold personality (doors) / ENOSYS.
 * Pure C11 clean-room Linux x86_64 surface — no GPL source.
 *
 * Soft inventory (Wave 12/14 base + Wave 35 exclusive deepen; this unit):
 *   "linux: nr class soft …"     — table inventory + runtime (legacy + deepen)
 *   "linux: dispatch soft …"     — twin path/API tallies (agent-friendly)
 *
 * Catalog (prefix-stable; never hard-gates product / not bar3):
 *   linux: nr class soft PASS|PARTIAL|NONE …   — product inventory (scripts)
 *   linux: nr class soft hits_h=…              — runtime hits
 *   linux: nr class soft armed=…               — integrity + cold route
 *   linux: nr class soft fill|register|…       — Wave 12 deepen lines
 *   linux: nr class soft rates|ret|honesty|…   — Wave 14 complementary
 *   linux: nr class soft edge|share|catalog    — Wave 15 complementary
 *   linux: nr class soft idle (dispatch not init)
 *   linux: dispatch soft inventory|init|classify|…  — Wave 12 twin
 *   linux: dispatch soft rates|ret|last|honesty|deepen|… — Wave 14 twin
 *   linux: dispatch soft edge|share|catalog    — Wave 15 twin
 *   linux: dispatch soft path claim=… (soft inventory; not bar3)
 * greppable: linux: nr class soft
 * greppable: linux: dispatch soft
 *
 * Soft only; Linux ABI hybrid product remains open (not bar3).
 */
#include <gj/cold_ipc.h>
#include <gj/klog.h>
#include <gj/linux_dispatch.h>
#include <gj/syscall.h>
#include <gj/wow64.h>

typedef i64 (*gj_linux_hot_fn_t)(struct gj_linux_regs *pRegs);

static gj_linux_hot_fn_t g_apfnHot[GJ_LINUX_NR_TABLE];
static u8 g_aPath[GJ_LINUX_NR_TABLE]; /* enum gj_linux_path */

static gj_linux_cold_fn_t g_pfnCold;
static void *g_pColdCtx;

static struct gj_linux_dispatch_stats g_stats;

/* Deep NR-table classification + set_hot/set_cold coverage (soft). */
static struct gj_linux_nr_class_stats g_class;

/*
 * Init published: PATH_HOT is 0, so BSS g_aPath looks fully HOT. soft_log /
 * stats_get must not invent product coverage before dispatch_init finishes.
 */
static int g_fNrClassLive;

/*
 * Soft product inventory (Wave 12/14 base + Wave 35 exclusive deepen).
 * File-local path tallies. Cumulative unless noted; wrap OK; never hard-gates.
 * greppable: linux: nr class soft … / linux: dispatch soft …
 */
static u32 g_u32SoftInitEnter;      /* gj_linux_dispatch_init entries */
static u32 g_u32SoftInitOk;         /* init completed + live published */
static u32 g_u32SoftLogN;           /* soft inventory log emissions */
static u32 g_u32SoftStatsGet;       /* nr_class_stats_get entries */
static u32 g_u32SoftStatsGetNull;   /* stats_get with pOut == NULL */
static u32 g_u32SoftStatsGetIdle;   /* stats_get pre-init fail-closed */
static u32 g_u32SoftStatsGetLive;   /* stats_get post-init snapshot */
static u32 g_u32SoftStatsReset;     /* dispatch_stats_reset entries */
static u32 g_u32SoftDispatchStatsGet; /* coarse dispatch_stats_get entries */
static u32 g_u32SoftDispatchStatsGetNull;
static u32 g_u32SoftClassify;       /* gj_linux_classify entries */
static u32 g_u32SoftClassifyOor;    /* classify NR >= table */
static u32 g_u32SoftClassifyHot;    /* classify → HOT */
static u32 g_u32SoftClassifyCold;   /* classify → COLD */
static u32 g_u32SoftClassifyNone;   /* classify → NONE (in-table) */
static u32 g_u32SoftSetColdBind;    /* set_cold_handler with non-NULL pfn */
static u32 g_u32SoftSetColdUnbind;  /* set_cold_handler with NULL pfn */
static u32 g_u32SoftWow64Enter;     /* dispatch with wow64 enabled */
static u32 g_u32SoftWow64Map;       /* wow64_translate_nr success */
static u32 g_u32SoftPathHot;        /* dispatch saw PATH_HOT (before defer) */
static u32 g_u32SoftPathCold;       /* dispatch saw PATH_COLD */
static u32 g_u32SoftPathNone;       /* dispatch saw PATH_NONE */
static u32 g_u32SoftPathHotDirect;  /* HOT fn returned non-ENOSYS */
static u32 g_u32SoftPathHotDefer;   /* HOT fn returned -ENOSYS */
static u32 g_u32SoftPathHotNull;    /* PATH_HOT with NULL pfn at runtime */
static u32 g_u32SoftPathOor;        /* dispatch NR OOR */
static u32 g_u32SoftPathNullGuard;  /* NULL pRegs */
static u32 g_u32SoftPathColdIpc;    /* cold_ipc_submit */
static u32 g_u32SoftPathColdLeg;    /* legacy g_pfnCold */
static u32 g_u32SoftPathColdBare;   /* cold/hot-defer bare ENOSYS */
/* Wave 14+: terminal i64Ret soft outcome (post-handler). */
static u32 g_u32SoftRetNeg;         /* i64Ret < 0 */
static u32 g_u32SoftRetZero;        /* i64Ret == 0 */
static u32 g_u32SoftRetPos;         /* i64Ret > 0 */
static u32 g_u32SoftRetNote;        /* soft_note_ret calls */
static u8  g_fSoftInvOnce;          /* one-shot deep dump after activity */

/* Wave 15 soft inventory stamp + area count (greppable deepen). */
#define LINUX_DISPATCH_SOFT_WAVE 38u
#define LINUX_DISPATCH_SOFT_AREAS 70u

static void soft_inc(u32 *pCtr);
static void soft_note_ret(i64 i64Ret);
static void soft_dispatch_deepen_log(void);
static void soft_inventory_maybe_once(void);

/** Soft: bump path tally (u32 wrap is fine for telemetry). */
static void
soft_inc(u32 *pCtr)
{
    if (pCtr == NULL) {
        return;
    }
    if (*pCtr < 0xffffffffu) {
        (*pCtr)++;
    }
}

/**
 * Wave 14: soft terminal-return outcome after a live dispatch finishes.
 * Never mutates product path; wrap-saturate via soft_inc.
 */
static void
soft_note_ret(i64 i64Ret)
{
    soft_inc(&g_u32SoftRetNote);
    if (i64Ret < 0) {
        soft_inc(&g_u32SoftRetNeg);
    } else if (i64Ret == 0) {
        soft_inc(&g_u32SoftRetZero);
    } else {
        soft_inc(&g_u32SoftRetPos);
    }
}

/**
 * Authoritative path-table scan (slot inventory + integrity soft deepen).
 * Caller must only publish results when g_fNrClassLive (or during init end).
 */
static void
nr_class_scan_slots(void)
{
    u32 iNr;
    u32 u32Hot = 0;
    u32 u32Cold = 0;
    u32 u32None = 0;
    u32 u32HotArmed = 0;
    u32 u32HotNull = 0;
    u32 u32PathBad = 0;

    for (iNr = 0; iNr < GJ_LINUX_NR_TABLE; iNr++) {
        u8 u8Path = g_aPath[iNr];

        if (u8Path == (u8)GJ_LINUX_PATH_HOT) {
            u32Hot++;
            if (g_apfnHot[iNr] != NULL) {
                u32HotArmed++;
            } else {
                u32HotNull++;
            }
        } else if (u8Path == (u8)GJ_LINUX_PATH_COLD) {
            u32Cold++;
        } else if (u8Path == (u8)GJ_LINUX_PATH_NONE) {
            u32None++;
        } else {
            /* Corrupt / unknown path byte — soft integrity, not product. */
            u32PathBad++;
        }
    }
    g_class.u32TableSize = GJ_LINUX_NR_TABLE;
    g_class.u32HotSlots = u32Hot;
    g_class.u32ColdSlots = u32Cold;
    g_class.u32NoneSlots = u32None;
    g_class.u32Classified = u32Hot + u32Cold;
    g_class.u32HotArmed = u32HotArmed;
    g_class.u32HotNullSlots = u32HotNull;
    g_class.u32PathBad = u32PathBad;
    if (g_class.u32MaxHotNr > g_class.u32MaxColdNr) {
        g_class.u32MaxClassNr = g_class.u32MaxHotNr;
    } else {
        g_class.u32MaxClassNr = g_class.u32MaxColdNr;
    }
}

static void
set_hot(u32 u32Nr, gj_linux_hot_fn_t pfn)
{
    if (u32Nr < GJ_LINUX_NR_TABLE && pfn != NULL) {
        g_apfnHot[u32Nr] = pfn;
        g_aPath[u32Nr] = (u8)GJ_LINUX_PATH_HOT;
        g_class.u32SetHotOk++;
        if (u32Nr > g_class.u32MaxHotNr) {
            g_class.u32MaxHotNr = u32Nr;
        }
    } else {
        g_class.u32SetHotReject++;
    }
}

static void
set_cold(u32 u32Nr)
{
    if (u32Nr < GJ_LINUX_NR_TABLE) {
        g_apfnHot[u32Nr] = NULL;
        g_aPath[u32Nr] = (u8)GJ_LINUX_PATH_COLD;
        g_class.u32SetColdOk++;
        if (u32Nr > g_class.u32MaxColdNr) {
            g_class.u32MaxColdNr = u32Nr;
        }
    } else {
        g_class.u32SetColdReject++;
    }
}

/**
 * Wave 12 + Wave 14/15 deepen lines under both greppable prefixes.
 * Caller must already have refreshed g_class via nr_class_scan_slots when live.
 * Wave 12 primary lines stay field-stable; Wave 14/15 add complementary surfaces.
 * greppable: linux: nr class soft
 * greppable: linux: dispatch soft
 */
static void
soft_dispatch_deepen_log(void)
{
    u32 u32Table;
    u32 u32Hot;
    u32 u32Cold;
    u32 u32None;
    u32 u32Class;
    u32 u32Sum;
    u32 u32PctHot;
    u32 u32PctCold;
    u32 u32PctNone;
    u32 u32PctClass;
    u32 u32SumOk;
    u32 u32ArmedOk;
    u32 u32Attached;
    u32 u32LegacyBound;
    u32 u32Live;
    u32 u32Wow64On;
    u64 u64HitSum;
    u64 u64BpHot;
    u64 u64BpCold;
    u64 u64BpEnosys;
    u64 u64BpRetNeg;
    u32 u32RetSum;

    soft_inc(&g_u32SoftLogN);

    u32Table = g_class.u32TableSize;
    u32Hot = g_class.u32HotSlots;
    u32Cold = g_class.u32ColdSlots;
    u32None = g_class.u32NoneSlots;
    u32Class = g_class.u32Classified;
    u32Sum = u32Hot + u32Cold + u32None + g_class.u32PathBad;
    u32Live = g_fNrClassLive ? 1u : 0u;
    u32Attached = cold_ipc_personality_attached() ? 1u : 0u;
    u32LegacyBound = (g_pfnCold != NULL) ? 1u : 0u;
    u32Wow64On = wow64_enabled() ? 1u : 0u;
    u32SumOk = (u32Sum == u32Table && g_class.u32PathBad == 0) ? 1u : 0u;
    u32ArmedOk =
        (g_class.u32HotNullSlots == 0 && g_class.u32HotArmed == u32Hot) ? 1u
                                                                         : 0u;

    /* Soft percent of table (integer; 0 if table empty). */
    if (u32Table > 0) {
        u32PctHot = (u32Hot * 100u) / u32Table;
        u32PctCold = (u32Cold * 100u) / u32Table;
        u32PctNone = (u32None * 100u) / u32Table;
        u32PctClass = (u32Class * 100u) / u32Table;
    } else {
        u32PctHot = 0;
        u32PctCold = 0;
        u32PctNone = 0;
        u32PctClass = 0;
    }

    /* Wave 14: soft hit / ret share (basis points). */
    u64HitSum = g_class.u64HotHits + g_class.u64ColdHits + g_class.u64Enosys;
    if (u64HitSum != 0) {
        u64BpHot = (g_class.u64HotHits * 10000ull) / u64HitSum;
        u64BpCold = (g_class.u64ColdHits * 10000ull) / u64HitSum;
        u64BpEnosys = (g_class.u64Enosys * 10000ull) / u64HitSum;
    } else {
        u64BpHot = 0;
        u64BpCold = 0;
        u64BpEnosys = 0;
    }
    u32RetSum = g_u32SoftRetNeg + g_u32SoftRetZero + g_u32SoftRetPos;
    if (u32RetSum != 0u) {
        u64BpRetNeg = ((u64)g_u32SoftRetNeg * 10000ull) / (u64)u32RetSum;
    } else {
        u64BpRetNeg = 0;
    }

    /*
     * Wave 12 nr class deepen (additive; legacy 3 lines stay above).
     * Grep: linux: nr class soft fill
     */
    kprintf("linux: nr class soft fill pct_hot=%u pct_cold=%u pct_none=%u "
            "pct_class=%u sum=%u table=%u sum_ok=%u\n",
            u32PctHot, u32PctCold, u32PctNone, u32PctClass, u32Sum, u32Table,
            u32SumOk);

    /* Grep: linux: nr class soft register */
    kprintf("linux: nr class soft register set_hot=%u set_cold=%u rej_h=%u "
            "rej_c=%u max_h=%u max_c=%u max=%u\n",
            g_class.u32SetHotOk, g_class.u32SetColdOk, g_class.u32SetHotReject,
            g_class.u32SetColdReject, g_class.u32MaxHotNr, g_class.u32MaxColdNr,
            g_class.u32MaxClassNr);

    /* Grep: linux: nr class soft integrity */
    kprintf("linux: nr class soft integrity armed=%u null_slots=%u "
            "path_bad=%u armed_ok=%u live=%u\n",
            g_class.u32HotArmed, g_class.u32HotNullSlots, g_class.u32PathBad,
            u32ArmedOk, u32Live);

    /* Grep: linux: nr class soft route */
    kprintf("linux: nr class soft route hits_h=%llu hits_c=%llu enosys=%llu "
            "defer=%llu hot_null=%llu oor=%llu none_path=%llu entries=%llu\n",
            (unsigned long long)g_class.u64HotHits,
            (unsigned long long)g_class.u64ColdHits,
            (unsigned long long)g_class.u64Enosys,
            (unsigned long long)g_class.u64HotDeferCold,
            (unsigned long long)g_class.u64HotNull,
            (unsigned long long)g_class.u64Oor,
            (unsigned long long)g_class.u64NonePath,
            (unsigned long long)g_class.u64Entries);

    /* Grep: linux: nr class soft cold_route */
    kprintf("linux: nr class soft cold_route ipc=%llu leg=%llu bare=%llu "
            "attached=%u legacy_bound=%u last_nr=%llu last_ret=%llu\n",
            (unsigned long long)g_class.u64ColdIpc,
            (unsigned long long)g_class.u64ColdLegacy,
            (unsigned long long)g_class.u64ColdBare, u32Attached, u32LegacyBound,
            (unsigned long long)g_class.u64LastNr,
            (unsigned long long)g_class.u64LastRet);

    /* Grep: linux: nr class soft path */
    kprintf("linux: nr class soft path claim=hot+cold+none hybrid=OptionC "
            "(soft inventory; not bar3)\n");

    /*
     * Wave 14/15 complementary nr class surfaces (never reshape primary lines).
     * Grep: linux: nr class soft rates
     */
    kprintf("linux: nr class soft rates bp_hot=%llu bp_cold=%llu "
            "bp_enosys=%llu hit_sum=%llu bp_ret_neg=%llu ret_sum=%u "
            "wave=%u\n",
            (unsigned long long)u64BpHot, (unsigned long long)u64BpCold,
            (unsigned long long)u64BpEnosys, (unsigned long long)u64HitSum,
            (unsigned long long)u64BpRetNeg, u32RetSum,
            (unsigned)LINUX_DISPATCH_SOFT_WAVE);

    /* Grep: linux: nr class soft ret */
    kprintf("linux: nr class soft ret neg=%u zero=%u pos=%u note=%u "
            "last_ret=%llu wave=%u\n",
            g_u32SoftRetNeg, g_u32SoftRetZero, g_u32SoftRetPos, g_u32SoftRetNote,
            (unsigned long long)g_class.u64LastRet,
            (unsigned)LINUX_DISPATCH_SOFT_WAVE);

    /* Grep: linux: nr class soft honesty */
    kprintf("linux: nr class soft honesty hybrid=OptionC open=1 bar3=0 "
            "product_linux_abi=open soft_only=1 live=%u wave=%u "
            "(soft inventory; never closes hybrid)\n",
            u32Live, (unsigned)LINUX_DISPATCH_SOFT_WAVE);

    /* Grep: linux: nr class soft edge (Wave 20 deepen) */
    kprintf("linux: nr class soft edge entries=%llu null_guard=%u "
            "path_hot=%u path_cold=%u path_none=%u path_oor=%u "
            "wow64=%u wave=%u\n",
            (unsigned long long)g_class.u64Entries, g_u32SoftPathNullGuard,
            g_u32SoftPathHot, g_u32SoftPathCold, g_u32SoftPathNone,
            g_u32SoftPathOor, g_u32SoftWow64Enter,
            (unsigned)LINUX_DISPATCH_SOFT_WAVE);

    /* Grep: linux: nr class soft share (Wave 20 deepen) */
    kprintf("linux: nr class soft share pct_hot=%u pct_cold=%u "
            "pct_none=%u pct_class=%u bp_hot=%llu bp_cold=%llu "
            "armed_ok=%u wave=%u\n",
            u32PctHot, u32PctCold, u32PctNone, u32PctClass,
            (unsigned long long)u64BpHot, (unsigned long long)u64BpCold,
            u32ArmedOk, (unsigned)LINUX_DISPATCH_SOFT_WAVE);

    /* Grep: linux: nr class soft surfaces (Wave 20 deepen) */
    kprintf("linux: nr class soft surfaces count=%u wave=%u "
            "names=inventory,path,rates,ret,honesty,edge,share,catalog,"
            "surfaces,note,return,retmap,deepen,PASS\n",
            LINUX_DISPATCH_SOFT_AREAS,
            (unsigned)LINUX_DISPATCH_SOFT_WAVE);

    /* Grep: linux: nr class soft note (Wave 20 deepen) */
    kprintf("linux: nr class soft note milestone=wave38 exclusive=1 "
            "soft_only=1 not_bar3=1 wave=%u\n",
            (unsigned)LINUX_DISPATCH_SOFT_WAVE);

    /* Grep: linux: nr class soft catalog (Wave 20 deepen) */
    kprintf("linux: nr class soft catalog wave=%u areas=%u "
            "surfaces=fill,register,integrity,route,cold_route,path,"
            "rates,ret,honesty,edge,share,catalog,deepen\n",
            (unsigned)LINUX_DISPATCH_SOFT_WAVE, LINUX_DISPATCH_SOFT_AREAS);

    /* Grep: linux: nr class soft return (Wave 20 deepen) */
    kprintf("linux: nr class soft return live=%u table=%u class=%u "
            "ret_neg=%u ret_zero=%u ret_pos=%u product_gate=0 wave=%u\n",
            u32Live, u32Table, u32Class,
            g_u32SoftRetNeg, g_u32SoftRetZero, g_u32SoftRetPos,
            (unsigned)LINUX_DISPATCH_SOFT_WAVE);

    /* Grep: linux: nr class soft retmap — Wave 19 return-surface map */
    kprintf("linux: nr class soft retmap live|table|class ret=neg|zero|pos product_gate=0 soft_only=1 wave=%u\n",
            (unsigned)LINUX_DISPATCH_SOFT_WAVE);

    /* Grep: linux: nr class soft deepen */
    /*
     * ---- Wave 19 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft≠product; not bar3.
     */
    /* Grep: linux: nr class: soft retclass — Wave 19 return-class taxonomy (kept) */
    kprintf("linux: nr class: soft retclass ok|fail|inval|nodev|busy|nomem "
            "soft_only=1 product_gate=0 wave=%u "
            "(retclass taxonomy; Soft≠product; not bar3)\n",
            (unsigned)LINUX_DISPATCH_SOFT_WAVE);
    /* Grep: linux: nr class: soft retlane — Wave 19 return-lane catalog (kept) */
    kprintf("linux: nr class: soft retlane inv|selftest|rate|retcode|retmap|class "
            "product_kernel=OPEN soft_ne_product=1 wave=%u "
            "(retlane catalog; Soft≠product)\n",
            (unsigned)LINUX_DISPATCH_SOFT_WAVE);
    /*
     * ---- Wave 20 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft≠product; not bar3.
     */
    /* Grep: linux: nr class: soft retbound — Wave 20 return-bound honesty (kept) */
    kprintf("linux: nr class: soft retbound soft_only=1 product_gate=0 hard_gate=0 "
            "never_blocks_m0=1 wave=%u "
            "(retbound honesty; Soft≠product; not bar3)\n",
            (unsigned)LINUX_DISPATCH_SOFT_WAVE);
    /* Grep: linux: nr class: soft retseal — Wave 20 seal stamp (kept) */
    kprintf("linux: nr class: soft retseal exclusive=1 soft_ne_product=1 "
            "product_kernel=OPEN bar3=0 wave=%u "
            "(retseal stamp; Soft≠product)\n",
            (unsigned)LINUX_DISPATCH_SOFT_WAVE);
            /*
             * ---- Wave 21 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
            */
            /* Grep: linux: nr class: soft retpulse — Wave 21 return-pulse honesty (kept) */
            kprintf("linux: nr class: soft retpulse soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retpulse honesty; Soft≠product; not bar3)\n",
                    (unsigned)LINUX_DISPATCH_SOFT_WAVE);
            /* Grep: linux: nr class: soft retmark — Wave 21 mark stamp (kept) */
            kprintf("linux: nr class: soft retmark exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retmark stamp; Soft≠product)\n",
                    (unsigned)LINUX_DISPATCH_SOFT_WAVE);
            /*
             * ---- Wave 22 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
            */
            /* Grep: linux: nr class: soft retphase — Wave 22 return-phase honesty (kept) */
            kprintf("linux: nr class: soft retphase soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retphase honesty; Soft≠product; not bar3)\n",
                    (unsigned)LINUX_DISPATCH_SOFT_WAVE);
            /* Grep: linux: nr class: soft retbadge — Wave 22 badge stamp (kept) */
            kprintf("linux: nr class: soft retbadge exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbadge stamp; Soft≠product)\n",
                    (unsigned)LINUX_DISPATCH_SOFT_WAVE);
/*
 * ---- Wave 23 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
            */
            /* Grep: linux: nr class: soft rettoken — Wave 23 return-token honesty (kept) */
            kprintf("linux: nr class: soft rettoken soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(rettoken honesty; Soft≠product; not bar3)\n",
                    (unsigned)LINUX_DISPATCH_SOFT_WAVE);
            /* Grep: linux: nr class: soft retcrest — Wave 23 crest stamp (kept) */
            kprintf("linux: nr class: soft retcrest exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retcrest stamp; Soft≠product)\n",
                    (unsigned)LINUX_DISPATCH_SOFT_WAVE);
            /*
             * ---- Wave 24 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: linux: nr class: soft retvault — Wave 24 return-vault honesty (kept) */
            kprintf("linux: nr class: soft retvault soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retvault honesty; Soft≠product; not bar3)\n",
                    (unsigned)LINUX_DISPATCH_SOFT_WAVE);
            /* Grep: linux: nr class: soft retbanner — Wave 24 banner stamp (kept) */
            kprintf("linux: nr class: soft retbanner exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbanner stamp; Soft≠product)\n",
                    (unsigned)LINUX_DISPATCH_SOFT_WAVE);
            /*
             * ---- Wave 25 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: linux: nr class: soft retledger — Wave 25 return-ledger honesty (kept) */
            kprintf("linux: nr class: soft retledger soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retledger honesty; Soft≠product; not bar3)\n",
                    (unsigned)LINUX_DISPATCH_SOFT_WAVE);
            /* Grep: linux: nr class: soft retbeacon — Wave 25 beacon stamp (kept) */
            kprintf("linux: nr class: soft retbeacon exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbeacon stamp; Soft≠product)\n",
                    (unsigned)LINUX_DISPATCH_SOFT_WAVE);
            /*
             * ---- Wave 26 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: linux: nr class: soft retcipher — Wave 26 return-cipher honesty (kept) */
            kprintf("linux: nr class: soft retcipher soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retcipher honesty; Soft≠product; not bar3)\n",
                    (unsigned)LINUX_DISPATCH_SOFT_WAVE);
            /* Grep: linux: nr class: soft retflame — Wave 26 flame stamp (kept) */
            kprintf("linux: nr class: soft retflame exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retflame stamp; Soft≠product)\n",
                    (unsigned)LINUX_DISPATCH_SOFT_WAVE);
                    /*
                     * ---- Wave 27 complementary surfaces (kept) (never reshape primary).
                     * Return surfaces only — soft inventory; never hard-gates product paths.
                     * Soft≠product; not bar3.
                     */
                    /* Grep: linux: nr class: soft retprism — Wave 27 return-prism honesty (kept) */
                    kprintf("linux: nr class: soft retprism soft_only=1 product_gate=0 soft_ne_product=1 "
                            "never_blocks_m0=1 wave=%u "
                            "(retprism honesty; Soft≠product; not bar3)\n",
                            (unsigned)LINUX_DISPATCH_SOFT_WAVE);
                    /* Grep: linux: nr class: soft retforge — Wave 27 forge stamp (kept) */
                    kprintf("linux: nr class: soft retforge exclusive=1 soft_ne_product=1 "
                            "product_kernel=OPEN bar3=0 wave=%u "
                            "(retforge stamp; Soft≠product)\n",
                            (unsigned)LINUX_DISPATCH_SOFT_WAVE);
                            /*
                             * ---- Wave 28 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: linux: nr class: soft retshard — Wave 28 return-shard honesty (kept) */
                            kprintf("linux: nr class: soft retshard soft_only=1 product_gate=0 soft_ne_product=1 "
                                "never_blocks_m0=1 wave=%u "
                                "(retshard honesty; Soft≠product; not bar3)\n",
                                (unsigned)LINUX_DISPATCH_SOFT_WAVE);
                            /* Grep: linux: nr class: soft retcrown — Wave 28 crown stamp (kept) */
                            kprintf("linux: nr class: soft retcrown exclusive=1 soft_ne_product=1 "
                                "product_kernel=OPEN bar3=0 wave=%u "
                                "(retcrown stamp; Soft≠product)\n",
                                (unsigned)LINUX_DISPATCH_SOFT_WAVE);
    kprintf("linux: nr class soft deepen wave=%u areas=%u live=%u "
            "table=%u class=%u log_n=%u\n",
            (unsigned)LINUX_DISPATCH_SOFT_WAVE, LINUX_DISPATCH_SOFT_AREAS,
            u32Live, u32Table, u32Class, g_u32SoftLogN);

    /*
     * Twin prefix: linux: dispatch soft … (agent-friendly alias).
     * Grep: linux: dispatch soft inventory
     */
    kprintf("linux: dispatch soft inventory live=%u table=%u hot=%u cold=%u "
            "none=%u class=%u path_bad=%u log_n=%u wave=%u\n",
            u32Live, u32Table, u32Hot, u32Cold, u32None, u32Class,
            g_class.u32PathBad, g_u32SoftLogN,
            (unsigned)LINUX_DISPATCH_SOFT_WAVE);

    /* Grep: linux: dispatch soft init */
    kprintf("linux: dispatch soft init enter=%u ok=%u stats_get=%u "
            "stats_get_null=%u stats_get_idle=%u stats_get_live=%u "
            "stats_reset=%u disp_stats_get=%u disp_stats_null=%u\n",
            g_u32SoftInitEnter, g_u32SoftInitOk, g_u32SoftStatsGet,
            g_u32SoftStatsGetNull, g_u32SoftStatsGetIdle, g_u32SoftStatsGetLive,
            g_u32SoftStatsReset, g_u32SoftDispatchStatsGet,
            g_u32SoftDispatchStatsGetNull);

    /* Grep: linux: dispatch soft classify */
    kprintf("linux: dispatch soft classify enter=%u hot=%u cold=%u none=%u "
            "oor=%u\n",
            g_u32SoftClassify, g_u32SoftClassifyHot, g_u32SoftClassifyCold,
            g_u32SoftClassifyNone, g_u32SoftClassifyOor);

    /* Grep: linux: dispatch soft cold_bind */
    kprintf("linux: dispatch soft cold_bind bind=%u unbind=%u pfn=%u "
            "attached=%u\n",
            g_u32SoftSetColdBind, g_u32SoftSetColdUnbind, u32LegacyBound,
            u32Attached);

    /* Grep: linux: dispatch soft wow64 */
    kprintf("linux: dispatch soft wow64 enter=%u map=%u enabled=%u\n",
            g_u32SoftWow64Enter, g_u32SoftWow64Map, u32Wow64On);

    /* Grep: linux: dispatch soft runtime */
    kprintf("linux: dispatch soft runtime entries=%llu null_guard=%u "
            "path_hot=%u path_cold=%u path_none=%u path_oor=%u "
            "hot_direct=%u hot_defer=%u hot_null=%u "
            "cold_ipc=%u cold_leg=%u cold_bare=%u\n",
            (unsigned long long)g_class.u64Entries, g_u32SoftPathNullGuard,
            g_u32SoftPathHot, g_u32SoftPathCold, g_u32SoftPathNone,
            g_u32SoftPathOor, g_u32SoftPathHotDirect, g_u32SoftPathHotDefer,
            g_u32SoftPathHotNull, g_u32SoftPathColdIpc, g_u32SoftPathColdLeg,
            g_u32SoftPathColdBare);

    /* Grep: linux: dispatch soft hits */
    kprintf("linux: dispatch soft hits hot=%llu cold=%llu enosys=%llu "
            "coarse_h=%llu coarse_c=%llu coarse_e=%llu\n",
            (unsigned long long)g_class.u64HotHits,
            (unsigned long long)g_class.u64ColdHits,
            (unsigned long long)g_class.u64Enosys,
            (unsigned long long)g_stats.u64HotHits,
            (unsigned long long)g_stats.u64ColdHits,
            (unsigned long long)g_stats.u64Enosys);

    /* Grep: linux: dispatch soft path */
    kprintf("linux: dispatch soft path claim=classify+hot+cold_ipc+legacy "
            "hybrid=OptionC wave=%u (soft inventory; not bar3)\n",
            (unsigned)LINUX_DISPATCH_SOFT_WAVE);

    /*
     * Wave 14/15 complementary dispatch twin surfaces.
     * Grep: linux: dispatch soft rates
     */
    kprintf("linux: dispatch soft rates bp_hot=%llu bp_cold=%llu "
            "bp_enosys=%llu hit_sum=%llu bp_ret_neg=%llu wave=%u\n",
            (unsigned long long)u64BpHot, (unsigned long long)u64BpCold,
            (unsigned long long)u64BpEnosys, (unsigned long long)u64HitSum,
            (unsigned long long)u64BpRetNeg,
            (unsigned)LINUX_DISPATCH_SOFT_WAVE);

    /* Grep: linux: dispatch soft ret */
    kprintf("linux: dispatch soft ret neg=%u zero=%u pos=%u note=%u "
            "wave=%u\n",
            g_u32SoftRetNeg, g_u32SoftRetZero, g_u32SoftRetPos,
            g_u32SoftRetNote, (unsigned)LINUX_DISPATCH_SOFT_WAVE);

    /* Grep: linux: dispatch soft last */
    kprintf("linux: dispatch soft last nr=%llu ret=%llu entries=%llu "
            "once=%u wave=%u\n",
            (unsigned long long)g_class.u64LastNr,
            (unsigned long long)g_class.u64LastRet,
            (unsigned long long)g_class.u64Entries,
            g_fSoftInvOnce ? 1u : 0u,
            (unsigned)LINUX_DISPATCH_SOFT_WAVE);

    /* Grep: linux: dispatch soft honesty */
    kprintf("linux: dispatch soft honesty hybrid=OptionC open=1 bar3=0 "
            "product_linux_abi=open soft_only=1 wave=%u "
            "(soft inventory; never closes hybrid)\n",
            (unsigned)LINUX_DISPATCH_SOFT_WAVE);

    /* Grep: linux: dispatch soft edge (Wave 20 deepen) */
    kprintf("linux: dispatch soft edge entries=%llu null_guard=%u "
            "path_hot=%u path_cold=%u path_none=%u path_oor=%u "
            "hot_direct=%u hot_defer=%u cold_ipc=%u wave=%u\n",
            (unsigned long long)g_class.u64Entries, g_u32SoftPathNullGuard,
            g_u32SoftPathHot, g_u32SoftPathCold, g_u32SoftPathNone,
            g_u32SoftPathOor, g_u32SoftPathHotDirect, g_u32SoftPathHotDefer,
            g_u32SoftPathColdIpc, (unsigned)LINUX_DISPATCH_SOFT_WAVE);

    /* Grep: linux: dispatch soft share (Wave 20 deepen) */
    kprintf("linux: dispatch soft share bp_hot=%llu bp_cold=%llu "
            "bp_enosys=%llu bp_ret_neg=%llu pct_class=%u armed_ok=%u "
            "wave=%u\n",
            (unsigned long long)u64BpHot, (unsigned long long)u64BpCold,
            (unsigned long long)u64BpEnosys, (unsigned long long)u64BpRetNeg,
            u32PctClass, u32ArmedOk, (unsigned)LINUX_DISPATCH_SOFT_WAVE);

    /* Grep: linux: dispatch soft surfaces (Wave 20 deepen) */
    kprintf("linux: dispatch soft surfaces count=%u wave=%u "
            "names=inventory,path,rates,ret,last,honesty,edge,share,"
            "catalog,surfaces,note,return,retmap,deepen,PASS\n",
            LINUX_DISPATCH_SOFT_AREAS,
            (unsigned)LINUX_DISPATCH_SOFT_WAVE);

    /* Grep: linux: dispatch soft note (Wave 20 deepen) */
    kprintf("linux: dispatch soft note milestone=wave38 exclusive=1 "
            "soft_only=1 not_bar3=1 wave=%u\n",
            (unsigned)LINUX_DISPATCH_SOFT_WAVE);

    /* Grep: linux: dispatch soft catalog (Wave 20 deepen) */
    kprintf("linux: dispatch soft catalog wave=%u areas=%u "
            "surfaces=inventory,init,classify,cold_bind,wow64,runtime,"
            "hits,path,rates,ret,last,honesty,edge,share,catalog,"
            "deepen,PASS\n",
            (unsigned)LINUX_DISPATCH_SOFT_WAVE, LINUX_DISPATCH_SOFT_AREAS);

    /* Grep: linux: dispatch soft return (Wave 20 deepen) */
    kprintf("linux: dispatch soft return live=%u hot=%u cold=%u none=%u "
            "ret_neg=%u ret_zero=%u ret_pos=%u product_gate=0 wave=%u\n",
            u32Live, u32Hot, u32Cold, u32None,
            g_u32SoftRetNeg, g_u32SoftRetZero, g_u32SoftRetPos,
            (unsigned)LINUX_DISPATCH_SOFT_WAVE);

    /* Grep: linux: dispatch soft deepen */
    /*
     * ---- Wave 19 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft≠product; not bar3.
     */
    /* Grep: linux: dispatch: soft retclass — Wave 19 return-class taxonomy (kept) */
    kprintf("linux: dispatch: soft retclass ok|fail|inval|nodev|busy|nomem "
            "soft_only=1 product_gate=0 wave=%u "
            "(retclass taxonomy; Soft≠product; not bar3)\n",
            (unsigned)LINUX_DISPATCH_SOFT_WAVE);
    /* Grep: linux: dispatch: soft retlane — Wave 19 return-lane catalog (kept) */
    kprintf("linux: dispatch: soft retlane inv|selftest|rate|retcode|retmap|class "
            "product_kernel=OPEN soft_ne_product=1 wave=%u "
            "(retlane catalog; Soft≠product)\n",
            (unsigned)LINUX_DISPATCH_SOFT_WAVE);
    /*
     * ---- Wave 20 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft≠product; not bar3.
     */
    /* Grep: linux: dispatch: soft retbound — Wave 20 return-bound honesty (kept) */
    kprintf("linux: dispatch: soft retbound soft_only=1 product_gate=0 hard_gate=0 "
            "never_blocks_m0=1 wave=%u "
            "(retbound honesty; Soft≠product; not bar3)\n",
            (unsigned)LINUX_DISPATCH_SOFT_WAVE);
    /* Grep: linux: dispatch: soft retseal — Wave 20 seal stamp (kept) */
    kprintf("linux: dispatch: soft retseal exclusive=1 soft_ne_product=1 "
            "product_kernel=OPEN bar3=0 wave=%u "
            "(retseal stamp; Soft≠product)\n",
            (unsigned)LINUX_DISPATCH_SOFT_WAVE);
            /*
             * ---- Wave 21 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
            */
            /* Grep: linux: dispatch: soft retpulse — Wave 21 return-pulse honesty (kept) */
            kprintf("linux: dispatch: soft retpulse soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retpulse honesty; Soft≠product; not bar3)\n",
                    (unsigned)LINUX_DISPATCH_SOFT_WAVE);
            /* Grep: linux: dispatch: soft retmark — Wave 21 mark stamp (kept) */
            kprintf("linux: dispatch: soft retmark exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retmark stamp; Soft≠product)\n",
                    (unsigned)LINUX_DISPATCH_SOFT_WAVE);
            /*
             * ---- Wave 22 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
            */
            /* Grep: linux: dispatch: soft retphase — Wave 22 return-phase honesty (kept) */
            kprintf("linux: dispatch: soft retphase soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retphase honesty; Soft≠product; not bar3)\n",
                    (unsigned)LINUX_DISPATCH_SOFT_WAVE);
            /* Grep: linux: dispatch: soft retbadge — Wave 22 badge stamp (kept) */
            kprintf("linux: dispatch: soft retbadge exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbadge stamp; Soft≠product)\n",
                    (unsigned)LINUX_DISPATCH_SOFT_WAVE);
/*
 * ---- Wave 23 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
            */
            /* Grep: linux: dispatch: soft rettoken — Wave 23 return-token honesty (kept) */
            kprintf("linux: dispatch: soft rettoken soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(rettoken honesty; Soft≠product; not bar3)\n",
                    (unsigned)LINUX_DISPATCH_SOFT_WAVE);
            /* Grep: linux: dispatch: soft retcrest — Wave 23 crest stamp (kept) */
            kprintf("linux: dispatch: soft retcrest exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retcrest stamp; Soft≠product)\n",
                    (unsigned)LINUX_DISPATCH_SOFT_WAVE);
            /*
             * ---- Wave 24 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: linux: dispatch: soft retvault — Wave 24 return-vault honesty (kept) */
            kprintf("linux: dispatch: soft retvault soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retvault honesty; Soft≠product; not bar3)\n",
                    (unsigned)LINUX_DISPATCH_SOFT_WAVE);
            /* Grep: linux: dispatch: soft retbanner — Wave 24 banner stamp (kept) */
            kprintf("linux: dispatch: soft retbanner exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbanner stamp; Soft≠product)\n",
                    (unsigned)LINUX_DISPATCH_SOFT_WAVE);
            /*
             * ---- Wave 25 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: linux: dispatch: soft retledger — Wave 25 return-ledger honesty (kept) */
            kprintf("linux: dispatch: soft retledger soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retledger honesty; Soft≠product; not bar3)\n",
                    (unsigned)LINUX_DISPATCH_SOFT_WAVE);
            /* Grep: linux: dispatch: soft retbeacon — Wave 25 beacon stamp (kept) */
            kprintf("linux: dispatch: soft retbeacon exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbeacon stamp; Soft≠product)\n",
                    (unsigned)LINUX_DISPATCH_SOFT_WAVE);
            /*
             * ---- Wave 26 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: linux: dispatch: soft retcipher — Wave 26 return-cipher honesty (kept) */
            kprintf("linux: dispatch: soft retcipher soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retcipher honesty; Soft≠product; not bar3)\n",
                    (unsigned)LINUX_DISPATCH_SOFT_WAVE);
            /* Grep: linux: dispatch: soft retflame — Wave 26 flame stamp (kept) */
            kprintf("linux: dispatch: soft retflame exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retflame stamp; Soft≠product)\n",
                    (unsigned)LINUX_DISPATCH_SOFT_WAVE);
                    /*
                     * ---- Wave 27 complementary surfaces (kept) (never reshape primary).
                     * Return surfaces only — soft inventory; never hard-gates product paths.
                     * Soft≠product; not bar3.
                     */
                    /* Grep: linux: dispatch: soft retprism — Wave 27 return-prism honesty (kept) */
                    kprintf("linux: dispatch: soft retprism soft_only=1 product_gate=0 soft_ne_product=1 "
                            "never_blocks_m0=1 wave=%u "
                            "(retprism honesty; Soft≠product; not bar3)\n",
                            (unsigned)LINUX_DISPATCH_SOFT_WAVE);
                    /* Grep: linux: dispatch: soft retforge — Wave 27 forge stamp (kept) */
                    kprintf("linux: dispatch: soft retforge exclusive=1 soft_ne_product=1 "
                            "product_kernel=OPEN bar3=0 wave=%u "
                            "(retforge stamp; Soft≠product)\n",
                            (unsigned)LINUX_DISPATCH_SOFT_WAVE);
                            /*
                             * ---- Wave 28 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: linux: dispatch: soft retshard — Wave 28 return-shard honesty (kept) */
                            kprintf("linux: dispatch: soft retshard soft_only=1 product_gate=0 soft_ne_product=1 "
                                "never_blocks_m0=1 wave=%u "
                                "(retshard honesty; Soft≠product; not bar3)\n",
                                (unsigned)LINUX_DISPATCH_SOFT_WAVE);
                            /* Grep: linux: dispatch: soft retcrown — Wave 28 crown stamp (kept) */
                            kprintf("linux: dispatch: soft retcrown exclusive=1 soft_ne_product=1 "
                                "product_kernel=OPEN bar3=0 wave=%u "
                                "(retcrown stamp; Soft≠product)\n",
                                (unsigned)LINUX_DISPATCH_SOFT_WAVE);
                                /*
                             * ---- Wave 29 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: linux: dispatch: soft retglyph — Wave 29 return-glyph honesty (kept) */
                            kprintf("linux: dispatch: soft retglyph soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retglyph honesty; Soft≠product; not bar3)\n",
                                    (unsigned)LINUX_DISPATCH_SOFT_WAVE);
                            /* Grep: linux: dispatch: soft retscepter — Wave 29 scepter stamp (kept) */
                            kprintf("linux: dispatch: soft retscepter exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=%u "
                                    "(retscepter stamp; Soft≠product)\n",
                                    (unsigned)LINUX_DISPATCH_SOFT_WAVE);
                                /*
                             * ---- Wave 30 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: linux: dispatch: soft retsigil — Wave 30 return-sigil honesty (kept) */
                            kprintf("linux: dispatch: soft retsigil soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retsigil honesty; Soft≠product; not bar3)\n",
                                    (unsigned)LINUX_DISPATCH_SOFT_WAVE);
                            /* Grep: linux: dispatch: soft retemblem — Wave 30 emblem stamp (kept) */
                            kprintf("linux: dispatch: soft retemblem exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=%u "
                                    "(retemblem stamp; Soft≠product)\n",
                                    (unsigned)LINUX_DISPATCH_SOFT_WAVE);
                            /*
                             * ---- Wave 31 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: linux: dispatch: soft retaegis — Wave 31 return-aegis honesty (kept) */
                            kprintf("linux: dispatch: soft retaegis soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retaegis honesty; Soft≠product; not bar3)\n",
                                    (unsigned)LINUX_DISPATCH_SOFT_WAVE);
                            /* Grep: linux: dispatch: soft retsigil — Wave 30 return-sigil honesty (kept) */
                            kprintf("linux: dispatch: soft retsigil soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retsigil honesty; Soft≠product; not bar3)\n",
                                    (unsigned)LINUX_DISPATCH_SOFT_WAVE);
                            /* Grep: linux: dispatch: soft retmantle — Wave 31 mantle stamp (kept) */
                            kprintf("linux: dispatch: soft retmantle exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=%u "
                                    "(retmantle stamp; Soft≠product)\n",
                                    (unsigned)LINUX_DISPATCH_SOFT_WAVE);
/*
 * ---- Wave 32 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: linux: dispatch: soft retbulwark — Wave 32 return-bulwark honesty (kept) */
kprintf("linux: dispatch: soft retbulwark soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbulwark honesty; Soft≠product; not bar3)\n",
        (unsigned)LINUX_DISPATCH_SOFT_WAVE);
/* Grep: linux: dispatch: soft retpanoply — Wave 32 panoply stamp (kept) */
kprintf("linux: dispatch: soft retpanoply exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retpanoply stamp; Soft≠product)\n",
        (unsigned)LINUX_DISPATCH_SOFT_WAVE);
/*
 * ---- Wave 33 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: linux: dispatch: soft retbastion — Wave 33 return-bastion honesty (kept) */
kprintf("linux: dispatch: soft retbastion soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbastion honesty; Soft≠product; not bar3)\n",
        (unsigned)LINUX_DISPATCH_SOFT_WAVE);
/* Grep: linux: dispatch: soft retcitadel — Wave 33 citadel stamp (kept) */
kprintf("linux: dispatch: soft retcitadel exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retcitadel stamp; Soft≠product)\n",
        (unsigned)LINUX_DISPATCH_SOFT_WAVE);
/*
 * ---- Wave 34 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: linux: dispatch: soft retredoubt — Wave 34 return-redoubt honesty */
kprintf("linux: dispatch: soft retredoubt soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retredoubt honesty; Soft≠product; not bar3)\n",
        (unsigned)LINUX_DISPATCH_SOFT_WAVE);
/* Grep: linux: dispatch: soft retkeep — Wave 34 exclusive keep stamp */
kprintf("linux: dispatch: soft retkeep exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retkeep stamp; Soft≠product)\n",
        (unsigned)LINUX_DISPATCH_SOFT_WAVE);
/*
 * ---- Wave 35 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: linux: dispatch: soft retfortress — Wave 35 return-fortress honesty */
kprintf("linux: dispatch: soft retfortress soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retfortress honesty; Soft≠product; not bar3)\n",
        (unsigned)LINUX_DISPATCH_SOFT_WAVE);
/* Grep: linux: dispatch: soft retpalace — Wave 35 exclusive palace stamp */
kprintf("linux: dispatch: soft retpalace exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retpalace stamp; Soft≠product)\n",
        (unsigned)LINUX_DISPATCH_SOFT_WAVE);
/*
 * ---- Wave 36 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: linux: dispatch: soft rethold — Wave 36 return-hold honesty */
kprintf("linux: dispatch: soft rethold soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(rethold honesty; Soft≠product; not bar3)\n",
        (unsigned)LINUX_DISPATCH_SOFT_WAVE);
/* Grep: linux: dispatch: soft retspire — Wave 36 exclusive spire stamp */
kprintf("linux: dispatch: soft retspire exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retspire stamp; Soft≠product)\n",
        (unsigned)LINUX_DISPATCH_SOFT_WAVE);
/*
 * ---- Wave 37 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: linux: dispatch: soft retwall — Wave 37 return-wall honesty */
kprintf("linux: dispatch: soft retwall soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retwall honesty; Soft≠product; not bar3)\n",
        (unsigned)LINUX_DISPATCH_SOFT_WAVE);
/* Grep: linux: dispatch: soft retgate — Wave 37 exclusive gate stamp */
kprintf("linux: dispatch: soft retgate exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retgate stamp; Soft≠product)\n",
        (unsigned)LINUX_DISPATCH_SOFT_WAVE);
/*
 * ---- Wave 38 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: linux: dispatch: soft retmoat — Wave 38 return-moat honesty */
kprintf("linux: dispatch: soft retmoat soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retmoat honesty; Soft≠product; not bar3)\n",
        (unsigned)LINUX_DISPATCH_SOFT_WAVE);
/* Grep: linux: dispatch: soft retower — Wave 38 exclusive tower stamp */
kprintf("linux: dispatch: soft retower exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retower stamp; Soft≠product)\n",
        (unsigned)LINUX_DISPATCH_SOFT_WAVE);
                            kprintf("linux: dispatch soft deepen wave=%u areas=%u live=%u "
            "table=%u hot=%u cold=%u none=%u log_n=%u\n",
            (unsigned)LINUX_DISPATCH_SOFT_WAVE, LINUX_DISPATCH_SOFT_AREAS,
            u32Live, u32Table, u32Hot, u32Cold, u32None, g_u32SoftLogN);

    /* Grep: linux: dispatch soft PASS / linux: dispatch soft inventory PASS */
    kprintf("linux: dispatch soft inventory PASS wave=%u live=%u "
            "class=%u log_n=%u\n",
            (unsigned)LINUX_DISPATCH_SOFT_WAVE, u32Live, u32Class,
            g_u32SoftLogN);
    kprintf("linux: dispatch soft PASS wave=%u log_n=%u\n",
            (unsigned)LINUX_DISPATCH_SOFT_WAVE, g_u32SoftLogN);
}

/**
 * After first product dispatch/classify activity, print soft inventory once
 * (mirrors native/wow64 soft-stats-once). Diagnostics only — never gates.
 */
static void
soft_inventory_maybe_once(void)
{
    if (g_fSoftInvOnce != 0) {
        return;
    }
    if (g_class.u64Entries == 0 && g_u32SoftClassify == 0 &&
        g_u32SoftPathNullGuard == 0) {
        return;
    }
    g_fSoftInvOnce = 1;
    /*
     * Full soft_log path (legacy + deepen). Safe only when live; otherwise
     * emit idle deepen twin only.
     */
    if (g_fNrClassLive) {
        gj_linux_nr_class_soft_log();
    } else {
        soft_inc(&g_u32SoftLogN);
        kprintf("linux: dispatch soft idle (dispatch not init)\n");
    }
}

void
gj_linux_dispatch_init(void)
{
    u32 iNr;

    soft_inc(&g_u32SoftInitEnter);

    /* Unpublished until registration + scan complete (PATH_HOT=0 BSS trap). */
    g_fNrClassLive = 0;
    g_class.u32Live = 0;
    g_fSoftInvOnce = 0;

    for (iNr = 0; iNr < GJ_LINUX_NR_TABLE; iNr++) {
        g_apfnHot[iNr] = NULL;
        g_aPath[iNr] = (u8)GJ_LINUX_PATH_NONE;
    }
    g_pfnCold = NULL;
    g_pColdCtx = NULL;
    g_stats.u64HotHits = 0;
    g_stats.u64ColdHits = 0;
    g_stats.u64Enosys = 0;

    /* Classification coverage counters (set_hot/set_cold deepen). */
    g_class.u32TableSize = GJ_LINUX_NR_TABLE;
    g_class.u32HotSlots = 0;
    g_class.u32ColdSlots = 0;
    g_class.u32NoneSlots = 0;
    g_class.u32Classified = 0;
    g_class.u32MaxHotNr = 0;
    g_class.u32MaxColdNr = 0;
    g_class.u32MaxClassNr = 0;
    g_class.u32SetHotOk = 0;
    g_class.u32SetColdOk = 0;
    g_class.u32SetHotReject = 0;
    g_class.u32SetColdReject = 0;
    g_class.u32HotArmed = 0;
    g_class.u32HotNullSlots = 0;
    g_class.u32PathBad = 0;
    g_class.u64HotHits = 0;
    g_class.u64ColdHits = 0;
    g_class.u64Enosys = 0;
    g_class.u64HotDeferCold = 0;
    g_class.u64HotNull = 0;
    g_class.u64Oor = 0;
    g_class.u64NonePath = 0;
    g_class.u64Entries = 0;
    g_class.u64NullGuard = 0;
    g_class.u64ColdIpc = 0;
    g_class.u64ColdLegacy = 0;
    g_class.u64ColdBare = 0;
    g_class.u64LastNr = 0;
    g_class.u64LastRet = 0;

    /* ---- Hot paths (kernel) ------------------------------------------- */
    set_hot(LINUX_NR_write, gj_linux_hot_write);
    set_hot(LINUX_NR_writev, gj_linux_hot_writev);
    set_hot(LINUX_NR_readv, gj_linux_hot_readv);
    set_hot(LINUX_NR_preadv, gj_linux_hot_preadv);
    set_hot(LINUX_NR_pwritev, gj_linux_hot_pwritev);
    set_hot(LINUX_NR_preadv2, gj_linux_hot_preadv);
    set_hot(LINUX_NR_pwritev2, gj_linux_hot_pwritev);
    set_hot(LINUX_NR_pread64, gj_linux_hot_pread64);
    set_hot(LINUX_NR_pwrite64, gj_linux_hot_pwrite64);
    set_hot(LINUX_NR_tkill, gj_linux_hot_tkill);
    set_hot(LINUX_NR_tgkill, gj_linux_hot_tgkill);
    set_hot(LINUX_NR_sigaltstack, gj_linux_hot_sigaltstack);
    set_hot(LINUX_NR_sched_yield, gj_linux_hot_sched_yield);
    set_hot(LINUX_NR_sched_getscheduler, gj_linux_hot_sched_getscheduler);
    set_hot(LINUX_NR_sched_setscheduler, gj_linux_hot_sched_setscheduler);
    set_hot(LINUX_NR_sched_getparam, gj_linux_hot_sched_getparam);
    set_hot(LINUX_NR_sched_setparam, gj_linux_hot_sched_setparam);
    set_hot(LINUX_NR_sched_get_priority_max, gj_linux_hot_sched_get_priority_max);
    set_hot(LINUX_NR_sched_get_priority_min, gj_linux_hot_sched_get_priority_min);
    set_hot(LINUX_NR_sched_setaffinity, gj_linux_hot_sched_setaffinity);
    set_hot(LINUX_NR_sched_getaffinity, gj_linux_hot_sched_getaffinity);
    set_hot(LINUX_NR_sched_setattr, gj_linux_hot_sched_setattr);
    set_hot(LINUX_NR_sched_getattr, gj_linux_hot_sched_getattr);
    set_hot(LINUX_NR_msync, gj_linux_hot_msync);
    set_hot(LINUX_NR_mincore, gj_linux_hot_mincore);
    set_hot(LINUX_NR_getpid, gj_linux_hot_getpid);
    set_hot(LINUX_NR_gettid, gj_linux_hot_gettid);
    set_hot(LINUX_NR_getuid, gj_linux_hot_getuid);
    set_hot(LINUX_NR_getgid, gj_linux_hot_getgid);
    set_hot(LINUX_NR_geteuid, gj_linux_hot_geteuid);
    set_hot(LINUX_NR_getegid, gj_linux_hot_getegid);
    set_hot(LINUX_NR_setuid, gj_linux_hot_setuid);
    set_hot(LINUX_NR_setgid, gj_linux_hot_setgid);
    set_hot(LINUX_NR_setreuid, gj_linux_hot_setreuid);
    set_hot(LINUX_NR_setregid, gj_linux_hot_setregid);
    set_hot(LINUX_NR_setresuid, gj_linux_hot_setresuid);
    set_hot(LINUX_NR_setresgid, gj_linux_hot_setresgid);
    set_hot(LINUX_NR_getresuid, gj_linux_hot_getresuid);
    set_hot(LINUX_NR_getresgid, gj_linux_hot_getresgid);
    set_hot(LINUX_NR_setpgid, gj_linux_hot_setpgid);
    set_hot(LINUX_NR_setsid, gj_linux_hot_setsid);
    set_hot(LINUX_NR_setgroups, gj_linux_hot_setgroups);
    set_hot(LINUX_NR_getgroups, gj_linux_hot_getgroups);
    set_hot(LINUX_NR_getpgrp, gj_linux_hot_getpgrp);
    set_hot(LINUX_NR_getppid, gj_linux_hot_getppid);
    set_hot(LINUX_NR_getsid, gj_linux_hot_getsid);
    set_hot(LINUX_NR_mlock, gj_linux_hot_mlock);
    set_hot(LINUX_NR_munlock, gj_linux_hot_munlock);
    set_hot(LINUX_NR_mlockall, gj_linux_hot_mlockall);
    set_hot(LINUX_NR_munlockall, gj_linux_hot_munlockall);
    set_hot(LINUX_NR_mlock2, gj_linux_hot_mlock);
    set_hot(LINUX_NR_mbind, gj_linux_hot_mbind);
    set_hot(LINUX_NR_set_mempolicy, gj_linux_hot_set_mempolicy);
    set_hot(LINUX_NR_get_mempolicy, gj_linux_hot_get_mempolicy);
    set_hot(LINUX_NR_pkey_mprotect, gj_linux_hot_mprotect);
    set_hot(LINUX_NR_pkey_alloc, gj_linux_hot_pkey_alloc);
    set_hot(LINUX_NR_pkey_free, gj_linux_hot_pkey_free);
    set_hot(LINUX_NR_personality, gj_linux_hot_personality);
    set_hot(LINUX_NR_exit, gj_linux_hot_exit);
    set_hot(LINUX_NR_exit_group, gj_linux_hot_exit_group);
    set_hot(LINUX_NR_brk, gj_linux_hot_brk);
    set_hot(LINUX_NR_mmap, gj_linux_hot_mmap);
    set_hot(LINUX_NR_munmap, gj_linux_hot_munmap);
    set_hot(LINUX_NR_mremap, gj_linux_hot_mremap);
    set_hot(LINUX_NR_mprotect, gj_linux_hot_mprotect);
    set_hot(LINUX_NR_clock_gettime, gj_linux_hot_clock_gettime);
    set_hot(LINUX_NR_clock_getres, gj_linux_hot_clock_getres);
    set_hot(LINUX_NR_clock_nanosleep, gj_linux_hot_clock_nanosleep);
    set_hot(LINUX_NR_clock_settime, gj_linux_hot_clock_settime);
    set_hot(LINUX_NR_clock_adjtime, gj_linux_hot_clock_adjtime);
    set_hot(LINUX_NR_nanosleep, gj_linux_hot_nanosleep);
    set_hot(LINUX_NR_gettimeofday, gj_linux_hot_gettimeofday);
    set_hot(LINUX_NR_settimeofday, gj_linux_hot_settimeofday);
    set_hot(LINUX_NR_time, gj_linux_hot_time);
    set_hot(LINUX_NR_futex, gj_linux_hot_futex);
    set_hot(LINUX_NR_futex_wait, gj_linux_hot_futex_wait2);
    set_hot(LINUX_NR_futex_wake, gj_linux_hot_futex_wake2);
    set_hot(LINUX_NR_futex_requeue, gj_linux_hot_futex);
    set_hot(LINUX_NR_arch_prctl, gj_linux_hot_arch_prctl);
    set_hot(LINUX_NR_set_tid_address, gj_linux_hot_set_tid_address);
    set_hot(LINUX_NR_sysinfo, gj_linux_hot_sysinfo);
    set_hot(LINUX_NR_times, gj_linux_hot_times);
    set_hot(LINUX_NR_getrusage, gj_linux_hot_getrusage);
    set_hot(LINUX_NR_getpriority, gj_linux_hot_getpriority);
    set_hot(LINUX_NR_setpriority, gj_linux_hot_setpriority);
    set_hot(LINUX_NR_uname, gj_linux_hot_uname);
    set_hot(LINUX_NR_getrandom, gj_linux_hot_getrandom);
    set_hot(LINUX_NR_getsockopt, gj_linux_hot_getsockopt);
    set_hot(LINUX_NR_setsockopt, gj_linux_hot_setsockopt);
    set_hot(LINUX_NR_getsockname, gj_linux_hot_getsockname);
    set_hot(LINUX_NR_getpeername, gj_linux_hot_getpeername);
    set_hot(LINUX_NR_shutdown, gj_linux_hot_shutdown);
    set_hot(LINUX_NR_rt_sigaction, gj_linux_hot_rt_sigaction);
    set_hot(LINUX_NR_rt_sigprocmask, gj_linux_hot_rt_sigprocmask);
    set_hot(LINUX_NR_rt_sigreturn, gj_linux_hot_rt_sigreturn);
    set_hot(LINUX_NR_getitimer, gj_linux_hot_getitimer);
    set_hot(LINUX_NR_setitimer, gj_linux_hot_setitimer);
    set_hot(LINUX_NR_alarm, gj_linux_hot_alarm);
    set_hot(LINUX_NR_pause, gj_linux_hot_pause);
    set_hot(LINUX_NR_prctl, gj_linux_hot_prctl);
    set_hot(LINUX_NR_set_robust_list, gj_linux_hot_set_robust_list);
    set_hot(LINUX_NR_get_robust_list, gj_linux_hot_get_robust_list);
    set_hot(LINUX_NR_waitid, gj_linux_hot_waitid);
    set_hot(LINUX_NR_process_vm_readv, gj_linux_hot_process_vm_readv);
    set_hot(LINUX_NR_process_vm_writev, gj_linux_hot_process_vm_writev);
    set_hot(LINUX_NR_membarrier, gj_linux_hot_membarrier);
    set_hot(LINUX_NR_rseq, gj_linux_hot_rseq);
    set_hot(LINUX_NR_capget, gj_linux_hot_capget);
    set_hot(LINUX_NR_capset, gj_linux_hot_capset);
    set_hot(LINUX_NR_getcpu, gj_linux_hot_getcpu);

    /* ---- Cold paths (doors / libprotonrt personality) ------------------ */
    set_cold(LINUX_NR_read);
    set_cold(LINUX_NR_open);
    set_cold(LINUX_NR_close);
    set_cold(LINUX_NR_openat);
    set_cold(LINUX_NR_stat);
    set_cold(LINUX_NR_fstat);
    set_cold(LINUX_NR_newfstatat);
    set_cold(LINUX_NR_statx);
    set_cold(LINUX_NR_statfs);
    set_cold(LINUX_NR_fstatfs);
    set_cold(LINUX_NR_openat2);
    set_cold(LINUX_NR_faccessat2);
    set_cold(LINUX_NR_ioctl);
    set_cold(LINUX_NR_poll);
    set_cold(LINUX_NR_ppoll);
    set_cold(LINUX_NR_epoll_create);
    set_cold(LINUX_NR_epoll_create1);
    set_cold(LINUX_NR_io_setup);
    set_cold(LINUX_NR_io_destroy);
    set_cold(LINUX_NR_io_submit);
    set_cold(LINUX_NR_io_getevents);
    set_cold(LINUX_NR_io_cancel);
    set_cold(LINUX_NR_epoll_ctl);
    set_cold(LINUX_NR_epoll_wait);
    set_cold(LINUX_NR_epoll_pwait);
    set_cold(LINUX_NR_getdents64);
    set_cold(LINUX_NR_memfd_create);
    set_cold(LINUX_NR_socket);
    set_cold(LINUX_NR_connect);
    set_cold(LINUX_NR_bind);
    set_cold(LINUX_NR_listen);
    set_cold(LINUX_NR_accept);
    set_cold(LINUX_NR_accept4);
    set_cold(LINUX_NR_sendfile);
    set_cold(LINUX_NR_fallocate);
    set_cold(LINUX_NR_sendto);
    set_cold(LINUX_NR_recvfrom);
    set_cold(LINUX_NR_sendmsg);
    set_cold(LINUX_NR_recvmsg);
    set_cold(LINUX_NR_sendmmsg);
    set_cold(LINUX_NR_recvmmsg);
    set_cold(LINUX_NR_dup3);
    set_cold(LINUX_NR_clone);
    set_cold(LINUX_NR_clone3);
    set_cold(LINUX_NR_fork);
    set_cold(LINUX_NR_vfork);
    set_cold(LINUX_NR_execve);
    set_cold(LINUX_NR_execveat);
    set_cold(LINUX_NR_pidfd_open);
    set_cold(LINUX_NR_pidfd_send_signal);
    set_cold(LINUX_NR_close_range);
    set_cold(LINUX_NR_fcntl);
    set_cold(LINUX_NR_fsync);
    set_cold(LINUX_NR_fdatasync);
    set_cold(LINUX_NR_sync);
    set_cold(LINUX_NR_syncfs);
    set_cold(LINUX_NR_inotify_init1);
    set_cold(LINUX_NR_inotify_add_watch);
    set_cold(LINUX_NR_inotify_rm_watch);
    set_cold(LINUX_NR_renameat);
    set_cold(LINUX_NR_renameat2);
    set_cold(LINUX_NR_linkat);
    set_cold(LINUX_NR_symlinkat);
    set_cold(LINUX_NR_copy_file_range);
    set_cold(LINUX_NR_io_pgetevents);
    set_cold(LINUX_NR_userfaultfd);
    set_cold(LINUX_NR_seccomp);
    set_cold(LINUX_NR_bpf);
    set_cold(LINUX_NR_keyctl);
    set_cold(LINUX_NR_add_key);
    set_cold(LINUX_NR_request_key);
    set_cold(LINUX_NR_name_to_handle_at);
    set_cold(LINUX_NR_open_by_handle_at);
    /*
     * io_uring stays cold: min ring impl lives in protonrt cold path
     * (io_uring_min.c via protonrt_cold_link). Do not promote to hot.
     */
    set_cold(LINUX_NR_io_uring_setup);
    set_cold(LINUX_NR_io_uring_enter);
    set_cold(LINUX_NR_io_uring_register);
    set_cold(LINUX_NR_open_tree);
    set_cold(LINUX_NR_move_mount);
    set_cold(LINUX_NR_fsopen);
    set_cold(LINUX_NR_fsconfig);
    set_cold(LINUX_NR_fsmount);
    set_cold(LINUX_NR_fspick);
    set_cold(LINUX_NR_mount_setattr);
    set_cold(LINUX_NR_quotactl_fd);
    set_cold(LINUX_NR_pidfd_getfd);
    set_cold(LINUX_NR_process_madvise);
    set_cold(LINUX_NR_epoll_pwait2);
    set_cold(LINUX_NR_fanotify_init);
    set_cold(LINUX_NR_fanotify_mark);
    set_cold(LINUX_NR_kcmp);
    set_cold(LINUX_NR_quotactl);
    set_cold(LINUX_NR_remap_file_pages);
    set_cold(LINUX_NR_restart_syscall);
    set_cold(LINUX_NR_migrate_pages);
    set_cold(LINUX_NR_move_pages);
    set_cold(LINUX_NR_perf_event_open);
    set_cold(LINUX_NR_timer_create);
    set_cold(LINUX_NR_timer_settime);
    set_cold(LINUX_NR_timer_gettime);
    set_cold(LINUX_NR_timer_getoverrun);
    set_cold(LINUX_NR_timer_delete);
    set_cold(LINUX_NR_mq_open);
    set_cold(LINUX_NR_mq_unlink);
    set_cold(LINUX_NR_mq_timedsend);
    set_cold(LINUX_NR_mq_timedreceive);
    set_cold(LINUX_NR_mq_notify);
    set_cold(LINUX_NR_mq_getsetattr);
    set_cold(LINUX_NR_inotify_init);
    set_cold(LINUX_NR_splice);
    set_cold(LINUX_NR_tee);
    set_cold(LINUX_NR_pipe);
    set_cold(LINUX_NR_pipe2);
    set_cold(LINUX_NR_socketpair);
    set_cold(LINUX_NR_eventfd2);
    set_cold(LINUX_NR_eventfd);
    set_cold(LINUX_NR_timerfd_create);
    set_cold(LINUX_NR_timerfd_settime);
    set_cold(LINUX_NR_timerfd_gettime);
    set_cold(LINUX_NR_signalfd4);
    set_cold(LINUX_NR_prlimit64);
    set_cold(LINUX_NR_getrlimit);
    set_cold(LINUX_NR_setrlimit);
    set_cold(LINUX_NR_unshare);
    set_cold(LINUX_NR_setns);
    set_cold(LINUX_NR_chroot);
    set_cold(LINUX_NR_mount);
    set_cold(LINUX_NR_umount2);
    set_cold(LINUX_NR_pivot_root);
    set_cold(LINUX_NR_swapon);
    set_cold(LINUX_NR_swapoff);
    set_cold(LINUX_NR_reboot);
    set_cold(LINUX_NR_sethostname);
    set_cold(LINUX_NR_setdomainname);
    set_cold(LINUX_NR_syslog);
    set_cold(LINUX_NR_ustat);
    set_cold(LINUX_NR_sysfs);
    set_cold(LINUX_NR_bdflush);
    set_cold(LINUX_NR_readahead);
    set_cold(LINUX_NR_sync_file_range);
    set_cold(LINUX_NR_vmsplice);
    set_cold(LINUX_NR_getdents);
    set_cold(LINUX_NR_rt_sigsuspend);
    set_cold(LINUX_NR_rt_sigtimedwait);
    set_cold(LINUX_NR_rt_sigpending);
    set_cold(LINUX_NR_rt_sigqueueinfo);
    set_cold(LINUX_NR_sched_rr_get_interval);
    set_cold(LINUX_NR_landlock_create_ruleset);
    set_cold(LINUX_NR_landlock_add_rule);
    set_cold(LINUX_NR_landlock_restrict_self);
    set_cold(LINUX_NR_memfd_secret);
    set_cold(LINUX_NR_process_mrelease);
    set_cold(LINUX_NR_cachestat);
    set_cold(LINUX_NR_fchmodat2);
    set_cold(LINUX_NR_futex_waitv);
    set_cold(LINUX_NR_set_mempolicy_home_node);
    set_cold(LINUX_NR_map_shadow_stack);
    set_cold(LINUX_NR_madvise);
    set_cold(LINUX_NR_lseek);
    set_cold(LINUX_NR_getcwd);
    set_cold(LINUX_NR_chdir);
    set_cold(LINUX_NR_fchdir);
    set_cold(LINUX_NR_select);
    set_cold(LINUX_NR_pselect6);
    set_cold(LINUX_NR_symlink);
    set_cold(LINUX_NR_readlinkat);
    set_cold(LINUX_NR_fchmodat);
    set_cold(LINUX_NR_getxattr);
    set_cold(LINUX_NR_lgetxattr);
    set_cold(LINUX_NR_fgetxattr);
    set_cold(LINUX_NR_setxattr);
    set_cold(LINUX_NR_lsetxattr);
    set_cold(LINUX_NR_fsetxattr);
    set_cold(LINUX_NR_listxattr);
    set_cold(LINUX_NR_llistxattr);
    set_cold(LINUX_NR_flistxattr);
    set_cold(LINUX_NR_removexattr);
    set_cold(LINUX_NR_lremovexattr);
    set_cold(LINUX_NR_fremovexattr);
    set_cold(LINUX_NR_utime);
    set_cold(LINUX_NR_utimensat);
    set_cold(LINUX_NR_mkdir);
    set_cold(LINUX_NR_mkdirat);
    set_cold(LINUX_NR_rmdir);
    set_cold(LINUX_NR_link);
    set_cold(LINUX_NR_unlink);
    set_cold(LINUX_NR_unlinkat);
    set_cold(LINUX_NR_creat);
    set_cold(LINUX_NR_flock);
    set_cold(LINUX_NR_fchmod);
    set_cold(LINUX_NR_fchown);
    set_cold(LINUX_NR_access);
    set_cold(LINUX_NR_faccessat);
    set_cold(LINUX_NR_readlink);
    set_cold(LINUX_NR_chmod);
    set_cold(LINUX_NR_rename);
    set_cold(LINUX_NR_ftruncate);
    set_cold(LINUX_NR_dup);
    set_cold(LINUX_NR_dup2);
    set_cold(LINUX_NR_wait4);
    set_cold(LINUX_NR_kill);
    set_cold(LINUX_NR_lstat);

    /* Authoritative slot scan + publish live + greppable soft coverage. */
    nr_class_scan_slots();
    g_fNrClassLive = 1;
    g_class.u32Live = 1;
    soft_inc(&g_u32SoftInitOk);
    gj_linux_nr_class_soft_log();
}

void
gj_linux_set_cold_handler(gj_linux_cold_fn_t pfn, void *pCtx)
{
    g_pfnCold = pfn;
    g_pColdCtx = pCtx;
    if (pfn != NULL) {
        soft_inc(&g_u32SoftSetColdBind);
    } else {
        soft_inc(&g_u32SoftSetColdUnbind);
    }
}

enum gj_linux_path
gj_linux_classify(u64 u64Nr)
{
    enum gj_linux_path ePath;

    soft_inc(&g_u32SoftClassify);
    if (u64Nr >= GJ_LINUX_NR_TABLE) {
        soft_inc(&g_u32SoftClassifyOor);
        soft_inventory_maybe_once();
        return GJ_LINUX_PATH_NONE;
    }
    ePath = (enum gj_linux_path)g_aPath[(u32)u64Nr];
    if (ePath == GJ_LINUX_PATH_HOT) {
        soft_inc(&g_u32SoftClassifyHot);
    } else if (ePath == GJ_LINUX_PATH_COLD) {
        soft_inc(&g_u32SoftClassifyCold);
    } else {
        soft_inc(&g_u32SoftClassifyNone);
    }
    soft_inventory_maybe_once();
    return ePath;
}

void
gj_linux_syscall_dispatch(struct gj_linux_regs *pRegs)
{
    enum gj_linux_path ePath;
    gj_linux_hot_fn_t pfnHot;
    u32 u32Nr;

    if (pRegs == NULL) {
        g_class.u64NullGuard++;
        soft_inc(&g_u32SoftPathNullGuard);
        soft_inventory_maybe_once();
        return;
    }
    g_class.u64Entries++;
    g_class.u64LastNr = pRegs->u64Nr;
    pRegs->i64Ret = -LINUX_ENOSYS;

    /* WoW64: map i386 NR → x86_64 NR; zero-extend lower 32 of args */
    if (wow64_enabled()) {
        u32 u32Nr64;

        soft_inc(&g_u32SoftWow64Enter);
        if (wow64_translate_nr((u32)pRegs->u64Nr, &u32Nr64) == 0) {
            pRegs->u64Nr = u32Nr64;
            g_class.u64LastNr = pRegs->u64Nr;
            soft_inc(&g_u32SoftWow64Map);
        }
        pRegs->u64Arg0 &= 0xffffffffull;
        pRegs->u64Arg1 &= 0xffffffffull;
        pRegs->u64Arg2 &= 0xffffffffull;
        pRegs->u64Arg3 &= 0xffffffffull;
        pRegs->u64Arg4 &= 0xffffffffull;
        pRegs->u64Arg5 &= 0xffffffffull;
    }

    if (pRegs->u64Nr >= GJ_LINUX_NR_TABLE) {
        g_stats.u64Enosys++;
        g_class.u64Enosys++;
        g_class.u64Oor++;
        soft_inc(&g_u32SoftPathOor);
        g_class.u64LastRet = (u64)pRegs->i64Ret;
        soft_note_ret(pRegs->i64Ret);
        soft_inventory_maybe_once();
        return;
    }
    u32Nr = (u32)pRegs->u64Nr;
    ePath = (enum gj_linux_path)g_aPath[u32Nr];

    if (ePath == GJ_LINUX_PATH_HOT) {
        soft_inc(&g_u32SoftPathHot);
        pfnHot = g_apfnHot[u32Nr];
        if (pfnHot != NULL) {
            pRegs->i64Ret = pfnHot(pRegs);
            /* Hot may return -ENOSYS to defer (e.g. non-anon mmap). */
            if (pRegs->i64Ret != -LINUX_ENOSYS) {
                g_stats.u64HotHits++;
                g_class.u64HotHits++;
                soft_inc(&g_u32SoftPathHotDirect);
                g_class.u64LastRet = (u64)pRegs->i64Ret;
                soft_note_ret(pRegs->i64Ret);
                soft_inventory_maybe_once();
                return;
            }
            g_class.u64HotDeferCold++;
            soft_inc(&g_u32SoftPathHotDefer);
        } else {
            /* HOT slot without fn → defer (table inconsistency). */
            g_class.u64HotNull++;
            soft_inc(&g_u32SoftPathHotNull);
        }
        /* Fall through to cold personality. */
    } else if (ePath == GJ_LINUX_PATH_COLD) {
        soft_inc(&g_u32SoftPathCold);
    } else {
        soft_inc(&g_u32SoftPathNone);
    }

    if (ePath == GJ_LINUX_PATH_COLD || ePath == GJ_LINUX_PATH_HOT) {
        /* Doors / sync service via cold_ipc (libprotonrt). */
        if (cold_ipc_personality_attached()) {
            pRegs->i64Ret = cold_ipc_submit(pRegs, 0);
            g_stats.u64ColdHits++;
            g_class.u64ColdHits++;
            g_class.u64ColdIpc++;
            soft_inc(&g_u32SoftPathColdIpc);
            g_class.u64LastRet = (u64)pRegs->i64Ret;
            soft_note_ret(pRegs->i64Ret);
            soft_inventory_maybe_once();
            return;
        }
        /* Legacy direct cold hook (tests / early bring-up). */
        if (g_pfnCold != NULL) {
            pRegs->i64Ret = g_pfnCold(pRegs, g_pColdCtx);
            g_stats.u64ColdHits++;
            g_class.u64ColdHits++;
            g_class.u64ColdLegacy++;
            soft_inc(&g_u32SoftPathColdLeg);
            g_class.u64LastRet = (u64)pRegs->i64Ret;
            soft_note_ret(pRegs->i64Ret);
            soft_inventory_maybe_once();
            return;
        }
        g_stats.u64Enosys++;
        g_class.u64Enosys++;
        g_class.u64ColdBare++;
        soft_inc(&g_u32SoftPathColdBare);
        pRegs->i64Ret = -LINUX_ENOSYS;
        g_class.u64LastRet = (u64)pRegs->i64Ret;
        soft_note_ret(pRegs->i64Ret);
        soft_inventory_maybe_once();
        return;
    }

    g_stats.u64Enosys++;
    g_class.u64Enosys++;
    g_class.u64NonePath++;
    pRegs->i64Ret = -LINUX_ENOSYS;
    g_class.u64LastRet = (u64)pRegs->i64Ret;
    soft_note_ret(pRegs->i64Ret);
    soft_inventory_maybe_once();
}

void
gj_linux_dispatch_stats_get(struct gj_linux_dispatch_stats *pOut)
{
    soft_inc(&g_u32SoftDispatchStatsGet);
    if (pOut == NULL) {
        soft_inc(&g_u32SoftDispatchStatsGetNull);
        return;
    }
    *pOut = g_stats;
}

void
gj_linux_dispatch_stats_reset(void)
{
    soft_inc(&g_u32SoftStatsReset);
    g_stats.u64HotHits = 0;
    g_stats.u64ColdHits = 0;
    g_stats.u64Enosys = 0;
    /* Runtime class counters only; slot coverage stays post-init. */
    g_class.u64HotHits = 0;
    g_class.u64ColdHits = 0;
    g_class.u64Enosys = 0;
    g_class.u64HotDeferCold = 0;
    g_class.u64HotNull = 0;
    g_class.u64Oor = 0;
    g_class.u64NonePath = 0;
    g_class.u64Entries = 0;
    g_class.u64NullGuard = 0;
    g_class.u64ColdIpc = 0;
    g_class.u64ColdLegacy = 0;
    g_class.u64ColdBare = 0;
    g_class.u64LastNr = 0;
    g_class.u64LastRet = 0;
    /* Wave 12/14: runtime path tallies only; registration inventory stays. */
    g_u32SoftPathHot = 0;
    g_u32SoftPathCold = 0;
    g_u32SoftPathNone = 0;
    g_u32SoftPathHotDirect = 0;
    g_u32SoftPathHotDefer = 0;
    g_u32SoftPathHotNull = 0;
    g_u32SoftPathOor = 0;
    g_u32SoftPathNullGuard = 0;
    g_u32SoftPathColdIpc = 0;
    g_u32SoftPathColdLeg = 0;
    g_u32SoftPathColdBare = 0;
    g_u32SoftWow64Enter = 0;
    g_u32SoftWow64Map = 0;
    /* Wave 14: terminal ret soft tallies. */
    g_u32SoftRetNeg = 0;
    g_u32SoftRetZero = 0;
    g_u32SoftRetPos = 0;
    g_u32SoftRetNote = 0;
    /* Allow one-shot deepen again after a product reset. */
    g_fSoftInvOnce = 0;
}

void
gj_linux_nr_class_stats_get(struct gj_linux_nr_class_stats *pOut)
{
    soft_inc(&g_u32SoftStatsGet);
    if (pOut == NULL) {
        soft_inc(&g_u32SoftStatsGetNull);
        return;
    }
    /*
     * Pre-init: fail closed. Do not scan BSS (PATH_HOT==0 looks fully hot).
     * Publish zeros + table size only.
     */
    if (!g_fNrClassLive) {
        soft_inc(&g_u32SoftStatsGetIdle);
        *pOut = g_class;
        pOut->u32TableSize = GJ_LINUX_NR_TABLE;
        pOut->u32Live = 0;
        pOut->u32HotSlots = 0;
        pOut->u32ColdSlots = 0;
        pOut->u32NoneSlots = 0;
        pOut->u32Classified = 0;
        pOut->u32HotArmed = 0;
        pOut->u32HotNullSlots = 0;
        pOut->u32PathBad = 0;
        return;
    }
    /* Refresh slot scan so callers see live table state. */
    soft_inc(&g_u32SoftStatsGetLive);
    nr_class_scan_slots();
    g_class.u32Live = 1;
    *pOut = g_class;
}

void
gj_linux_nr_class_soft_log(void)
{
    const char *szVerdict;
    u32 u32Rej;
    u32 u32Sum;

    /*
     * Safe pre-init path: PATH_HOT==0 would make a raw scan report hot=512.
     * Emit greppable idle NONE; never claim product PASS before live.
     */
    if (!g_fNrClassLive) {
        soft_inc(&g_u32SoftLogN);
        kprintf("linux: nr class soft NONE hot=0 cold=0 none=0 class=0 "
                "max=0 table=%u set_hot=0 set_cold=0 rej_h=0 rej_c=0\n",
                (unsigned)GJ_LINUX_NR_TABLE);
        kprintf("linux: nr class soft idle (dispatch not init)\n");
        /* Twin idle (Wave 12). Grep: linux: dispatch soft idle */
        kprintf("linux: dispatch soft idle (dispatch not init) log_n=%u\n",
                g_u32SoftLogN);
        return;
    }

    nr_class_scan_slots();
    g_class.u32Live = 1;
    u32Rej = g_class.u32SetHotReject + g_class.u32SetColdReject;
    u32Sum = g_class.u32HotSlots + g_class.u32ColdSlots + g_class.u32NoneSlots +
             g_class.u32PathBad;

    /*
     * Verdict (soft product inventory; never hard-gates):
     *   NONE    — no classified slots
     *   PASS    — hybrid hot+cold, no rejects, armed==hot, no integrity noise
     *   PARTIAL — classified but rejects / one-sided / integrity soft fail
     */
    if (g_class.u32Classified == 0) {
        szVerdict = "NONE";
    } else if (u32Rej == 0 && g_class.u32HotSlots > 0 &&
               g_class.u32ColdSlots > 0 && g_class.u32HotNullSlots == 0 &&
               g_class.u32PathBad == 0 && u32Sum == g_class.u32TableSize &&
               g_class.u32HotArmed == g_class.u32HotSlots) {
        szVerdict = "PASS";
    } else {
        szVerdict = "PARTIAL";
    }

    /*
     * Greppable soft coverage lines (product / smoke inventory):
     *   linux: nr class soft PASS|PARTIAL|NONE hot=… cold=… …
     * Stable format — scripts/gj-quick-keys.sh / gj-product-summary.sh.
     */
    kprintf("linux: nr class soft %s hot=%u cold=%u none=%u class=%u "
            "max=%u table=%u set_hot=%u set_cold=%u rej_h=%u rej_c=%u\n",
            szVerdict, g_class.u32HotSlots, g_class.u32ColdSlots,
            g_class.u32NoneSlots, g_class.u32Classified, g_class.u32MaxClassNr,
            g_class.u32TableSize, g_class.u32SetHotOk, g_class.u32SetColdOk,
            g_class.u32SetHotReject, g_class.u32SetColdReject);
    kprintf("linux: nr class soft hits_h=%llu hits_c=%llu enosys=%llu "
            "defer=%llu hot_null=%llu oor=%llu none_path=%llu "
            "max_h=%u max_c=%u\n",
            (unsigned long long)g_class.u64HotHits,
            (unsigned long long)g_class.u64ColdHits,
            (unsigned long long)g_class.u64Enosys,
            (unsigned long long)g_class.u64HotDeferCold,
            (unsigned long long)g_class.u64HotNull,
            (unsigned long long)g_class.u64Oor,
            (unsigned long long)g_class.u64NonePath, g_class.u32MaxHotNr,
            g_class.u32MaxColdNr);
    kprintf("linux: nr class soft armed=%u null_slots=%u path_bad=%u "
            "live=%u entries=%llu null=%llu cold_ipc=%llu cold_leg=%llu "
            "cold_bare=%llu last_nr=%llu last_ret=%llu\n",
            g_class.u32HotArmed, g_class.u32HotNullSlots, g_class.u32PathBad,
            g_class.u32Live, (unsigned long long)g_class.u64Entries,
            (unsigned long long)g_class.u64NullGuard,
            (unsigned long long)g_class.u64ColdIpc,
            (unsigned long long)g_class.u64ColdLegacy,
            (unsigned long long)g_class.u64ColdBare,
            (unsigned long long)g_class.u64LastNr,
            (unsigned long long)g_class.u64LastRet);

    /* Wave 12 + Wave 35 exclusive deepen (additive; never hard-gates). */
    soft_dispatch_deepen_log();
}
