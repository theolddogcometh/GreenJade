/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Early CPU identity + feature soft probe for aarch64 product scaffold
 * (QEMU virt). Pure freestanding C; MRS system registers only.
 *
 * -------------------------------------------------------------------------
 * Soft feature path (no MMU/GIC/timer ownership)
 * -------------------------------------------------------------------------
 * Soft-read ID_AA64* / CTR / DCZID and emit greppable feature markers:
 *   EL field from CurrentEL, EL3 implement from PFR0, ASID/granule from
 *   MMFR0, AES/SHA/CRC/atomics from ISAR0, I/D line sizes from CTR_EL0,
 *   DC ZVA block from DCZID_EL0. Non-fatal; product path still prints
 *   aarch64: cpu PASS whenever MRS is reachable.
 *
 * Also invokes aarch64_uart_soft_selftest (uart_pl011.c) so PL011 soft
 * markers appear once during early bring-up without a kmain change.
 *
 * Greppable:
 *   aarch64: cpu EL=… mpidr=… midr=…
 *   aarch64: cpu features soft pfr0=… isar0=… mmfr0=… ctr=… dczid=…
 *   aarch64: cpu soft PASS | FAIL
 *   aarch64: cpu PASS
 *   (+ uart soft markers from aarch64_uart_soft_selftest)
 *
 * Freestanding pure C; no GPL Linux cpu_info paste.
 */
#include "types_arch.h"

extern void aarch64_uart_puts(const char *sz);
extern void aarch64_uart_put_hex(unsigned long v);
extern void aarch64_uart_put_hex_n(unsigned long v, unsigned cNibbles);
extern void aarch64_uart_soft_selftest(void);

/* ID_AA64PFR0_EL1 field helpers (architecture reference manual). */
#define PFR0_EL0_SHIFT  0u
#define PFR0_EL1_SHIFT  4u
#define PFR0_EL2_SHIFT  8u
#define PFR0_EL3_SHIFT  12u
#define PFR0_EL_MASK    0xful
#define PFR0_FP_SHIFT   16u
#define PFR0_ADVSIMD_SHIFT 20u

/* ID_AA64MMFR0_EL1 — soft granule / PARange bits. */
#define MMFR0_PARANGE_SHIFT 0u
#define MMFR0_PARANGE_MASK  0xful
#define MMFR0_ASIDBITS_SHIFT 4u
#define MMFR0_ASIDBITS_MASK  0xful
#define MMFR0_TGRAN4_SHIFT  28u
#define MMFR0_TGRAN4_MASK   0xful

/* ID_AA64ISAR0_EL1 — soft crypto / atomic feature nibbles. */
#define ISAR0_AES_SHIFT     4u
#define ISAR0_SHA1_SHIFT    8u
#define ISAR0_SHA2_SHIFT    12u
#define ISAR0_CRC32_SHIFT   16u
#define ISAR0_ATOMIC_SHIFT  20u
#define ISAR0_FIELD_MASK    0xful

/* CTR_EL0 — cache type soft decode. */
#define CTR_DMINLINE_SHIFT 16u
#define CTR_DMINLINE_MASK  0xful
#define CTR_IMINLINE_SHIFT 0u
#define CTR_IMINLINE_MASK  0xful

/* DCZID_EL0 — DC ZVA block soft. */
#define DCZID_BS_MASK  0xful
#define DCZID_DZP_BIT  (1ul << 4)

/*
 * Soft feature observe: read ID registers, decode a few product-relevant
 * fields, print detail line. Returns 1 if EL1 and EL0 AArch64 are present
 * in PFR0 (QEMU virt shape) and CurrentEL is EL1.
 */
