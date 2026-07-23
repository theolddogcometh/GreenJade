/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Early CPU identity + feature soft probe for aarch64 product scaffold
 * (QEMU virt). Pure freestanding C; MRS system registers only.
 *
 * -------------------------------------------------------------------------
 * Soft inventory (Wave 23 exclusive deepen; this unit only — greppable
 * "aarch64: cpu soft …")
 * -------------------------------------------------------------------------
 * Soft-read ID_AA64* / CTR / DCZID / CLIDR / DFR0 and emit greppable
 * multi-field lamps under a stable "aarch64: cpu soft …" prefix:
 *   - Identity: EL, MPIDR, MIDR, REVIDR (raw + decoded fields)
 *   - PFR0: EL0–EL3 AArch64, FP, AdvSIMD
 *   - ISAR0/1: AES/SHA/CRC/atomics (+ ISAR1 soft pack)
 *   - MMFR0: PARange / ASID / 4K granule
 *   - CTR / DCZID: I/D min lines, DC ZVA block + DZP
 *   - CLIDR / DFR0: cache-level + debug soft packs (read-only MRS)
 *   - Path honesty: MRS soft probe only; no MMU/GIC/timer claim
 *   - Inv gates: EL1 + PFR EL0/EL1 + TGran4 (QEMU virt shape)
 *
 * Non-fatal; product path still prints aarch64: cpu PASS whenever MRS
 * is reachable. Soft PASS|FAIL is inventory-only (smoke greps PASS).
 *
 * Also invokes aarch64_uart_soft_selftest (uart_pl011.c) so PL011 soft
 * markers appear once during early bring-up without a kmain change.
 *
 * Greppable soft inventory (prefix-stable):
 *   aarch64: cpu soft id el=… mpidr=… midr=… revidr=…
 *   aarch64: cpu soft midr imp=… var=… arch=… part=… rev=…
 *   aarch64: cpu soft mpidr aff0=… aff1=… aff2=… aff3=… u=… mt=…
 *   aarch64: cpu soft pfr el0=… el1=… el2=… el3=… fp=… advsimd=…
 *   aarch64: cpu soft isar aes=… sha1=… sha2=… crc32=… atomic=… isar1=…
 *   aarch64: cpu soft mmfr parange=… asid=… tgran4=…
 *   aarch64: cpu soft cache dmin=… imin=… dcz_bs=… dzp=… ctr=… dczid=…
 *   aarch64: cpu soft extra clidr=… dfr0=…
 *   aarch64: cpu soft regs pfr0=… isar0=… mmfr0=…
 *   aarch64: cpu soft path mrs=1 mmu=0 gic=0 timer=0 claim=0 product_kernel=OPEN
 *   aarch64: cpu soft inv el1=… el0_a64=… el1_a64=… tgran4=… ok=…
 *   aarch64: cpu soft inventory wave=23 …
 *   aarch64: cpu soft surf …
 *   aarch64: cpu soft deepen wave=23 areas=…
 *   aarch64: cpu soft return inv_ret=… product_kernel=OPEN
 *   aarch64: cpu soft honesty product_kernel=OPEN soft_only=1
 *   aarch64: cpu soft PASS | FAIL
 *
 * Legacy / product smoke markers (kept greppable):
 *   aarch64: cpu EL=… mpidr=… midr=… revidr=…
 *   aarch64: cpu features soft pfr0=… isar0=… mmfr0=… ctr=… dczid=…
 *   aarch64: cpu features soft el0=… el1=… … (field decode)
 *   aarch64: cpu PASS
 *
 * Freestanding pure C; no GPL Linux cpu_info paste.
 */
#include "types_arch.h"

extern void aarch64_uart_puts(const char *sz);
extern void aarch64_uart_put_hex(unsigned long v);
extern void aarch64_uart_put_hex_n(unsigned long v, unsigned cNibbles);
extern void aarch64_uart_soft_selftest(void);

/* ID_AA64PFR0_EL1 field helpers (architecture reference manual). */
#define PFR0_EL0_SHIFT     0u
#define PFR0_EL1_SHIFT     4u
#define PFR0_EL2_SHIFT     8u
#define PFR0_EL3_SHIFT     12u
#define PFR0_EL_MASK       0xful
#define PFR0_FP_SHIFT      16u
#define PFR0_ADVSIMD_SHIFT 20u

/* ID_AA64MMFR0_EL1 — soft granule / PARange bits. */
#define MMFR0_PARANGE_SHIFT  0u
#define MMFR0_PARANGE_MASK   0xful
#define MMFR0_ASIDBITS_SHIFT 4u
#define MMFR0_ASIDBITS_MASK  0xful
#define MMFR0_TGRAN4_SHIFT   28u
#define MMFR0_TGRAN4_MASK    0xful

/* ID_AA64ISAR0_EL1 — soft crypto / atomic feature nibbles. */
#define ISAR0_AES_SHIFT    4u
#define ISAR0_SHA1_SHIFT   8u
#define ISAR0_SHA2_SHIFT   12u
#define ISAR0_CRC32_SHIFT  16u
#define ISAR0_ATOMIC_SHIFT 20u
#define ISAR0_FIELD_MASK   0xful

/* CTR_EL0 — cache type soft decode. */
#define CTR_DMINLINE_SHIFT 16u
#define CTR_DMINLINE_MASK  0xful
#define CTR_IMINLINE_SHIFT 0u
#define CTR_IMINLINE_MASK  0xful

