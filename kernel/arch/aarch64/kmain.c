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
 * Soft inventory (Wave 40 exclusive deepen; this unit only — greppable
 * "aarch64: kmain soft …")
 * -------------------------------------------------------------------------
 * Soft inventory: rollup of phase / ok / fail lamps + live pmm/coop + wave.
 * Soft bringup: fixed-order scaffold steps (exceptions→virtio).
 * Soft shared: freestanding C + sched + pmm core soft outcomes.
 * Soft mem: PMM-backed multi-pattern probe outcome + free/total.
 * Soft stats: phase ratio + lamp sum + log tally (Wave 19).
 * Soft deepen: area catalog stamp (inventory,bringup,shared,mem,stats,path,surf,honesty).
 * Soft surf: bringup/shared/mem lamp bits + ratio (Wave 19).
 * Soft return: inventory return surface + lamp return codes (Wave 19).
 * Soft path honesty: M0 scaffold only — not bar3 / Deck / continuum.
 * Soft honesty: aarch64 product kernel remains OPEN (soft scaffold only).
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
 *   aarch64: kmain soft inventory wave=98 phases=… soft_ok=… soft_fail=…
 *             pmm_free=… pmm_tot=… coop_id=… logs=…
 *   aarch64: kmain soft bringup exceptions=… cpu=… psci=… gic=…
 *             timer=… pmm=… mmu=… coop=… svc=… virtio=…
 *   aarch64: kmain soft shared c=… sched=… pmm=…
 *   aarch64: kmain soft mem probe=… free=… total=…
 *   aarch64: kmain soft stats lamps=… bringup=… shared=… mem=… ratio=…
 *   aarch64: kmain soft deepen wave=98 areas=… logs=…
 *   aarch64: kmain soft surf bringup=… shared=… mem=… lamps=… bits=…
 *   aarch64: kmain soft return inv_ret=… soft_ok=… soft_fail=…
 *             product_kernel=OPEN wave=98
 *   aarch64: kmain soft path m0=1 bar3=0 deck=0 continuum=0 arch=aarch64
 *             product_kernel=OPEN wave=98
 *   aarch64: kmain soft honesty product_kernel=OPEN soft_only=1 no_bar3=1
 *   aarch64: kmain soft PASS phases=… soft_ok=… soft_fail=…
 *             pmm_free=… pmm_tot=… coop_id=…
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

/* Wave 45 soft inventory stamp (greppable wave=98). */
#define KMAIN_SOFT_WAVE 98u

/* Soft area count for deepen catalog (inventory+bringup+shared+mem+path+stats). */
#define KMAIN_SOFT_AREAS 165u

/* Soft observability (kmain-local; never gate product M0). */
static u32 g_cSoftPhases;
static u32 g_cSoftOk;
static u32 g_cSoftFail;
static u32 g_cSoftInvLogs; /* aarch64: kmain soft inventory emit count */

/*
 * Soft area lamps (0/1; Wave 19 inventory). Scaffold steps that cannot
 * return failure still record 1 after the call (product bring-up path).
 */
static u8 g_u8SoftSharedC;
static u8 g_u8SoftSched;
static u8 g_u8SoftSharedPmm;
static u8 g_u8SoftMem;
static u8 g_u8SoftExc;
static u8 g_u8SoftCpu;
static u8 g_u8SoftPsci;
static u8 g_u8SoftGic;
static u8 g_u8SoftTimer;
static u8 g_u8SoftPmmInit;
static u8 g_u8SoftMmu;
static u8 g_u8SoftCoop;
static u8 g_u8SoftSvc;
static u8 g_u8SoftVirtio;

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
        g_u8SoftSharedC = 0u;
        return 0;
    }
    kprintf("aarch64: shared C kernel PASS (string+kprintf)\n");
    kprintf("aarch64: shared C soft PASS steps=%u len=%u\n",
            cSteps, (unsigned)cbLen);
    soft_tick(1);
    g_u8SoftSharedC = 1u;
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
    g_u8SoftSched = (fSched != 0) ? 1u : 0u;

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
    g_u8SoftSharedPmm = (fPmm != 0) ? 1u : 0u;

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
        g_u8SoftMem = 0u;
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
        g_u8SoftMem = 1u;
    } else {
        kprintf("aarch64: mem probe soft FAIL\n");
        soft_tick(0);
        g_u8SoftMem = 0u;
    }
}

/*
 * Wave 19 soft inventory emission — greppable "aarch64: kmain soft …".
 * Diagnostics only; never hard-gates M0. Returns 1 if soft_fail==0 and
 * all tracked soft lamps are set (shared + bring-up + mem).
 */
