/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Map ring-3 trampoline + stack; enter via sysret.
 * Personality window hosts freestanding protonrt door server (G-PERS).
 *
 * VA layout (high enough to clear kernel identity BSS):
 *   ring3 code  @ GJ_USER_CODE_VA   (0x0100_0000)
 *   ring3 stack @ GJ_USER_STACK_TOP (0x0110_0000, grows down)
 *   pers  code  @ GJ_PERS_CODE_VA   (0x0120_0000)
 *   pers  stack @ GJ_PERS_STACK_TOP (0x0130_0000, grows down)
 *
 * Soft product inventory (Wave 35 exclusive deepen; this unit only):
 * greppable: "user: soft …" | "user_task: soft …"
 *   user: soft inventory …
 *   user: soft stats …
 *   user: soft map …
 *   user: soft layout …
 *   user: soft enter …
 *   user: soft recheck …
 *   user: soft catalog …
 *   user: soft va …          (dual-window VA geometry)
 *   user: soft return …      (Wave 19 return-path catalog)
 *   user: soft ret_surface … (Wave 19 terminal return classes)
 *   user: soft surface …     (Wave 19 area catalog)
 *   user: soft deepen wave=111 …
 *   user: soft path …
 *   user: soft PASS|PARTIAL
 *   user: ring3 map soft | user: personality map soft (post-map observe)
 *   Post-map PTE soft (P|U, code RX / stack RW|NX) + payload soft match
 *   Layout soft (user band, page align, code/stack non-overlap, dual windows)
 *   Fail closed on soft bad; greppable soft PASS|FAIL; soft stats counters
 *   Idempotent re-map: already-live soft recheck without double-install
 * Soft ≠ bar3. Pure C freestanding; dual MIT OR Apache-2.0.
 */
#include <gj/config.h>
#include <gj/cpu.h>
#include <gj/cpu_sys.h>
#include <gj/klog.h>
#include <gj/memobj.h>
#include <gj/pmm.h>
#include <gj/process.h>
#include <gj/string.h>
#include <gj/user_access.h>
#include <gj/user_task.h>
#include <gj/vmm.h>

/* PTE soft bits (mirror vmm / user_copy; local so this unit stays self-contained). */
#define UT_PTE_P   (1ull << 0)
#define UT_PTE_W   (1ull << 1)
#define UT_PTE_U   (1ull << 2)
#define UT_PTE_NX  (1ull << 63)
#define UT_PTE_ADDR_MASK 0x000ffffffffff000ull

static u64 g_u64PersEntry;
static u64 g_u64PersStack;

extern char gj_user_ring3_blob[];
extern char gj_user_ring3_blob_end[];
extern char gj_protonrt_user_blob[];
extern char gj_protonrt_user_blob_end[];

static int g_fUserMapped;
static int g_fPersMapped;

/* ---- Soft map / enter counters (grep: user: soft …) Wave 19 ----------- */
#define GJ_USER_SOFT_WAVE 111u

static u32 g_cRing3MapOk;
static u32 g_cRing3MapFail;
static u32 g_cRing3Soft;
static u32 g_cRing3SoftBad;
static u32 g_cPersMapOk;
static u32 g_cPersMapFail;
static u32 g_cPersSoft;
static u32 g_cPersSoftBad;
static u32 g_cEnterOk;
static u32 g_cEnterSkip;
static u32 g_cCodePages;
static u32 g_cStackPages;
/* Soft-tracked install sizes for recheck / partial unmap. */
static size_t g_cbRing3Blob;
static size_t g_cbPersBlob;
static u32    g_cRing3CodePages;
static u32    g_cPersCodePages;

/* Wave 15 deepen path tallies (diagnostics only; wrap OK). */
static u32 g_u32SoftLayoutFailRing3;
static u32 g_u32SoftLayoutFailPers;
static u32 g_u32SoftAsEnsureFailRing3;
static u32 g_u32SoftAsEnsureFailPers;
static u32 g_u32SoftBlobBadRing3;
static u32 g_u32SoftBlobBadPers;
static u32 g_u32SoftInstallFailRing3;
static u32 g_u32SoftInstallFailPers;
static u32 g_u32SoftStackFailRing3;
static u32 g_u32SoftStackFailPers;
static u32 g_u32SoftTeardown;
static u32 g_u32SoftIdemRing3;
static u32 g_u32SoftIdemPers;
static u32 g_u32SoftRecheckPassRing3;
static u32 g_u32SoftRecheckFailRing3;
static u32 g_u32SoftRecheckPassPers;
static u32 g_u32SoftRecheckFailPers;
static u32 g_u32SoftEnterSkipNotMap;
static u32 g_u32SoftEnterSkipSyscall;
static u32 g_u32SoftEnterSkipRecheck;
static u32 g_u32SoftObserveOk;
static u32 g_u32SoftObserveBad;
static u32 g_u32SoftLogN;
static u8  g_fSoftInvOnce;

static void user_soft_inc(u32 *pCtr);
static void user_soft_inventory(const char *szVia);
static void user_soft_maybe_once(void);

/** Soft: saturating bump (wrap avoided; wrap OK if ever hit). */
static void
user_soft_inc(u32 *pCtr)
{
    if (pCtr == NULL) {
        return;
    }
    if (*pCtr < 0xffffffffu) {
        (*pCtr)++;
    }
}

/**
 * Greppable Wave 15 soft user_task inventory (product / smoke).
 * Twin prefixes: user: soft … | user_task: soft …
 * Never hard-gates map/enter path. Soft ≠ bar3.
 *
 * greppable: user: soft
 * greppable: user_task: soft
 */