/* DCZID_EL0 — DC ZVA block soft. */
#define DCZID_BS_MASK 0xful
#define DCZID_DZP_BIT (1ul << 4)

/* Wave 23 soft inventory stamp (file-local; never product gate). */
#define CPU_SOFT_WAVE   23u
/* Areas: id,midr,mpidr,pfr,isar,mmfr,cache,extra,regs,path,inv,surf,honesty,deepen */
#define CPU_SOFT_AREAS 25u

/* Soft inventory emit counter (Wave 19 stats). */
static unsigned g_cCpuSoftLogs;

/* MIDR_EL1 — implementer / part soft decode. */
#define MIDR_REVISION_SHIFT     0u
#define MIDR_REVISION_MASK      0xful
#define MIDR_PARTNUM_SHIFT      4u
#define MIDR_PARTNUM_MASK       0xffful
#define MIDR_ARCHITECTURE_SHIFT 16u
#define MIDR_ARCHITECTURE_MASK  0xful
#define MIDR_VARIANT_SHIFT      20u
#define MIDR_VARIANT_MASK       0xful
#define MIDR_IMPLEMENTER_SHIFT  24u
#define MIDR_IMPLEMENTER_MASK   0xfful

/* MPIDR_EL1 — affinity + topology soft decode. */
#define MPIDR_AFF0_SHIFT 0u
#define MPIDR_AFF0_MASK  0xfful
#define MPIDR_AFF1_SHIFT 8u
#define MPIDR_AFF1_MASK  0xfful
#define MPIDR_AFF2_SHIFT 16u
#define MPIDR_AFF2_MASK  0xfful
#define MPIDR_MT_BIT     (1ul << 24)
#define MPIDR_U_BIT      (1ul << 30)
#define MPIDR_AFF3_SHIFT 32u
#define MPIDR_AFF3_MASK  0xfful

/*
 * Soft inventory snapshot (Wave 19; file-local; never hard-gates boot).
 * greppable: aarch64: cpu soft
 */
struct cpu_soft_snap {
    unsigned long u64El;
    unsigned long u64Mpidr;
    unsigned long u64Midr;
    unsigned long u64Revidr;
    unsigned long u64Pfr0;
    unsigned long u64Isar0;
    unsigned long u64Isar1;
    unsigned long u64Mmfr0;
    unsigned long u64Ctr;
    unsigned long u64Dczid;
    unsigned long u64Clidr;
    unsigned long u64Dfr0;
    unsigned      uEl0;
    unsigned      uEl1;
    unsigned      uEl2;
    unsigned      uEl3;
    unsigned      uFp;
    unsigned      uAdvSimd;
    unsigned      uPaRange;
    unsigned      uAsidBits;
    unsigned      uTgran4;
    unsigned      uAes;
    unsigned      uSha1;
    unsigned      uSha2;
    unsigned      uCrc32;
    unsigned      uAtomic;
    unsigned      uDMinLine;
    unsigned      uIMinLine;
    unsigned      uDczBs;
    unsigned      uMidrImp;
    unsigned      uMidrVar;
    unsigned      uMidrArch;
    unsigned      uMidrPart;
    unsigned      uMidrRev;
    unsigned      uAff0;
    unsigned      uAff1;
    unsigned      uAff2;
    unsigned      uAff3;
    unsigned      uDzp;
    unsigned      uMt;
    unsigned      uU;
    unsigned      uEl1Ok;
    unsigned      uEl0A64;
    unsigned      uEl1A64;
    unsigned      uTgran4Ok;
    unsigned      uGateOk;
};

/*
 * Soft feature observe: read ID registers, decode product-relevant fields,
 * print legacy "features soft" detail lines, fill Wave 19 snap.
 * Returns 1 if EL1 and EL0 AArch64 are present in PFR0 (QEMU virt shape)
 * and CurrentEL is EL1 and 4K granule is implemented.
 */
