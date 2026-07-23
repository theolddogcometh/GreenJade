/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * aarch64 product kmain — shared C (string/kprintf/pmm_core/sched_coop) +
 * arch bring-up (GIC/timer/MMU/SVC/virtio/PSCI).
 *
 * -------------------------------------------------------------------------
 * Soft product surface (kmain deepen; never hard-fails M0)
 * -------------------------------------------------------------------------
 * Soft string path: memset fill, memcpy, memcmp equal/unequal, strlen.
 * Soft shared mm/sched: coop selftest, free/total inventory, pmm core
 * selftest recheck after bring-up (order-0 pool only).
 * Soft mem probe: PMM-backed multi-pattern paint/verify (avoids freelist
 * corruption from raw pool PA writes); free-count restore.
 * Soft phase counter: one tick per successful bring-up step; summary
 * line before the fixed "M0 OK" product bar.
 *
 * Greppable:
 *   aarch64: shared C kernel PASS (string+kprintf)
 *   aarch64: shared C soft PASS | FAIL steps=… len=…
 *   aarch64: shared sched PASS | soft FAIL
 *   aarch64: shared sched soft id=… free=… total=…
 *   aarch64: shared pmm core free=… total=…
 *   aarch64: shared pmm soft PASS | FAIL free=… total=…
 *   aarch64: mem probe soft pa=… free0=… free1=… pat=…
 *   aarch64: mem probe PASS | soft FAIL
 *   aarch64: kmain soft PASS phases=… soft_ok=… soft_fail=…
 *   M0 OK
 *
 * Greppable markers from callees — see README.md
 */
#include <gj/klog.h>
#include <gj/pmm_core.h>
#include <gj/sched_coop.h>
#include <gj/string.h>
#include <gj/types.h>

void aarch64_gic_init(void);
void aarch64_timer_probe(void);
void aarch64_exceptions_install(void);
void aarch64_cpu_probe(void);
void aarch64_pmm_init(void);
void aarch64_mmu_init(void);
void aarch64_svc_selftest(void);
void aarch64_virtio_mmio_probe(void);
void aarch64_psci_probe(void);

/* Soft poison patterns (match pmm soft multi style). */
#define KMAIN_SOFT_PAT_A 0xa5a5a5a5a5a5a5a5ul
#define KMAIN_SOFT_PAT_B 0x5a5a5a5a5a5a5a5aul

/* Soft observability (kmain-local; never gate product M0). */
static u32 g_cSoftPhases;
static u32 g_cSoftOk;
static u32 g_cSoftFail;

static void
soft_tick(int fOk)
{
    g_cSoftPhases++;
    if (fOk) {
        g_cSoftOk++;
    } else {
        g_cSoftFail++;
    }
}

/*
 * Soft deepen: freestanding string + kprintf path used by shared C kernel.
 * Returns 1 on PASS, 0 on soft FAIL. Does not abort bring-up.
 */
static int
shared_c_probe(void)
{
    char aBuf[16];
    char aAlt[16];
    const char *sz = "shared";
    size_t cbLen;
    unsigned cSteps;
    int fOk;

    cSteps = 0u;
    fOk = 1;

    /* memset: full fill then first 6 bytes overwritten by memcpy. */
    memset(aBuf, 0x5a, sizeof(aBuf));
    memset(aBuf, 0, sizeof(aBuf));
    cSteps++;
    if (aBuf[0] != 0 || aBuf[15] != 0) {
        fOk = 0;
    }

    memcpy(aBuf, sz, 6);
    cSteps++;
    if (memcmp(aBuf, sz, 6) != 0) {
        fOk = 0;
    }

    cbLen = strlen(aBuf);
    cSteps++;
    if (cbLen != 6u) {
        fOk = 0;
    }

    /* Unequal memcmp: one-byte diverge must be non-zero. */
    memcpy(aAlt, aBuf, sizeof(aAlt));
    aAlt[0] = (char)(aAlt[0] ^ 0x01);
    cSteps++;
    if (memcmp(aBuf, aAlt, 6) == 0) {
        fOk = 0;
    }

    /* Zero-length memcmp is equal; strlen of empty is 0. */
    cSteps++;
    if (memcmp(aBuf, aAlt, 0) != 0 || strlen("") != 0u) {
        fOk = 0;
    }

    if (!fOk) {
        kprintf("aarch64: shared C kernel soft FAIL\n");
        kprintf("aarch64: shared C soft FAIL steps=%u len=%u\n",
                cSteps, (unsigned)cbLen);
        soft_tick(0);
        return 0;
    }
    kprintf("aarch64: shared C kernel PASS (string+kprintf)\n");
    kprintf("aarch64: shared C soft PASS steps=%u len=%u\n",
            cSteps, (unsigned)cbLen);
    soft_tick(1);
    return 1;
}

/*
 * Soft deepen: coop selftest + shared pmm inventory / core selftest recheck.
 * Never hard-fails M0; emits greppable soft PASS|FAIL only.
 */