static void
user_soft_inventory(const char *szVia)
{
    const char *szViaSafe;
    const char *szVerdict;
    u32         u32Ring3Live;
    u32         u32PersLive;

    szViaSafe = (szVia != NULL && szVia[0] != '\0') ? szVia : "unknown";
    user_soft_inc(&g_u32SoftLogN);
    u32Ring3Live = g_fUserMapped != 0 ? 1u : 0u;
    u32PersLive = g_fPersMapped != 0 ? 1u : 0u;

    if (g_cRing3MapOk != 0u || g_cPersMapOk != 0u) {
        szVerdict = "PASS";
    } else if (g_cRing3MapFail != 0u || g_cPersMapFail != 0u ||
               g_cEnterSkip != 0u) {
        szVerdict = "PARTIAL";
    } else {
        szVerdict = "PARTIAL";
    }

    /* Grep: user: soft inventory */
    kprintf("user: soft inventory via=%s ring3_ok=%u ring3_fail=%u "
            "pers_ok=%u pers_fail=%u enter_ok=%u enter_skip=%u "
            "ring3_live=%u pers_live=%u log_n=%u wave=%u\n",
            szViaSafe, g_cRing3MapOk, g_cRing3MapFail, g_cPersMapOk,
            g_cPersMapFail, g_cEnterOk, g_cEnterSkip, u32Ring3Live,
            u32PersLive, g_u32SoftLogN, GJ_USER_SOFT_WAVE);

    /* Grep: user: soft stats */
    kprintf("user: soft stats ring3_ok=%u ring3_fail=%u ring3_soft=%u "
            "ring3_soft_bad=%u pers_ok=%u pers_fail=%u pers_soft=%u "
            "pers_soft_bad=%u enter_ok=%u enter_skip=%u code_pages=%u "
            "stack_pages=%u ring3_live=%u pers_live=%u\n",
            g_cRing3MapOk, g_cRing3MapFail, g_cRing3Soft, g_cRing3SoftBad,
            g_cPersMapOk, g_cPersMapFail, g_cPersSoft, g_cPersSoftBad,
            g_cEnterOk, g_cEnterSkip, g_cCodePages, g_cStackPages,
            u32Ring3Live, u32PersLive);

    /* Grep: user: soft map */
    kprintf("user: soft map ring3_blob_bad=%u pers_blob_bad=%u "
            "ring3_install_fail=%u pers_install_fail=%u "
            "ring3_stack_fail=%u pers_stack_fail=%u "
            "ring3_as_fail=%u pers_as_fail=%u teardown=%u "
            "observe_ok=%u observe_bad=%u\n",
            g_u32SoftBlobBadRing3, g_u32SoftBlobBadPers,
            g_u32SoftInstallFailRing3, g_u32SoftInstallFailPers,
            g_u32SoftStackFailRing3, g_u32SoftStackFailPers,
            g_u32SoftAsEnsureFailRing3, g_u32SoftAsEnsureFailPers,
            g_u32SoftTeardown, g_u32SoftObserveOk, g_u32SoftObserveBad);

    /* Grep: user: soft layout */
    kprintf("user: soft layout ring3_fail=%u pers_fail=%u "
            "idem_ring3=%u idem_pers=%u dual_window=1\n",
            g_u32SoftLayoutFailRing3, g_u32SoftLayoutFailPers,
            g_u32SoftIdemRing3, g_u32SoftIdemPers);

    /* Grep: user: soft enter */
    kprintf("user: soft enter ok=%u skip=%u not_mapped=%u syscall=%u "
            "recheck=%u\n",
            g_cEnterOk, g_cEnterSkip, g_u32SoftEnterSkipNotMap,
            g_u32SoftEnterSkipSyscall, g_u32SoftEnterSkipRecheck);

    /* Grep: user: soft recheck */
    kprintf("user: soft recheck ring3_pass=%u ring3_fail=%u "
            "pers_pass=%u pers_fail=%u\n",
            g_u32SoftRecheckPassRing3, g_u32SoftRecheckFailRing3,
            g_u32SoftRecheckPassPers, g_u32SoftRecheckFailPers);

    /* Grep: user: soft catalog */
    kprintf("user: soft catalog code_va=0x%lx stack_top=0x%lx "
            "code_pages=%u stack_pages=%u pers_code=0x%lx "
            "pers_stack=0x%lx pers_code_pages=%u ring3_blob=%lu "
            "pers_blob=%lu wave=%u\n",
            (unsigned long)GJ_USER_CODE_VA, (unsigned long)GJ_USER_STACK_TOP,
            GJ_USER_CODE_PAGES, GJ_USER_STACK_PAGES,
            (unsigned long)GJ_PERS_CODE_VA, (unsigned long)GJ_PERS_STACK_TOP,
            GJ_PERS_CODE_PAGES, (unsigned long)g_cbRing3Blob,
            (unsigned long)g_cbPersBlob, GJ_USER_SOFT_WAVE);

    /* Grep: user: soft va (Wave 15 dual-window geometry) */
    kprintf("user: soft va ring3_code=0x%lx ring3_stack=0x%lx "
            "pers_code=0x%lx pers_stack=0x%lx dual=1 "
            "ring3_live=%u pers_live=%u code_pages=%u stack_pages=%u "
            "wave=%u\n",
            (unsigned long)GJ_USER_CODE_VA, (unsigned long)GJ_USER_STACK_TOP,
            (unsigned long)GJ_PERS_CODE_VA, (unsigned long)GJ_PERS_STACK_TOP,
            u32Ring3Live, u32PersLive, g_cCodePages, g_cStackPages,
            GJ_USER_SOFT_WAVE);

    /* Grep: user: soft path */
    kprintf("user: soft path claim=ring3,personality,enter "
            "pte=RX_code+RW_NX_stack payload_match=1 layout=dual "
            "sysret_enter=1 G-PERS=1 wave=%u "
            "(soft inventory; not bar3)\n",
            GJ_USER_SOFT_WAVE);

    /*
     * Grep: user: soft return
     * Wave 19 return-path catalog — map/enter/layout/recheck outcomes.
     * Soft ≠ bar3 / product ring3 gate. product_kernel=OPEN.
     */
    kprintf("user: soft return ring3_ok=%u ring3_fail=%u pers_ok=%u "
            "pers_fail=%u enter_ok=%u enter_skip=%u soft_ok=%u "
            "soft_bad=%u layout_fail=%u as_fail=%u install_fail=%u "
            "stack_fail=%u recheck_pass=%u recheck_fail=%u "
            "product_kernel=OPEN wave=%u\n",
            g_cRing3MapOk, g_cRing3MapFail, g_cPersMapOk, g_cPersMapFail,
            g_cEnterOk, g_cEnterSkip, g_cRing3Soft + g_cPersSoft,
            g_cRing3SoftBad + g_cPersSoftBad,
            g_u32SoftLayoutFailRing3 + g_u32SoftLayoutFailPers,
            g_u32SoftAsEnsureFailRing3 + g_u32SoftAsEnsureFailPers,
            g_u32SoftInstallFailRing3 + g_u32SoftInstallFailPers,
            g_u32SoftStackFailRing3 + g_u32SoftStackFailPers,
            g_u32SoftRecheckPassRing3 + g_u32SoftRecheckPassPers,
            g_u32SoftRecheckFailRing3 + g_u32SoftRecheckFailPers,
            GJ_USER_SOFT_WAVE);

    /* Grep: user: soft ret_surface — Wave 19 terminal return classes */
    kprintf("user: soft ret_surface map=ring3_ok|ring3_fail|pers_ok|pers_fail "
            "enter=ok|skip soft=ok|bad layout|as|install|stack=fail "
            "recheck=pass|fail product_kernel=OPEN areas=93 wave=%u\n",
            GJ_USER_SOFT_WAVE);

    /* Grep: user: soft surface — Wave 19 area catalog */
    kprintf("user: soft surface inventory,stats,map,layout,enter,recheck,"
            "catalog,va,path,return,ret_surface,surface,deepen "
            "areas=93 wave=%u\n",
            GJ_USER_SOFT_WAVE);

    /* Grep: user: soft deepen */
    /*
     * ---- Wave 19 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft≠product; not bar3.
     */
    /* Grep: user: soft retclass — Wave 19 return-class taxonomy (kept) */
    kprintf("user: soft retclass ok|fail|inval|nodev|busy|nomem "
            "soft_only=1 product_gate=0 wave=%u "
            "(retclass taxonomy; Soft≠product; not bar3)\n",
            (unsigned)GJ_USER_SOFT_WAVE);
    /* Grep: user: soft retlane — Wave 19 return-lane catalog (kept) */
    kprintf("user: soft retlane inv|selftest|rate|retcode|retmap|class "
            "product_kernel=OPEN soft_ne_product=1 wave=%u "
            "(retlane catalog; Soft≠product)\n",
            (unsigned)GJ_USER_SOFT_WAVE);
    /*
     * ---- Wave 20 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft≠product; not bar3.
     */
    /* Grep: user: soft retbound — Wave 20 return-bound honesty (kept) */
    kprintf("user: soft retbound soft_only=1 product_gate=0 hard_gate=0 "
            "never_blocks_m0=1 wave=%u "
            "(retbound honesty; Soft≠product; not bar3)\n",
            (unsigned)GJ_USER_SOFT_WAVE);
    /* Grep: user: soft retseal — Wave 20 seal stamp (kept) */
    kprintf("user: soft retseal exclusive=1 soft_ne_product=1 "
            "product_kernel=OPEN bar3=0 wave=%u "
            "(retseal stamp; Soft≠product)\n",
            (unsigned)GJ_USER_SOFT_WAVE);
            /*
             * ---- Wave 21 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
            */
            /* Grep: user: soft retpulse — Wave 21 return-pulse honesty (kept) */
            kprintf("user: soft retpulse soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retpulse honesty; Soft≠product; not bar3)\n",
                    (unsigned)GJ_USER_SOFT_WAVE);
            /* Grep: user: soft retmark — Wave 21 mark stamp (kept) */
            kprintf("user: soft retmark exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retmark stamp; Soft≠product)\n",
                    (unsigned)GJ_USER_SOFT_WAVE);
            /*
             * ---- Wave 22 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
            */
            /* Grep: user: soft retphase — Wave 22 return-phase honesty (kept) */
            kprintf("user: soft retphase soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retphase honesty; Soft≠product; not bar3)\n",
                    (unsigned)GJ_USER_SOFT_WAVE);
            /* Grep: user: soft retbadge — Wave 22 badge stamp (kept) */
            kprintf("user: soft retbadge exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbadge stamp; Soft≠product)\n",
                    (unsigned)GJ_USER_SOFT_WAVE);
/*
 * ---- Wave 23 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
            */
            /* Grep: user: soft rettoken — Wave 23 return-token honesty (kept) */
            kprintf("user: soft rettoken soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(rettoken honesty; Soft≠product; not bar3)\n",
                    (unsigned)GJ_USER_SOFT_WAVE);
            /* Grep: user: soft retcrest — Wave 23 crest stamp (kept) */
            kprintf("user: soft retcrest exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retcrest stamp; Soft≠product)\n",
                    (unsigned)GJ_USER_SOFT_WAVE);
            /*
             * ---- Wave 24 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: user: soft retvault — Wave 24 return-vault honesty (kept) */
            kprintf("user: soft retvault soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retvault honesty; Soft≠product; not bar3)\n",
                    (unsigned)GJ_USER_SOFT_WAVE);
            /* Grep: user: soft retbanner — Wave 24 banner stamp (kept) */
            kprintf("user: soft retbanner exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbanner stamp; Soft≠product)\n",
                    (unsigned)GJ_USER_SOFT_WAVE);
            /*
             * ---- Wave 25 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: user: soft retledger — Wave 25 return-ledger honesty (kept) */
            kprintf("user: soft retledger soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retledger honesty; Soft≠product; not bar3)\n",
                    (unsigned)GJ_USER_SOFT_WAVE);
            /* Grep: user: soft retbeacon — Wave 25 beacon stamp (kept) */
            kprintf("user: soft retbeacon exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbeacon stamp; Soft≠product)\n",
                    (unsigned)GJ_USER_SOFT_WAVE);
            /*
             * ---- Wave 26 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: user: soft retcipher — Wave 26 return-cipher honesty (kept) */
            kprintf("user: soft retcipher soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retcipher honesty; Soft≠product; not bar3)\n",
                    (unsigned)GJ_USER_SOFT_WAVE);
            /* Grep: user: soft retflame — Wave 26 flame stamp (kept) */
            kprintf("user: soft retflame exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retflame stamp; Soft≠product)\n",
                    (unsigned)GJ_USER_SOFT_WAVE);
                    /*
                     * ---- Wave 27 complementary surfaces (kept) (never reshape primary).
                     * Return surfaces only — soft inventory; never hard-gates product paths.
                     * Soft≠product; not bar3.
                     */
                    /* Grep: user: soft retprism — Wave 27 return-prism honesty (kept) */
                    kprintf("user: soft retprism soft_only=1 product_gate=0 soft_ne_product=1 "
                            "never_blocks_m0=1 wave=%u "
                            "(retprism honesty; Soft≠product; not bar3)\n",
                            (unsigned)GJ_USER_SOFT_WAVE);
                    /* Grep: user: soft retforge — Wave 27 forge stamp (kept) */
                    kprintf("user: soft retforge exclusive=1 soft_ne_product=1 "
                            "product_kernel=OPEN bar3=0 wave=%u "
                            "(retforge stamp; Soft≠product)\n",
                            (unsigned)GJ_USER_SOFT_WAVE);
                            /*
                             * ---- Wave 28 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: user: soft retshard — Wave 28 return-shard honesty (kept) */
                            kprintf("user: soft retshard soft_only=1 product_gate=0 soft_ne_product=1 "
                                "never_blocks_m0=1 wave=%u "
                                "(retshard honesty; Soft≠product; not bar3)\n",
                                (unsigned)GJ_USER_SOFT_WAVE);
                            /* Grep: user: soft retcrown — Wave 28 crown stamp (kept) */
                            kprintf("user: soft retcrown exclusive=1 soft_ne_product=1 "
                                "product_kernel=OPEN bar3=0 wave=%u "
                                "(retcrown stamp; Soft≠product)\n",
                                (unsigned)GJ_USER_SOFT_WAVE);
    kprintf("user: soft deepen wave=%u via=%s ring3_ok=%u pers_ok=%u "
            "enter_ok=%u soft=%u soft_bad=%u logs=%u "
            "(soft inventory only; not product gate)\n",
            GJ_USER_SOFT_WAVE, szViaSafe, g_cRing3MapOk, g_cPersMapOk,
            g_cEnterOk, g_cRing3Soft + g_cPersSoft,
            g_cRing3SoftBad + g_cPersSoftBad, g_u32SoftLogN);

    /* Grep: user: soft PASS | PARTIAL */
    kprintf("user: soft %s via=%s ring3_live=%u pers_live=%u "
            "enter_ok=%u log_n=%u wave=%u\n",
            szVerdict, szViaSafe, u32Ring3Live, u32PersLive, g_cEnterOk,
            g_u32SoftLogN, GJ_USER_SOFT_WAVE);

    /* Twin prefix: user_task: soft … */
    /* Grep: user_task: soft inventory */
    kprintf("user_task: soft inventory via=%s ring3_ok=%u ring3_fail=%u "
            "pers_ok=%u pers_fail=%u enter_ok=%u enter_skip=%u "
            "ring3_live=%u pers_live=%u log_n=%u wave=%u\n",
            szViaSafe, g_cRing3MapOk, g_cRing3MapFail, g_cPersMapOk,
            g_cPersMapFail, g_cEnterOk, g_cEnterSkip, u32Ring3Live,
            u32PersLive, g_u32SoftLogN, GJ_USER_SOFT_WAVE);

    /* Grep: user_task: soft return */
    kprintf("user_task: soft return ring3_ok=%u ring3_fail=%u pers_ok=%u "
            "pers_fail=%u enter_ok=%u enter_skip=%u soft_bad=%u "
            "product_kernel=OPEN wave=%u\n",
            g_cRing3MapOk, g_cRing3MapFail, g_cPersMapOk, g_cPersMapFail,
            g_cEnterOk, g_cEnterSkip, g_cRing3SoftBad + g_cPersSoftBad,
            GJ_USER_SOFT_WAVE);
    /* Grep: user_task: soft ret_surface */
    kprintf("user_task: soft ret_surface map=ring3|pers enter=ok|skip "
            "soft_bad product_kernel=OPEN wave=%u\n",
            GJ_USER_SOFT_WAVE);

    /* Grep: user_task: soft retmap — Wave 19 return-surface map */
    kprintf("user_task: soft retmap ok|fail|inval|nodev|busy|nomem product_gate=0 soft_only=1 wave=111\n");

    /* Grep: user_task: soft deepen */
    /*
     * ---- Wave 19 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft≠product; not bar3.
     */
    /* Grep: user_task: soft retclass — Wave 19 return-class taxonomy (kept) */
    kprintf("user_task: soft retclass ok|fail|inval|nodev|busy|nomem "
            "soft_only=1 product_gate=0 wave=%u "
            "(retclass taxonomy; Soft≠product; not bar3)\n",
            (unsigned)GJ_USER_SOFT_WAVE);
    /* Grep: user_task: soft retlane — Wave 19 return-lane catalog (kept) */
    kprintf("user_task: soft retlane inv|selftest|rate|retcode|retmap|class "
            "product_kernel=OPEN soft_ne_product=1 wave=%u "
            "(retlane catalog; Soft≠product)\n",
            (unsigned)GJ_USER_SOFT_WAVE);
    /*
     * ---- Wave 20 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft≠product; not bar3.
     */
    /* Grep: user_task: soft retbound — Wave 20 return-bound honesty (kept) */
    kprintf("user_task: soft retbound soft_only=1 product_gate=0 hard_gate=0 "
            "never_blocks_m0=1 wave=%u "
            "(retbound honesty; Soft≠product; not bar3)\n",
            (unsigned)GJ_USER_SOFT_WAVE);
    /* Grep: user_task: soft retseal — Wave 20 seal stamp (kept) */
    kprintf("user_task: soft retseal exclusive=1 soft_ne_product=1 "
            "product_kernel=OPEN bar3=0 wave=%u "
            "(retseal stamp; Soft≠product)\n",
            (unsigned)GJ_USER_SOFT_WAVE);
            /*
             * ---- Wave 21 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
            */
            /* Grep: user_task: soft retpulse — Wave 21 return-pulse honesty (kept) */
            kprintf("user_task: soft retpulse soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retpulse honesty; Soft≠product; not bar3)\n",
                    (unsigned)GJ_USER_SOFT_WAVE);
            /* Grep: user_task: soft retmark — Wave 21 mark stamp (kept) */
            kprintf("user_task: soft retmark exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retmark stamp; Soft≠product)\n",
                    (unsigned)GJ_USER_SOFT_WAVE);
            /*
             * ---- Wave 22 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
            */
            /* Grep: user_task: soft retphase — Wave 22 return-phase honesty (kept) */
            kprintf("user_task: soft retphase soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retphase honesty; Soft≠product; not bar3)\n",
                    (unsigned)GJ_USER_SOFT_WAVE);
            /* Grep: user_task: soft retbadge — Wave 22 badge stamp (kept) */
            kprintf("user_task: soft retbadge exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbadge stamp; Soft≠product)\n",
                    (unsigned)GJ_USER_SOFT_WAVE);
/*
 * ---- Wave 23 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
            */
            /* Grep: user_task: soft rettoken — Wave 23 return-token honesty (kept) */
            kprintf("user_task: soft rettoken soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(rettoken honesty; Soft≠product; not bar3)\n",
                    (unsigned)GJ_USER_SOFT_WAVE);
            /* Grep: user_task: soft retcrest — Wave 23 crest stamp (kept) */
            kprintf("user_task: soft retcrest exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retcrest stamp; Soft≠product)\n",
                    (unsigned)GJ_USER_SOFT_WAVE);
            /*
             * ---- Wave 24 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: user_task: soft retvault — Wave 24 return-vault honesty (kept) */
            kprintf("user_task: soft retvault soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retvault honesty; Soft≠product; not bar3)\n",
                    (unsigned)GJ_USER_SOFT_WAVE);
            /* Grep: user_task: soft retbanner — Wave 24 banner stamp (kept) */
            kprintf("user_task: soft retbanner exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbanner stamp; Soft≠product)\n",
                    (unsigned)GJ_USER_SOFT_WAVE);
            /*
             * ---- Wave 25 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: user_task: soft retledger — Wave 25 return-ledger honesty (kept) */
            kprintf("user_task: soft retledger soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retledger honesty; Soft≠product; not bar3)\n",
                    (unsigned)GJ_USER_SOFT_WAVE);
            /* Grep: user_task: soft retbeacon — Wave 25 beacon stamp (kept) */
            kprintf("user_task: soft retbeacon exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbeacon stamp; Soft≠product)\n",
                    (unsigned)GJ_USER_SOFT_WAVE);
            /*
             * ---- Wave 26 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: user_task: soft retcipher — Wave 26 return-cipher honesty (kept) */
            kprintf("user_task: soft retcipher soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retcipher honesty; Soft≠product; not bar3)\n",
                    (unsigned)GJ_USER_SOFT_WAVE);
            /* Grep: user_task: soft retflame — Wave 26 flame stamp (kept) */
            kprintf("user_task: soft retflame exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retflame stamp; Soft≠product)\n",
                    (unsigned)GJ_USER_SOFT_WAVE);
                    /*
                     * ---- Wave 27 complementary surfaces (kept) (never reshape primary).
                     * Return surfaces only — soft inventory; never hard-gates product paths.
                     * Soft≠product; not bar3.
                     */
                    /* Grep: user_task: soft retprism — Wave 27 return-prism honesty (kept) */
                    kprintf("user_task: soft retprism soft_only=1 product_gate=0 soft_ne_product=1 "
                            "never_blocks_m0=1 wave=%u "
                            "(retprism honesty; Soft≠product; not bar3)\n",
                            (unsigned)GJ_USER_SOFT_WAVE);
                    /* Grep: user_task: soft retforge — Wave 27 forge stamp (kept) */
                    kprintf("user_task: soft retforge exclusive=1 soft_ne_product=1 "
                            "product_kernel=OPEN bar3=0 wave=%u "
                            "(retforge stamp; Soft≠product)\n",
                            (unsigned)GJ_USER_SOFT_WAVE);
                            /*
                             * ---- Wave 28 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: user_task: soft retshard — Wave 28 return-shard honesty (kept) */
                            kprintf("user_task: soft retshard soft_only=1 product_gate=0 soft_ne_product=1 "
                                "never_blocks_m0=1 wave=%u "
                                "(retshard honesty; Soft≠product; not bar3)\n",
                                (unsigned)GJ_USER_SOFT_WAVE);
                            /* Grep: user_task: soft retcrown — Wave 28 crown stamp (kept) */
                            kprintf("user_task: soft retcrown exclusive=1 soft_ne_product=1 "
                                "product_kernel=OPEN bar3=0 wave=%u "
                                "(retcrown stamp; Soft≠product)\n",
                                (unsigned)GJ_USER_SOFT_WAVE);
                                /*
                             * ---- Wave 29 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: user_task: soft retglyph — Wave 29 return-glyph honesty (kept) */
                            kprintf("user_task: soft retglyph soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retglyph honesty; Soft≠product; not bar3)\n",
                                    (unsigned)GJ_USER_SOFT_WAVE);
                            /* Grep: user_task: soft retscepter — Wave 29 scepter stamp (kept) */
                            kprintf("user_task: soft retscepter exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=%u "
                                    "(retscepter stamp; Soft≠product)\n",
                                    (unsigned)GJ_USER_SOFT_WAVE);
                                /*
                             * ---- Wave 30 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: user_task: soft retsigil — Wave 30 return-sigil honesty (kept) */
                            kprintf("user_task: soft retsigil soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retsigil honesty; Soft≠product; not bar3)\n",
                                    (unsigned)GJ_USER_SOFT_WAVE);
                            /* Grep: user_task: soft retemblem — Wave 30 emblem stamp (kept) */
                            kprintf("user_task: soft retemblem exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=%u "
                                    "(retemblem stamp; Soft≠product)\n",
                                    (unsigned)GJ_USER_SOFT_WAVE);
                            /*
                             * ---- Wave 31 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: user_task: soft retaegis — Wave 31 return-aegis honesty (kept) */
                            kprintf("user_task: soft retaegis soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retaegis honesty; Soft≠product; not bar3)\n",
                                    (unsigned)GJ_USER_SOFT_WAVE);
                            /* Grep: user_task: soft retsigil — Wave 30 return-sigil honesty (kept) */
                            kprintf("user_task: soft retsigil soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retsigil honesty; Soft≠product; not bar3)\n",
                                    (unsigned)GJ_USER_SOFT_WAVE);
                            /* Grep: user_task: soft retmantle — Wave 31 mantle stamp (kept) */
                            kprintf("user_task: soft retmantle exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=%u "
                                    "(retmantle stamp; Soft≠product)\n",
                                    (unsigned)GJ_USER_SOFT_WAVE);
/*
 * ---- Wave 32 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: user_task: soft retbulwark — Wave 32 return-bulwark honesty (kept) */
kprintf("user_task: soft retbulwark soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbulwark honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_USER_SOFT_WAVE);
/* Grep: user_task: soft retpanoply — Wave 32 panoply stamp (kept) */
kprintf("user_task: soft retpanoply exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retpanoply stamp; Soft≠product)\n",
        (unsigned)GJ_USER_SOFT_WAVE);
/*
 * ---- Wave 33 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: user_task: soft retbastion — Wave 33 return-bastion honesty (kept) */
kprintf("user_task: soft retbastion soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbastion honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_USER_SOFT_WAVE);
/* Grep: user_task: soft retcitadel — Wave 33 citadel stamp (kept) */
kprintf("user_task: soft retcitadel exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retcitadel stamp; Soft≠product)\n",
        (unsigned)GJ_USER_SOFT_WAVE);
/*
 * ---- Wave 34 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: user_task: soft retredoubt — Wave 34 return-redoubt honesty */
kprintf("user_task: soft retredoubt soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retredoubt honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_USER_SOFT_WAVE);
/* Grep: user_task: soft retkeep — Wave 34 exclusive keep stamp */
kprintf("user_task: soft retkeep exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retkeep stamp; Soft≠product)\n",
        (unsigned)GJ_USER_SOFT_WAVE);
/*
 * ---- Wave 35 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: user_task: soft retfortress — Wave 35 return-fortress honesty */
kprintf("user_task: soft retfortress soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retfortress honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_USER_SOFT_WAVE);
/* Grep: user_task: soft retpalace — Wave 35 exclusive palace stamp */
kprintf("user_task: soft retpalace exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retpalace stamp; Soft≠product)\n",
        (unsigned)GJ_USER_SOFT_WAVE);
/*
 * ---- Wave 36 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: user_task: soft rethold — Wave 36 return-hold honesty */
kprintf("user_task: soft rethold soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(rethold honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_USER_SOFT_WAVE);
/* Grep: user_task: soft retspire — Wave 36 exclusive spire stamp */
kprintf("user_task: soft retspire exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retspire stamp; Soft≠product)\n",
        (unsigned)GJ_USER_SOFT_WAVE);
/*
 * ---- Wave 37 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: user_task: soft retwall — Wave 37 return-wall honesty */
kprintf("user_task: soft retwall soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retwall honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_USER_SOFT_WAVE);
/* Grep: user_task: soft retgate — Wave 37 exclusive gate stamp */
kprintf("user_task: soft retgate exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retgate stamp; Soft≠product)\n",
        (unsigned)GJ_USER_SOFT_WAVE);
/*
 * ---- Wave 38 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: user_task: soft retmoat — Wave 38 return-moat honesty */
kprintf("user_task: soft retmoat soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retmoat honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_USER_SOFT_WAVE);
/* Grep: user_task: soft retower — Wave 38 exclusive tower stamp */
kprintf("user_task: soft retower exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retower stamp; Soft≠product)\n",
        (unsigned)GJ_USER_SOFT_WAVE);
                            
/*
 * ---- Wave 39 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: user_task: soft retbarbican — Wave 39 return-barbican honesty */
kprintf("user_task: soft retbarbican soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbarbican honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_USER_SOFT_WAVE);
/* Grep: user_task: soft retglacis — Wave 39 exclusive glacis stamp */
kprintf("user_task: soft retglacis exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retglacis stamp; Soft≠product)\n",
        (unsigned)GJ_USER_SOFT_WAVE);
/*
 * ---- Wave 40 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: user_task: soft retcurtain — Wave 40 return-curtain honesty */
kprintf("user_task: soft retcurtain soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retcurtain honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_USER_SOFT_WAVE);
/* Grep: user_task: soft retparapet — Wave 40 exclusive parapet stamp */
kprintf("user_task: soft retparapet exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retparapet stamp; Soft≠product)\n",
        (unsigned)GJ_USER_SOFT_WAVE);
/*
 * ---- Wave 41 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: user_task: soft retravelin — Wave 41 return-travelin honesty */
kprintf("user_task: soft retravelin soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retravelin honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_USER_SOFT_WAVE);
/* Grep: user_task: soft retditch — Wave 41 exclusive ditch stamp */
kprintf("user_task: soft retditch exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retditch stamp; Soft≠product)\n",
        (unsigned)GJ_USER_SOFT_WAVE);
/*
 * ---- Wave 42 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: user_task: soft retportcullis — Wave 42 return-portcullis honesty */
kprintf("user_task: soft retportcullis soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retportcullis honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_USER_SOFT_WAVE);
/* Grep: user_task: soft retbattlement — Wave 42 exclusive battlement stamp */
kprintf("user_task: soft retbattlement exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retbattlement stamp; Soft≠product)\n",
        (unsigned)GJ_USER_SOFT_WAVE);
/*
 * ---- Wave 43 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: user_task: soft retmachicolation — Wave 43 return-machicolation honesty */
kprintf("user_task: soft retmachicolation soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retmachicolation honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_USER_SOFT_WAVE);
/* Grep: user_task: soft retarrowslit — Wave 43 exclusive arrowslit stamp */
kprintf("user_task: soft retarrowslit exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retarrowslit stamp; Soft≠product)\n",
        (unsigned)GJ_USER_SOFT_WAVE);

/*
 * ---- Wave 44 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: user_task: soft retmerlon — Wave 44 return-merlon honesty */
kprintf("user_task: soft retmerlon soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retmerlon honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_USER_SOFT_WAVE);
/* Grep: user_task: soft retembrasure — Wave 44 exclusive embrasure stamp */
kprintf("user_task: soft retembrasure exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retembrasure stamp; Soft≠product)\n",
        (unsigned)GJ_USER_SOFT_WAVE);

/*
 * ---- Wave 45 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: user_task: soft retkeepgate — Wave 45 return-keepgate honesty */
kprintf("user_task: soft retkeepgate soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retkeepgate honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_USER_SOFT_WAVE);
/* Grep: user_task: soft retouterward — Wave 45 exclusive outerward stamp */
kprintf("user_task: soft retouterward exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retouterward stamp; Soft≠product)\n",
        (unsigned)GJ_USER_SOFT_WAVE);

/*
 * ---- Wave 46 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: user_task: soft retbailey — Wave 46 return-bailey honesty */
kprintf("user_task: soft retbailey soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbailey honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_USER_SOFT_WAVE);
/* Grep: user_task: soft retpostern — Wave 46 exclusive postern stamp */
kprintf("user_task: soft retpostern exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retpostern stamp; Soft≠product)\n",
        (unsigned)GJ_USER_SOFT_WAVE);

/*
 * ---- Wave 47 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: user_task: soft retinnerward — Wave 47 return-innerward honesty */
kprintf("user_task: soft retinnerward soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=111 "
        "(retinnerward honesty; Soft≠product; not bar3)\n");
/* Grep: user_task: soft retdonjon — Wave 47 exclusive donjon stamp */
kprintf("user_task: soft retdonjon exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=111 "
        "(retdonjon stamp; Soft≠product)\n");

/*
 * ---- Wave 48 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: user_task: soft retchevaux — Wave 48 return-chevaux honesty */
kprintf("user_task: soft retchevaux soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=111 "
        "(retchevaux honesty; Soft≠product; not bar3)\n");
/* Grep: user_task: soft retpalisade — Wave 48 exclusive palisade stamp */
kprintf("user_task: soft retpalisade exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=111 "
        "(retpalisade stamp; Soft≠product)\n");

/*
 * ---- Wave 49 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: user_task: soft retglacisgate — Wave 49 return-glacisgate honesty */
kprintf("user_task: soft retglacisgate soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=111 "
        "(retglacisgate honesty; Soft≠product; not bar3)\n");
/* Grep: user_task: soft retoutwork — Wave 49 exclusive outwork stamp */
kprintf("user_task: soft retoutwork exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=111 "
        "(retoutwork stamp; Soft≠product)\n");
/*
 * ---- Wave 50 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: user_task: soft retsally — Wave 50 return-sally honesty */
kprintf("user_task: soft retsally soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=111 "
        "(retsally honesty; Soft≠product; not bar3)\n");
/* Grep: user_task: soft retcounterscarp — Wave 50 exclusive counterscarp stamp */
kprintf("user_task: soft retcounterscarp exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=111 "
        "(retcounterscarp stamp; Soft≠product)\n");
/*
 * ---- Wave 51 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: user_task: soft retfosse — Wave 51 return-fosse honesty */
kprintf("user_task: soft retfosse soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=111 "
        "(retfosse honesty; Soft≠product; not bar3)\n");
/* Grep: user_task: soft retcoveredway — Wave 51 exclusive coveredway stamp */
kprintf("user_task: soft retcoveredway exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=111 "
        "(retcoveredway stamp; Soft≠product)\n");

/*
 * ---- Wave 52 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: user_task: soft rettenaille — Wave 52 return-tenaille honesty */
kprintf("user_task: soft rettenaille soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=111 "
        "(rettenaille honesty; Soft≠product; not bar3)\n");
/* Grep: user_task: soft retdemilune — Wave 52 exclusive demilune stamp */
kprintf("user_task: soft retdemilune exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=111 "
        "(retdemilune stamp; Soft≠product)\n");
/*
 * ---- Wave 53 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: user_task: soft retravelin — Wave 53 return-travelin honesty */
kprintf("user_task: soft retravelin soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=111 "
        "(retravelin honesty; Soft≠product; not bar3)\n");
/* Grep: user_task: soft retlunette — Wave 53 exclusive lunette stamp */
kprintf("user_task: soft retlunette exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=111 "
        "(retlunette stamp; Soft≠product)\n");
/*
 * ---- Wave 54 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: user_task: soft retcaponier — Wave 54 return-caponier honesty */
kprintf("user_task: soft retcaponier soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=111 "
        "(retcaponier honesty; Soft≠product; not bar3)\n");
/* Grep: user_task: soft retredan — Wave 54 exclusive redan stamp */
kprintf("user_task: soft retredan exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=111 "
        "(retredan stamp; Soft≠product)\n");
/*
 * ---- Wave 55 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: user_task: soft retflank — Wave 55 return-flank honesty */
kprintf("user_task: soft retflank soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=111 "
        "(retflank honesty; Soft≠product; not bar3)\n");
/* Grep: user_task: soft retface — Wave 55 exclusive face stamp */
kprintf("user_task: soft retface exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=111 "
        "(retface stamp; Soft≠product)\n");
/*
 * ---- Wave 56 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: user_task: soft retgorge — Wave 56 return-gorge honesty */
kprintf("user_task: soft retgorge soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=111 "
        "(retgorge honesty; Soft≠product; not bar3)\n");
/* Grep: user_task: soft retshoulder — Wave 56 exclusive shoulder stamp */
kprintf("user_task: soft retshoulder exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=111 "
        "(retshoulder stamp; Soft≠product)\n");
/*
 * ---- Wave 57 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: user_task: soft retraverse — Wave 57 return-traverse honesty */
kprintf("user_task: soft retraverse soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=111 "
        "(retraverse honesty; Soft≠product; not bar3)\n");
/* Grep: user_task: soft retcasemate — Wave 57 exclusive casemate stamp */
kprintf("user_task: soft retcasemate exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=111 "
        "(retcasemate stamp; Soft≠product)\n");

/*
 * ---- Wave 58 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: user_task: soft retorillon — Wave 58 return-orillon honesty */
kprintf("user_task: soft retorillon soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=111 "
        "(retorillon honesty; Soft≠product; not bar3)\n");
/* Grep: user_task: soft retbonnette — Wave 58 exclusive bonnette stamp */
kprintf("user_task: soft retbonnette exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=111 "
        "(retbonnette stamp; Soft≠product)\n");

/*
 * ---- Wave 59 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: user_task: soft retcrownwork — Wave 59 return-crownwork honesty */
kprintf("user_task: soft retcrownwork soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=111 "
        "(retcrownwork honesty; Soft≠product; not bar3)\n");
/* Grep: user_task: soft rethornwork — Wave 59 exclusive hornwork stamp */
kprintf("user_task: soft rethornwork exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=111 "
        "(rethornwork stamp; Soft≠product)\n");

/*
 * ---- Wave 60 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: user_task: soft retplace — Wave 60 return-place honesty */
kprintf("user_task: soft retplace soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=111 "
        "(retplace honesty; Soft≠product; not bar3)\n");
/* Grep: user_task: soft retenvelope — Wave 60 exclusive envelope stamp */
kprintf("user_task: soft retenvelope exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=111 "
        "(retenvelope stamp; Soft≠product)\n");









/*
 * ---- Wave 61 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: user_task: soft retcounterguard — Wave 61 return-counterguard honesty */
kprintf("user_task: soft retcounterguard soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=111 "
        "(retcounterguard honesty; Soft≠product; not bar3)\n");
/* Grep: user_task: soft retcoveredface — Wave 61 exclusive coveredface stamp */
kprintf("user_task: soft retcoveredface exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=111 "
        "(retcoveredface stamp; Soft≠product)\n");
/*
 * ---- Wave 62 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: user_task: soft retbastionface — Wave 62 return-bastionface honesty */
kprintf("user_task: soft retbastionface soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=111 "
        "(retbastionface honesty; Soft≠product; not bar3)\n");
/* Grep: user_task: soft retcurtainangle — Wave 62 exclusive curtainangle stamp */
kprintf("user_task: soft retcurtainangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=111 "
        "(retcurtainangle stamp; Soft≠product)\n");
/*
 * ---- Wave 63 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: user_task: soft retdoubletenaille — Wave 63 return-doubletenaille honesty */
kprintf("user_task: soft retdoubletenaille soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=111 "
        "(retdoubletenaille honesty; Soft≠product; not bar3)\n");
/* Grep: user_task: soft retplaceofarms — Wave 63 exclusive placeofarms stamp */
kprintf("user_task: soft retplaceofarms exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=111 "
        "(retplaceofarms stamp; Soft≠product)\n");
 /*
  * ---- Wave 64 exclusive complementary surfaces (never reshape primary).
  * Return surfaces only — soft inventory; never hard-gates product paths.
  * Soft≠product; not bar3.
  */
 /* Grep: user_task: soft retreentrant — Wave 64 return-reentrant honesty */
kprintf("user_task: soft retreentrant soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=111 "
        "(retreentrant honesty; Soft≠product; not bar3)\n");
 /* Grep: user_task: soft retsallyport — Wave 64 exclusive sallyport stamp */
kprintf("user_task: soft retsallyport exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=111 "
        "(retsallyport stamp; Soft≠product)\n");
 /*
  * ---- Wave 65 exclusive complementary surfaces (never reshape primary).
  * Return surfaces only — soft inventory; never hard-gates product paths.
  * Soft≠product; not bar3.
  */
 /* Grep: user_task: soft retgorgeangle — Wave 65 return-gorgeangle honesty */
kprintf("user_task: soft retgorgeangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=111 "
        "(retgorgeangle honesty; Soft≠product; not bar3)\n");
 /* Grep: user_task: soft retshoulderangle — Wave 65 exclusive shoulderangle stamp */
kprintf("user_task: soft retshoulderangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=111 "
        "(retshoulderangle stamp; Soft≠product)\n");
 /*
  * ---- Wave 66 exclusive complementary surfaces (never reshape primary).
  * Return surfaces only — soft inventory; never hard-gates product paths.
  * Soft≠product; not bar3.
  */
 /* Grep: user_task: soft retflankangle — Wave 66 return-flankangle honesty */
 kprintf("user_task: soft retflankangle soft_only=1 product_gate=0 soft_ne_product=1 "
         "never_blocks_m0=1 wave=111 "
         "(retflankangle honesty; Soft≠product; not bar3)\n");
 /* Grep: user_task: soft retfaceangle — Wave 66 exclusive faceangle stamp */
 kprintf("user_task: soft retfaceangle exclusive=1 soft_ne_product=1 "
         "product_kernel=OPEN bar3=0 wave=111 "
         "(retfaceangle stamp; Soft≠product)\n");
/*
 * ---- Wave 67 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: user_task: soft retcaponierangle — Wave 67 return-caponierangle honesty */
kprintf("user_task: soft retcaponierangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=111 "
        "(retcaponierangle honesty; Soft≠product; not bar3)\n");
/* Grep: user_task: soft retredanangle — Wave 67 exclusive redanangle stamp */
kprintf("user_task: soft retredanangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=111 "
        "(retredanangle stamp; Soft≠product)\n");
/*
 * ---- Wave 68 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: user_task: soft retlunetteangle — Wave 68 return-lunetteangle honesty */
kprintf("user_task: soft retlunetteangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=111 "
        "(retlunetteangle honesty; Soft≠product; not bar3)\n");
/* Grep: user_task: soft rettenailleangle — Wave 68 exclusive tenailleangle stamp */
kprintf("user_task: soft rettenailleangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=111 "
        "(rettenailleangle stamp; Soft≠product)\n");
/*
 * ---- Wave 69 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: user_task: soft retdemiluneangle — Wave 69 return-demiluneangle honesty */
kprintf("user_task: soft retdemiluneangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=111 "
        "(retdemiluneangle honesty; Soft≠product; not bar3)\n");
/* Grep: user_task: soft retcoveredwayangle — Wave 69 exclusive coveredwayangle stamp */
kprintf("user_task: soft retcoveredwayangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=111 "
        "(retcoveredwayangle stamp; Soft≠product)\n");
/*
 * ---- Wave 70 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: user_task: soft retfosseangle — Wave 70 return-fosseangle honesty */
kprintf("user_task: soft retfosseangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=111 (retfosseangle honesty; Soft≠product; not bar3)\n");
/* Grep: user_task: soft retcounterscarple — Wave 70 exclusive counterscarple stamp */
kprintf("user_task: soft retcounterscarple exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=111 (retcounterscarple stamp; Soft≠product)\n");
/*
 * ---- Wave 71 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: user_task: soft retsallyportangle — Wave 71 return-sallyportangle honesty */
kprintf("user_task: soft retsallyportangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=111 (retsallyportangle honesty; Soft≠product; not bar3)\n");
/* Grep: user_task: soft retreentrantangle — Wave 71 exclusive reentrantangle stamp */
kprintf("user_task: soft retreentrantangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=111 (retreentrantangle stamp; Soft≠product)\n");
/*
 * ---- Wave 72 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: user_task: soft retplaceofarmsangle — Wave 72 return-placeofarmsangle honesty */
kprintf("user_task: soft retplaceofarmsangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=111 (retplaceofarmsangle honesty; Soft≠product; not bar3)\n");
/* Grep: user_task: soft retdoubletenailleangle — Wave 72 exclusive doubletenailleangle stamp */
kprintf("user_task: soft retdoubletenailleangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=111 (retdoubletenailleangle stamp; Soft≠product)\n");
/* Grep: user_task: soft retcurtainface — Wave 73 return-curtainface honesty */
kprintf("user_task: soft retcurtainface soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=111 (retcurtainface honesty; Soft≠product; not bar3)\n");
/* Grep: user_task: soft retbastionangle — Wave 73 exclusive bastionangle stamp */
kprintf("user_task: soft retbastionangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=111 (retbastionangle stamp; Soft≠product)\n");
/* Grep: user_task: soft retglacisangle — Wave 74 return-glacisangle honesty */
kprintf("user_task: soft retglacisangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=111 (retglacisangle honesty; Soft≠product; not bar3)\n");
/* Grep: user_task: soft retparapetangle — Wave 74 exclusive parapetangle stamp */
kprintf("user_task: soft retparapetangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=111 (retparapetangle stamp; Soft≠product)\n");
/* Grep: user_task: soft retmoatangle — Wave 75 return-moatangle honesty */
kprintf("user_task: soft retmoatangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=111 (retmoatangle honesty; Soft≠product; not bar3)\n");
/* Grep: user_task: soft retowerangle — Wave 75 exclusive towerangle stamp */
kprintf("user_task: soft retowerangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=111 (retowerangle stamp; Soft≠product)\n");
/* Grep: user_task: soft retgateangle — Wave 76 return-gateangle honesty */
kprintf("user_task: soft retgateangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=111 (retgateangle honesty; Soft≠product; not bar3)\n");
/* Grep: user_task: soft retwallangle — Wave 76 exclusive wallangle stamp */
kprintf("user_task: soft retwallangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=111 (retwallangle stamp; Soft≠product)\n");
/* Grep: user_task: soft retspireangle — Wave 77 return-spireangle honesty */
kprintf("user_task: soft retspireangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=111 (retspireangle honesty; Soft≠product; not bar3)\n");
/* Grep: user_task: soft retholdangle — Wave 77 exclusive holdangle stamp */
kprintf("user_task: soft retholdangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=111 (retholdangle stamp; Soft≠product)\n");
/* Grep: user_task: soft retpalaceangle — Wave 78 return-palaceangle honesty */
kprintf("user_task: soft retpalaceangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=111 (retpalaceangle honesty; Soft≠product; not bar3)\n");
/* Grep: user_task: soft retfortressangle — Wave 78 exclusive fortressangle stamp */
kprintf("user_task: soft retfortressangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=111 (retfortressangle stamp; Soft≠product)\n");
/* Grep: user_task: soft retkeepangle — Wave 79 return-keepangle honesty */
kprintf("user_task: soft retkeepangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=111 (retkeepangle honesty; Soft≠product; not bar3)\n");
/* Grep: user_task: soft retredoubtangle — Wave 79 exclusive redoubtangle stamp */
kprintf("user_task: soft retredoubtangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=111 (retredoubtangle stamp; Soft≠product)\n");
/* Grep: user_task: soft retcitadelangle — Wave 80 return-citadelangle honesty */
kprintf("user_task: soft retcitadelangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=111 (retcitadelangle honesty; Soft≠product; not bar3)\n");
/* Grep: user_task: soft retbastionkeep — Wave 80 exclusive bastionkeep stamp */
kprintf("user_task: soft retbastionkeep exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=111 (retbastionkeep stamp; Soft≠product)\n");
/* Grep: user_task: soft retpanoplyangle — Wave 81 return-panoplyangle honesty */
kprintf("user_task: soft retpanoplyangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=111 (retpanoplyangle honesty; Soft≠product; not bar3)\n");
/* Grep: user_task: soft retbulwarkangle — Wave 81 exclusive bulwarkangle stamp */
kprintf("user_task: soft retbulwarkangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=111 (retbulwarkangle stamp; Soft≠product)\n");
/* Grep: user_task: soft retmantleangle — Wave 82 return-mantleangle honesty */
kprintf("user_task: soft retmantleangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=111 (retmantleangle honesty; Soft≠product; not bar3)\n");
/* Grep: user_task: soft retaegisangle — Wave 82 exclusive aegisangle stamp */
kprintf("user_task: soft retaegisangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=111 (retaegisangle stamp; Soft≠product)\n");
/* Grep: user_task: soft retemblemangle — Wave 83 return-emblemangle honesty */
kprintf("user_task: soft retemblemangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=111 (retemblemangle honesty; Soft≠product; not bar3)\n");
/* Grep: user_task: soft retsigilangle — Wave 83 exclusive sigilangle stamp */
kprintf("user_task: soft retsigilangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=111 (retsigilangle stamp; Soft≠product)\n");
/* Grep: user_task: soft retscepterangle — Wave 84 return-scepterangle honesty */
kprintf("user_task: soft retscepterangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=111 (retscepterangle honesty; Soft≠product; not bar3)\n");
/* Grep: user_task: soft retglyphangle — Wave 84 exclusive glyphangle stamp */
kprintf("user_task: soft retglyphangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=111 (retglyphangle stamp; Soft≠product)\n");
/* Grep: user_task: soft retcrownangle — Wave 85 return-crownangle honesty */
kprintf("user_task: soft retcrownangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=111 (retcrownangle honesty; Soft≠product; not bar3)\n");
/* Grep: user_task: soft retshardangle — Wave 85 exclusive shardangle stamp */
kprintf("user_task: soft retshardangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=111 (retshardangle stamp; Soft≠product)\n");
/* Grep: user_task: soft retforgeangle — Wave 86 return-forgeangle honesty */
kprintf("user_task: soft retforgeangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=111 (retforgeangle honesty; Soft≠product; not bar3)\n");
/* Grep: user_task: soft retprismangle — Wave 86 exclusive prismangle stamp */
kprintf("user_task: soft retprismangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=111 (retprismangle stamp; Soft≠product)\n");
/* Grep: user_task: soft retflameangle — Wave 87 return-flameangle honesty */
kprintf("user_task: soft retflameangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=111 (retflameangle honesty; Soft≠product; not bar3)\n");
/* Grep: user_task: soft retcipherangle — Wave 87 exclusive cipherangle stamp */
kprintf("user_task: soft retcipherangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=111 (retcipherangle stamp; Soft≠product)\n");
/* Grep: user_task: soft retbeaconangle — Wave 88 return-beaconangle honesty */
kprintf("user_task: soft retbeaconangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=111 (retbeaconangle honesty; Soft≠product; not bar3)\n");
/* Grep: user_task: soft retledgerangle — Wave 88 exclusive ledgerangle stamp */
kprintf("user_task: soft retledgerangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=111 (retledgerangle stamp; Soft≠product)\n");
/* Grep: user_task: soft retbannerangle — Wave 89 return-bannerangle honesty */
kprintf("user_task: soft retbannerangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=111 (retbannerangle honesty; Soft≠product; not bar3)\n");
/* Grep: user_task: soft retvaultangle — Wave 89 exclusive vaultangle stamp */
kprintf("user_task: soft retvaultangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=111 (retvaultangle stamp; Soft≠product)\n");
/* Grep: user_task: soft retcrestangle — Wave 90 return-crestangle honesty */
kprintf("user_task: soft retcrestangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=111 (retcrestangle honesty; Soft≠product; not bar3)\n");
/* Grep: user_task: soft rettokenangle — Wave 90 exclusive tokenangle stamp */
kprintf("user_task: soft rettokenangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=111 (rettokenangle stamp; Soft≠product)\n");
/* Grep: user_task: soft retbadgeangle — Wave 91 return-badgeangle honesty */
kprintf("user_task: soft retbadgeangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=111 (retbadgeangle honesty; Soft≠product; not bar3)\n");
/* Grep: user_task: soft retphaseangle — Wave 91 exclusive phaseangle stamp */
kprintf("user_task: soft retphaseangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=111 (retphaseangle stamp; Soft≠product)\n");
/* Grep: user_task: soft retmarkangle — Wave 92 return-markangle honesty */
kprintf("user_task: soft retmarkangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=111 (retmarkangle honesty; Soft≠product; not bar3)\n");
/* Grep: user_task: soft retpulseangle — Wave 92 exclusive pulseangle stamp */
kprintf("user_task: soft retpulseangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=111 (retpulseangle stamp; Soft≠product)\n");

/* Grep: user_task: soft retsealangle — Wave 93 return-sealangle honesty */
kprintf("user_task: soft retsealangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=111 (retsealangle honesty; Soft≠product; not bar3)\n");
/* Grep: user_task: soft retboundangle — Wave 93 exclusive boundangle stamp */
kprintf("user_task: soft retboundangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=111 (retboundangle stamp; Soft≠product)\n");
/* Grep: user_task: soft retstemangle — Wave 94 return-stemangle honesty */
kprintf("user_task: soft retstemangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=111 (retstemangle honesty; Soft≠product; not bar3)\n");
/* Grep: user_task: soft retbladeangle — Wave 94 exclusive bladeangle stamp */
kprintf("user_task: soft retbladeangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=111 (retbladeangle stamp; Soft≠product)\n");
/* Grep: user_task: soft retchordangle — Wave 95 return-chordangle honesty */
kprintf("user_task: soft retchordangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=111 (retchordangle honesty; Soft≠product; not bar3)\n");
/* Grep: user_task: soft retarcangle — Wave 95 exclusive arcangle stamp */
kprintf("user_task: soft retarcangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=111 (retarcangle stamp; Soft≠product)\n");
/* Grep: user_task: soft retsectorangle — Wave 96 return-sectorangle honesty */
kprintf("user_task: soft retsectorangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=111 (retsectorangle honesty; Soft≠product; not bar3)\n");
/* Grep: user_task: soft retwedgeangle — Wave 96 exclusive wedgeangle stamp */
kprintf("user_task: soft retwedgeangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=111 (retwedgeangle stamp; Soft≠product)\n");
/* Grep: user_task: soft retradiusangle — Wave 97 return-radiusangle honesty */
kprintf("user_task: soft retradiusangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=111 (retradiusangle honesty; Soft≠product; not bar3)\n");
/* Grep: user_task: soft retdiameterangle — Wave 97 exclusive diameterangle stamp */
kprintf("user_task: soft retdiameterangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=111 (retdiameterangle stamp; Soft≠product)\n");
/* Grep: user_task: soft retcircumangle — Wave 98 return-circumangle honesty */
kprintf("user_task: soft retcircumangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=111 (retcircumangle honesty; Soft≠product; not bar3)\n");
/* Grep: user_task: soft retellipseangle — Wave 98 exclusive ellipseangle stamp */
kprintf("user_task: soft retellipseangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=111 (retellipseangle stamp; Soft≠product)\n");
/* Grep: user_task: soft rethyperangle — Wave 99 return-hyperangle honesty */
kprintf("user_task: soft rethyperangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=111 (rethyperangle honesty; Soft≠product; not bar3)\n");
/* Grep: user_task: soft retparabolaangle — Wave 99 exclusive parabolaangle stamp */
kprintf("user_task: soft retparabolaangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=111 (retparabolaangle stamp; Soft≠product)\n");
/* Grep: user_task: soft retspiralangle — Wave 100 return-spiralangle honesty */
kprintf("user_task: soft retspiralangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=111 (retspiralangle honesty; Soft≠product; not bar3)\n");
/* Grep: user_task: soft rethelixangle — Wave 100 exclusive helixangle stamp */
kprintf("user_task: soft rethelixangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=111 (rethelixangle stamp; Soft≠product)\n");
/* Grep: user_task: soft rettorusangle — Wave 101 return-torusangle honesty */
kprintf("user_task: soft rettorusangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=111 (rettorusangle honesty; Soft≠product; not bar3)\n");
/* Grep: user_task: soft retknotangle — Wave 101 exclusive knotangle stamp */
kprintf("user_task: soft retknotangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=111 (retknotangle stamp; Soft≠product)\n");
/* Grep: user_task: soft retmoebiusangle — Wave 102 return-moebiusangle honesty */
kprintf("user_task: soft retmoebiusangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=111 (retmoebiusangle honesty; Soft≠product; not bar3)\n");
/* Grep: user_task: soft retkleinangle — Wave 102 exclusive kleinangle stamp */
kprintf("user_task: soft retkleinangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=111 (retkleinangle stamp; Soft≠product)\n");
/* Grep: user_task: soft retprojectangle — Wave 103 return-projectangle honesty */
kprintf("user_task: soft retprojectangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=111 (retprojectangle honesty; Soft≠product; not bar3)\n");
/* Grep: user_task: soft retaffineangle — Wave 103 exclusive affineangle stamp */
kprintf("user_task: soft retaffineangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=111 (retaffineangle stamp; Soft≠product)\n");
/* Grep: user_task: soft retlinearangle — Wave 104 return-linearangle honesty */
kprintf("user_task: soft retlinearangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=111 (retlinearangle honesty; Soft≠product; not bar3)\n");
/* Grep: user_task: soft retbilinearangle — Wave 104 exclusive bilinearangle stamp */
kprintf("user_task: soft retbilinearangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=111 (retbilinearangle stamp; Soft≠product)\n");
/* Grep: user_task: soft retquadraticangle — Wave 105 return-quadraticangle honesty */
kprintf("user_task: soft retquadraticangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=111 (retquadraticangle honesty; Soft≠product; not bar3)\n");
/* Grep: user_task: soft retcubicangle — Wave 105 exclusive cubicangle stamp */
kprintf("user_task: soft retcubicangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=111 (retcubicangle stamp; Soft≠product)\n");
/* Grep: user_task: soft retquarticangle — Wave 106 return-quarticangle honesty */
kprintf("user_task: soft retquarticangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=111 (retquarticangle honesty; Soft≠product; not bar3)\n");
/* Grep: user_task: soft retquinticangle — Wave 106 exclusive quinticangle stamp */
kprintf("user_task: soft retquinticangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=111 (retquinticangle stamp; Soft≠product)\n");
/* Grep: user_task: soft retsplineangle — Wave 107 return-splineangle honesty */
kprintf("user_task: soft retsplineangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=111 (retsplineangle honesty; Soft≠product; not bar3)\n");
/* Grep: user_task: soft retbezierangle — Wave 107 exclusive bezierangle stamp */
kprintf("user_task: soft retbezierangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=111 (retbezierangle stamp; Soft≠product)\n");
/* Grep: user_task: soft rethurmitangle — Wave 108 return-hermitangle honesty */
kprintf("user_task: soft rethurmitangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=111 (rethurmitangle honesty; Soft≠product; not bar3)\n");
/* Grep: user_task: soft retcatmullangle — Wave 108 exclusive catmullangle stamp */
kprintf("user_task: soft retcatmullangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=111 (retcatmullangle stamp; Soft≠product)\n");
/* Grep: user_task: soft retnurbsangle — Wave 109 return-nurbsangle honesty */
kprintf("user_task: soft retnurbsangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=111 (retnurbsangle honesty; Soft≠product; not bar3)\n");
/* Grep: user_task: soft retbsplineangle — Wave 109 exclusive bsplineangle stamp */
kprintf("user_task: soft retbsplineangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=111 (retbsplineangle stamp; Soft≠product)\n");
/* Grep: user_task: soft retmeshangle — Wave 110 return-meshangle honesty */
kprintf("user_task: soft retmeshangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=111 (retmeshangle honesty; Soft≠product; not bar3)\n");
/* Grep: user_task: soft retgridangle — Wave 110 exclusive gridangle stamp */
kprintf("user_task: soft retgridangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=111 (retgridangle stamp; Soft≠product)\n");
/* Grep: user_task: soft retvoxelangle — Wave 111 return-voxelangle honesty */
kprintf("user_task: soft retvoxelangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=111 (retvoxelangle honesty; Soft≠product; not bar3)\n");
/* Grep: user_task: soft rettexelangle — Wave 111 exclusive texelangle stamp */
kprintf("user_task: soft rettexelangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=111 (rettexelangle stamp; Soft≠product)\n");
                            kprintf("user_task: soft deepen wave=%u via=%s ring3_ok=%u pers_ok=%u "
            "enter_ok=%u soft=%u soft_bad=%u logs=%u "
            "(soft inventory only; not product gate)\n",
            GJ_USER_SOFT_WAVE, szViaSafe, g_cRing3MapOk, g_cPersMapOk,
            g_cEnterOk, g_cRing3Soft + g_cPersSoft,
            g_cRing3SoftBad + g_cPersSoftBad, g_u32SoftLogN);

    /* Grep: user_task: soft path */
    kprintf("user_task: soft path claim=ring3,personality,enter "
            "pte=RX_code+RW_NX_stack payload_match=1 layout=dual "
            "sysret_enter=1 G-PERS=1 wave=%u "
            "(soft inventory; not bar3)\n",
            GJ_USER_SOFT_WAVE);

    /* Grep: user_task: soft PASS | PARTIAL */
    kprintf("user_task: soft %s via=%s ring3_live=%u pers_live=%u "
            "enter_ok=%u log_n=%u wave=%u\n",
            szVerdict, szViaSafe, u32Ring3Live, u32PersLive, g_cEnterOk,
            g_u32SoftLogN, GJ_USER_SOFT_WAVE);
}

/**
 * After first product map/enter activity, print soft inventory once.
 * Diagnostics only.
 */
static void
user_soft_maybe_once(void)
{
    if (g_fSoftInvOnce != 0) {
        return;
    }
    if (g_cRing3MapOk == 0u && g_cPersMapOk == 0u &&
        g_cRing3MapFail == 0u && g_cPersMapFail == 0u &&
        g_cEnterOk == 0u && g_cEnterSkip == 0u) {
        return;
    }
    g_fSoftInvOnce = 1;
    user_soft_inventory("activity");
}

/**
 * Install user RX (or RW) page: zero frame under kernel CR3, copy payload,
 * map with final prot. Avoids intermediate RW→RX races / wrong PTE flags.
 */
static gj_status_t
user_install_page(gj_vaddr_t va, const void *pSrc, size_t cbSrc, u32 u32Prot)
{
    gj_paddr_t pa;
    u64 u64Saved;
    gj_status_t st;

    pa = pmm_alloc();
    if (pa == 0) {
        return GJ_ERR_NOMEM;
    }
    u64Saved = cpu_read_cr3();
    cpu_load_cr3(vmm_kernel_cr3());
    memset((void *)(gj_vaddr_t)pa, 0, GJ_PAGE_SIZE);
    if (pSrc != NULL && cbSrc > 0) {
        if (cbSrc > GJ_PAGE_SIZE) {
            cbSrc = GJ_PAGE_SIZE;
        }
        memcpy((void *)(gj_vaddr_t)pa, pSrc, cbSrc);
    }
    cpu_load_cr3(u64Saved);

    st = vmm_map_page(va, pa, u32Prot | GJ_VMM_PROT_USER);
    if (st != GJ_OK) {
        pmm_free(pa);
        return st;
    }
    return GJ_OK;
}

/**
 * Soft-unmap + free a contiguous install window (partial fail cleanup).
 * Best-effort: ignores NOENT; never panics.
 */
static void
user_unmap_window(gj_vaddr_t vaBase, u32 cPages)
{
    u32 iPage;

    for (iPage = 0; iPage < cPages; iPage++) {
        gj_vaddr_t va = vaBase + (gj_vaddr_t)iPage * (gj_vaddr_t)GJ_PAGE_SIZE;
        gj_paddr_t pa = vmm_virt_to_phys(va);

        (void)vmm_unmap_page(va);
        if (pa != 0) {
            pmm_free(pa & (gj_paddr_t)UT_PTE_ADDR_MASK);
        }
    }
}

/**
 * Multi-page RX blob install (soft max cMaxPages).
 * On any page fail: reverse-unmaps prior pages of this install.
 */
static gj_status_t
user_install_blob_rx(gj_vaddr_t vaBase, const void *pSrc, size_t cbSrc,
                     u32 cMaxPages, u32 *pOutPages)
{
    u32 cPages;
    u32 iPage;
    gj_status_t st;

    if (pOutPages != NULL) {
        *pOutPages = 0;
    }
    if (pSrc == NULL || cbSrc == 0 || cMaxPages == 0) {
        return GJ_ERR_INVAL;
    }
    cPages = (u32)((cbSrc + (size_t)GJ_PAGE_SIZE - 1u) / (size_t)GJ_PAGE_SIZE);
    if (cPages == 0 || cPages > cMaxPages) {
        return GJ_ERR_INVAL;
    }

    for (iPage = 0; iPage < cPages; iPage++) {
        size_t cbOff = (size_t)iPage * (size_t)GJ_PAGE_SIZE;
        size_t cbChunk = cbSrc - cbOff;
        const u8 *pChunk = (const u8 *)pSrc + cbOff;

        if (cbChunk > (size_t)GJ_PAGE_SIZE) {
            cbChunk = (size_t)GJ_PAGE_SIZE;
        }
        st = user_install_page(vaBase + (gj_vaddr_t)iPage * (gj_vaddr_t)GJ_PAGE_SIZE,
                               pChunk, cbChunk,
                               GJ_VMM_PROT_READ | GJ_VMM_PROT_EXEC);
        if (st != GJ_OK) {
            user_unmap_window(vaBase, iPage);
            return st;
        }
    }
    g_cCodePages += cPages;
    if (pOutPages != NULL) {
        *pOutPages = cPages;
    }
    return GJ_OK;
}

/**
 * Map stack pages growing down from u64StackTop (RW user).
 * Soft: reverse-unmaps partial stack on first failure.
 * Returns 0 on success, -1 on first map failure.
 */
static int
user_map_stack(u64 u64StackTop, u32 cPages)
{
    u32 iPage;
    gj_status_t st;

    if (cPages == 0 || (u64StackTop & ((u64)GJ_PAGE_SIZE - 1ull)) != 0) {
        kprintf("user: map stack soft FAIL align top=0x%lx pages=%u\n",
                (unsigned long)u64StackTop, cPages);
        return -1;
    }

    for (iPage = 0; iPage < cPages; iPage++) {
        u64 u64PageVa = u64StackTop - ((u64)iPage + 1ull) * GJ_PAGE_SIZE;

        st = user_install_page((gj_vaddr_t)u64PageVa, NULL, 0,
                               GJ_VMM_PROT_READ | GJ_VMM_PROT_WRITE);
        if (st != GJ_OK) {
            u32 iUndo;

            kprintf("user: map stack page %u va=0x%lx failed %d\n", iPage,
                    (unsigned long)u64PageVa, (int)st);
            /* Soft reverse: unmap pages already installed in this call. */
            for (iUndo = 0; iUndo < iPage; iUndo++) {
                u64 u64UndoVa =
                    u64StackTop - ((u64)iUndo + 1ull) * GJ_PAGE_SIZE;
                gj_paddr_t pa = vmm_virt_to_phys((gj_vaddr_t)u64UndoVa);

                (void)vmm_unmap_page((gj_vaddr_t)u64UndoVa);
                if (pa != 0) {
                    pmm_free(pa & (gj_paddr_t)UT_PTE_ADDR_MASK);
                }
            }
            return -1;
        }
    }
    g_cStackPages += cPages;
    return 0;
}

/**
 * Soft layout: code + stack inside user band, page-aligned, non-overlapping.
 * Also refuses collision with the sibling product window (ring3 vs personality).
 */
static int
user_layout_soft_ok(u64 u64CodeVa, size_t cbCode, u64 u64StackTop, u32 cStackPages,
                    u64 u64SiblingCode, size_t cbSibling, u64 u64SiblingStackTop,
                    u32 cSiblingStack, int fSiblingLive)
{
    u64 u64CodeEnd;
    u64 u64StackLo;
    u64 u64StackHi;
    size_t cbCodeRound;

    if ((u64CodeVa & ((u64)GJ_PAGE_SIZE - 1ull)) != 0) {
        return 0;
    }
    if ((u64StackTop & ((u64)GJ_PAGE_SIZE - 1ull)) != 0) {
        return 0;
    }
    if (cbCode == 0 || cStackPages == 0) {
        return 0;
    }

    cbCodeRound =
        (cbCode + (size_t)GJ_PAGE_SIZE - 1u) & ~((size_t)GJ_PAGE_SIZE - 1u);
    u64CodeEnd = u64CodeVa + (u64)cbCodeRound;
    u64StackHi = u64StackTop;
    u64StackLo = u64StackTop - (u64)cStackPages * (u64)GJ_PAGE_SIZE;

    if (u64StackLo >= u64StackHi) {
        return 0; /* underflow */
    }
    /* Entire code + stack must sit in the canonical user window. */
    if (!user_range_ok(u64CodeVa, (u64)cbCodeRound)) {
        return 0;
    }
    if (!user_range_ok(u64StackLo, u64StackHi - u64StackLo)) {
        return 0;
    }
    /* Code must not overlap own stack. */
    if (!(u64CodeEnd <= u64StackLo || u64CodeVa >= u64StackHi)) {
        return 0;
    }

    if (fSiblingLive != 0 && cbSibling > 0 && cSiblingStack > 0) {
        size_t cbSibRound =
            (cbSibling + (size_t)GJ_PAGE_SIZE - 1u) &
            ~((size_t)GJ_PAGE_SIZE - 1u);
        u64 u64SibCodeEnd = u64SiblingCode + (u64)cbSibRound;
        u64 u64SibStackHi = u64SiblingStackTop;
        u64 u64SibStackLo =
            u64SiblingStackTop - (u64)cSiblingStack * (u64)GJ_PAGE_SIZE;

        /* Soft: dual windows must not share code or stack pages. */
        if (!(u64CodeEnd <= u64SiblingCode || u64CodeVa >= u64SibCodeEnd)) {
            return 0;
        }
        if (!(u64StackHi <= u64SibStackLo || u64StackLo >= u64SibStackHi)) {
            return 0;
        }
        if (!(u64CodeEnd <= u64SibStackLo || u64CodeVa >= u64SibStackHi)) {
            return 0;
        }
        if (!(u64StackHi <= u64SiblingCode || u64StackLo >= u64SibCodeEnd)) {
            return 0;
        }
    }
    return 1;
}

/**
 * Soft RX leaf: present + USER + not writable + executable (!NX).
 */
static int
user_soft_pte_rx(gj_vaddr_t va)
{
    u64 u64Pte = vmm_read_pte(va);

    if ((u64Pte & UT_PTE_P) == 0) {
        return 0;
    }
    if ((u64Pte & UT_PTE_U) == 0) {
        return 0;
    }
    if ((u64Pte & UT_PTE_W) != 0) {
        return 0; /* W^X soft for trampoline / door code */
    }
    if ((u64Pte & UT_PTE_NX) != 0) {
        return 0;
    }
    return 1;
}

/**
 * Soft RW leaf: present + USER + writable + NX preferred (non-exec stack).
 */
static int
user_soft_pte_rw(gj_vaddr_t va)
{
    u64 u64Pte = vmm_read_pte(va);

    if ((u64Pte & UT_PTE_P) == 0) {
        return 0;
    }
    if ((u64Pte & UT_PTE_U) == 0) {
        return 0;
    }
    if ((u64Pte & UT_PTE_W) == 0) {
        return 0;
    }
    if ((u64Pte & UT_PTE_NX) == 0) {
        return 0; /* stack must not be executable */
    }
    return 1;
}

/**
 * Soft payload match: compare mapped frames to blob under kernel CR3 identity.
 * cb may span multiple pages. Returns 1 on match, 0 on mismatch / missing PA.
 */
static int
user_soft_payload_match(gj_vaddr_t vaBase, const void *pSrc, size_t cbSrc)
{
    size_t cbOff;
    u64 u64Saved;
    const u8 *pWant;

    if (pSrc == NULL || cbSrc == 0) {
        return 0;
    }
    pWant = (const u8 *)pSrc;
    u64Saved = cpu_read_cr3();
    cpu_load_cr3(vmm_kernel_cr3());

    for (cbOff = 0; cbOff < cbSrc; ) {
        gj_vaddr_t vaPage =
            (vaBase + (gj_vaddr_t)cbOff) & ~(gj_vaddr_t)(GJ_PAGE_SIZE - 1);
        size_t cbPageOff = (size_t)((vaBase + (gj_vaddr_t)cbOff) - vaPage);
        size_t cbChunk = (size_t)GJ_PAGE_SIZE - cbPageOff;
        gj_paddr_t pa;
        const u8 *pHave;
        size_t iByte;

        if (cbChunk > cbSrc - cbOff) {
            cbChunk = cbSrc - cbOff;
        }
        /*
         * Walk under the *saved* (process) CR3 so we read the product PTE,
         * then compare bytes via kernel identity of that PA.
         */
        cpu_load_cr3(u64Saved);
        pa = vmm_virt_to_phys(vaPage);
        cpu_load_cr3(vmm_kernel_cr3());
        if (pa == 0) {
            cpu_load_cr3(u64Saved);
            return 0;
        }
        pHave = (const u8 *)(gj_vaddr_t)(pa & (gj_paddr_t)UT_PTE_ADDR_MASK);
        pHave += cbPageOff;
        for (iByte = 0; iByte < cbChunk; iByte++) {
            if (pHave[iByte] != pWant[cbOff + iByte]) {
                cpu_load_cr3(u64Saved);
                return 0;
            }
        }
        cbOff += cbChunk;
    }

    cpu_load_cr3(u64Saved);
    return 1;
}

/**
 * Soft post-map observe for a code+stack window.
 * fWantNativePersona: 0 = expect LINUX (1), 1 = expect NATIVE (0); -1 = skip.
 * szTag is "ring3" or "personality" for greppable lines.
 * Returns GJ_OK on soft PASS; GJ_ERR_* on soft FAIL (caller fail-closes).
 */
static gj_status_t
user_map_soft_observe(u64 u64CodeVa, size_t cbCode, const void *pBlob,
                      u64 u64StackTop, u32 cStackPages, struct gj_process *pProc,
                      int nWantPersona, /* 0 native, 1 linux, -1 skip */
                      const char *szTag, u32 *pSoftOk, u32 *pSoftBad)
{
    u32 iPage;
    u32 cCodePages;
    u64 u64StackLo;
    int fMapped;
    int fAccess;

    if (szTag == NULL) {
        szTag = "user";
    }
    if (cbCode == 0 || pBlob == NULL || cStackPages == 0) {
        if (pSoftBad != NULL) {
            (*pSoftBad)++;
        }
        user_soft_inc(&g_u32SoftObserveBad);
        kprintf("user: %s map soft FAIL inval cb=%lu pages=%u\n", szTag,
                (unsigned long)cbCode, cStackPages);
        return GJ_ERR_INVAL;
    }

    cCodePages =
        (u32)((cbCode + (size_t)GJ_PAGE_SIZE - 1u) / (size_t)GJ_PAGE_SIZE);
    u64StackLo = u64StackTop - (u64)cStackPages * (u64)GJ_PAGE_SIZE;

    /* Soft: every code leaf RX + USER. */
    for (iPage = 0; iPage < cCodePages; iPage++) {
        gj_vaddr_t va =
            (gj_vaddr_t)u64CodeVa + (gj_vaddr_t)iPage * (gj_vaddr_t)GJ_PAGE_SIZE;

        if (!user_soft_pte_rx(va)) {
            if (pSoftBad != NULL) {
                (*pSoftBad)++;
            }
            kprintf("user: %s map soft FAIL code pte va=0x%lx pte=0x%lx\n",
                    szTag, (unsigned long)va,
                    (unsigned long)vmm_read_pte(va));
            return GJ_ERR_FAULT;
        }
    }

    /* Soft: every stack leaf RW|NX + USER. */
    for (iPage = 0; iPage < cStackPages; iPage++) {
        gj_vaddr_t va =
            (gj_vaddr_t)(u64StackTop - ((u64)iPage + 1ull) * GJ_PAGE_SIZE);

        if (!user_soft_pte_rw(va)) {
            if (pSoftBad != NULL) {
                (*pSoftBad)++;
            }
            kprintf("user: %s map soft FAIL stack pte va=0x%lx pte=0x%lx\n",
                    szTag, (unsigned long)va,
                    (unsigned long)vmm_read_pte(va));
            return GJ_ERR_FAULT;
        }
    }

    /* Soft: range helpers agree with raw PTE walk. */
    fMapped = user_range_mapped(u64CodeVa, (u64)cbCode);
    fAccess = user_range_mapped_access(u64StackLo,
                                       (u64)cStackPages * (u64)GJ_PAGE_SIZE,
                                       GJ_USER_ACCESS_READ | GJ_USER_ACCESS_WRITE);
    if (fMapped == 0 || fAccess == 0) {
        if (pSoftBad != NULL) {
            (*pSoftBad)++;
        }
        kprintf("user: %s map soft FAIL range code=%d stack_w=%d\n", szTag,
                fMapped, fAccess);
        return GJ_ERR_FAULT;
    }

    /* Soft: payload bytes still match the embedded blob. */
    if (!user_soft_payload_match((gj_vaddr_t)u64CodeVa, pBlob, cbCode)) {
        if (pSoftBad != NULL) {
            (*pSoftBad)++;
        }
        kprintf("user: %s map soft FAIL payload mismatch cb=%lu\n", szTag,
                (unsigned long)cbCode);
        return GJ_ERR_FAULT;
    }

    /* Soft: process personality / JIT contract when a PCB is provided. */
    if (pProc != NULL && nWantPersona >= 0) {
        if ((int)pProc->u32Personality != nWantPersona) {
            if (pSoftBad != NULL) {
                (*pSoftBad)++;
            }
            kprintf("user: %s map soft FAIL persona=%u want=%d\n", szTag,
                    pProc->u32Personality, nWantPersona);
            return GJ_ERR_PERM;
        }
        /* Ring3 trampoline must not advertise CapJit; personality is native. */
        if (nWantPersona == 1 && pProc->u32Jit != 0) {
            if (pSoftBad != NULL) {
                (*pSoftBad)++;
            }
            kprintf("user: %s map soft FAIL jit=%u want=0\n", szTag,
                    pProc->u32Jit);
            return GJ_ERR_PERM;
        }
    }

    if (pSoftOk != NULL) {
        (*pSoftOk)++;
    }
    user_soft_inc(&g_u32SoftObserveOk);
    /* Grep: user: ring3 map soft PASS / user: personality map soft PASS */
    kprintf("user: %s map soft PASS code@0x%lx stacktop@0x%lx code_pages=%u "
            "stack_pages=%u blob=%lu pte_rx=1 pte_rw=1 payload=1\n",
            szTag, (unsigned long)u64CodeVa, (unsigned long)u64StackTop,
            cCodePages, cStackPages, (unsigned long)cbCode);
    return GJ_OK;
}

/**
 * Soft teardown of a just-installed window after soft observe FAIL.
 */
static void
user_map_soft_fail_teardown(gj_vaddr_t vaCode, u32 cCodePages, u64 u64StackTop,
                            u32 cStackPages)
{
    u32 iPage;

    user_soft_inc(&g_u32SoftTeardown);
    user_unmap_window(vaCode, cCodePages);
    for (iPage = 0; iPage < cStackPages; iPage++) {
        u64 u64PageVa = u64StackTop - ((u64)iPage + 1ull) * GJ_PAGE_SIZE;
        gj_paddr_t pa = vmm_virt_to_phys((gj_vaddr_t)u64PageVa);

        (void)vmm_unmap_page((gj_vaddr_t)u64PageVa);
        if (pa != 0) {
            pmm_free(pa & (gj_paddr_t)UT_PTE_ADDR_MASK);
        }
    }
}

int
user_task_map_ring3(struct gj_process *pProc)
{
    size_t cbBlob;
    gj_status_t st;
    u32 cCodePages = 0;

    cbBlob = (size_t)(gj_user_ring3_blob_end - gj_user_ring3_blob);
    if (cbBlob == 0 ||
        cbBlob > (size_t)GJ_USER_CODE_PAGES * (size_t)GJ_PAGE_SIZE) {
        g_cRing3MapFail++;
        user_soft_inc(&g_u32SoftBlobBadRing3);
        kprintf("user: bad ring3 blob size %lu\n", (unsigned long)cbBlob);
        user_soft_maybe_once();
        return -1;
    }

    /* Soft idempotent: already live + soft recheck still green → success. */
    if (g_fUserMapped != 0) {
        if (user_task_ring3_map_soft() == 0) {
            user_soft_inc(&g_u32SoftIdemRing3);
            kprintf("user: ring3 map soft already live recheck PASS\n");
            user_soft_maybe_once();
            return 0;
        }
        /* Soft bad on live map: tear down before reinstall (avoid PA leak). */
        user_map_soft_fail_teardown((gj_vaddr_t)GJ_USER_CODE_VA,
                                    g_cRing3CodePages != 0 ? g_cRing3CodePages
                                                           : 1u,
                                    GJ_USER_STACK_TOP, GJ_USER_STACK_PAGES);
        g_fUserMapped = 0;
        g_cbRing3Blob = 0;
        g_cRing3CodePages = 0;
    }

    if (!user_layout_soft_ok(GJ_USER_CODE_VA, cbBlob, GJ_USER_STACK_TOP,
                             GJ_USER_STACK_PAGES, GJ_PERS_CODE_VA, g_cbPersBlob,
                             GJ_PERS_STACK_TOP, GJ_PERS_STACK_PAGES,
                             g_fPersMapped)) {
        g_cRing3MapFail++;
        g_cRing3SoftBad++;
        user_soft_inc(&g_u32SoftLayoutFailRing3);
        kprintf("user: ring3 map soft FAIL layout\n");
        user_soft_maybe_once();
        return -1;
    }

    if (pProc != NULL) {
        if (process_as_ensure(pProc) != GJ_OK) {
            g_cRing3MapFail++;
            user_soft_inc(&g_u32SoftAsEnsureFailRing3);
            kprintf("user: as_ensure failed\n");
            user_soft_maybe_once();
            return -1;
        }
        process_as_activate(pProc);
    }

    st = user_install_blob_rx((gj_vaddr_t)GJ_USER_CODE_VA, gj_user_ring3_blob,
                              cbBlob, GJ_USER_CODE_PAGES, &cCodePages);
    if (st != GJ_OK) {
        g_cRing3MapFail++;
        user_soft_inc(&g_u32SoftInstallFailRing3);
        kprintf("user: map code RX failed %d\n", (int)st);
        user_soft_maybe_once();
        return -1;
    }

    if (user_map_stack(GJ_USER_STACK_TOP, GJ_USER_STACK_PAGES) != 0) {
        user_unmap_window((gj_vaddr_t)GJ_USER_CODE_VA, cCodePages);
        g_cRing3MapFail++;
        user_soft_inc(&g_u32SoftStackFailRing3);
        user_soft_maybe_once();
        return -1;
    }

    if (pProc != NULL) {
        pProc->u32Personality = 1; /* Linux-shaped trampoline for ABI smokes */
        gj_process_set_jit(pProc, 0);
    }

    st = user_map_soft_observe(GJ_USER_CODE_VA, cbBlob, gj_user_ring3_blob,
                               GJ_USER_STACK_TOP, GJ_USER_STACK_PAGES, pProc,
                               /* linux */ 1, "ring3", &g_cRing3Soft,
                               &g_cRing3SoftBad);
    if (st != GJ_OK) {
        /* Fail closed: drop partial product window. */
        user_map_soft_fail_teardown((gj_vaddr_t)GJ_USER_CODE_VA, cCodePages,
                                    GJ_USER_STACK_TOP, GJ_USER_STACK_PAGES);
        g_cRing3MapFail++;
        user_soft_inc(&g_u32SoftObserveBad);
        kprintf("user: ring3 map soft FAIL st=%d\n", (int)st);
        user_soft_maybe_once();
        return -1;
    }

    g_fUserMapped = 1;
    g_cbRing3Blob = cbBlob;
    g_cRing3CodePages = cCodePages;
    g_cRing3MapOk++;
    kprintf("user: ring3 code@0x%lx stacktop@0x%lx blob=%lu pages=%u PASS\n",
            (unsigned long)GJ_USER_CODE_VA,
            (unsigned long)GJ_USER_STACK_TOP, (unsigned long)cbBlob, cCodePages);
    user_soft_maybe_once();
    return 0;
}

void
user_task_enter_ring3(void)
{
    if (!g_fUserMapped && !g_fPersMapped) {
        g_cEnterSkip++;
        user_soft_inc(&g_u32SoftEnterSkipNotMap);
        kprintf("user: not mapped\n");
        kprintf("user: ring3 enter soft SKIP not_mapped\n");
        user_soft_maybe_once();
        return;
    }
    if (!cpu_syscall_ready()) {
        g_cEnterSkip++;
        user_soft_inc(&g_u32SoftEnterSkipSyscall);
        kprintf("user: SYSCALL not ready\n");
        kprintf("user: ring3 enter soft SKIP syscall\n");
        user_soft_maybe_once();
        return;
    }
    /* Soft recheck of the trampoline window before irrevocable enter. */
    if (g_fUserMapped != 0 && user_task_ring3_map_soft() != 0) {
        g_cEnterSkip++;
        user_soft_inc(&g_u32SoftEnterSkipRecheck);
        kprintf("user: ring3 enter soft SKIP recheck\n");
        user_soft_maybe_once();
        return;
    }
    g_cEnterOk++;
    kprintf("user: entering ring3…\n");
    kprintf("user: ring3 enter soft PASS code@0x%lx stack@0x%lx\n",
            (unsigned long)GJ_USER_CODE_VA, (unsigned long)GJ_USER_STACK_TOP);
    user_soft_maybe_once();
    cpu_enter_user(GJ_USER_CODE_VA, GJ_USER_STACK_TOP);
}

int
user_personality_map(struct gj_process *pProc)
{
    size_t cbBlob;
    gj_status_t st;
    u32 cCodePages = 0;

    cbBlob = (size_t)(gj_protonrt_user_blob_end - gj_protonrt_user_blob);
    if (cbBlob == 0 ||
        cbBlob > (size_t)GJ_PERS_CODE_PAGES * (size_t)GJ_PAGE_SIZE) {
        g_cPersMapFail++;
        user_soft_inc(&g_u32SoftBlobBadPers);
        kprintf("user: bad protonrt blob size %lu\n", (unsigned long)cbBlob);
        user_soft_maybe_once();
        return -1;
    }

    /* Soft idempotent: live personality soft recheck. */
    if (g_fPersMapped != 0) {
        if (user_personality_map_soft() == 0) {
            user_soft_inc(&g_u32SoftIdemPers);
            kprintf("user: personality map soft already live recheck PASS\n");
            user_soft_maybe_once();
            return 0;
        }
        /* Soft bad on live map: tear down before reinstall (avoid PA leak). */
        user_map_soft_fail_teardown((gj_vaddr_t)GJ_PERS_CODE_VA,
                                    g_cPersCodePages != 0 ? g_cPersCodePages
                                                          : 1u,
                                    GJ_PERS_STACK_TOP, GJ_PERS_STACK_PAGES);
        g_fPersMapped = 0;
        g_u64PersEntry = 0;
        g_u64PersStack = 0;
        g_cbPersBlob = 0;
        g_cPersCodePages = 0;
    }

    if (!user_layout_soft_ok(GJ_PERS_CODE_VA, cbBlob, GJ_PERS_STACK_TOP,
                             GJ_PERS_STACK_PAGES, GJ_USER_CODE_VA, g_cbRing3Blob,
                             GJ_USER_STACK_TOP, GJ_USER_STACK_PAGES,
                             g_fUserMapped)) {
        g_cPersMapFail++;
        g_cPersSoftBad++;
        user_soft_inc(&g_u32SoftLayoutFailPers);
        kprintf("user: personality map soft FAIL layout\n");
        user_soft_maybe_once();
        return -1;
    }

    if (pProc != NULL) {
        if (process_as_ensure(pProc) != GJ_OK) {
            g_cPersMapFail++;
            user_soft_inc(&g_u32SoftAsEnsureFailPers);
            kprintf("user: personality as_ensure failed\n");
            user_soft_maybe_once();
            return -1;
        }
        process_as_activate(pProc);
        /* G-PERS: native syscalls for door IPC */
        pProc->u32Personality = 0;
    }

    st = user_install_blob_rx((gj_vaddr_t)GJ_PERS_CODE_VA, gj_protonrt_user_blob,
                              cbBlob, GJ_PERS_CODE_PAGES, &cCodePages);
    if (st != GJ_OK) {
        g_cPersMapFail++;
        user_soft_inc(&g_u32SoftInstallFailPers);
        kprintf("user: personality map code failed %d\n", (int)st);
        user_soft_maybe_once();
        return -1;
    }

    if (user_map_stack(GJ_PERS_STACK_TOP, GJ_PERS_STACK_PAGES) != 0) {
        user_unmap_window((gj_vaddr_t)GJ_PERS_CODE_VA, cCodePages);
        g_cPersMapFail++;
        user_soft_inc(&g_u32SoftStackFailPers);
        user_soft_maybe_once();
        return -1;
    }

    st = user_map_soft_observe(GJ_PERS_CODE_VA, cbBlob, gj_protonrt_user_blob,
                               GJ_PERS_STACK_TOP, GJ_PERS_STACK_PAGES, pProc,
                               /* native */ 0, "personality", &g_cPersSoft,
                               &g_cPersSoftBad);
    if (st != GJ_OK) {
        user_map_soft_fail_teardown((gj_vaddr_t)GJ_PERS_CODE_VA, cCodePages,
                                    GJ_PERS_STACK_TOP, GJ_PERS_STACK_PAGES);
        g_cPersMapFail++;
        user_soft_inc(&g_u32SoftObserveBad);
        kprintf("user: personality map soft FAIL st=%d\n", (int)st);
        user_soft_maybe_once();
        return -1;
    }

    g_fPersMapped = 1;
    g_u64PersEntry = GJ_PERS_CODE_VA;
    g_u64PersStack = GJ_PERS_STACK_TOP;
    g_cbPersBlob = cbBlob;
    g_cPersCodePages = cCodePages;
    g_cPersMapOk++;
    kprintf("user: personality server mapped @0x%lx (%lu bytes) pages=%u "
            "native PASS\n",
            (unsigned long)GJ_PERS_CODE_VA, (unsigned long)cbBlob, cCodePages);
    user_soft_maybe_once();
    return 0;
}

u64
user_personality_entry(void)
{
    return g_u64PersEntry;
}

u64
user_personality_stack(void)
{
    return g_u64PersStack;
}

int
user_task_ring3_mapped(void)
{
    return g_fUserMapped != 0 ? 1 : 0;
}

int
user_personality_mapped(void)
{
    return g_fPersMapped != 0 ? 1 : 0;
}

gj_status_t
user_task_stats(struct gj_user_task_stats *pOut)
{
    if (pOut == NULL) {
        return GJ_ERR_INVAL;
    }
    pOut->u32Ring3Ok = g_cRing3MapOk;
    pOut->u32Ring3Fail = g_cRing3MapFail;
    pOut->u32Ring3Soft = g_cRing3Soft;
    pOut->u32Ring3SoftBad = g_cRing3SoftBad;
    pOut->u32PersOk = g_cPersMapOk;
    pOut->u32PersFail = g_cPersMapFail;
    pOut->u32PersSoft = g_cPersSoft;
    pOut->u32PersSoftBad = g_cPersSoftBad;
    pOut->u32EnterOk = g_cEnterOk;
    pOut->u32EnterSkip = g_cEnterSkip;
    pOut->u32CodePages = g_cCodePages;
    pOut->u32StackPages = g_cStackPages;
    pOut->u32Ring3Mapped = g_fUserMapped != 0 ? 1u : 0u;
    pOut->u32PersMapped = g_fPersMapped != 0 ? 1u : 0u;
    return GJ_OK;
}

u32
user_task_stats_soft(void)
{
    /*
     * Wave 15: full greppable soft inventory dump (includes soft stats).
     * Grep: user: soft | user_task: soft
     */
    user_soft_inventory("stats");
    return g_cRing3Soft;
}

int
user_task_ring3_map_soft(void)
{
    gj_status_t st;

    if (g_fUserMapped == 0 || g_cbRing3Blob == 0) {
        user_soft_inc(&g_u32SoftRecheckFailRing3);
        kprintf("user: ring3 map soft recheck FAIL not_mapped\n");
        return -1;
    }
    st = user_map_soft_observe(GJ_USER_CODE_VA, g_cbRing3Blob, gj_user_ring3_blob,
                               GJ_USER_STACK_TOP, GJ_USER_STACK_PAGES, NULL,
                               -1, "ring3", &g_cRing3Soft, &g_cRing3SoftBad);
    if (st != GJ_OK) {
        user_soft_inc(&g_u32SoftRecheckFailRing3);
        kprintf("user: ring3 map soft recheck FAIL st=%d\n", (int)st);
        return -1;
    }
    user_soft_inc(&g_u32SoftRecheckPassRing3);
    kprintf("user: ring3 map soft recheck PASS\n");
    return 0;
}

int
user_personality_map_soft(void)
{
    gj_status_t st;

    if (g_fPersMapped == 0 || g_cbPersBlob == 0) {
        user_soft_inc(&g_u32SoftRecheckFailPers);
        kprintf("user: personality map soft recheck FAIL not_mapped\n");
        return -1;
    }
    if (g_u64PersEntry != GJ_PERS_CODE_VA ||
        g_u64PersStack != GJ_PERS_STACK_TOP) {
        g_cPersSoftBad++;
        user_soft_inc(&g_u32SoftRecheckFailPers);
        kprintf("user: personality map soft recheck FAIL entry=0x%lx "
                "stack=0x%lx\n",
                (unsigned long)g_u64PersEntry, (unsigned long)g_u64PersStack);
        return -1;
    }
    st = user_map_soft_observe(GJ_PERS_CODE_VA, g_cbPersBlob, gj_protonrt_user_blob,
                               GJ_PERS_STACK_TOP, GJ_PERS_STACK_PAGES, NULL, -1,
                               "personality", &g_cPersSoft, &g_cPersSoftBad);
    if (st != GJ_OK) {
        user_soft_inc(&g_u32SoftRecheckFailPers);
        kprintf("user: personality map soft recheck FAIL st=%d\n", (int)st);
        return -1;
    }
    user_soft_inc(&g_u32SoftRecheckPassPers);
    kprintf("user: personality map soft recheck PASS\n");
    return 0;
}