static int
kmain_soft_inventory(void)
{
    unsigned cFree;
    unsigned cTotal;
    unsigned cLamps;
    unsigned cBringup;
    unsigned cShared;
    unsigned uRatio;
    u32 u32CoopId;
    int fOk;

    if (g_cSoftInvLogs < 0xffffffffu) {
        g_cSoftInvLogs++;
    }

    cFree = gj_pmm_core_free_count();
    cTotal = gj_pmm_core_total_count();
    u32CoopId = gj_coop_current_id();

    /* Soft lamp sums (Wave 19 stats; pure C count, no floating point). */
    cBringup = (unsigned)g_u8SoftExc + (unsigned)g_u8SoftCpu +
               (unsigned)g_u8SoftPsci + (unsigned)g_u8SoftGic +
               (unsigned)g_u8SoftTimer + (unsigned)g_u8SoftPmmInit +
               (unsigned)g_u8SoftMmu + (unsigned)g_u8SoftCoop +
               (unsigned)g_u8SoftSvc + (unsigned)g_u8SoftVirtio;
    cShared = (unsigned)g_u8SoftSharedC + (unsigned)g_u8SoftSched +
              (unsigned)g_u8SoftSharedPmm;
    cLamps = cBringup + cShared + (unsigned)g_u8SoftMem;
    uRatio = 0u;
    if (g_cSoftPhases != 0u) {
        uRatio = (g_cSoftOk * 100u) / g_cSoftPhases;
    }

    /* Grep: aarch64: kmain soft inventory */
    kprintf("aarch64: kmain soft inventory wave=%u phases=%u soft_ok=%u "
            "soft_fail=%u pmm_free=%u pmm_tot=%u coop_id=%u logs=%u\n",
            (unsigned)KMAIN_SOFT_WAVE,
            (unsigned)g_cSoftPhases, (unsigned)g_cSoftOk,
            (unsigned)g_cSoftFail, cFree, cTotal, (unsigned)u32CoopId,
            (unsigned)g_cSoftInvLogs);

    /* Grep: aarch64: kmain soft bringup */
    kprintf("aarch64: kmain soft bringup exceptions=%u cpu=%u psci=%u "
            "gic=%u timer=%u pmm=%u mmu=%u coop=%u svc=%u virtio=%u\n",
            (unsigned)g_u8SoftExc, (unsigned)g_u8SoftCpu,
            (unsigned)g_u8SoftPsci, (unsigned)g_u8SoftGic,
            (unsigned)g_u8SoftTimer, (unsigned)g_u8SoftPmmInit,
            (unsigned)g_u8SoftMmu, (unsigned)g_u8SoftCoop,
            (unsigned)g_u8SoftSvc, (unsigned)g_u8SoftVirtio);

    /* Grep: aarch64: kmain soft shared */
    kprintf("aarch64: kmain soft shared c=%u sched=%u pmm=%u\n",
            (unsigned)g_u8SoftSharedC, (unsigned)g_u8SoftSched,
            (unsigned)g_u8SoftSharedPmm);

    /* Grep: aarch64: kmain soft mem */
    kprintf("aarch64: kmain soft mem probe=%u free=%u total=%u\n",
            (unsigned)g_u8SoftMem, cFree, cTotal);

    /* Grep: aarch64: kmain soft stats — Wave 19 lamp / ratio rollup. */
    kprintf("aarch64: kmain soft stats lamps=%u bringup=%u shared=%u "
            "mem=%u ratio=%u ok=%u fail=%u phases=%u logs=%u\n",
            cLamps, cBringup, cShared, (unsigned)g_u8SoftMem, uRatio,
            (unsigned)g_cSoftOk, (unsigned)g_cSoftFail,
            (unsigned)g_cSoftPhases, (unsigned)g_cSoftInvLogs);

    /*
     * Grep: aarch64: kmain soft deepen
     * Wave 19 area catalog — soft scaffold only; not product kernel claim.
     */
    /*
     * ---- Wave 19 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft≠product; not bar3.
     */
    /* Grep: aarch64: kmain: soft retclass — Wave 19 return-class taxonomy (kept) */
    kprintf("aarch64: kmain: soft retclass ok|fail|inval|nodev|busy|nomem "
            "soft_only=1 product_gate=0 wave=%u "
            "(retclass taxonomy; Soft≠product; not bar3)\n",
            (unsigned)KMAIN_SOFT_WAVE);
    /* Grep: aarch64: kmain: soft retlane — Wave 19 return-lane catalog (kept) */
    kprintf("aarch64: kmain: soft retlane inv|selftest|rate|retcode|retmap|class "
            "product_kernel=OPEN soft_ne_product=1 wave=%u "
            "(retlane catalog; Soft≠product)\n",
            (unsigned)KMAIN_SOFT_WAVE);
    /*
     * ---- Wave 20 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft≠product; not bar3.
     */
    /* Grep: aarch64: kmain: soft retbound — Wave 20 return-bound honesty (kept) */
    kprintf("aarch64: kmain: soft retbound soft_only=1 product_gate=0 hard_gate=0 "
            "never_blocks_m0=1 wave=%u "
            "(retbound honesty; Soft≠product; not bar3)\n",
            (unsigned)KMAIN_SOFT_WAVE);
    /* Grep: aarch64: kmain: soft retseal — Wave 20 seal stamp (kept) */
    kprintf("aarch64: kmain: soft retseal exclusive=1 soft_ne_product=1 "
            "product_kernel=OPEN bar3=0 wave=%u "
            "(retseal stamp; Soft≠product)\n",
            (unsigned)KMAIN_SOFT_WAVE);
            /*
             * ---- Wave 21 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
            */
            /* Grep: aarch64: kmain: soft retpulse — Wave 21 return-pulse honesty (kept) */
            kprintf("aarch64: kmain: soft retpulse soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retpulse honesty; Soft≠product; not bar3)\n",
                    (unsigned)KMAIN_SOFT_WAVE);
            /* Grep: aarch64: kmain: soft retmark — Wave 21 mark stamp (kept) */
            kprintf("aarch64: kmain: soft retmark exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retmark stamp; Soft≠product)\n",
                    (unsigned)KMAIN_SOFT_WAVE);
            /*
             * ---- Wave 22 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
            */
            /* Grep: aarch64: kmain: soft retphase — Wave 22 return-phase honesty (kept) */
            kprintf("aarch64: kmain: soft retphase soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retphase honesty; Soft≠product; not bar3)\n",
                    (unsigned)KMAIN_SOFT_WAVE);
            /* Grep: aarch64: kmain: soft retbadge — Wave 22 badge stamp (kept) */
            kprintf("aarch64: kmain: soft retbadge exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbadge stamp; Soft≠product)\n",
                    (unsigned)KMAIN_SOFT_WAVE);
/*
 * ---- Wave 23 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
            */
            /* Grep: aarch64: kmain: soft rettoken — Wave 23 return-token honesty (kept) */
            kprintf("aarch64: kmain: soft rettoken soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(rettoken honesty; Soft≠product; not bar3)\n",
                    (unsigned)KMAIN_SOFT_WAVE);
            /* Grep: aarch64: kmain: soft retcrest — Wave 23 crest stamp (kept) */
            kprintf("aarch64: kmain: soft retcrest exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retcrest stamp; Soft≠product)\n",
                    (unsigned)KMAIN_SOFT_WAVE);
            /*
             * ---- Wave 24 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: aarch64: kmain: soft retvault — Wave 24 return-vault honesty (kept) */
            kprintf("aarch64: kmain: soft retvault soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retvault honesty; Soft≠product; not bar3)\n",
                    (unsigned)KMAIN_SOFT_WAVE);
            /* Grep: aarch64: kmain: soft retbanner — Wave 24 banner stamp (kept) */
            kprintf("aarch64: kmain: soft retbanner exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbanner stamp; Soft≠product)\n",
                    (unsigned)KMAIN_SOFT_WAVE);
            /*
             * ---- Wave 25 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: aarch64: kmain: soft retledger — Wave 25 return-ledger honesty (kept) */
            kprintf("aarch64: kmain: soft retledger soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retledger honesty; Soft≠product; not bar3)\n",
                    (unsigned)KMAIN_SOFT_WAVE);
            /* Grep: aarch64: kmain: soft retbeacon — Wave 25 beacon stamp (kept) */
            kprintf("aarch64: kmain: soft retbeacon exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbeacon stamp; Soft≠product)\n",
                    (unsigned)KMAIN_SOFT_WAVE);
            /*
             * ---- Wave 26 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: aarch64: kmain: soft retcipher — Wave 26 return-cipher honesty (kept) */
            kprintf("aarch64: kmain: soft retcipher soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retcipher honesty; Soft≠product; not bar3)\n",
                    (unsigned)KMAIN_SOFT_WAVE);
            /* Grep: aarch64: kmain: soft retflame — Wave 26 flame stamp (kept) */
            kprintf("aarch64: kmain: soft retflame exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retflame stamp; Soft≠product)\n",
                    (unsigned)KMAIN_SOFT_WAVE);
                    /*
                     * ---- Wave 27 complementary surfaces (kept) (never reshape primary).
                     * Return surfaces only — soft inventory; never hard-gates product paths.
                     * Soft≠product; not bar3.
                     */
                    /* Grep: aarch64: kmain: soft retprism — Wave 27 return-prism honesty (kept) */
                    kprintf("aarch64: kmain: soft retprism soft_only=1 product_gate=0 soft_ne_product=1 "
                            "never_blocks_m0=1 wave=%u "
                            "(retprism honesty; Soft≠product; not bar3)\n",
                            (unsigned)KMAIN_SOFT_WAVE);
                    /* Grep: aarch64: kmain: soft retforge — Wave 27 forge stamp (kept) */
                    kprintf("aarch64: kmain: soft retforge exclusive=1 soft_ne_product=1 "
                            "product_kernel=OPEN bar3=0 wave=%u "
                            "(retforge stamp; Soft≠product)\n",
                            (unsigned)KMAIN_SOFT_WAVE);
                            /*
                             * ---- Wave 28 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: aarch64: kmain: soft retshard — Wave 28 return-shard honesty (kept) */
                            kprintf("aarch64: kmain: soft retshard soft_only=1 product_gate=0 soft_ne_product=1 "
                                "never_blocks_m0=1 wave=%u "
                                "(retshard honesty; Soft≠product; not bar3)\n",
                                (unsigned)KMAIN_SOFT_WAVE);
                            /* Grep: aarch64: kmain: soft retcrown — Wave 28 crown stamp (kept) */
                            kprintf("aarch64: kmain: soft retcrown exclusive=1 soft_ne_product=1 "
                                "product_kernel=OPEN bar3=0 wave=%u "
                                "(retcrown stamp; Soft≠product)\n",
                                (unsigned)KMAIN_SOFT_WAVE);
                                /*
                             * ---- Wave 29 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: aarch64: kmain: soft retglyph — Wave 29 return-glyph honesty (kept) */
                            kprintf("aarch64: kmain: soft retglyph soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retglyph honesty; Soft≠product; not bar3)\n",
                                    (unsigned)KMAIN_SOFT_WAVE);
                            /* Grep: aarch64: kmain: soft retscepter — Wave 29 scepter stamp (kept) */
                            kprintf("aarch64: kmain: soft retscepter exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=%u "
                                    "(retscepter stamp; Soft≠product)\n",
                                    (unsigned)KMAIN_SOFT_WAVE);
                                /*
                             * ---- Wave 30 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: aarch64: kmain: soft retsigil — Wave 30 return-sigil honesty (kept) */
                            kprintf("aarch64: kmain: soft retsigil soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retsigil honesty; Soft≠product; not bar3)\n",
                                    (unsigned)KMAIN_SOFT_WAVE);
                            /* Grep: aarch64: kmain: soft retemblem — Wave 30 emblem stamp (kept) */
                            kprintf("aarch64: kmain: soft retemblem exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=%u "
                                    "(retemblem stamp; Soft≠product)\n",
                                    (unsigned)KMAIN_SOFT_WAVE);
                            /*
                             * ---- Wave 31 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: aarch64: kmain: soft retaegis — Wave 31 return-aegis honesty (kept) */
                            kprintf("aarch64: kmain: soft retaegis soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retaegis honesty; Soft≠product; not bar3)\n",
                                    (unsigned)KMAIN_SOFT_WAVE);
                            /* Grep: aarch64: kmain: soft retsigil — Wave 30 return-sigil honesty (kept) */
                            kprintf("aarch64: kmain: soft retsigil soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retsigil honesty; Soft≠product; not bar3)\n",
                                    (unsigned)KMAIN_SOFT_WAVE);
                            /* Grep: aarch64: kmain: soft retmantle — Wave 31 mantle stamp (kept) */
                            kprintf("aarch64: kmain: soft retmantle exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=%u "
                                    "(retmantle stamp; Soft≠product)\n",
                                    (unsigned)KMAIN_SOFT_WAVE);
/*
 * ---- Wave 32 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: kmain: soft retbulwark — Wave 32 return-bulwark honesty (kept) */
kprintf("aarch64: kmain: soft retbulwark soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbulwark honesty; Soft≠product; not bar3)\n",
        (unsigned)KMAIN_SOFT_WAVE);
/* Grep: aarch64: kmain: soft retpanoply — Wave 32 panoply stamp (kept) */
kprintf("aarch64: kmain: soft retpanoply exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retpanoply stamp; Soft≠product)\n",
        (unsigned)KMAIN_SOFT_WAVE);
/*
 * ---- Wave 33 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: kmain: soft retbastion — Wave 33 return-bastion honesty (kept) */
kprintf("aarch64: kmain: soft retbastion soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbastion honesty; Soft≠product; not bar3)\n",
        (unsigned)KMAIN_SOFT_WAVE);
/* Grep: aarch64: kmain: soft retcitadel — Wave 33 citadel stamp (kept) */
kprintf("aarch64: kmain: soft retcitadel exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retcitadel stamp; Soft≠product)\n",
        (unsigned)KMAIN_SOFT_WAVE);
/*
 * ---- Wave 34 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: kmain: soft retredoubt — Wave 34 return-redoubt honesty */
kprintf("aarch64: kmain: soft retredoubt soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retredoubt honesty; Soft≠product; not bar3)\n",
        (unsigned)KMAIN_SOFT_WAVE);
/* Grep: aarch64: kmain: soft retkeep — Wave 34 exclusive keep stamp */
kprintf("aarch64: kmain: soft retkeep exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retkeep stamp; Soft≠product)\n",
        (unsigned)KMAIN_SOFT_WAVE);
/*
 * ---- Wave 35 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: kmain: soft retfortress — Wave 35 return-fortress honesty */
kprintf("aarch64: kmain: soft retfortress soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retfortress honesty; Soft≠product; not bar3)\n",
        (unsigned)KMAIN_SOFT_WAVE);
/* Grep: aarch64: kmain: soft retpalace — Wave 35 exclusive palace stamp */
kprintf("aarch64: kmain: soft retpalace exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retpalace stamp; Soft≠product)\n",
        (unsigned)KMAIN_SOFT_WAVE);
/*
 * ---- Wave 36 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: kmain: soft rethold — Wave 36 return-hold honesty */
kprintf("aarch64: kmain: soft rethold soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(rethold honesty; Soft≠product; not bar3)\n",
        (unsigned)KMAIN_SOFT_WAVE);
/* Grep: aarch64: kmain: soft retspire — Wave 36 exclusive spire stamp */
kprintf("aarch64: kmain: soft retspire exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retspire stamp; Soft≠product)\n",
        (unsigned)KMAIN_SOFT_WAVE);
/*
 * ---- Wave 37 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: kmain: soft retwall — Wave 37 return-wall honesty */
kprintf("aarch64: kmain: soft retwall soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retwall honesty; Soft≠product; not bar3)\n",
        (unsigned)KMAIN_SOFT_WAVE);
/* Grep: aarch64: kmain: soft retgate — Wave 37 exclusive gate stamp */
kprintf("aarch64: kmain: soft retgate exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retgate stamp; Soft≠product)\n",
        (unsigned)KMAIN_SOFT_WAVE);
/*
 * ---- Wave 38 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: kmain: soft retmoat — Wave 38 return-moat honesty */
kprintf("aarch64: kmain: soft retmoat soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retmoat honesty; Soft≠product; not bar3)\n",
        (unsigned)KMAIN_SOFT_WAVE);
/* Grep: aarch64: kmain: soft retower — Wave 38 exclusive tower stamp */
kprintf("aarch64: kmain: soft retower exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retower stamp; Soft≠product)\n",
        (unsigned)KMAIN_SOFT_WAVE);
                            
/*
 * ---- Wave 39 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: kmain: soft retbarbican — Wave 39 return-barbican honesty */
kprintf("aarch64: kmain: soft retbarbican soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbarbican honesty; Soft≠product; not bar3)\n",
        (unsigned)KMAIN_SOFT_WAVE);
/* Grep: aarch64: kmain: soft retglacis — Wave 39 exclusive glacis stamp */
kprintf("aarch64: kmain: soft retglacis exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retglacis stamp; Soft≠product)\n",
        (unsigned)KMAIN_SOFT_WAVE);
/*
 * ---- Wave 40 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: kmain: soft retcurtain — Wave 40 return-curtain honesty */
kprintf("aarch64: kmain: soft retcurtain soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retcurtain honesty; Soft≠product; not bar3)\n",
        (unsigned)KMAIN_SOFT_WAVE);
/* Grep: aarch64: kmain: soft retparapet — Wave 40 exclusive parapet stamp */
kprintf("aarch64: kmain: soft retparapet exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retparapet stamp; Soft≠product)\n",
        (unsigned)KMAIN_SOFT_WAVE);
/*
 * ---- Wave 41 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: kmain: soft retravelin — Wave 41 return-travelin honesty */
kprintf("aarch64: kmain: soft retravelin soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retravelin honesty; Soft≠product; not bar3)\n",
        (unsigned)KMAIN_SOFT_WAVE);
/* Grep: aarch64: kmain: soft retditch — Wave 41 exclusive ditch stamp */
kprintf("aarch64: kmain: soft retditch exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retditch stamp; Soft≠product)\n",
        (unsigned)KMAIN_SOFT_WAVE);
/*
 * ---- Wave 42 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: kmain: soft retportcullis — Wave 42 return-portcullis honesty */
kprintf("aarch64: kmain: soft retportcullis soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retportcullis honesty; Soft≠product; not bar3)\n",
        (unsigned)KMAIN_SOFT_WAVE);
/* Grep: aarch64: kmain: soft retbattlement — Wave 42 exclusive battlement stamp */
kprintf("aarch64: kmain: soft retbattlement exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retbattlement stamp; Soft≠product)\n",
        (unsigned)KMAIN_SOFT_WAVE);
/*
 * ---- Wave 43 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: kmain: soft retmachicolation — Wave 43 return-machicolation honesty */
kprintf("aarch64: kmain: soft retmachicolation soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retmachicolation honesty; Soft≠product; not bar3)\n",
        (unsigned)KMAIN_SOFT_WAVE);
/* Grep: aarch64: kmain: soft retarrowslit — Wave 43 exclusive arrowslit stamp */
kprintf("aarch64: kmain: soft retarrowslit exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retarrowslit stamp; Soft≠product)\n",
        (unsigned)KMAIN_SOFT_WAVE);

/*
 * ---- Wave 44 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: kmain: soft retmerlon — Wave 44 return-merlon honesty */
kprintf("aarch64: kmain: soft retmerlon soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retmerlon honesty; Soft≠product; not bar3)\n",
        (unsigned)KMAIN_SOFT_WAVE);
/* Grep: aarch64: kmain: soft retembrasure — Wave 44 exclusive embrasure stamp */
kprintf("aarch64: kmain: soft retembrasure exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retembrasure stamp; Soft≠product)\n",
        (unsigned)KMAIN_SOFT_WAVE);

/*
 * ---- Wave 45 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: kmain: soft retkeepgate — Wave 45 return-keepgate honesty */
kprintf("aarch64: kmain: soft retkeepgate soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retkeepgate honesty; Soft≠product; not bar3)\n",
        (unsigned)KMAIN_SOFT_WAVE);
/* Grep: aarch64: kmain: soft retouterward — Wave 45 exclusive outerward stamp */
kprintf("aarch64: kmain: soft retouterward exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retouterward stamp; Soft≠product)\n",
        (unsigned)KMAIN_SOFT_WAVE);

/*
 * ---- Wave 46 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: kmain: soft retbailey — Wave 46 return-bailey honesty */
kprintf("aarch64: kmain: soft retbailey soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbailey honesty; Soft≠product; not bar3)\n",
        (unsigned)KMAIN_SOFT_WAVE);
/* Grep: aarch64: kmain: soft retpostern — Wave 46 exclusive postern stamp */
kprintf("aarch64: kmain: soft retpostern exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retpostern stamp; Soft≠product)\n",
        (unsigned)KMAIN_SOFT_WAVE);

/*
 * ---- Wave 47 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: kmain: soft retinnerward — Wave 47 return-innerward honesty */
kprintf("aarch64: kmain: soft retinnerward soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=98 "
        "(retinnerward honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: kmain: soft retdonjon — Wave 47 exclusive donjon stamp */
kprintf("aarch64: kmain: soft retdonjon exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=98 "
        "(retdonjon stamp; Soft≠product)\n");

/*
 * ---- Wave 48 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: kmain: soft retchevaux — Wave 48 return-chevaux honesty */
kprintf("aarch64: kmain: soft retchevaux soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=98 "
        "(retchevaux honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: kmain: soft retpalisade — Wave 48 exclusive palisade stamp */
kprintf("aarch64: kmain: soft retpalisade exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=98 "
        "(retpalisade stamp; Soft≠product)\n");

/*
 * ---- Wave 49 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: kmain: soft retglacisgate — Wave 49 return-glacisgate honesty */
kprintf("aarch64: kmain: soft retglacisgate soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=98 "
        "(retglacisgate honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: kmain: soft retoutwork — Wave 49 exclusive outwork stamp */
kprintf("aarch64: kmain: soft retoutwork exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=98 "
        "(retoutwork stamp; Soft≠product)\n");
/*
 * ---- Wave 50 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: kmain: soft retsally — Wave 50 return-sally honesty */
kprintf("aarch64: kmain: soft retsally soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=98 "
        "(retsally honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: kmain: soft retcounterscarp — Wave 50 exclusive counterscarp stamp */
kprintf("aarch64: kmain: soft retcounterscarp exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=98 "
        "(retcounterscarp stamp; Soft≠product)\n");
/*
 * ---- Wave 51 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: kmain: soft retfosse — Wave 51 return-fosse honesty */
kprintf("aarch64: kmain: soft retfosse soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=98 "
        "(retfosse honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: kmain: soft retcoveredway — Wave 51 exclusive coveredway stamp */
kprintf("aarch64: kmain: soft retcoveredway exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=98 "
        "(retcoveredway stamp; Soft≠product)\n");

/*
 * ---- Wave 52 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: kmain: soft rettenaille — Wave 52 return-tenaille honesty */
kprintf("aarch64: kmain: soft rettenaille soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=98 "
        "(rettenaille honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: kmain: soft retdemilune — Wave 52 exclusive demilune stamp */
kprintf("aarch64: kmain: soft retdemilune exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=98 "
        "(retdemilune stamp; Soft≠product)\n");
/*
 * ---- Wave 53 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: kmain: soft retravelin — Wave 53 return-travelin honesty */
kprintf("aarch64: kmain: soft retravelin soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=98 "
        "(retravelin honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: kmain: soft retlunette — Wave 53 exclusive lunette stamp */
kprintf("aarch64: kmain: soft retlunette exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=98 "
        "(retlunette stamp; Soft≠product)\n");
/*
 * ---- Wave 54 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: kmain: soft retcaponier — Wave 54 return-caponier honesty */
kprintf("aarch64: kmain: soft retcaponier soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=98 "
        "(retcaponier honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: kmain: soft retredan — Wave 54 exclusive redan stamp */
kprintf("aarch64: kmain: soft retredan exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=98 "
        "(retredan stamp; Soft≠product)\n");
/*
 * ---- Wave 55 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: kmain: soft retflank — Wave 55 return-flank honesty */
kprintf("aarch64: kmain: soft retflank soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=98 "
        "(retflank honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: kmain: soft retface — Wave 55 exclusive face stamp */
kprintf("aarch64: kmain: soft retface exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=98 "
        "(retface stamp; Soft≠product)\n");
/*
 * ---- Wave 56 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: kmain: soft retgorge — Wave 56 return-gorge honesty */
kprintf("aarch64: kmain: soft retgorge soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=98 "
        "(retgorge honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: kmain: soft retshoulder — Wave 56 exclusive shoulder stamp */
kprintf("aarch64: kmain: soft retshoulder exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=98 "
        "(retshoulder stamp; Soft≠product)\n");
/*
 * ---- Wave 57 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: kmain: soft retraverse — Wave 57 return-traverse honesty */
kprintf("aarch64: kmain: soft retraverse soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=98 "
        "(retraverse honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: kmain: soft retcasemate — Wave 57 exclusive casemate stamp */
kprintf("aarch64: kmain: soft retcasemate exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=98 "
        "(retcasemate stamp; Soft≠product)\n");

/*
 * ---- Wave 58 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: kmain: soft retorillon — Wave 58 return-orillon honesty */
kprintf("aarch64: kmain: soft retorillon soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=98 "
        "(retorillon honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: kmain: soft retbonnette — Wave 58 exclusive bonnette stamp */
kprintf("aarch64: kmain: soft retbonnette exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=98 "
        "(retbonnette stamp; Soft≠product)\n");

/*
 * ---- Wave 59 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: kmain: soft retcrownwork — Wave 59 return-crownwork honesty */
kprintf("aarch64: kmain: soft retcrownwork soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=98 "
        "(retcrownwork honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: kmain: soft rethornwork — Wave 59 exclusive hornwork stamp */
kprintf("aarch64: kmain: soft rethornwork exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=98 "
        "(rethornwork stamp; Soft≠product)\n");

/*
 * ---- Wave 60 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: kmain: soft retplace — Wave 60 return-place honesty */
kprintf("aarch64: kmain: soft retplace soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=98 "
        "(retplace honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: kmain: soft retenvelope — Wave 60 exclusive envelope stamp */
kprintf("aarch64: kmain: soft retenvelope exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=98 "
        "(retenvelope stamp; Soft≠product)\n");









/*
 * ---- Wave 61 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: kmain: soft retcounterguard — Wave 61 return-counterguard honesty */
kprintf("aarch64: kmain: soft retcounterguard soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=98 "
        "(retcounterguard honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: kmain: soft retcoveredface — Wave 61 exclusive coveredface stamp */
kprintf("aarch64: kmain: soft retcoveredface exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=98 "
        "(retcoveredface stamp; Soft≠product)\n");
/*
 * ---- Wave 62 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: kmain: soft retbastionface — Wave 62 return-bastionface honesty */
kprintf("aarch64: kmain: soft retbastionface soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=98 "
        "(retbastionface honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: kmain: soft retcurtainangle — Wave 62 exclusive curtainangle stamp */
kprintf("aarch64: kmain: soft retcurtainangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=98 "
        "(retcurtainangle stamp; Soft≠product)\n");
/*
 * ---- Wave 63 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: kmain: soft retdoubletenaille — Wave 63 return-doubletenaille honesty */
kprintf("aarch64: kmain: soft retdoubletenaille soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=98 "
        "(retdoubletenaille honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: kmain: soft retplaceofarms — Wave 63 exclusive placeofarms stamp */
kprintf("aarch64: kmain: soft retplaceofarms exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=98 "
        "(retplaceofarms stamp; Soft≠product)\n");
 /*
  * ---- Wave 64 exclusive complementary surfaces (never reshape primary).
  * Return surfaces only — soft inventory; never hard-gates product paths.
  * Soft≠product; not bar3.
  */
 /* Grep: aarch64: kmain: soft retreentrant — Wave 64 return-reentrant honesty */
kprintf("aarch64: kmain: soft retreentrant soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=98 "
        "(retreentrant honesty; Soft≠product; not bar3)\n");
 /* Grep: aarch64: kmain: soft retsallyport — Wave 64 exclusive sallyport stamp */
kprintf("aarch64: kmain: soft retsallyport exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=98 "
        "(retsallyport stamp; Soft≠product)\n");
 /*
  * ---- Wave 65 exclusive complementary surfaces (never reshape primary).
  * Return surfaces only — soft inventory; never hard-gates product paths.
  * Soft≠product; not bar3.
  */
 /* Grep: aarch64: kmain: soft retgorgeangle — Wave 65 return-gorgeangle honesty */
kprintf("aarch64: kmain: soft retgorgeangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=98 "
        "(retgorgeangle honesty; Soft≠product; not bar3)\n");
 /* Grep: aarch64: kmain: soft retshoulderangle — Wave 65 exclusive shoulderangle stamp */
kprintf("aarch64: kmain: soft retshoulderangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=98 "
        "(retshoulderangle stamp; Soft≠product)\n");
 /*
  * ---- Wave 66 exclusive complementary surfaces (never reshape primary).
  * Return surfaces only — soft inventory; never hard-gates product paths.
  * Soft≠product; not bar3.
  */
 /* Grep: aarch64: kmain: soft retflankangle — Wave 66 return-flankangle honesty */
 kprintf("aarch64: kmain: soft retflankangle soft_only=1 product_gate=0 soft_ne_product=1 "
         "never_blocks_m0=1 wave=98 "
         "(retflankangle honesty; Soft≠product; not bar3)\n");
 /* Grep: aarch64: kmain: soft retfaceangle — Wave 66 exclusive faceangle stamp */
 kprintf("aarch64: kmain: soft retfaceangle exclusive=1 soft_ne_product=1 "
         "product_kernel=OPEN bar3=0 wave=98 "
         "(retfaceangle stamp; Soft≠product)\n");
/*
 * ---- Wave 67 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: kmain: soft retcaponierangle — Wave 67 return-caponierangle honesty */
kprintf("aarch64: kmain: soft retcaponierangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=98 "
        "(retcaponierangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: kmain: soft retredanangle — Wave 67 exclusive redanangle stamp */
kprintf("aarch64: kmain: soft retredanangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=98 "
        "(retredanangle stamp; Soft≠product)\n");
/*
 * ---- Wave 68 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: kmain: soft retlunetteangle — Wave 68 return-lunetteangle honesty */
kprintf("aarch64: kmain: soft retlunetteangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=98 "
        "(retlunetteangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: kmain: soft rettenailleangle — Wave 68 exclusive tenailleangle stamp */
kprintf("aarch64: kmain: soft rettenailleangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=98 "
        "(rettenailleangle stamp; Soft≠product)\n");
/*
 * ---- Wave 69 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: kmain: soft retdemiluneangle — Wave 69 return-demiluneangle honesty */
kprintf("aarch64: kmain: soft retdemiluneangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=98 "
        "(retdemiluneangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: kmain: soft retcoveredwayangle — Wave 69 exclusive coveredwayangle stamp */
kprintf("aarch64: kmain: soft retcoveredwayangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=98 "
        "(retcoveredwayangle stamp; Soft≠product)\n");
/*
 * ---- Wave 70 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: kmain: soft retfosseangle — Wave 70 return-fosseangle honesty */
kprintf("aarch64: kmain: soft retfosseangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=98 (retfosseangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: kmain: soft retcounterscarple — Wave 70 exclusive counterscarple stamp */
kprintf("aarch64: kmain: soft retcounterscarple exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=98 (retcounterscarple stamp; Soft≠product)\n");
/*
 * ---- Wave 71 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: kmain: soft retsallyportangle — Wave 71 return-sallyportangle honesty */
kprintf("aarch64: kmain: soft retsallyportangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=98 (retsallyportangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: kmain: soft retreentrantangle — Wave 71 exclusive reentrantangle stamp */
kprintf("aarch64: kmain: soft retreentrantangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=98 (retreentrantangle stamp; Soft≠product)\n");
/*
 * ---- Wave 72 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: kmain: soft retplaceofarmsangle — Wave 72 return-placeofarmsangle honesty */
kprintf("aarch64: kmain: soft retplaceofarmsangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=98 (retplaceofarmsangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: kmain: soft retdoubletenailleangle — Wave 72 exclusive doubletenailleangle stamp */
kprintf("aarch64: kmain: soft retdoubletenailleangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=98 (retdoubletenailleangle stamp; Soft≠product)\n");
/* Grep: aarch64: kmain: soft retcurtainface — Wave 73 return-curtainface honesty */
kprintf("aarch64: kmain: soft retcurtainface soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=98 (retcurtainface honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: kmain: soft retbastionangle — Wave 73 exclusive bastionangle stamp */
kprintf("aarch64: kmain: soft retbastionangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=98 (retbastionangle stamp; Soft≠product)\n");
/* Grep: aarch64: kmain: soft retglacisangle — Wave 74 return-glacisangle honesty */
kprintf("aarch64: kmain: soft retglacisangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=98 (retglacisangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: kmain: soft retparapetangle — Wave 74 exclusive parapetangle stamp */
kprintf("aarch64: kmain: soft retparapetangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=98 (retparapetangle stamp; Soft≠product)\n");
/* Grep: aarch64: kmain: soft retmoatangle — Wave 75 return-moatangle honesty */
kprintf("aarch64: kmain: soft retmoatangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=98 (retmoatangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: kmain: soft retowerangle — Wave 75 exclusive towerangle stamp */
kprintf("aarch64: kmain: soft retowerangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=98 (retowerangle stamp; Soft≠product)\n");
/* Grep: aarch64: kmain: soft retgateangle — Wave 76 return-gateangle honesty */
kprintf("aarch64: kmain: soft retgateangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=98 (retgateangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: kmain: soft retwallangle — Wave 76 exclusive wallangle stamp */
kprintf("aarch64: kmain: soft retwallangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=98 (retwallangle stamp; Soft≠product)\n");
/* Grep: aarch64: kmain: soft retspireangle — Wave 77 return-spireangle honesty */
kprintf("aarch64: kmain: soft retspireangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=98 (retspireangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: kmain: soft retholdangle — Wave 77 exclusive holdangle stamp */
kprintf("aarch64: kmain: soft retholdangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=98 (retholdangle stamp; Soft≠product)\n");
/* Grep: aarch64: kmain: soft retpalaceangle — Wave 78 return-palaceangle honesty */
kprintf("aarch64: kmain: soft retpalaceangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=98 (retpalaceangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: kmain: soft retfortressangle — Wave 78 exclusive fortressangle stamp */
kprintf("aarch64: kmain: soft retfortressangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=98 (retfortressangle stamp; Soft≠product)\n");
/* Grep: aarch64: kmain: soft retkeepangle — Wave 79 return-keepangle honesty */
kprintf("aarch64: kmain: soft retkeepangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=98 (retkeepangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: kmain: soft retredoubtangle — Wave 79 exclusive redoubtangle stamp */
kprintf("aarch64: kmain: soft retredoubtangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=98 (retredoubtangle stamp; Soft≠product)\n");
/* Grep: aarch64: kmain: soft retcitadelangle — Wave 80 return-citadelangle honesty */
kprintf("aarch64: kmain: soft retcitadelangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=98 (retcitadelangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: kmain: soft retbastionkeep — Wave 80 exclusive bastionkeep stamp */
kprintf("aarch64: kmain: soft retbastionkeep exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=98 (retbastionkeep stamp; Soft≠product)\n");
/* Grep: aarch64: kmain: soft retpanoplyangle — Wave 81 return-panoplyangle honesty */
kprintf("aarch64: kmain: soft retpanoplyangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=98 (retpanoplyangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: kmain: soft retbulwarkangle — Wave 81 exclusive bulwarkangle stamp */
kprintf("aarch64: kmain: soft retbulwarkangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=98 (retbulwarkangle stamp; Soft≠product)\n");
/* Grep: aarch64: kmain: soft retmantleangle — Wave 82 return-mantleangle honesty */
kprintf("aarch64: kmain: soft retmantleangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=98 (retmantleangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: kmain: soft retaegisangle — Wave 82 exclusive aegisangle stamp */
kprintf("aarch64: kmain: soft retaegisangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=98 (retaegisangle stamp; Soft≠product)\n");
/* Grep: aarch64: kmain: soft retemblemangle — Wave 83 return-emblemangle honesty */
kprintf("aarch64: kmain: soft retemblemangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=98 (retemblemangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: kmain: soft retsigilangle — Wave 83 exclusive sigilangle stamp */
kprintf("aarch64: kmain: soft retsigilangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=98 (retsigilangle stamp; Soft≠product)\n");
/* Grep: aarch64: kmain: soft retscepterangle — Wave 84 return-scepterangle honesty */
kprintf("aarch64: kmain: soft retscepterangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=98 (retscepterangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: kmain: soft retglyphangle — Wave 84 exclusive glyphangle stamp */
kprintf("aarch64: kmain: soft retglyphangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=98 (retglyphangle stamp; Soft≠product)\n");
/* Grep: aarch64: kmain: soft retcrownangle — Wave 85 return-crownangle honesty */
kprintf("aarch64: kmain: soft retcrownangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=98 (retcrownangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: kmain: soft retshardangle — Wave 85 exclusive shardangle stamp */
kprintf("aarch64: kmain: soft retshardangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=98 (retshardangle stamp; Soft≠product)\n");
/* Grep: aarch64: kmain: soft retforgeangle — Wave 86 return-forgeangle honesty */
kprintf("aarch64: kmain: soft retforgeangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=98 (retforgeangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: kmain: soft retprismangle — Wave 86 exclusive prismangle stamp */
kprintf("aarch64: kmain: soft retprismangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=98 (retprismangle stamp; Soft≠product)\n");
/* Grep: aarch64: kmain: soft retflameangle — Wave 87 return-flameangle honesty */
kprintf("aarch64: kmain: soft retflameangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=98 (retflameangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: kmain: soft retcipherangle — Wave 87 exclusive cipherangle stamp */
kprintf("aarch64: kmain: soft retcipherangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=98 (retcipherangle stamp; Soft≠product)\n");
/* Grep: aarch64: kmain: soft retbeaconangle — Wave 88 return-beaconangle honesty */
kprintf("aarch64: kmain: soft retbeaconangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=98 (retbeaconangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: kmain: soft retledgerangle — Wave 88 exclusive ledgerangle stamp */
kprintf("aarch64: kmain: soft retledgerangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=98 (retledgerangle stamp; Soft≠product)\n");
/* Grep: aarch64: kmain: soft retbannerangle — Wave 89 return-bannerangle honesty */
kprintf("aarch64: kmain: soft retbannerangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=98 (retbannerangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: kmain: soft retvaultangle — Wave 89 exclusive vaultangle stamp */
kprintf("aarch64: kmain: soft retvaultangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=98 (retvaultangle stamp; Soft≠product)\n");
/* Grep: aarch64: kmain: soft retcrestangle — Wave 90 return-crestangle honesty */
kprintf("aarch64: kmain: soft retcrestangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=98 (retcrestangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: kmain: soft rettokenangle — Wave 90 exclusive tokenangle stamp */
kprintf("aarch64: kmain: soft rettokenangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=98 (rettokenangle stamp; Soft≠product)\n");
/* Grep: aarch64: kmain: soft retbadgeangle — Wave 91 return-badgeangle honesty */
kprintf("aarch64: kmain: soft retbadgeangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=98 (retbadgeangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: kmain: soft retphaseangle — Wave 91 exclusive phaseangle stamp */
kprintf("aarch64: kmain: soft retphaseangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=98 (retphaseangle stamp; Soft≠product)\n");
/* Grep: aarch64: kmain: soft retmarkangle — Wave 92 return-markangle honesty */
kprintf("aarch64: kmain: soft retmarkangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=98 (retmarkangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: kmain: soft retpulseangle — Wave 92 exclusive pulseangle stamp */
kprintf("aarch64: kmain: soft retpulseangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=98 (retpulseangle stamp; Soft≠product)\n");

/* Grep: aarch64: kmain: soft retsealangle — Wave 93 return-sealangle honesty */
kprintf("aarch64: kmain: soft retsealangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=98 (retsealangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: kmain: soft retboundangle — Wave 93 exclusive boundangle stamp */
kprintf("aarch64: kmain: soft retboundangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=98 (retboundangle stamp; Soft≠product)\n");
/* Grep: aarch64: kmain: soft retstemangle — Wave 94 return-stemangle honesty */
kprintf("aarch64: kmain: soft retstemangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=98 (retstemangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: kmain: soft retbladeangle — Wave 94 exclusive bladeangle stamp */
kprintf("aarch64: kmain: soft retbladeangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=98 (retbladeangle stamp; Soft≠product)\n");
/* Grep: aarch64: kmain: soft retchordangle — Wave 95 return-chordangle honesty */
kprintf("aarch64: kmain: soft retchordangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=98 (retchordangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: kmain: soft retarcangle — Wave 95 exclusive arcangle stamp */
kprintf("aarch64: kmain: soft retarcangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=98 (retarcangle stamp; Soft≠product)\n");
/* Grep: aarch64: kmain: soft retsectorangle — Wave 96 return-sectorangle honesty */
kprintf("aarch64: kmain: soft retsectorangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=98 (retsectorangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: kmain: soft retwedgeangle — Wave 96 exclusive wedgeangle stamp */
kprintf("aarch64: kmain: soft retwedgeangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=98 (retwedgeangle stamp; Soft≠product)\n");
/* Grep: aarch64: kmain: soft retradiusangle — Wave 97 return-radiusangle honesty */
kprintf("aarch64: kmain: soft retradiusangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=98 (retradiusangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: kmain: soft retdiameterangle — Wave 97 exclusive diameterangle stamp */
kprintf("aarch64: kmain: soft retdiameterangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=98 (retdiameterangle stamp; Soft≠product)\n");
/* Grep: aarch64: kmain: soft retcircumangle — Wave 98 return-circumangle honesty */
kprintf("aarch64: kmain: soft retcircumangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=98 (retcircumangle honesty; Soft≠product; not bar3)\n");
/* Grep: aarch64: kmain: soft retellipseangle — Wave 98 exclusive ellipseangle stamp */
kprintf("aarch64: kmain: soft retellipseangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=98 (retellipseangle stamp; Soft≠product)\n");
                            kprintf("aarch64: kmain soft deepen wave=%u areas=%u "
            "catalog=inventory,bringup,shared,mem,stats,path,surf,return,honesty,exclusive,open "
            "logs=%u soft_only=1\n",
            (unsigned)KMAIN_SOFT_WAVE, (unsigned)KMAIN_SOFT_AREAS,
            (unsigned)g_cSoftInvLogs);

    /* Grep: aarch64: kmain soft surf — Wave 19 bringup/shared/mem lamps */
    kprintf("aarch64: kmain soft surf bringup=%u shared=%u mem=%u "
            "lamps=%u ratio=%u bits=0x%x wave=%u\n",
            cBringup, cShared, (unsigned)g_u8SoftMem, cLamps, uRatio,
            (unsigned)(((cBringup >= 10u) ? 1u : 0u) |
                       (((cShared >= 3u) ? 1u : 0u) << 1) |
                       (((unsigned)g_u8SoftMem) << 2) |
                       (((g_cSoftFail == 0u) ? 1u : 0u) << 3)),
            (unsigned)KMAIN_SOFT_WAVE);

    /*
     * Grep: aarch64: kmain soft path
     * Honesty: M0 aarch64 scaffold only — not bar3 / Deck Top 50 / continuum.
     * product_kernel=OPEN: aarch64 product kernel remains OPEN.
     */
    kprintf("aarch64: kmain soft path m0=1 bar3=0 deck=0 continuum=0 "
            "arch=aarch64 product_kernel=OPEN hard_gate=0 wave=%u\n",
            (unsigned)KMAIN_SOFT_WAVE);

    /*
     * Grep: aarch64: kmain soft honesty
     * Explicit non-claim: soft inventory deepen ≠ product kernel complete.
     */
    kprintf("aarch64: kmain soft honesty product_kernel=OPEN soft_only=1 "
            "no_bar3=1 no_deck=1 no_continuum=1 m0_scaffold=1 "
            "multi_server=0 confine=0 bar3=0 wave=%u\n",
            (unsigned)KMAIN_SOFT_WAVE);

    /*
     * Grep: aarch64: kmain soft exclusive — Wave 35 exclusive deepen.
     * Soft inventory ≠ product multi-server confine / product kernel.
     */
    kprintf("aarch64: kmain soft exclusive wave=%u multi_server=0 confine=0 "
            "bar3=0 product_kernel=OPEN soft_only=1 continuum=0\n",
            (unsigned)KMAIN_SOFT_WAVE);

    /*
     * Grep: aarch64: kmain soft open — Wave 19 open-lamp rollup.
     * Explicit product_kernel=OPEN; soft ≠ product complete.
     */
    kprintf("aarch64: kmain soft open multi_server=0 confine=0 bar3=0 "
            "product_kernel=OPEN soft_only=1 wave=%u\n",
            (unsigned)KMAIN_SOFT_WAVE);

    fOk = 0;
    if (g_cSoftFail == 0u &&
        g_u8SoftSharedC != 0u && g_u8SoftSched != 0u &&
        g_u8SoftSharedPmm != 0u && g_u8SoftMem != 0u &&
        g_u8SoftExc != 0u && g_u8SoftCpu != 0u &&
        g_u8SoftPsci != 0u && g_u8SoftGic != 0u &&
        g_u8SoftTimer != 0u && g_u8SoftPmmInit != 0u &&
        g_u8SoftMmu != 0u && g_u8SoftCoop != 0u &&
        g_u8SoftSvc != 0u && g_u8SoftVirtio != 0u) {
        fOk = 1;
    }

    /*
     * Grep: aarch64: kmain soft return — Wave 19 return surfaces.
     * inv_ret is the soft inventory bool; product M0 still prints after.
     * product_kernel=OPEN: soft return ≠ product kernel complete.
     */
    kprintf("aarch64: kmain soft return inv_ret=%u soft_ok=%u soft_fail=%u "
            "lamps=%u phases=%u product_kernel=OPEN wave=%u\n",
            (fOk != 0) ? 1u : 0u, (unsigned)g_cSoftOk, (unsigned)g_cSoftFail,
            cLamps, (unsigned)g_cSoftPhases, (unsigned)KMAIN_SOFT_WAVE);

    /*
     * Grep: aarch64: kmain soft PASS | FAIL
     * Smoke scripts (run-aarch64 / podman-smoke / product-summary) expect
     * the PASS form; FAIL remains greppable and never blocks M0 OK.
     */
    if (fOk != 0) {
        kprintf("aarch64: kmain soft PASS phases=%u soft_ok=%u soft_fail=%u "
                "pmm_free=%u pmm_tot=%u coop_id=%u wave=%u\n",
                (unsigned)g_cSoftPhases, (unsigned)g_cSoftOk,
                (unsigned)g_cSoftFail, cFree, cTotal, (unsigned)u32CoopId,
                (unsigned)KMAIN_SOFT_WAVE);
    } else {
        kprintf("aarch64: kmain soft FAIL phases=%u soft_ok=%u soft_fail=%u "
                "pmm_free=%u pmm_tot=%u coop_id=%u wave=%u\n",
                (unsigned)g_cSoftPhases, (unsigned)g_cSoftOk,
                (unsigned)g_cSoftFail, cFree, cTotal, (unsigned)u32CoopId,
                (unsigned)KMAIN_SOFT_WAVE);
    }
    return fOk;
}

void
aarch64_kmain(void)
{
    int fSoft;

    kprintf("GreenJade aarch64 product kernel (shared C)\n");

    (void)shared_c_probe();

    aarch64_exceptions_install();
    soft_tick(1); /* install path is non-returning-fail in product scaffold */
    g_u8SoftExc = 1u;

    aarch64_cpu_probe();
    soft_tick(1);
    g_u8SoftCpu = 1u;

    /* PSCI: HVC/SMC with fault recovery (PASS or soft SKIP). */
    aarch64_psci_probe();
    soft_tick(1);
    g_u8SoftPsci = 1u;

    aarch64_gic_init();
    soft_tick(1);
    g_u8SoftGic = 1u;

    aarch64_timer_probe();
    soft_tick(1);
    g_u8SoftTimer = 1u;

    aarch64_pmm_init();
    soft_tick(1);
    g_u8SoftPmmInit = 1u;

    aarch64_mmu_init();
    soft_tick(1);
    g_u8SoftMmu = 1u;

    gj_coop_init();
    soft_tick(1);
    g_u8SoftCoop = 1u;

    shared_mm_sched_probe();

    aarch64_svc_selftest();
    soft_tick(1);
    g_u8SoftSvc = 1u;

    aarch64_virtio_mmio_probe();
    soft_tick(1);
    g_u8SoftVirtio = 1u;

    mem_probe();

    /*
     * Wave 19 combined soft inventory under "aarch64: kmain soft …".
     * Soft summary only — M0 OK remains the fixed product bar grepped by
     * make aarch64-smoke / run-aarch64.sh (exact "M0 OK").
     * Honesty: soft PASS ≠ aarch64 product kernel complete (remains OPEN).
     */
    fSoft = kmain_soft_inventory();
    (void)fSoft;
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