static int
cpu_features_soft_observe(unsigned long u64El,
                          unsigned long *pPfr0,
                          unsigned long *pIsar0,
                          unsigned long *pMmfr0,
                          unsigned long *pCtr,
                          unsigned long *pDczid)
{
    unsigned long u64Pfr0;
    unsigned long u64Isar0;
    unsigned long u64Mmfr0;
    unsigned long u64Ctr;
    unsigned long u64Dczid;
    unsigned uEl0;
    unsigned uEl1;
    unsigned uEl2;
    unsigned uEl3;
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
    __asm__ volatile("mrs %0, id_aa64mmfr0_el1" : "=r"(u64Mmfr0));
    __asm__ volatile("mrs %0, ctr_el0" : "=r"(u64Ctr));
    __asm__ volatile("mrs %0, dczid_el0" : "=r"(u64Dczid));

    if (pPfr0 != 0) {
        *pPfr0 = u64Pfr0;
    }
    if (pIsar0 != 0) {
        *pIsar0 = u64Isar0;
    }
    if (pMmfr0 != 0) {
        *pMmfr0 = u64Mmfr0;
    }
    if (pCtr != 0) {
        *pCtr = u64Ctr;
    }
    if (pDczid != 0) {
        *pDczid = u64Dczid;
    }

    uEl0 = (unsigned)((u64Pfr0 >> PFR0_EL0_SHIFT) & PFR0_EL_MASK);
    uEl1 = (unsigned)((u64Pfr0 >> PFR0_EL1_SHIFT) & PFR0_EL_MASK);
    uEl2 = (unsigned)((u64Pfr0 >> PFR0_EL2_SHIFT) & PFR0_EL_MASK);
    uEl3 = (unsigned)((u64Pfr0 >> PFR0_EL3_SHIFT) & PFR0_EL_MASK);

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

    (void)PFR0_FP_SHIFT;
    (void)PFR0_ADVSIMD_SHIFT;
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
    unsigned long u64Pfr0;
    unsigned long u64Isar0;
    unsigned long u64Mmfr0;
    unsigned long u64Ctr;
    unsigned long u64Dczid;
    int fSoft;

    __asm__ volatile("mrs %0, mpidr_el1" : "=r"(u64Mpidr));
    __asm__ volatile("mrs %0, midr_el1" : "=r"(u64Midr));
    __asm__ volatile("mrs %0, CurrentEL" : "=r"(u64CurrentEl));
    __asm__ volatile("mrs %0, revidr_el1" : "=r"(u64Revidr));
    u64El = (u64CurrentEl >> 2) & 0x3ul;

    aarch64_uart_puts("aarch64: cpu EL=");
    aarch64_uart_put_hex(u64El);
    aarch64_uart_puts(" mpidr=");
    aarch64_uart_put_hex(u64Mpidr);
    aarch64_uart_puts(" midr=");
    aarch64_uart_put_hex(u64Midr);
    aarch64_uart_puts(" revidr=");
    aarch64_uart_put_hex(u64Revidr);
    aarch64_uart_puts("\n");

    /* Soft feature deepen (ID regs + decode). */
    u64Pfr0 = 0ul;
    u64Isar0 = 0ul;
    u64Mmfr0 = 0ul;
    u64Ctr = 0ul;
    u64Dczid = 0ul;
    fSoft = cpu_features_soft_observe(u64El, &u64Pfr0, &u64Isar0, &u64Mmfr0,
                                      &u64Ctr, &u64Dczid);

    /* Greppable primary marker (smoke scripts). */
    aarch64_uart_puts("aarch64: cpu PASS\n");
    if (fSoft != 0) {
        aarch64_uart_puts("aarch64: cpu soft PASS\n");
    } else {
        aarch64_uart_puts("aarch64: cpu soft FAIL\n");
    }

    /*
     * UART soft deepen (one-shot PL011 identity + hex helpers). Kept after
     * cpu PASS so existing smoke order for "cpu PASS" stays stable.
     */
    aarch64_uart_soft_selftest();

    (void)u64Pfr0;
    (void)u64Isar0;
    (void)u64Mmfr0;
    (void)u64Ctr;
    (void)u64Dczid;
}