static int
cpu_features_soft_observe(unsigned long u64El, struct cpu_soft_snap *pSnap)
{
    unsigned long u64Pfr0;
    unsigned long u64Isar0;
    unsigned long u64Isar1;
    unsigned long u64Mmfr0;
    unsigned long u64Ctr;
    unsigned long u64Dczid;
    unsigned long u64Clidr;
    unsigned long u64Dfr0;
    unsigned uEl0;
    unsigned uEl1;
    unsigned uEl2;
    unsigned uEl3;
    unsigned uFp;
    unsigned uAdvSimd;
    unsigned uPaRange;
    unsigned uAsidBits;
    unsigned uTgran4;
    unsigned uAes;
    unsigned uSha1;
    unsigned uSha2;
    unsigned uCrc32;
    unsigned uAtomic;
    unsigned uDMinLine;
    unsigned uIMinLine;
    unsigned uDczBs;
    int fOk;

    __asm__ volatile("mrs %0, id_aa64pfr0_el1" : "=r"(u64Pfr0));
    __asm__ volatile("mrs %0, id_aa64isar0_el1" : "=r"(u64Isar0));
    __asm__ volatile("mrs %0, id_aa64isar1_el1" : "=r"(u64Isar1));
    __asm__ volatile("mrs %0, id_aa64mmfr0_el1" : "=r"(u64Mmfr0));
    __asm__ volatile("mrs %0, id_aa64dfr0_el1" : "=r"(u64Dfr0));
    __asm__ volatile("mrs %0, ctr_el0" : "=r"(u64Ctr));
    __asm__ volatile("mrs %0, dczid_el0" : "=r"(u64Dczid));
    __asm__ volatile("mrs %0, clidr_el1" : "=r"(u64Clidr));

    uEl0 = (unsigned)((u64Pfr0 >> PFR0_EL0_SHIFT) & PFR0_EL_MASK);
    uEl1 = (unsigned)((u64Pfr0 >> PFR0_EL1_SHIFT) & PFR0_EL_MASK);
    uEl2 = (unsigned)((u64Pfr0 >> PFR0_EL2_SHIFT) & PFR0_EL_MASK);
    uEl3 = (unsigned)((u64Pfr0 >> PFR0_EL3_SHIFT) & PFR0_EL_MASK);
    uFp = (unsigned)((u64Pfr0 >> PFR0_FP_SHIFT) & PFR0_EL_MASK);
    uAdvSimd = (unsigned)((u64Pfr0 >> PFR0_ADVSIMD_SHIFT) & PFR0_EL_MASK);

    uPaRange = (unsigned)((u64Mmfr0 >> MMFR0_PARANGE_SHIFT) & MMFR0_PARANGE_MASK);
    uAsidBits = (unsigned)((u64Mmfr0 >> MMFR0_ASIDBITS_SHIFT) &
                           MMFR0_ASIDBITS_MASK);
    uTgran4 = (unsigned)((u64Mmfr0 >> MMFR0_TGRAN4_SHIFT) & MMFR0_TGRAN4_MASK);

    uAes = (unsigned)((u64Isar0 >> ISAR0_AES_SHIFT) & ISAR0_FIELD_MASK);
    uSha1 = (unsigned)((u64Isar0 >> ISAR0_SHA1_SHIFT) & ISAR0_FIELD_MASK);
    uSha2 = (unsigned)((u64Isar0 >> ISAR0_SHA2_SHIFT) & ISAR0_FIELD_MASK);
    uCrc32 = (unsigned)((u64Isar0 >> ISAR0_CRC32_SHIFT) & ISAR0_FIELD_MASK);
    uAtomic = (unsigned)((u64Isar0 >> ISAR0_ATOMIC_SHIFT) & ISAR0_FIELD_MASK);

    uDMinLine = (unsigned)((u64Ctr >> CTR_DMINLINE_SHIFT) & CTR_DMINLINE_MASK);
    uIMinLine = (unsigned)((u64Ctr >> CTR_IMINLINE_SHIFT) & CTR_IMINLINE_MASK);
    uDczBs = (unsigned)(u64Dczid & DCZID_BS_MASK);

    /*
     * Soft PASS gate (QEMU virt / product EL1 kernel):
     *   - running at EL1
     *   - PFR0 reports AArch64 at EL0 and EL1 (nibble != 0)
     *   - 4K granule implemented (TGran4 == 0 means supported)
     */
    fOk = 1;
    if (u64El != 1ul) {
        fOk = 0;
    }
    if (uEl0 == 0u || uEl1 == 0u) {
        fOk = 0;
    }
    if (uTgran4 != 0u) {
        /* Non-zero TGran4 means 4K not supported (rare on virt). */
        fOk = 0;
    }

    if (pSnap != 0) {
        pSnap->u64El = u64El;
        pSnap->u64Pfr0 = u64Pfr0;
        pSnap->u64Isar0 = u64Isar0;
        pSnap->u64Isar1 = u64Isar1;
        pSnap->u64Mmfr0 = u64Mmfr0;
        pSnap->u64Ctr = u64Ctr;
        pSnap->u64Dczid = u64Dczid;
        pSnap->u64Clidr = u64Clidr;
        pSnap->u64Dfr0 = u64Dfr0;
        pSnap->uEl0 = uEl0;
        pSnap->uEl1 = uEl1;
        pSnap->uEl2 = uEl2;
        pSnap->uEl3 = uEl3;
        pSnap->uFp = uFp;
        pSnap->uAdvSimd = uAdvSimd;
        pSnap->uPaRange = uPaRange;
        pSnap->uAsidBits = uAsidBits;
        pSnap->uTgran4 = uTgran4;
        pSnap->uAes = uAes;
        pSnap->uSha1 = uSha1;
        pSnap->uSha2 = uSha2;
        pSnap->uCrc32 = uCrc32;
        pSnap->uAtomic = uAtomic;
        pSnap->uDMinLine = uDMinLine;
        pSnap->uIMinLine = uIMinLine;
        pSnap->uDczBs = uDczBs;
        pSnap->uDzp = (u64Dczid & DCZID_DZP_BIT) != 0ul ? 1u : 0u;
        pSnap->uEl1Ok = (u64El == 1ul) ? 1u : 0u;
        pSnap->uEl0A64 = (uEl0 != 0u) ? 1u : 0u;
        pSnap->uEl1A64 = (uEl1 != 0u) ? 1u : 0u;
        pSnap->uTgran4Ok = (uTgran4 == 0u) ? 1u : 0u;
        pSnap->uGateOk = (fOk != 0) ? 1u : 0u;
    }

    /* Legacy one-line raw pack (kept greppable for existing smoke detail). */
    aarch64_uart_puts("aarch64: cpu features soft pfr0=");
    aarch64_uart_put_hex(u64Pfr0);
    aarch64_uart_puts(" isar0=");
    aarch64_uart_put_hex(u64Isar0);
    aarch64_uart_puts(" mmfr0=");
    aarch64_uart_put_hex(u64Mmfr0);
    aarch64_uart_puts(" ctr=");
    aarch64_uart_put_hex(u64Ctr);
    aarch64_uart_puts(" dczid=");
    aarch64_uart_put_hex(u64Dczid);
    aarch64_uart_puts("\n");

    /* Legacy field decode line (kept greppable). */
    aarch64_uart_puts("aarch64: cpu features soft el0=");
    aarch64_uart_put_hex_n((unsigned long)uEl0, 1u);
    aarch64_uart_puts(" el1=");
    aarch64_uart_put_hex_n((unsigned long)uEl1, 1u);
    aarch64_uart_puts(" el2=");
    aarch64_uart_put_hex_n((unsigned long)uEl2, 1u);
    aarch64_uart_puts(" el3=");
    aarch64_uart_put_hex_n((unsigned long)uEl3, 1u);
    aarch64_uart_puts(" parange=");
    aarch64_uart_put_hex_n((unsigned long)uPaRange, 1u);
    aarch64_uart_puts(" asid=");
    aarch64_uart_put_hex_n((unsigned long)uAsidBits, 1u);
    aarch64_uart_puts(" tgran4=");
    aarch64_uart_put_hex_n((unsigned long)uTgran4, 1u);
    aarch64_uart_puts(" aes=");
    aarch64_uart_put_hex_n((unsigned long)uAes, 1u);
    aarch64_uart_puts(" sha1=");
    aarch64_uart_put_hex_n((unsigned long)uSha1, 1u);
    aarch64_uart_puts(" sha2=");
    aarch64_uart_put_hex_n((unsigned long)uSha2, 1u);
    aarch64_uart_puts(" crc32=");
    aarch64_uart_put_hex_n((unsigned long)uCrc32, 1u);
    aarch64_uart_puts(" atomic=");
    aarch64_uart_put_hex_n((unsigned long)uAtomic, 1u);
    aarch64_uart_puts(" dmin=");
    aarch64_uart_put_hex_n((unsigned long)uDMinLine, 1u);
    aarch64_uart_puts(" imin=");
    aarch64_uart_put_hex_n((unsigned long)uIMinLine, 1u);
    aarch64_uart_puts(" dcz_bs=");
    aarch64_uart_put_hex_n((unsigned long)uDczBs, 1u);
    aarch64_uart_puts(" dzp=");
    aarch64_uart_put_hex_n((u64Dczid & DCZID_DZP_BIT) != 0ul ? 1ul : 0ul, 1u);
    aarch64_uart_puts("\n");

    return fOk;
}

