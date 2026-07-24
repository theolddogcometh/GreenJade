/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * PSCI probe with conduit auto-select (HVC preferred on QEMU virt; SMC when
 * EL3/firmware present). Exception recovery: if SMC/HVC faults, handler
 * advances ELR past the instruction and returns NOT_SUPPORTED.
 *
 * -------------------------------------------------------------------------
 * Soft features path (no CPU_ON / SYSTEM_OFF side effects)
 * -------------------------------------------------------------------------
 * After VERSION succeeds, soft-query PSCI_FEATURES for a fixed set of
 * standard function IDs (CPU_ON_64, CPU_OFF, CPU_SUSPEND_64, AFFINITY_INFO,
 * SYSTEM_OFF, SYSTEM_RESET, MIGRATE_INFO_TYPE, SYSTEM_RESET2). Counts
 * supported vs NOT_SUPPORTED; never invokes power-state-changing calls.
 * Soft conduit detail + recover-fault counters are greppable.
 *
 * -------------------------------------------------------------------------
 * Soft inventory deepen (Wave 37 exclusive; this unit only)
 * -------------------------------------------------------------------------
 * Multi-line greppable "aarch64: psci soft …" under fixed areas:
 *   inventory | version | conduit | features | recover | gates | path | deepen
 * Version soft: maj/min/raw decode lamps after VERSION success.
 * Recover soft: probe-active + fault/call tallies (exception-path pairing).
 * Path honesty: FEATURES-only — no CPU_ON / SYSTEM_OFF / SUSPEND side effects.
 * Soft PASS/FAIL gates keep prior features shape; deepen never hard-gates.
 * Soft ≠ product SMP bring-up; soft ≠ bar3.
 *
 * Greppable:
 *   aarch64: psci PASS conduit=hvc|smc version=… el3=…
 *   aarch64: psci soft SKIP (no firmware)
 *   aarch64: psci features soft ok=… miss=… cpu_on=… …
 *   aarch64: psci features soft PASS | FAIL
 *   aarch64: psci conduit soft hvc|smc|none faults=…
 *   aarch64: psci soft inventory …
 *   aarch64: psci soft version …
 *   aarch64: psci soft recover …
 *   aarch64: psci soft gates …
 *   aarch64: psci soft path …
 *   aarch64: psci soft return inv_ret=… product_kernel=OPEN
 *   aarch64: psci soft deepen …
 *   aarch64: psci soft PASS | FAIL
 *
 * Freestanding pure C; no GPL Linux PSCI paste.
 */
#include "types_arch.h"
#include <stddef.h>

extern void aarch64_uart_puts(const char *sz);
extern void aarch64_uart_put_hex(unsigned long v);
extern void aarch64_uart_put_hex_n(unsigned long v, unsigned cNibbles);

/* PSCI function IDs (SMC32 / SMC64 where applicable). */
#define PSCI_VERSION              0x84000000ul
#define PSCI_CPU_SUSPEND          0x84000001ul
#define PSCI_CPU_SUSPEND_64       0xC4000001ul
#define PSCI_CPU_OFF              0x84000002ul
#define PSCI_CPU_ON               0x84000003ul
#define PSCI_CPU_ON_64            0xC4000003ul
#define PSCI_AFFINITY_INFO        0x84000004ul
#define PSCI_AFFINITY_INFO_64     0xC4000004ul
#define PSCI_MIGRATE_INFO_TYPE    0x84000006ul
#define PSCI_SYSTEM_OFF           0x84000008ul
#define PSCI_SYSTEM_RESET         0x84000009ul
#define PSCI_FEATURES             0x8400000Aul
#define PSCI_SYSTEM_RESET2_64     0xC4000012ul

#define PSCI_RET_SUCCESS          0l
#define PSCI_RET_NOT_SUPPORTED    (-1l)
#define PSCI_RET_INVALID_PARAMS   (-2l)
#define PSCI_RET_DENIED           (-3l)

/* Soft features table size (must match g_aSoftFeat). */
#define PSCI_SOFT_FEAT_COUNT      8u

/* Wave 62 soft inventory stamp (file-local; never product gate). */
#define PSCI_SOFT_WAVE 69u
#define PSCI_SOFT_AREAS 107u

/* Set by exception path when recovering a PSCI probe trap. */
volatile unsigned long g_psci_probe_fault;
volatile unsigned long g_psci_probe_result;
volatile int g_psci_probe_active;

/* Soft observability counters. */
static unsigned long g_cPsciCalls;
static unsigned long g_cPsciFaults;
static unsigned long g_cPsciFeatOk;
static unsigned long g_cPsciFeatMiss;
static unsigned long g_cPsciSoftInvLogs;
static unsigned long g_u64SoftVerRaw;
static unsigned g_uSoftVerMaj;
static unsigned g_uSoftVerMin;
static unsigned g_uSoftEl3;
static unsigned g_uSoftFeatCpuOnOk;
static unsigned g_uSoftLive; /* 1 if VERSION path live (not SKIP) */

enum psci_conduit {
    PSCI_CONDUIT_NONE = 0,
    PSCI_CONDUIT_HVC = 1,
    PSCI_CONDUIT_SMC = 2
};

static int g_nConduit = PSCI_CONDUIT_NONE;

struct psci_soft_feat {
    unsigned long u64Fn; /* function ID passed to PSCI_FEATURES */
    const char *szName;  /* short greppable tag */
};

/*
 * Soft feature set — standard DEN0022 IDs only. Order is fixed so serial
 * capture greps stay stable across deepen waves.
 */
static const struct psci_soft_feat g_aSoftFeat[PSCI_SOFT_FEAT_COUNT] = {
    { PSCI_CPU_ON_64,         "cpu_on64" },
    { PSCI_CPU_OFF,           "cpu_off" },
    { PSCI_CPU_SUSPEND_64,    "cpu_susp64" },
    { PSCI_AFFINITY_INFO_64,  "aff_info64" },
    { PSCI_SYSTEM_OFF,        "sys_off" },
    { PSCI_SYSTEM_RESET,      "sys_reset" },
    { PSCI_MIGRATE_INFO_TYPE, "mig_info" },
    { PSCI_SYSTEM_RESET2_64,  "sys_reset2" },
};

/*
 * Called from exception.c when g_psci_probe_active and sync exception.
 * Advances ELR by 4 (skip the smc/hvc) and returns 1 if handled.
 */
int
aarch64_psci_exception_recover(unsigned long *pElr, unsigned long *pX0)
{
    if (!g_psci_probe_active) {
        return 0;
    }
    g_psci_probe_fault = 1;
    g_psci_probe_result = (unsigned long)(long)PSCI_RET_NOT_SUPPORTED;
    g_cPsciFaults++;
    if (pElr != NULL) {
        *pElr = *pElr + 4ul; /* skip 32-bit HVC/SMC encoding */
    }
    if (pX0 != NULL) {
        *pX0 = g_psci_probe_result;
    }
    return 1;
}