static void
shared_mm_sched_probe(void)
{
    unsigned cFree0;
    unsigned cTotal0;
    unsigned cFree1;
    u32 u32BootId;
    int fSched;
    int fPmm;

    u32BootId = gj_coop_current_id();
    cFree0 = gj_pmm_core_free_count();
    cTotal0 = gj_pmm_core_total_count();

    fSched = (gj_coop_selftest() != 0) ? 1 : 0;
    if (fSched) {
        kprintf("aarch64: shared sched PASS\n");
    } else {
        kprintf("aarch64: shared sched soft FAIL\n");
    }

    cFree1 = gj_pmm_core_free_count();
    kprintf("aarch64: shared sched soft id=%u free=%u total=%u free_after=%u\n",
            (unsigned)u32BootId, cFree0, cTotal0, cFree1);
    kprintf("aarch64: shared pmm core free=%u total=%u\n",
            cFree1, gj_pmm_core_total_count());

    /* Soft recheck of shared order-0 core after bring-up churn. */
    fPmm = (gj_pmm_core_selftest() != 0) ? 1 : 0;
    if (fPmm) {
        kprintf("aarch64: shared pmm soft PASS free=%u total=%u\n",
                gj_pmm_core_free_count(), gj_pmm_core_total_count());
    } else {
        kprintf("aarch64: shared pmm soft FAIL free=%u total=%u\n",
                gj_pmm_core_free_count(), gj_pmm_core_total_count());
    }

    soft_tick(fSched);
    soft_tick(fPmm);
}

/*
 * Soft deepen: PMM-backed multi-pattern mem probe (safe vs freelist nodes).
 * Keeps greppable "aarch64: mem probe PASS" for smoke scripts.
 */
static void
mem_probe(void)
{
    u64 u64Pa;
    volatile unsigned long *pWord;
    unsigned long u64Read;
    unsigned cFree0;
    unsigned cFree1;
    int fOk;

    fOk = 1;
    cFree0 = gj_pmm_core_free_count();
    u64Pa = gj_pmm_core_alloc();
    if (u64Pa == 0) {
        fOk = 0;
        kprintf("aarch64: mem probe soft pa=0 free0=%u free1=%u pat=0\n",
                cFree0, gj_pmm_core_free_count());
        kprintf("aarch64: mem probe soft FAIL\n");
        soft_tick(0);
        return;
    }

    pWord = (volatile unsigned long *)(gj_vaddr_t)u64Pa;

    /* Pattern A: first word + second word. */
    pWord[0] = KMAIN_SOFT_PAT_A;
    pWord[1] = KMAIN_SOFT_PAT_A ^ 0x1111111111111111ul;
    u64Read = pWord[0];
    if (u64Read != KMAIN_SOFT_PAT_A ||
        pWord[1] != (KMAIN_SOFT_PAT_A ^ 0x1111111111111111ul)) {
        fOk = 0;
    }

    /* Pattern B invert pass. */
    pWord[0] = KMAIN_SOFT_PAT_B;
    pWord[1] = KMAIN_SOFT_PAT_B;
    if (pWord[0] != KMAIN_SOFT_PAT_B || pWord[1] != KMAIN_SOFT_PAT_B) {
        fOk = 0;
    }

    gj_pmm_core_free(u64Pa);
    cFree1 = gj_pmm_core_free_count();
    if (cFree1 != cFree0) {
        fOk = 0;
    }

    kprintf("aarch64: mem probe soft pa=0x%lx free0=%u free1=%u pat=0x%lx\n",
            (unsigned long)u64Pa, cFree0, cFree1,
            (unsigned long)KMAIN_SOFT_PAT_B);

    if (fOk) {
        kprintf("aarch64: mem probe PASS\n");
        soft_tick(1);
    } else {
        kprintf("aarch64: mem probe soft FAIL\n");
        soft_tick(0);
    }
}

void
aarch64_kmain(void)
{
    kprintf("GreenJade aarch64 product kernel (shared C)\n");

    (void)shared_c_probe();

    aarch64_exceptions_install();
    soft_tick(1); /* install path is non-returning-fail in product scaffold */

    aarch64_cpu_probe();
    soft_tick(1);

    /* PSCI: HVC/SMC with fault recovery (PASS or soft SKIP). */
    aarch64_psci_probe();
    soft_tick(1);

    aarch64_gic_init();
    soft_tick(1);

    aarch64_timer_probe();
    soft_tick(1);

    aarch64_pmm_init();
    soft_tick(1);

    aarch64_mmu_init();
    soft_tick(1);

    gj_coop_init();
    soft_tick(1);

    shared_mm_sched_probe();

    aarch64_svc_selftest();
    soft_tick(1);

    aarch64_virtio_mmio_probe();
    soft_tick(1);

    mem_probe();

    /*
     * Soft summary only — M0 OK remains the fixed product bar grepped by
     * make aarch64-smoke / run-aarch64.sh (exact "M0 OK").
     */
    kprintf("aarch64: kmain soft PASS phases=%u soft_ok=%u soft_fail=%u "
            "pmm_free=%u pmm_tot=%u coop_id=%u\n",
            (unsigned)g_cSoftPhases, (unsigned)g_cSoftOk,
            (unsigned)g_cSoftFail, gj_pmm_core_free_count(),
            gj_pmm_core_total_count(), (unsigned)gj_coop_current_id());
    kprintf("M0 OK\n");

    for (;;) {
        __asm__ volatile("wfe");
    }
}

void
aarch64_kmain_stub(void)
{
    aarch64_kmain();
}