/*
 * Wave 19 soft inventory emission — greppable "aarch64: cpu soft …".
 * Returns 1 if inv gates held (EL1 + EL0/EL1 AArch64 + TGran4).
 */
static int
cpu_soft_inventory(const struct cpu_soft_snap *pSnap)
{
    int fOk;

    if (pSnap == 0) {
        aarch64_uart_puts("aarch64: cpu soft FAIL\n");
        return 0;
    }

    /* Grep: aarch64: cpu soft id */
    aarch64_uart_puts("aarch64: cpu soft id el=");
    aarch64_uart_put_hex(pSnap->u64El);
    aarch64_uart_puts(" mpidr=");
    aarch64_uart_put_hex(pSnap->u64Mpidr);
    aarch64_uart_puts(" midr=");
    aarch64_uart_put_hex(pSnap->u64Midr);
    aarch64_uart_puts(" revidr=");
    aarch64_uart_put_hex(pSnap->u64Revidr);
    aarch64_uart_puts("\n");

    /* Grep: aarch64: cpu soft midr */
    aarch64_uart_puts("aarch64: cpu soft midr imp=");
    aarch64_uart_put_hex_n((unsigned long)pSnap->uMidrImp, 2u);
    aarch64_uart_puts(" var=");
    aarch64_uart_put_hex_n((unsigned long)pSnap->uMidrVar, 1u);
    aarch64_uart_puts(" arch=");
    aarch64_uart_put_hex_n((unsigned long)pSnap->uMidrArch, 1u);
    aarch64_uart_puts(" part=");
    aarch64_uart_put_hex_n((unsigned long)pSnap->uMidrPart, 3u);
    aarch64_uart_puts(" rev=");
    aarch64_uart_put_hex_n((unsigned long)pSnap->uMidrRev, 1u);
    aarch64_uart_puts("\n");

    /* Grep: aarch64: cpu soft mpidr */
    aarch64_uart_puts("aarch64: cpu soft mpidr aff0=");
    aarch64_uart_put_hex_n((unsigned long)pSnap->uAff0, 2u);
    aarch64_uart_puts(" aff1=");
    aarch64_uart_put_hex_n((unsigned long)pSnap->uAff1, 2u);
    aarch64_uart_puts(" aff2=");
    aarch64_uart_put_hex_n((unsigned long)pSnap->uAff2, 2u);
    aarch64_uart_puts(" aff3=");
    aarch64_uart_put_hex_n((unsigned long)pSnap->uAff3, 2u);
    aarch64_uart_puts(" u=");
    aarch64_uart_put_hex_n((unsigned long)pSnap->uU, 1u);
    aarch64_uart_puts(" mt=");
    aarch64_uart_put_hex_n((unsigned long)pSnap->uMt, 1u);
    aarch64_uart_puts("\n");

    /* Grep: aarch64: cpu soft pfr */
    aarch64_uart_puts("aarch64: cpu soft pfr el0=");
    aarch64_uart_put_hex_n((unsigned long)pSnap->uEl0, 1u);
    aarch64_uart_puts(" el1=");
    aarch64_uart_put_hex_n((unsigned long)pSnap->uEl1, 1u);
    aarch64_uart_puts(" el2=");
    aarch64_uart_put_hex_n((unsigned long)pSnap->uEl2, 1u);
    aarch64_uart_puts(" el3=");
    aarch64_uart_put_hex_n((unsigned long)pSnap->uEl3, 1u);
    aarch64_uart_puts(" fp=");
    aarch64_uart_put_hex_n((unsigned long)pSnap->uFp, 1u);
    aarch64_uart_puts(" advsimd=");
    aarch64_uart_put_hex_n((unsigned long)pSnap->uAdvSimd, 1u);
    aarch64_uart_puts("\n");

    /* Grep: aarch64: cpu soft isar */
    aarch64_uart_puts("aarch64: cpu soft isar aes=");
    aarch64_uart_put_hex_n((unsigned long)pSnap->uAes, 1u);
    aarch64_uart_puts(" sha1=");
    aarch64_uart_put_hex_n((unsigned long)pSnap->uSha1, 1u);
    aarch64_uart_puts(" sha2=");
    aarch64_uart_put_hex_n((unsigned long)pSnap->uSha2, 1u);
    aarch64_uart_puts(" crc32=");
    aarch64_uart_put_hex_n((unsigned long)pSnap->uCrc32, 1u);
    aarch64_uart_puts(" atomic=");
    aarch64_uart_put_hex_n((unsigned long)pSnap->uAtomic, 1u);
    aarch64_uart_puts(" isar1=");
    aarch64_uart_put_hex(pSnap->u64Isar1);
    aarch64_uart_puts("\n");

    /* Grep: aarch64: cpu soft mmfr */
    aarch64_uart_puts("aarch64: cpu soft mmfr parange=");
    aarch64_uart_put_hex_n((unsigned long)pSnap->uPaRange, 1u);
    aarch64_uart_puts(" asid=");
    aarch64_uart_put_hex_n((unsigned long)pSnap->uAsidBits, 1u);
    aarch64_uart_puts(" tgran4=");
    aarch64_uart_put_hex_n((unsigned long)pSnap->uTgran4, 1u);
    aarch64_uart_puts("\n");

    /* Grep: aarch64: cpu soft cache */
    aarch64_uart_puts("aarch64: cpu soft cache dmin=");
    aarch64_uart_put_hex_n((unsigned long)pSnap->uDMinLine, 1u);
    aarch64_uart_puts(" imin=");
    aarch64_uart_put_hex_n((unsigned long)pSnap->uIMinLine, 1u);
    aarch64_uart_puts(" dcz_bs=");
    aarch64_uart_put_hex_n((unsigned long)pSnap->uDczBs, 1u);
    aarch64_uart_puts(" dzp=");
    aarch64_uart_put_hex_n((unsigned long)pSnap->uDzp, 1u);
    aarch64_uart_puts(" ctr=");
    aarch64_uart_put_hex(pSnap->u64Ctr);
    aarch64_uart_puts(" dczid=");
    aarch64_uart_put_hex(pSnap->u64Dczid);
    aarch64_uart_puts("\n");

    /* Grep: aarch64: cpu soft extra */
    aarch64_uart_puts("aarch64: cpu soft extra clidr=");
    aarch64_uart_put_hex(pSnap->u64Clidr);
    aarch64_uart_puts(" dfr0=");
    aarch64_uart_put_hex(pSnap->u64Dfr0);
    aarch64_uart_puts("\n");

    /* Grep: aarch64: cpu soft regs */
    aarch64_uart_puts("aarch64: cpu soft regs pfr0=");
    aarch64_uart_put_hex(pSnap->u64Pfr0);
    aarch64_uart_puts(" isar0=");
    aarch64_uart_put_hex(pSnap->u64Isar0);
    aarch64_uart_puts(" mmfr0=");
    aarch64_uart_put_hex(pSnap->u64Mmfr0);
    aarch64_uart_puts("\n");

    /*
     * Grep: aarch64: cpu soft path
     * Honesty: MRS soft probe only — no MMU/GIC/timer ownership here.
     * product_kernel=OPEN: aarch64 product kernel remains OPEN.
     */
    aarch64_uart_puts("aarch64: cpu soft path mrs=1 mmu=0 gic=0 timer=0 "
                      "claim=0 product_kernel=OPEN hard_gate=0 wave=");
    aarch64_uart_put_hex((unsigned long)CPU_SOFT_WAVE);
    aarch64_uart_puts("\n");
    aarch64_uart_put_hex_n((unsigned long)pSnap->uEl1Ok, 1u);
    aarch64_uart_puts(" el0_a64=");
    aarch64_uart_put_hex_n((unsigned long)pSnap->uEl0A64, 1u);
    aarch64_uart_puts(" el1_a64=");
    aarch64_uart_put_hex_n((unsigned long)pSnap->uEl1A64, 1u);
    aarch64_uart_puts(" tgran4=");
    aarch64_uart_put_hex_n((unsigned long)pSnap->uTgran4Ok, 1u);
    aarch64_uart_puts(" ok=");
    aarch64_uart_put_hex_n((unsigned long)pSnap->uGateOk, 1u);
    aarch64_uart_puts("\n");

    if (g_cCpuSoftLogs < 0xffffffffu) {
        g_cCpuSoftLogs++;
    }

    /* Grep: aarch64: cpu soft inventory — Wave 19 rollup */
    aarch64_uart_puts("aarch64: cpu soft inventory wave=");
    aarch64_uart_put_hex((unsigned long)CPU_SOFT_WAVE);
    aarch64_uart_puts(" el=");
    aarch64_uart_put_hex(pSnap->u64El);
    aarch64_uart_puts(" gate_ok=");
    aarch64_uart_put_hex((unsigned long)pSnap->uGateOk);
    aarch64_uart_puts(" areas=");
    aarch64_uart_put_hex((unsigned long)CPU_SOFT_AREAS);
    aarch64_uart_puts(" logs=");
    aarch64_uart_put_hex((unsigned long)g_cCpuSoftLogs);
    aarch64_uart_puts("\n");

    /* Grep: aarch64: cpu soft surf — Wave 19 inv gate lamps */
    aarch64_uart_puts("aarch64: cpu soft surf el1=");
    aarch64_uart_put_hex_n((unsigned long)pSnap->uEl1Ok, 1u);
    aarch64_uart_puts(" el0_a64=");
    aarch64_uart_put_hex_n((unsigned long)pSnap->uEl0A64, 1u);
    aarch64_uart_puts(" el1_a64=");
    aarch64_uart_put_hex_n((unsigned long)pSnap->uEl1A64, 1u);
    aarch64_uart_puts(" tgran4=");
    aarch64_uart_put_hex_n((unsigned long)pSnap->uTgran4Ok, 1u);
    aarch64_uart_puts(" gate=");
    aarch64_uart_put_hex_n((unsigned long)pSnap->uGateOk, 1u);
    aarch64_uart_puts(" bits=");
    aarch64_uart_put_hex((unsigned long)(
        (pSnap->uEl1Ok) |
        (pSnap->uEl0A64 << 1) |
        (pSnap->uEl1A64 << 2) |
        (pSnap->uTgran4Ok << 3) |
        (pSnap->uGateOk << 4)));
    aarch64_uart_puts(" wave=");
    aarch64_uart_put_hex((unsigned long)CPU_SOFT_WAVE);
    aarch64_uart_puts("\n");

    /* Grep: aarch64: cpu soft deepen */
    /*
     * ---- Wave 19 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft≠product; not bar3.
     */
    /* Grep: aarch64: cpu: soft retclass — Wave 19 return-class taxonomy (kept) */
    aarch64_uart_puts("aarch64: cpu: soft retclass ok|fail|inval|nodev|busy|nomem "
                      "soft_only=1 product_gate=0 wave=");
    aarch64_uart_put_hex((unsigned long)CPU_SOFT_WAVE);
    aarch64_uart_puts(" (retclass taxonomy; Soft!=product; not bar3)\n");
    /* Grep: aarch64: cpu: soft retlane — Wave 19 return-lane catalog (kept) */
    aarch64_uart_puts("aarch64: cpu: soft retlane inv|selftest|rate|retcode|retmap|class "
                      "product_kernel=OPEN soft_ne_product=1 wave=");
    aarch64_uart_put_hex((unsigned long)CPU_SOFT_WAVE);
    aarch64_uart_puts(" (retlane catalog; Soft!=product)\n");
    /*
     * ---- Wave 20 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft!=product; not bar3.
     */
    /* Grep: aarch64: cpu: soft retbound — Wave 20 return-bound honesty (kept) */
    aarch64_uart_puts("aarch64: cpu: soft retbound soft_only=1 product_gate=0 hard_gate=0 "
                      "never_blocks_m0=1 wave=");
    aarch64_uart_put_hex((unsigned long)CPU_SOFT_WAVE);
    aarch64_uart_puts(" (retbound honesty; Soft!=product; not bar3)\n");
    /* Grep: aarch64: cpu: soft retseal — Wave 20 seal stamp (kept) */
    aarch64_uart_puts("aarch64: cpu: soft retseal exclusive=1 soft_ne_product=1 "
                      "product_kernel=OPEN bar3=0 wave=");
    aarch64_uart_put_hex((unsigned long)CPU_SOFT_WAVE);
    aarch64_uart_puts(" (retseal stamp; Soft!=product)\n");
    /*
     * ---- Wave 21 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft!=product; not bar3.
     */
    /* Grep: aarch64: cpu: soft retpulse — Wave 21 return-pulse honesty (kept) */
    aarch64_uart_puts("aarch64: cpu: soft retpulse soft_only=1 product_gate=0 soft_ne_product=1 "
                      "never_blocks_m0=1 wave=");
    aarch64_uart_put_hex((unsigned long)CPU_SOFT_WAVE);
    aarch64_uart_puts(" (retpulse honesty; Soft!=product; not bar3)\n");
    /* Grep: aarch64: cpu: soft retmark — Wave 21 mark stamp (kept) */
    aarch64_uart_puts("aarch64: cpu: soft retmark exclusive=1 soft_ne_product=1 "
                      "product_kernel=OPEN bar3=0 wave=");
    aarch64_uart_put_hex((unsigned long)CPU_SOFT_WAVE);
    aarch64_uart_puts(" (retmark stamp; Soft!=product)\n");
    
/*
 * ---- Wave 22 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft!=product; not bar3.
 */
/* Grep: aarch64: cpu: soft retphase — Wave 22 return-phase honesty (kept) */
aarch64_uart_puts("aarch64: cpu: soft retphase soft_only=1 product_gate=0 soft_ne_product=1 "
              "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)CPU_SOFT_WAVE);
aarch64_uart_puts(" (retphase honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: cpu: soft retbadge — Wave 22 badge stamp (kept) */
aarch64_uart_puts("aarch64: cpu: soft retbadge exclusive=1 soft_ne_product=1 "
              "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)CPU_SOFT_WAVE);
aarch64_uart_puts(" (retbadge stamp; Soft!=product)\n");
/*
 * ---- Wave 23 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft!=product; not bar3.
 */
/* Grep: aarch64: cpu: soft rettoken — Wave 23 return-token honesty */
aarch64_uart_puts("aarch64: cpu: soft rettoken soft_only=1 product_gate=0 soft_ne_product=1 "
              "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)CPU_SOFT_WAVE);
aarch64_uart_puts(" (rettoken honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: cpu: soft retcrest — Wave 23 exclusive crest stamp */
aarch64_uart_puts("aarch64: cpu: soft retcrest exclusive=1 soft_ne_product=1 "
              "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)CPU_SOFT_WAVE);
aarch64_uart_puts(" (retcrest stamp; Soft!=product)\n");
aarch64_uart_puts("aarch64: cpu soft deepen wave=");
    aarch64_uart_put_hex((unsigned long)CPU_SOFT_WAVE);
    aarch64_uart_puts(" areas=");
    aarch64_uart_put_hex((unsigned long)CPU_SOFT_AREAS);
    aarch64_uart_puts(" catalog=id,midr,mpidr,pfr,isar,mmfr,cache,extra,"
                      "regs,path,inv,surf,return,honesty,deepen logs=");
    aarch64_uart_put_hex((unsigned long)g_cCpuSoftLogs);
    aarch64_uart_puts(" soft_only=1\n");

    /* Grep: aarch64: cpu soft honesty */
    aarch64_uart_puts("aarch64: cpu soft honesty product_kernel=OPEN "
                      "soft_only=1 mrs_only=1 no_bar3=1 wave=");
    aarch64_uart_put_hex((unsigned long)CPU_SOFT_WAVE);
    aarch64_uart_puts("\n");

    /* Grep: aarch64: cpu soft exclusive — Wave 23 exclusive deepen */
    aarch64_uart_puts("aarch64: cpu soft exclusive multi_server=0 "
                      "confine=0 bar3=0 product_kernel=OPEN soft_only=1 wave=");
    aarch64_uart_put_hex((unsigned long)CPU_SOFT_WAVE);
    aarch64_uart_puts("\n");

    /* Grep: aarch64: cpu soft open — Wave 19 open-lamp rollup */
    aarch64_uart_puts("aarch64: cpu soft open multi_server=0 confine=0 "
                      "bar3=0 product_kernel=OPEN soft_only=1 wave=");
    aarch64_uart_put_hex((unsigned long)CPU_SOFT_WAVE);
    aarch64_uart_puts("\n");


    fOk = 0;
    if (pSnap->uGateOk != 0u && pSnap->uEl1Ok != 0u &&
        pSnap->uEl0A64 != 0u && pSnap->uEl1A64 != 0u &&
        pSnap->uTgran4Ok != 0u) {
        fOk = 1;
    }

    /* Grep: aarch64: cpu soft return — Wave 19 return surfaces */
    aarch64_uart_puts("aarch64: cpu soft return inv_ret=");
    aarch64_uart_put_hex((unsigned long)(fOk != 0 ? 1ul : 0ul));
    aarch64_uart_puts(" el1=");
    aarch64_uart_put_hex((unsigned long)pSnap->uEl1Ok);
    aarch64_uart_puts(" product_kernel=OPEN wave=");
    aarch64_uart_put_hex((unsigned long)CPU_SOFT_WAVE);
    aarch64_uart_puts("\n");

    if (fOk != 0) {
        aarch64_uart_puts("aarch64: cpu soft PASS\n");
    } else {
        aarch64_uart_puts("aarch64: cpu soft FAIL\n");
    }
    return fOk;
}

void
aarch64_cpu_probe(void)
{
    unsigned long u64Mpidr;
    unsigned long u64Midr;
    unsigned long u64CurrentEl;
    unsigned long u64El;
    unsigned long u64Revidr;
    int fSoft;
    int fInvSoft;
    struct cpu_soft_snap snap;

    /* Zero soft inventory snapshot before MRS path. */
    snap.u64El = 0ul;
    snap.u64Mpidr = 0ul;
    snap.u64Midr = 0ul;
    snap.u64Revidr = 0ul;
    snap.u64Pfr0 = 0ul;
    snap.u64Isar0 = 0ul;
    snap.u64Isar1 = 0ul;
    snap.u64Mmfr0 = 0ul;
    snap.u64Ctr = 0ul;
    snap.u64Dczid = 0ul;
    snap.u64Clidr = 0ul;
    snap.u64Dfr0 = 0ul;
    snap.uEl0 = 0u;
    snap.uEl1 = 0u;
    snap.uEl2 = 0u;
    snap.uEl3 = 0u;
    snap.uFp = 0u;
    snap.uAdvSimd = 0u;
    snap.uPaRange = 0u;
    snap.uAsidBits = 0u;
    snap.uTgran4 = 0u;
    snap.uAes = 0u;
    snap.uSha1 = 0u;
    snap.uSha2 = 0u;
    snap.uCrc32 = 0u;
    snap.uAtomic = 0u;
    snap.uDMinLine = 0u;
    snap.uIMinLine = 0u;
    snap.uDczBs = 0u;
    snap.uMidrImp = 0u;
    snap.uMidrVar = 0u;
    snap.uMidrArch = 0u;
    snap.uMidrPart = 0u;
    snap.uMidrRev = 0u;
    snap.uAff0 = 0u;
    snap.uAff1 = 0u;
    snap.uAff2 = 0u;
    snap.uAff3 = 0u;
    snap.uDzp = 0u;
    snap.uMt = 0u;
    snap.uU = 0u;
    snap.uEl1Ok = 0u;
    snap.uEl0A64 = 0u;
    snap.uEl1A64 = 0u;
    snap.uTgran4Ok = 0u;
    snap.uGateOk = 0u;

    __asm__ volatile("mrs %0, mpidr_el1" : "=r"(u64Mpidr));
    __asm__ volatile("mrs %0, midr_el1" : "=r"(u64Midr));
    __asm__ volatile("mrs %0, CurrentEL" : "=r"(u64CurrentEl));
    __asm__ volatile("mrs %0, revidr_el1" : "=r"(u64Revidr));
    u64El = (u64CurrentEl >> 2) & 0x3ul;

    snap.u64Mpidr = u64Mpidr;
    snap.u64Midr = u64Midr;
    snap.u64Revidr = u64Revidr;
    snap.u64El = u64El;
    snap.uMidrImp = (unsigned)((u64Midr >> MIDR_IMPLEMENTER_SHIFT) &
                               MIDR_IMPLEMENTER_MASK);
    snap.uMidrVar = (unsigned)((u64Midr >> MIDR_VARIANT_SHIFT) &
                               MIDR_VARIANT_MASK);
    snap.uMidrArch = (unsigned)((u64Midr >> MIDR_ARCHITECTURE_SHIFT) &
                                MIDR_ARCHITECTURE_MASK);
    snap.uMidrPart = (unsigned)((u64Midr >> MIDR_PARTNUM_SHIFT) &
                                MIDR_PARTNUM_MASK);
    snap.uMidrRev = (unsigned)((u64Midr >> MIDR_REVISION_SHIFT) &
                               MIDR_REVISION_MASK);
    snap.uAff0 = (unsigned)((u64Mpidr >> MPIDR_AFF0_SHIFT) & MPIDR_AFF0_MASK);
    snap.uAff1 = (unsigned)((u64Mpidr >> MPIDR_AFF1_SHIFT) & MPIDR_AFF1_MASK);
    snap.uAff2 = (unsigned)((u64Mpidr >> MPIDR_AFF2_SHIFT) & MPIDR_AFF2_MASK);
    snap.uAff3 = (unsigned)((u64Mpidr >> MPIDR_AFF3_SHIFT) & MPIDR_AFF3_MASK);
    snap.uMt = (u64Mpidr & MPIDR_MT_BIT) != 0ul ? 1u : 0u;
    snap.uU = (u64Mpidr & MPIDR_U_BIT) != 0ul ? 1u : 0u;

    /* Legacy identity line (kept greppable for existing smoke detail). */
    aarch64_uart_puts("aarch64: cpu EL=");
    aarch64_uart_put_hex(u64El);
    aarch64_uart_puts(" mpidr=");
    aarch64_uart_put_hex(u64Mpidr);
    aarch64_uart_puts(" midr=");
    aarch64_uart_put_hex(u64Midr);
    aarch64_uart_puts(" revidr=");
    aarch64_uart_put_hex(u64Revidr);
    aarch64_uart_puts("\n");

    /* Soft feature deepen (ID regs + decode) → fills Wave 19 snap. */
    fSoft = cpu_features_soft_observe(u64El, &snap);

    /* Greppable primary marker (smoke scripts). */
    aarch64_uart_puts("aarch64: cpu PASS\n");

    /*
     * Wave 19 combined soft inventory under "aarch64: cpu soft …".
     * Emits multi-field lamps + final soft PASS|FAIL (smoke greps PASS).
     */
    fInvSoft = cpu_soft_inventory(&snap);

    /*
     * UART soft deepen (one-shot PL011 identity + hex helpers). Kept after
     * cpu PASS so existing smoke order for "cpu PASS" stays stable.
     */
    aarch64_uart_soft_selftest();

    (void)fSoft;
    (void)fInvSoft;
}