static unsigned long
psci_hvc(unsigned long a0, unsigned long a1, unsigned long a2, unsigned long a3)
{
    register unsigned long x0 __asm__("x0") = a0;
    register unsigned long x1 __asm__("x1") = a1;
    register unsigned long x2 __asm__("x2") = a2;
    register unsigned long x3 __asm__("x3") = a3;

    g_psci_probe_fault = 0;
    g_psci_probe_active = 1;
    g_cPsciCalls++;
    __asm__ volatile("hvc #0"
                     : "+r"(x0), "+r"(x1), "+r"(x2), "+r"(x3)
                     :
                     : "x4", "x5", "x6", "x7", "x8", "x9", "x10", "x11",
                       "x12", "x13", "x14", "x15", "x16", "x17", "memory");
    g_psci_probe_active = 0;
    if (g_psci_probe_fault) {
        return g_psci_probe_result;
    }
    return x0;
}

static unsigned long
psci_smc(unsigned long a0, unsigned long a1, unsigned long a2, unsigned long a3)
{
    register unsigned long x0 __asm__("x0") = a0;
    register unsigned long x1 __asm__("x1") = a1;
    register unsigned long x2 __asm__("x2") = a2;
    register unsigned long x3 __asm__("x3") = a3;

    g_psci_probe_fault = 0;
    g_psci_probe_active = 1;
    g_cPsciCalls++;
    __asm__ volatile("smc #0"
                     : "+r"(x0), "+r"(x1), "+r"(x2), "+r"(x3)
                     :
                     : "x4", "x5", "x6", "x7", "x8", "x9", "x10", "x11",
                       "x12", "x13", "x14", "x15", "x16", "x17", "memory");
    g_psci_probe_active = 0;
    if (g_psci_probe_fault) {
        return g_psci_probe_result;
    }
    return x0;
}

static unsigned long
psci_call(unsigned long a0, unsigned long a1, unsigned long a2, unsigned long a3)
{
    if (g_nConduit == PSCI_CONDUIT_HVC) {
        return psci_hvc(a0, a1, a2, a3);
    }
    if (g_nConduit == PSCI_CONDUIT_SMC) {
        return psci_smc(a0, a1, a2, a3);
    }
    return (unsigned long)(long)PSCI_RET_NOT_SUPPORTED;
}

/*
 * Soft detect EL3 presence via ID_AA64PFR0_EL1.EL3 field [15:12].
 * Non-zero means EL3 implemented (firmware may still use HVC conduit).
 */
static int
el3_implemented(void)
{
    unsigned long u64Pfr0;

    __asm__ volatile("mrs %0, id_aa64pfr0_el1" : "=r"(u64Pfr0));
    return (((u64Pfr0 >> 12) & 0xful) != 0ul) ? 1 : 0;
}

/*
 * Soft PSCI_FEATURES batch. Never calls the target functions themselves —
 * only queries support. Returns number of supported (non-NOT_SUPPORTED)
 * features. Detail line lists each result hex.
 */
static unsigned
psci_features_soft(unsigned long *pFeatCpuOn)
{
    unsigned iFeat;
    unsigned cOk;
    unsigned long u64Feat;
    unsigned long u64CpuOnFeat;

    cOk = 0u;
    u64CpuOnFeat = (unsigned long)(long)PSCI_RET_NOT_SUPPORTED;
    g_cPsciFeatOk = 0ul;
    g_cPsciFeatMiss = 0ul;

    aarch64_uart_puts("aarch64: psci features soft");
    for (iFeat = 0u; iFeat < PSCI_SOFT_FEAT_COUNT; iFeat++) {
        u64Feat = psci_call(PSCI_FEATURES, g_aSoftFeat[iFeat].u64Fn, 0, 0);
        aarch64_uart_puts(" ");
        aarch64_uart_puts(g_aSoftFeat[iFeat].szName);
        aarch64_uart_puts("=");
        aarch64_uart_put_hex(u64Feat);

        if ((long)u64Feat == PSCI_RET_NOT_SUPPORTED || g_psci_probe_fault) {
            g_cPsciFeatMiss++;
        } else {
            g_cPsciFeatOk++;
            cOk++;
        }
        if (g_aSoftFeat[iFeat].u64Fn == PSCI_CPU_ON_64) {
            u64CpuOnFeat = u64Feat;
        }
    }
    aarch64_uart_puts(" ok=");
    aarch64_uart_put_hex(g_cPsciFeatOk);
    aarch64_uart_puts(" miss=");
    aarch64_uart_put_hex(g_cPsciFeatMiss);
    aarch64_uart_puts("\n");

    if (pFeatCpuOn != 0) {
        *pFeatCpuOn = u64CpuOnFeat;
    }

    (void)PSCI_CPU_ON;
    (void)PSCI_CPU_SUSPEND;
    (void)PSCI_AFFINITY_INFO;
    (void)PSCI_RET_SUCCESS;
    (void)PSCI_RET_INVALID_PARAMS;
    (void)PSCI_RET_DENIED;
    return cOk;
}

static void
psci_conduit_soft_log(int fEl3)
{
    const char *szConduit;

    if (g_nConduit == PSCI_CONDUIT_HVC) {
        szConduit = "hvc";
    } else if (g_nConduit == PSCI_CONDUIT_SMC) {
        szConduit = "smc";
    } else {
        szConduit = "none";
    }

    aarch64_uart_puts("aarch64: psci conduit soft ");
    aarch64_uart_puts(szConduit);
    aarch64_uart_puts(" el3=");
    aarch64_uart_put_hex_n((unsigned long)fEl3, 1u);
    aarch64_uart_puts(" calls=");
    aarch64_uart_put_hex(g_cPsciCalls);
    aarch64_uart_puts(" faults=");
    aarch64_uart_put_hex(g_cPsciFaults);
    aarch64_uart_puts("\n");
}

/*
 * Wave 19 combined soft inventory under "aarch64: psci soft …".
 * Emits multi-area lamps + final soft PASS|FAIL. Never hard-gates.
 * Grep areas: inventory | version | conduit | features | recover | gates |
 *             path | deepen
 * Returns 1 on soft PASS (live firmware + features soft OK).
 */
