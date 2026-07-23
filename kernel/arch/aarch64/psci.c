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
 * Greppable:
 *   aarch64: psci PASS conduit=hvc|smc version=… el3=…
 *   aarch64: psci soft SKIP (no firmware)
 *   aarch64: psci features soft ok=… miss=… cpu_on=… …
 *   aarch64: psci features soft PASS | FAIL
 *   aarch64: psci conduit soft hvc|smc|none faults=…
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

/* Set by exception path when recovering a PSCI probe trap. */
volatile unsigned long g_psci_probe_fault;
volatile unsigned long g_psci_probe_result;
volatile int g_psci_probe_active;

/* Soft observability counters. */
static unsigned long g_cPsciCalls;
static unsigned long g_cPsciFaults;
static unsigned long g_cPsciFeatOk;
static unsigned long g_cPsciFeatMiss;

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

    fEl3 = el3_implemented();
    u64FeatCpuOn = (unsigned long)(long)PSCI_RET_NOT_SUPPORTED;
    cFeatOk = 0u;
    fFeatSoft = 0;

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
        return;
    }

    maj = (unsigned)((u64Ver >> 16) & 0xfffful);
    min = (unsigned)(u64Ver & 0xfffful);
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
}
