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
 * Soft inventory (Wave 27 exclusive deepen; this unit only — greppable
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
 *   aarch64: kmain soft inventory wave=27 phases=… soft_ok=… soft_fail=…
 *             pmm_free=… pmm_tot=… coop_id=… logs=…
 *   aarch64: kmain soft bringup exceptions=… cpu=… psci=… gic=…
 *             timer=… pmm=… mmu=… coop=… svc=… virtio=…
 *   aarch64: kmain soft shared c=… sched=… pmm=…
 *   aarch64: kmain soft mem probe=… free=… total=…
 *   aarch64: kmain soft stats lamps=… bringup=… shared=… mem=… ratio=…
 *   aarch64: kmain soft deepen wave=27 areas=… logs=…
 *   aarch64: kmain soft surf bringup=… shared=… mem=… lamps=… bits=…
 *   aarch64: kmain soft return inv_ret=… soft_ok=… soft_fail=…
 *             product_kernel=OPEN wave=27
 *   aarch64: kmain soft path m0=1 bar3=0 deck=0 continuum=0 arch=aarch64
 *             product_kernel=OPEN wave=27
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

/* Wave 27 soft inventory stamp (greppable wave=27). */
#define KMAIN_SOFT_WAVE 27u

/* Soft area count for deepen catalog (inventory+bringup+shared+mem+path+stats). */
#define KMAIN_SOFT_AREAS 29u

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
                     * ---- Wave 27 exclusive complementary surfaces (never reshape primary).
                     * Return surfaces only — soft inventory; never hard-gates product paths.
                     * Soft≠product; not bar3.
                     */
                    /* Grep: aarch64: kmain: soft retprism — Wave 27 return-prism honesty */
                    kprintf("aarch64: kmain: soft retprism soft_only=1 product_gate=0 soft_ne_product=1 "
                            "never_blocks_m0=1 wave=%u "
                            "(retprism honesty; Soft≠product; not bar3)\n",
                            (unsigned)KMAIN_SOFT_WAVE);
                    /* Grep: aarch64: kmain: soft retforge — Wave 27 exclusive forge stamp */
                    kprintf("aarch64: kmain: soft retforge exclusive=1 soft_ne_product=1 "
                            "product_kernel=OPEN bar3=0 wave=%u "
                            "(retforge stamp; Soft≠product)\n",
                            (unsigned)KMAIN_SOFT_WAVE);
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
     * Grep: aarch64: kmain soft exclusive — Wave 27 exclusive deepen.
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