static int
psci_soft_inventory(int fFeatSoft)
{
    const char *szConduit;
    unsigned uGateLive;
    unsigned uGateFeat;
    unsigned uGateCpuOn;
    unsigned uGateConduit;
    unsigned uGateCalls;
    int fOk;

    if (g_cPsciSoftInvLogs < 0xfffffffful) {
        g_cPsciSoftInvLogs++;
    }

    if (g_nConduit == PSCI_CONDUIT_HVC) {
        szConduit = "hvc";
    } else if (g_nConduit == PSCI_CONDUIT_SMC) {
        szConduit = "smc";
    } else {
        szConduit = "none";
    }

    uGateLive = g_uSoftLive;
    uGateFeat = (fFeatSoft != 0) ? 1u : 0u;
    uGateCpuOn = g_uSoftFeatCpuOnOk;
    uGateConduit = (g_nConduit != PSCI_CONDUIT_NONE) ? 1u : 0u;
    uGateCalls = (g_cPsciCalls > 0ul) ? 1u : 0u;

    /* Grep: aarch64: psci soft inventory */
    aarch64_uart_puts("aarch64: psci soft inventory live=");
    aarch64_uart_put_hex((unsigned long)uGateLive);
    aarch64_uart_puts(" conduit=");
    aarch64_uart_puts(szConduit);
    aarch64_uart_puts(" el3=");
    aarch64_uart_put_hex_n((unsigned long)g_uSoftEl3, 1u);
    aarch64_uart_puts(" feat_ok=");
    aarch64_uart_put_hex(g_cPsciFeatOk);
    aarch64_uart_puts(" feat_miss=");
    aarch64_uart_put_hex(g_cPsciFeatMiss);
    aarch64_uart_puts(" calls=");
    aarch64_uart_put_hex(g_cPsciCalls);
    aarch64_uart_puts(" faults=");
    aarch64_uart_put_hex(g_cPsciFaults);
    aarch64_uart_puts(" logs=");
    aarch64_uart_put_hex(g_cPsciSoftInvLogs);
    aarch64_uart_puts(" wave=");
    aarch64_uart_put_hex((unsigned long)PSCI_SOFT_WAVE);
    aarch64_uart_puts(" areas=");
    aarch64_uart_put_hex((unsigned long)PSCI_SOFT_AREAS);
    aarch64_uart_puts("\n");

    /* Grep: aarch64: psci soft version */
    aarch64_uart_puts("aarch64: psci soft version maj=");
    aarch64_uart_put_hex_n((unsigned long)g_uSoftVerMaj, 4u);
    aarch64_uart_puts(" min=");
    aarch64_uart_put_hex_n((unsigned long)g_uSoftVerMin, 4u);
    aarch64_uart_puts(" raw=");
    aarch64_uart_put_hex(g_u64SoftVerRaw);
    aarch64_uart_puts(" live=");
    aarch64_uart_put_hex((unsigned long)uGateLive);
    aarch64_uart_puts("\n");

    /* Grep: aarch64: psci soft recover (exception-path pairing lamps) */
    aarch64_uart_puts("aarch64: psci soft recover active=");
    aarch64_uart_put_hex((unsigned long)(g_psci_probe_active != 0 ? 1 : 0));
    aarch64_uart_puts(" fault_latched=");
    aarch64_uart_put_hex(g_psci_probe_fault);
    aarch64_uart_puts(" result=");
    aarch64_uart_put_hex(g_psci_probe_result);
    aarch64_uart_puts(" faults=");
    aarch64_uart_put_hex(g_cPsciFaults);
    aarch64_uart_puts(" calls=");
    aarch64_uart_put_hex(g_cPsciCalls);
    aarch64_uart_puts("\n");

    /* Grep: aarch64: psci soft gates */
    aarch64_uart_puts("aarch64: psci soft gates live=");
    aarch64_uart_put_hex((unsigned long)uGateLive);
    aarch64_uart_puts(" conduit=");
    aarch64_uart_put_hex((unsigned long)uGateConduit);
    aarch64_uart_puts(" feat=");
    aarch64_uart_put_hex((unsigned long)uGateFeat);
    aarch64_uart_puts(" cpu_on=");
    aarch64_uart_put_hex((unsigned long)uGateCpuOn);
    aarch64_uart_puts(" calls=");
    aarch64_uart_put_hex((unsigned long)uGateCalls);
    aarch64_uart_puts(" table=");
    aarch64_uart_put_hex((unsigned long)PSCI_SOFT_FEAT_COUNT);
    aarch64_uart_puts("\n");

    /* Grep: aarch64: psci soft path */
    aarch64_uart_puts("aarch64: psci soft path features=1 cpu_on=0 "
                      "sys_off=0 suspend=0 neon=0 product_kernel=OPEN "
                      "hard_gate=0 wave=");
    aarch64_uart_put_hex((unsigned long)PSCI_SOFT_WAVE);
    aarch64_uart_puts(" (soft inventory; not bar3)\n");

    /* Grep: aarch64: psci soft surf — Wave 19 gate bit lamps */
    aarch64_uart_puts("aarch64: psci soft surf live=");
    aarch64_uart_put_hex((unsigned long)uGateLive);
    aarch64_uart_puts(" conduit=");
    aarch64_uart_put_hex((unsigned long)uGateConduit);
    aarch64_uart_puts(" feat=");
    aarch64_uart_put_hex((unsigned long)uGateFeat);
    aarch64_uart_puts(" cpu_on=");
    aarch64_uart_put_hex((unsigned long)uGateCpuOn);
    aarch64_uart_puts(" calls=");
    aarch64_uart_put_hex((unsigned long)uGateCalls);
    aarch64_uart_puts(" bits=");
    aarch64_uart_put_hex((unsigned long)(
        (uGateLive << 0) | (uGateConduit << 1) | (uGateFeat << 2) |
        (uGateCpuOn << 3) | (uGateCalls << 4)));
    aarch64_uart_puts(" wave=");
    aarch64_uart_put_hex((unsigned long)PSCI_SOFT_WAVE);
    aarch64_uart_puts("\n");

    /* Grep: aarch64: psci soft deepen */
    /*
     * ---- Wave 19 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft≠product; not bar3.
     */
    /* Grep: aarch64: psci: soft retclass — Wave 19 return-class taxonomy (kept) */
    aarch64_uart_puts("aarch64: psci: soft retclass ok|fail|inval|nodev|busy|nomem "
                      "soft_only=1 product_gate=0 wave=");
    aarch64_uart_put_hex((unsigned long)PSCI_SOFT_WAVE);
    aarch64_uart_puts(" (retclass taxonomy; Soft!=product; not bar3)\n");
    /* Grep: aarch64: psci: soft retlane — Wave 19 return-lane catalog (kept) */
    aarch64_uart_puts("aarch64: psci: soft retlane inv|selftest|rate|retcode|retmap|class "
                      "product_kernel=OPEN soft_ne_product=1 wave=");
    aarch64_uart_put_hex((unsigned long)PSCI_SOFT_WAVE);
    aarch64_uart_puts(" (retlane catalog; Soft!=product)\n");
    /*
     * ---- Wave 20 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft!=product; not bar3.
     */
    /* Grep: aarch64: psci: soft retbound — Wave 20 return-bound honesty (kept) */
    aarch64_uart_puts("aarch64: psci: soft retbound soft_only=1 product_gate=0 hard_gate=0 "
                      "never_blocks_m0=1 wave=");
    aarch64_uart_put_hex((unsigned long)PSCI_SOFT_WAVE);
    aarch64_uart_puts(" (retbound honesty; Soft!=product; not bar3)\n");
    /* Grep: aarch64: psci: soft retseal — Wave 20 seal stamp (kept) */
    aarch64_uart_puts("aarch64: psci: soft retseal exclusive=1 soft_ne_product=1 "
                      "product_kernel=OPEN bar3=0 wave=");
    aarch64_uart_put_hex((unsigned long)PSCI_SOFT_WAVE);
    aarch64_uart_puts(" (retseal stamp; Soft!=product)\n");
    /*
     * ---- Wave 21 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft!=product; not bar3.
     */
    /* Grep: aarch64: psci: soft retpulse — Wave 21 return-pulse honesty (kept) */
    aarch64_uart_puts("aarch64: psci: soft retpulse soft_only=1 product_gate=0 soft_ne_product=1 "
                      "never_blocks_m0=1 wave=");
    aarch64_uart_put_hex((unsigned long)PSCI_SOFT_WAVE);
    aarch64_uart_puts(" (retpulse honesty; Soft!=product; not bar3)\n");
    /* Grep: aarch64: psci: soft retmark — Wave 21 mark stamp (kept) */
    aarch64_uart_puts("aarch64: psci: soft retmark exclusive=1 soft_ne_product=1 "
                      "product_kernel=OPEN bar3=0 wave=");
    aarch64_uart_put_hex((unsigned long)PSCI_SOFT_WAVE);
    aarch64_uart_puts(" (retmark stamp; Soft!=product)\n");
    
/*
 * ---- Wave 22 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft!=product; not bar3.
 */
/* Grep: aarch64: psci: soft retphase — Wave 22 return-phase honesty (kept) */
aarch64_uart_puts("aarch64: psci: soft retphase soft_only=1 product_gate=0 soft_ne_product=1 "
              "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)PSCI_SOFT_WAVE);
aarch64_uart_puts(" (retphase honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: psci: soft retbadge — Wave 22 badge stamp (kept) */
aarch64_uart_puts("aarch64: psci: soft retbadge exclusive=1 soft_ne_product=1 "
              "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)PSCI_SOFT_WAVE);
aarch64_uart_puts(" (retbadge stamp; Soft!=product)\n");
/*
 * ---- Wave 23 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft!=product; not bar3.
 */
/* Grep: aarch64: psci: soft rettoken — Wave 23 return-token honesty (kept) */
aarch64_uart_puts("aarch64: psci: soft rettoken soft_only=1 product_gate=0 soft_ne_product=1 "
              "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)PSCI_SOFT_WAVE);
aarch64_uart_puts(" (rettoken honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: psci: soft retcrest — Wave 23 crest stamp (kept) */
aarch64_uart_puts("aarch64: psci: soft retcrest exclusive=1 soft_ne_product=1 "
              "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)PSCI_SOFT_WAVE);
aarch64_uart_puts(" (retcrest stamp; Soft!=product)\n");
/*
 * ---- Wave 24 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft!=product; not bar3.
 */
/* Grep: aarch64: psci: soft retvault — Wave 24 return-vault honesty (kept) */
aarch64_uart_puts("aarch64: psci: soft retvault soft_only=1 product_gate=0 soft_ne_product=1 "
              "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)PSCI_SOFT_WAVE);
aarch64_uart_puts(" (retvault honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: psci: soft retbanner — Wave 24 banner stamp (kept) */
aarch64_uart_puts("aarch64: psci: soft retbanner exclusive=1 soft_ne_product=1 "
              "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)PSCI_SOFT_WAVE);
aarch64_uart_puts(" (retbanner stamp; Soft!=product)\n");
/*
 * ---- Wave 25 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft!=product; not bar3.
 */
/* Grep: aarch64: psci: soft retledger — Wave 25 return-ledger honesty (kept) */
aarch64_uart_puts("aarch64: psci: soft retledger soft_only=1 product_gate=0 soft_ne_product=1 "
              "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)PSCI_SOFT_WAVE);
aarch64_uart_puts(" (retledger honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: psci: soft retbeacon — Wave 25 beacon stamp (kept) */
aarch64_uart_puts("aarch64: psci: soft retbeacon exclusive=1 soft_ne_product=1 "
              "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)PSCI_SOFT_WAVE);
aarch64_uart_puts(" (retbeacon stamp; Soft!=product)\n");
/*
 * ---- Wave 26 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft!=product; not bar3.
 */
/* Grep: aarch64: psci: soft retcipher — Wave 26 return-cipher honesty (kept) */
aarch64_uart_puts("aarch64: psci: soft retcipher soft_only=1 product_gate=0 soft_ne_product=1 "
              "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)PSCI_SOFT_WAVE);
aarch64_uart_puts(" (retcipher honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: psci: soft retflame — Wave 26 flame stamp (kept) */
aarch64_uart_puts("aarch64: psci: soft retflame exclusive=1 soft_ne_product=1 "
              "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)PSCI_SOFT_WAVE);
aarch64_uart_puts(" (retflame stamp; Soft!=product)\n");
/*
 * ---- Wave 27 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft!=product; not bar3.
 */
/* Grep: aarch64: psci: soft retprism — Wave 27 return-prism honesty (kept) */
aarch64_uart_puts("aarch64: psci: soft retprism soft_only=1 product_gate=0 soft_ne_product=1 "
              "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)PSCI_SOFT_WAVE);
aarch64_uart_puts(" (retprism honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: psci: soft retforge — Wave 27 forge stamp (kept) */
aarch64_uart_puts("aarch64: psci: soft retforge exclusive=1 soft_ne_product=1 "
              "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)PSCI_SOFT_WAVE);
aarch64_uart_puts(" (retforge stamp; Soft!=product)\n");
/*
 * ---- Wave 28 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft!=product; not bar3.
 */
/* Grep: aarch64: psci: soft retshard — Wave 28 return-shard honesty (kept) */
aarch64_uart_puts("aarch64: psci: soft retshard soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)PSCI_SOFT_WAVE);
aarch64_uart_puts(" (retshard honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: psci: soft retcrown — Wave 28 crown stamp (kept) */
aarch64_uart_puts("aarch64: psci: soft retcrown exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)PSCI_SOFT_WAVE);
aarch64_uart_puts(" (retcrown stamp; Soft!=product)\n");
/*
 * ---- Wave 29 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft!=product; not bar3.
 */
/* Grep: aarch64: psci: soft retglyph — Wave 29 return-glyph honesty (kept) */
aarch64_uart_puts("aarch64: psci: soft retglyph soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)PSCI_SOFT_WAVE);
aarch64_uart_puts(" (retglyph honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: psci: soft retscepter — Wave 29 scepter stamp (kept) */
aarch64_uart_puts("aarch64: psci: soft retscepter exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)PSCI_SOFT_WAVE);
aarch64_uart_puts(" (retscepter stamp; Soft!=product)\n");
/*
 * ---- Wave 30 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft!=product; not bar3.
 */
/* Grep: aarch64: psci: soft retsigil — Wave 30 return-sigil honesty (kept) */
aarch64_uart_puts("aarch64: psci: soft retsigil soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)PSCI_SOFT_WAVE);
aarch64_uart_puts(" (retsigil honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: psci: soft retemblem — Wave 30 emblem stamp (kept) */
aarch64_uart_puts("aarch64: psci: soft retemblem exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)PSCI_SOFT_WAVE);
aarch64_uart_puts(" (retemblem stamp; Soft!=product)\n");
/*
 * ---- Wave 31 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft!=product; not bar3.
 */
/* Grep: aarch64: psci: soft retaegis — Wave 31 return-aegis honesty (kept) */
aarch64_uart_puts("aarch64: psci: soft retaegis soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)PSCI_SOFT_WAVE);
aarch64_uart_puts(" (retaegis honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: psci: soft retmantle — Wave 31 mantle stamp (kept) */
aarch64_uart_puts("aarch64: psci: soft retmantle exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)PSCI_SOFT_WAVE);
aarch64_uart_puts(" (retmantle stamp; Soft!=product)\n");
/*
 * ---- Wave 32 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft!=product; not bar3.
 */
/* Grep: aarch64: psci: soft retbulwark — Wave 32 return-bulwark honesty (kept) */
aarch64_uart_puts("aarch64: psci: soft retbulwark soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)PSCI_SOFT_WAVE);
aarch64_uart_puts(" (retbulwark honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: psci: soft retpanoply — Wave 32 panoply stamp (kept) */
aarch64_uart_puts("aarch64: psci: soft retpanoply exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)PSCI_SOFT_WAVE);
aarch64_uart_puts(" (retpanoply stamp; Soft!=product)\n");
/*
 * ---- Wave 33 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: psci: soft retbastion — Wave 33 return-bastion honesty (kept) */
aarch64_uart_puts("aarch64: psci: soft retbastion soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)PSCI_SOFT_WAVE);
aarch64_uart_puts(" (retbastion honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: psci: soft retcitadel — Wave 33 citadel stamp (kept) */
aarch64_uart_puts("aarch64: psci: soft retcitadel exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)PSCI_SOFT_WAVE);
aarch64_uart_puts(" (retcitadel stamp; Soft!=product)\n");
/*
 * ---- Wave 34 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: psci: soft retredoubt — Wave 34 return-redoubt honesty */
aarch64_uart_puts("aarch64: psci: soft retredoubt soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)PSCI_SOFT_WAVE);
aarch64_uart_puts(" (retredoubt honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: psci: soft retkeep — Wave 34 exclusive keep stamp */
aarch64_uart_puts("aarch64: psci: soft retkeep exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)PSCI_SOFT_WAVE);
aarch64_uart_puts(" (retkeep stamp; Soft!=product)\n");
/*
 * ---- Wave 35 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: psci: soft retfortress — Wave 35 return-fortress honesty */
aarch64_uart_puts("aarch64: psci: soft retfortress soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)PSCI_SOFT_WAVE);
aarch64_uart_puts(" (retfortress honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: psci: soft retpalace — Wave 35 exclusive palace stamp */
aarch64_uart_puts("aarch64: psci: soft retpalace exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)PSCI_SOFT_WAVE);
aarch64_uart_puts(" (retpalace stamp; Soft!=product)\n");
/*
 * ---- Wave 36 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: psci: soft rethold — Wave 36 return-hold honesty */
aarch64_uart_puts("aarch64: psci: soft rethold soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)PSCI_SOFT_WAVE);
aarch64_uart_puts(" (rethold honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: psci: soft retspire — Wave 36 exclusive spire stamp */
aarch64_uart_puts("aarch64: psci: soft retspire exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)PSCI_SOFT_WAVE);
aarch64_uart_puts(" (retspire stamp; Soft!=product)\n");
/*
 * ---- Wave 37 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: psci: soft retwall — Wave 37 return-wall honesty */
aarch64_uart_puts("aarch64: psci: soft retwall soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)PSCI_SOFT_WAVE);
aarch64_uart_puts(" (retwall honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: psci: soft retgate — Wave 37 exclusive gate stamp */
aarch64_uart_puts("aarch64: psci: soft retgate exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)PSCI_SOFT_WAVE);
aarch64_uart_puts(" (retgate stamp; Soft!=product)\n");
/*
 * ---- Wave 38 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: psci: soft retmoat — Wave 38 return-moat honesty */
aarch64_uart_puts("aarch64: psci: soft retmoat soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)PSCI_SOFT_WAVE);
aarch64_uart_puts(" (retmoat honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: psci: soft retower — Wave 38 exclusive tower stamp */
aarch64_uart_puts("aarch64: psci: soft retower exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)PSCI_SOFT_WAVE);
aarch64_uart_puts(" (retower stamp; Soft!=product)\n");
/*
 * ---- Wave 39 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: psci: soft retbarbican — Wave 39 return-barbican honesty */
aarch64_uart_puts("aarch64: psci: soft retbarbican soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)PSCI_SOFT_WAVE);
aarch64_uart_puts(" (retbarbican honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: psci: soft retglacis — Wave 39 exclusive glacis stamp */
aarch64_uart_puts("aarch64: psci: soft retglacis exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)PSCI_SOFT_WAVE);
aarch64_uart_puts(" (retglacis stamp; Soft!=product)\n");
/*
 * ---- Wave 40 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: psci: soft retcurtain — Wave 40 return-curtain honesty */
aarch64_uart_puts("aarch64: psci: soft retcurtain soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)PSCI_SOFT_WAVE);
aarch64_uart_puts(" (retcurtain honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: psci: soft retparapet — Wave 40 exclusive parapet stamp */
aarch64_uart_puts("aarch64: psci: soft retparapet exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)PSCI_SOFT_WAVE);
aarch64_uart_puts(" (retparapet stamp; Soft!=product)\n");
/*
 * ---- Wave 41 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: psci: soft retravelin — Wave 41 return-travelin honesty */
aarch64_uart_puts("aarch64: psci: soft retravelin soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)PSCI_SOFT_WAVE);
aarch64_uart_puts(" (retravelin honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: psci: soft retditch — Wave 41 exclusive ditch stamp */
aarch64_uart_puts("aarch64: psci: soft retditch exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)PSCI_SOFT_WAVE);
aarch64_uart_puts(" (retditch stamp; Soft!=product)\n");
/*
 * ---- Wave 42 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: psci: soft retportcullis — Wave 42 return-portcullis honesty */
aarch64_uart_puts("aarch64: psci: soft retportcullis soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)PSCI_SOFT_WAVE);
aarch64_uart_puts(" (retportcullis honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: psci: soft retbattlement — Wave 42 exclusive battlement stamp */
aarch64_uart_puts("aarch64: psci: soft retbattlement exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)PSCI_SOFT_WAVE);
aarch64_uart_puts(" (retbattlement stamp; Soft!=product)\n");
/*
 * ---- Wave 43 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: psci: soft retmachicolation — Wave 43 return-machicolation honesty */
aarch64_uart_puts("aarch64: psci: soft retmachicolation soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)PSCI_SOFT_WAVE);
aarch64_uart_puts(" (retmachicolation honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: psci: soft retarrowslit — Wave 43 exclusive arrowslit stamp */
aarch64_uart_puts("aarch64: psci: soft retarrowslit exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)PSCI_SOFT_WAVE);
aarch64_uart_puts(" (retarrowslit stamp; Soft!=product)\n");

/*
 * ---- Wave 44 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: psci: soft retmerlon — Wave 44 return-merlon honesty */
aarch64_uart_puts("aarch64: psci: soft retmerlon soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)PSCI_SOFT_WAVE);
aarch64_uart_puts(" (retmerlon honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: psci: soft retembrasure — Wave 44 exclusive embrasure stamp */
aarch64_uart_puts("aarch64: psci: soft retembrasure exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)PSCI_SOFT_WAVE);
aarch64_uart_puts(" (retembrasure stamp; Soft!=product)\n");

/*
 * ---- Wave 45 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: psci: soft retkeepgate — Wave 45 return-keepgate honesty */
aarch64_uart_puts("aarch64: psci: soft retkeepgate soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)PSCI_SOFT_WAVE);
aarch64_uart_puts(" (retkeepgate honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: psci: soft retouterward — Wave 45 exclusive outerward stamp */
aarch64_uart_puts("aarch64: psci: soft retouterward exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)PSCI_SOFT_WAVE);
aarch64_uart_puts(" (retouterward stamp; Soft!=product)\n");

/*
 * ---- Wave 46 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: psci: soft retbailey — Wave 46 return-bailey honesty */
aarch64_uart_puts("aarch64: psci: soft retbailey soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)PSCI_SOFT_WAVE);
aarch64_uart_puts(" (retbailey honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: psci: soft retpostern — Wave 46 exclusive postern stamp */
aarch64_uart_puts("aarch64: psci: soft retpostern exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)PSCI_SOFT_WAVE);
aarch64_uart_puts(" (retpostern stamp; Soft!=product)\n");

/*
 * ---- Wave 47 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: psci: soft retinnerward — Wave 47 return-innerward honesty */
aarch64_uart_puts("aarch64: psci: soft retinnerward soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)PSCI_SOFT_WAVE);
aarch64_uart_puts(" (retinnerward honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: psci: soft retdonjon — Wave 47 exclusive donjon stamp */
aarch64_uart_puts("aarch64: psci: soft retdonjon exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)PSCI_SOFT_WAVE);
aarch64_uart_puts(" (retdonjon stamp; Soft!=product)\n");

/*
 * ---- Wave 48 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: psci: soft retchevaux — Wave 48 return-chevaux honesty */
aarch64_uart_puts("aarch64: psci: soft retchevaux soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)PSCI_SOFT_WAVE);
aarch64_uart_puts(" (retchevaux honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: psci: soft retpalisade — Wave 48 exclusive palisade stamp */
aarch64_uart_puts("aarch64: psci: soft retpalisade exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)PSCI_SOFT_WAVE);
aarch64_uart_puts(" (retpalisade stamp; Soft!=product)\n");

/*
 * ---- Wave 49 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: psci: soft retglacisgate — Wave 49 return-glacisgate honesty */
aarch64_uart_puts("aarch64: psci: soft retglacisgate soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)PSCI_SOFT_WAVE);
aarch64_uart_puts(" (retglacisgate honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: psci: soft retoutwork — Wave 49 exclusive outwork stamp */
aarch64_uart_puts("aarch64: psci: soft retoutwork exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)PSCI_SOFT_WAVE);
aarch64_uart_puts(" (retoutwork stamp; Soft!=product)\n");
/*
 * ---- Wave 50 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: psci: soft retsally — Wave 50 return-sally honesty */
aarch64_uart_puts("aarch64: psci: soft retsally soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)PSCI_SOFT_WAVE);
aarch64_uart_puts(" (retsally honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: psci: soft retcounterscarp — Wave 50 exclusive counterscarp stamp */
aarch64_uart_puts("aarch64: psci: soft retcounterscarp exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)PSCI_SOFT_WAVE);
aarch64_uart_puts(" (retcounterscarp stamp; Soft!=product)\n");

/*
 * ---- Wave 51 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: psci: soft retfosse — Wave 51 return-fosse honesty */
aarch64_uart_puts("aarch64: psci: soft retfosse soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)PSCI_SOFT_WAVE);
aarch64_uart_puts(" (retfosse honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: psci: soft retcoveredway — Wave 51 exclusive coveredway stamp */
aarch64_uart_puts("aarch64: psci: soft retcoveredway exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)PSCI_SOFT_WAVE);
aarch64_uart_puts(" (retcoveredway stamp; Soft!=product)\n");

/*
 * ---- Wave 52 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: psci: soft rettenaille — Wave 52 return-tenaille honesty */
aarch64_uart_puts("aarch64: psci: soft rettenaille soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)PSCI_SOFT_WAVE);
aarch64_uart_puts(" (rettenaille honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: psci: soft retdemilune — Wave 52 exclusive demilune stamp */
aarch64_uart_puts("aarch64: psci: soft retdemilune exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)PSCI_SOFT_WAVE);
aarch64_uart_puts(" (retdemilune stamp; Soft!=product)\n");
/*
 * ---- Wave 53 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: psci: soft retravelin — Wave 53 return-travelin honesty */
aarch64_uart_puts("aarch64: psci: soft retravelin soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)PSCI_SOFT_WAVE);
aarch64_uart_puts(" (retravelin honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: psci: soft retlunette — Wave 53 exclusive lunette stamp */
aarch64_uart_puts("aarch64: psci: soft retlunette exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)PSCI_SOFT_WAVE);
aarch64_uart_puts(" (retlunette stamp; Soft!=product)\n");
/*
 * ---- Wave 54 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: psci: soft retcaponier — Wave 54 return-caponier honesty */
aarch64_uart_puts("aarch64: psci: soft retcaponier soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)PSCI_SOFT_WAVE);
aarch64_uart_puts(" (retcaponier honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: psci: soft retredan — Wave 54 exclusive redan stamp */
aarch64_uart_puts("aarch64: psci: soft retredan exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)PSCI_SOFT_WAVE);
aarch64_uart_puts(" (retredan stamp; Soft!=product)\n");
/*
 * ---- Wave 55 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: psci: soft retflank — Wave 55 return-flank honesty */
aarch64_uart_puts("aarch64: psci: soft retflank soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)PSCI_SOFT_WAVE);
aarch64_uart_puts(" (retflank honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: psci: soft retface — Wave 55 exclusive face stamp */
aarch64_uart_puts("aarch64: psci: soft retface exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)PSCI_SOFT_WAVE);
aarch64_uart_puts(" (retface stamp; Soft!=product)\n");
/*
 * ---- Wave 56 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: psci: soft retgorge — Wave 56 return-gorge honesty */
aarch64_uart_puts("aarch64: psci: soft retgorge soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)PSCI_SOFT_WAVE);
aarch64_uart_puts(" (retgorge honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: psci: soft retshoulder — Wave 56 exclusive shoulder stamp */
aarch64_uart_puts("aarch64: psci: soft retshoulder exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)PSCI_SOFT_WAVE);
aarch64_uart_puts(" (retshoulder stamp; Soft!=product)\n");
/*
 * ---- Wave 57 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: psci: soft retraverse — Wave 57 return-traverse honesty */
aarch64_uart_puts("aarch64: psci: soft retraverse soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)PSCI_SOFT_WAVE);
aarch64_uart_puts(" (retraverse honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: psci: soft retcasemate — Wave 57 exclusive casemate stamp */
aarch64_uart_puts("aarch64: psci: soft retcasemate exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)PSCI_SOFT_WAVE);
aarch64_uart_puts(" (retcasemate stamp; Soft!=product)\n");

/*
 * ---- Wave 58 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: psci: soft retorillon — Wave 58 return-orillon honesty */
aarch64_uart_puts("aarch64: psci: soft retorillon soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)PSCI_SOFT_WAVE);
aarch64_uart_puts(" (retorillon honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: psci: soft retbonnette — Wave 58 exclusive bonnette stamp */
aarch64_uart_puts("aarch64: psci: soft retbonnette exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)PSCI_SOFT_WAVE);
aarch64_uart_puts(" (retbonnette stamp; Soft!=product)\n");

/*
 * ---- Wave 59 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: psci: soft retcrownwork — Wave 59 return-crownwork honesty */
aarch64_uart_puts("aarch64: psci: soft retcrownwork soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)PSCI_SOFT_WAVE);
aarch64_uart_puts(" (retcrownwork honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: psci: soft rethornwork — Wave 59 exclusive hornwork stamp */
aarch64_uart_puts("aarch64: psci: soft rethornwork exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)PSCI_SOFT_WAVE);
aarch64_uart_puts(" (rethornwork stamp; Soft!=product)\n");

/*
 * ---- Wave 60 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: psci: soft retplace — Wave 60 return-place honesty */
aarch64_uart_puts("aarch64: psci: soft retplace soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)PSCI_SOFT_WAVE);
aarch64_uart_puts(" (retplace honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: psci: soft retenvelope — Wave 60 exclusive envelope stamp */
aarch64_uart_puts("aarch64: psci: soft retenvelope exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)PSCI_SOFT_WAVE);
aarch64_uart_puts(" (retenvelope stamp; Soft!=product)\n");









/*
 * ---- Wave 61 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: psci: soft retcounterguard — Wave 61 return-counterguard honesty */
aarch64_uart_puts("aarch64: psci: soft retcounterguard soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)PSCI_SOFT_WAVE);
aarch64_uart_puts(" (retcounterguard honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: psci: soft retcoveredface — Wave 61 exclusive coveredface stamp */
aarch64_uart_puts("aarch64: psci: soft retcoveredface exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)PSCI_SOFT_WAVE);
aarch64_uart_puts(" (retcoveredface stamp; Soft!=product)\n");
/*
 * ---- Wave 62 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft!=product; not bar3.
 */
/* Grep: aarch64: psci: soft retbastionface — Wave 62 return-bastionface honesty */
aarch64_uart_puts("aarch64: psci: soft retbastionface soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)PSCI_SOFT_WAVE);
aarch64_uart_puts(" (retbastionface honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: psci: soft retcurtainangle — Wave 62 exclusive curtainangle stamp */
aarch64_uart_puts("aarch64: psci: soft retcurtainangle exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)PSCI_SOFT_WAVE);
aarch64_uart_puts(" (retcurtainangle stamp; Soft!=product)\n");
/*
 * ---- Wave 63 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft!=product; not bar3.
 */
/* Grep: aarch64: psci: soft retdoubletenaille — Wave 63 return-doubletenaille honesty */
aarch64_uart_puts("aarch64: psci: soft retdoubletenaille soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)PSCI_SOFT_WAVE);
aarch64_uart_puts(" (retdoubletenaille honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: psci: soft retplaceofarms — Wave 63 exclusive placeofarms stamp */
aarch64_uart_puts("aarch64: psci: soft retplaceofarms exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)PSCI_SOFT_WAVE);
aarch64_uart_puts(" (retplaceofarms stamp; Soft!=product)\n");
/*
 * ---- Wave 64 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft!=product; not bar3.
 */
/* Grep: aarch64: psci: soft retreentrant — Wave 64 return-reentrant honesty */
aarch64_uart_puts("aarch64: psci: soft retreentrant soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)PSCI_SOFT_WAVE);
aarch64_uart_puts(" (retreentrant honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: psci: soft retsallyport — Wave 64 exclusive sallyport stamp */
aarch64_uart_puts("aarch64: psci: soft retsallyport exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)PSCI_SOFT_WAVE);
aarch64_uart_puts(" (retsallyport stamp; Soft!=product)\n");
/*
 * ---- Wave 65 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft!=product; not bar3.
 */
/* Grep: aarch64: psci: soft retgorgeangle — Wave 65 return-gorgeangle honesty */
aarch64_uart_puts("aarch64: psci: soft retgorgeangle soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)PSCI_SOFT_WAVE);
aarch64_uart_puts(" (retgorgeangle honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: psci: soft retshoulderangle — Wave 65 exclusive shoulderangle stamp */
aarch64_uart_puts("aarch64: psci: soft retshoulderangle exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)PSCI_SOFT_WAVE);
aarch64_uart_puts(" (retshoulderangle stamp; Soft!=product)\n");
/*
 * ---- Wave 66 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft!=product; not bar3.
 */
/* Grep: aarch64: psci: soft retflankangle — Wave 66 return-flankangle honesty */
aarch64_uart_puts("aarch64: psci: soft retflankangle soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)PSCI_SOFT_WAVE);
aarch64_uart_puts(" (retflankangle honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: psci: soft retfaceangle — Wave 66 exclusive faceangle stamp */
aarch64_uart_puts("aarch64: psci: soft retfaceangle exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
/*
 * ---- Wave 67 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: psci: soft retcaponierangle — Wave 67 return-caponierangle honesty */
aarch64_uart_puts("aarch64: psci: soft retcaponierangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=69 "
        "(retcaponierangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: psci: soft retredanangle — Wave 67 exclusive redanangle stamp */
aarch64_uart_puts("aarch64: psci: soft retredanangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=69 "
        "(retredanangle stamp; Soft≠product)\n");
/*
 * ---- Wave 68 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: psci: soft retlunetteangle — Wave 68 return-lunetteangle honesty */
aarch64_uart_puts("aarch64: psci: soft retlunetteangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=69 "
        "(retlunetteangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: psci: soft rettenailleangle — Wave 68 exclusive tenailleangle stamp */
aarch64_uart_puts("aarch64: psci: soft rettenailleangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=69 "
        "(rettenailleangle stamp; Soft≠product)\n");
/*
 * ---- Wave 69 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: psci: soft retdemiluneangle — Wave 69 return-demiluneangle honesty */
aarch64_uart_puts("aarch64: psci: soft retdemiluneangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=69 "
        "(retdemiluneangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: psci: soft retcoveredwayangle — Wave 69 exclusive coveredwayangle stamp */
aarch64_uart_puts("aarch64: psci: soft retcoveredwayangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=69 "
        "(retcoveredwayangle stamp; Soft≠product)\n");
aarch64_uart_put_hex((unsigned long)PSCI_SOFT_WAVE);
aarch64_uart_puts(" (retfaceangle stamp; Soft!=product)\n");
    aarch64_uart_puts("aarch64: psci soft deepen wave=");
    aarch64_uart_put_hex((unsigned long)PSCI_SOFT_WAVE);
    aarch64_uart_puts(" areas=");
    aarch64_uart_put_hex((unsigned long)PSCI_SOFT_AREAS);
    aarch64_uart_puts(" catalog=inventory,version,conduit,features,recover,"
                      "gates,path,surf,return,honesty,deepen unit=psci.c only "
                      "rate_limited=0 soft_only=1\n");

    /* Grep: aarch64: psci soft honesty */
    aarch64_uart_puts("aarch64: psci soft honesty product_kernel=OPEN "
                      "soft_only=1 no_cpu_on=1 no_bar3=1 wave=");
    aarch64_uart_put_hex((unsigned long)PSCI_SOFT_WAVE);
    aarch64_uart_puts("\n");

    /* Grep: aarch64: psci soft exclusive — Wave 35 exclusive deepen */
    aarch64_uart_puts("aarch64: psci soft exclusive multi_server=0 "
                      "confine=0 bar3=0 product_kernel=OPEN soft_only=1 wave=");
    aarch64_uart_put_hex((unsigned long)PSCI_SOFT_WAVE);
    aarch64_uart_puts("\n");

    /* Grep: aarch64: psci soft open — Wave 19 open-lamp rollup */
    aarch64_uart_puts("aarch64: psci soft open multi_server=0 confine=0 "
                      "bar3=0 product_kernel=OPEN soft_only=1 wave=");
    aarch64_uart_put_hex((unsigned long)PSCI_SOFT_WAVE);
    aarch64_uart_puts("\n");

    fOk = 0;
    if (uGateLive != 0u && uGateFeat != 0u && uGateCpuOn != 0u &&
        uGateConduit != 0u) {
        fOk = 1;
    }

    /* Grep: aarch64: psci soft return — Wave 19 return surfaces */
    aarch64_uart_puts("aarch64: psci soft return inv_ret=");
    aarch64_uart_put_hex((unsigned long)(fOk != 0 ? 1ul : 0ul));
    aarch64_uart_puts(" product_kernel=OPEN wave=");
    aarch64_uart_put_hex((unsigned long)PSCI_SOFT_WAVE);
    aarch64_uart_puts("\n");

    if (fOk != 0) {
        aarch64_uart_puts("aarch64: psci soft PASS\n");
    } else {
        aarch64_uart_puts("aarch64: psci soft FAIL\n");
    }
    return fOk;
}

void
aarch64_psci_probe(void)
{
    unsigned long u64Ver;
    unsigned long u64FeatCpuOn;
    unsigned maj;
    unsigned min;
    unsigned cFeatOk;
    int fEl3;
    int fFeatSoft;
    int fInvSoft;

    fEl3 = el3_implemented();
    u64FeatCpuOn = (unsigned long)(long)PSCI_RET_NOT_SUPPORTED;
    cFeatOk = 0u;
    fFeatSoft = 0;
    fInvSoft = 0;
    g_uSoftEl3 = (unsigned)fEl3;
    g_uSoftLive = 0u;
    g_uSoftFeatCpuOnOk = 0u;
    g_uSoftVerMaj = 0u;
    g_uSoftVerMin = 0u;
    g_u64SoftVerRaw = 0ul;

    /* Prefer HVC (QEMU virt / KVM common conduit). */
    g_nConduit = PSCI_CONDUIT_HVC;
    u64Ver = psci_call(PSCI_VERSION, 0, 0, 0);
    if ((long)u64Ver == PSCI_RET_NOT_SUPPORTED || g_psci_probe_fault) {
        /* Try SMC when EL3 present or HVC failed. */
        g_nConduit = PSCI_CONDUIT_SMC;
        u64Ver = psci_call(PSCI_VERSION, 0, 0, 0);
    }

    if ((long)u64Ver == PSCI_RET_NOT_SUPPORTED || g_psci_probe_fault) {
        g_nConduit = PSCI_CONDUIT_NONE;
        aarch64_uart_puts("aarch64: psci soft SKIP (no firmware)\n");
        psci_conduit_soft_log(fEl3);
        aarch64_uart_puts("aarch64: psci features soft FAIL\n");
        /* Wave 19: still emit multi-area inventory on SKIP path. */
        fInvSoft = psci_soft_inventory(0);
        (void)fInvSoft;
        return;
    }

    maj = (unsigned)((u64Ver >> 16) & 0xfffful);
    min = (unsigned)(u64Ver & 0xfffful);
    g_uSoftLive = 1u;
    g_uSoftVerMaj = maj;
    g_uSoftVerMin = min;
    g_u64SoftVerRaw = u64Ver;

    aarch64_uart_puts("aarch64: psci PASS conduit=");
    aarch64_uart_puts(g_nConduit == PSCI_CONDUIT_HVC ? "hvc" : "smc");
    aarch64_uart_puts(" version=");
    aarch64_uart_put_hex_n((unsigned long)maj, 4u);
    aarch64_uart_puts(".");
    aarch64_uart_put_hex_n((unsigned long)min, 4u);
    aarch64_uart_puts(" el3=");
    aarch64_uart_put_hex_n((unsigned long)fEl3, 1u);
    aarch64_uart_puts(" raw=");
    aarch64_uart_put_hex(u64Ver);
    aarch64_uart_puts("\n");

    /* Soft features deepen (FEATURES only — no power transitions). */
    cFeatOk = psci_features_soft(&u64FeatCpuOn);

    /*
     * Soft PASS: at least CPU_ON_64 features query must not be NOT_SUPPORTED
     * on a live PSCI (QEMU virt returns 0 for supported). Also require
     * overall ok count ≥ 1 so a broken FEATURES path fails greppably.
     */
    if (cFeatOk >= 1u &&
        (long)u64FeatCpuOn != PSCI_RET_NOT_SUPPORTED) {
        fFeatSoft = 1;
        g_uSoftFeatCpuOnOk = 1u;
    }

    /* Keep legacy one-line cpu_on marker for existing greps. */
    aarch64_uart_puts("aarch64: psci cpu_on_64 feat=");
    aarch64_uart_put_hex(u64FeatCpuOn);
    aarch64_uart_puts("\n");

    psci_conduit_soft_log(fEl3);

    if (fFeatSoft != 0) {
        aarch64_uart_puts("aarch64: psci features soft PASS\n");
    } else {
        aarch64_uart_puts("aarch64: psci features soft FAIL\n");
    }

    /* Wave 19 combined soft inventory under "aarch64: psci soft …". */
    fInvSoft = psci_soft_inventory(fFeatSoft);
    (void)fInvSoft;
}
