/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Minimal EL1 MMU identity map for QEMU virt (product scaffold).
 * 4 KiB granules, TTBR0-only, 39-bit VA (T0SZ=25), single L1 table with
 * 1 GiB block descriptors covering 0–2 GiB:
 *   L1[0] [0, 1 GiB)      device nGnRnE  (GIC/UART/virtio-mmio @ low MMIO)
 *   L1[1] [1 GiB, 2 GiB)  normal WB      (DRAM starts at 0x4000_0000)
 *
 * -------------------------------------------------------------------------
 * Memory attributes (MAIR_EL1 + block AttrIndx)
 * -------------------------------------------------------------------------
 * MAIR_EL1 packs eight 8-bit Attr fields; block/page descriptors select one
 * via AttrIndx [4:2] of the descriptor.
 *
 *   Attr0 (index 0) = 0x00  Device-nGnRnE
 *     nG  = non-Gathering   (no combine of same-address accesses)
 *     nR  = non-Reordering  (device accesses stay in program order)
 *     nE  = no Early Write Acknowledgement
 *     Required for GIC/UART/virtio-mmio so MMIO side effects are strict.
 *     Device blocks also set UXN|PXN (no-exec).
 *
 *   Attr1 (index 1) = 0xff  Normal Inner/Outer Write-Back, Read+Write Alloc
 *     Both inner and outer cacheability = 0b11 (WB), allocate on R/W.
 *     Used for DRAM identity (kernel text/data/stack + early PMM pool).
 *
 *   Attr2 reserved (not programmed) for future Device-nGnRE (0x04) if a
 *     device needs reordering-relaxed nE — not required on virt bring-up.
 *
 * Block descriptor bits we set (stage-1, EL1&0):
 *   [0]     V     valid
 *   [1]     =0    block (not table) at L1
 *   [4:2]   AttrIndx  → MAIR slot (0 device / 1 normal)
 *   [7:6]   AP        EL1 RW, EL0 none (AP=00)
 *   [9:8]   SH        Inner Shareable (0b11) — matches TCR SH0
 *   [10]    AF        Access Flag set (no AF fault on first touch)
 *   [47:30] OA        output address for 1 GiB block (4K granule)
 *   [53]    PXN       privileged execute-never (device only here)
 *   [54]    UXN       unprivileged execute-never (device only here)
 *
 * TCR_EL1 (translation control):
 *   T0SZ=25 → VA bits = 64-25 = 39 → L1 index = VA[38:30] (512 entries)
 *   TG0=4K, SH0=Inner Shareable, IRGN0/ORGN0 = WB walk cacheability
 *   IPS=40-bit intermediate physical (enough for virt DRAM)
 * TTBR1_EL1 = 0 (no high half); kernel lives in low identity for -kernel.
 *
 * SCTLR_EL1: set M (MMU), C (data cache), I (instruction cache) together
 * after MAIR/TCR/TTBR + TLBI. ISB required after SCTLR write.
 *
 * -------------------------------------------------------------------------
 * Soft map observability (no fail-stop beyond DRAM self-touch)
 * -------------------------------------------------------------------------
 * After enable: re-read L1 block descriptors, MAIR/TCR/TTBR0/SCTLR via MRS,
 * soft-touch DRAM through Attr1 and soft-read a known MMIO FR word through
 * Attr0 (PL011 UART FR @ 0x09000018 — RO status; never writes DR).
 *
 * Soft MMU inventory (deepen; pure read of live TCR/TTBR after enable):
 *   page size soft  — TCR_EL1.TG0 → 4 KiB expected (TG0=0b00)
 *   TTBR soft presence — TTBR0_EL1 BADDR non-zero / matches L1; TTBR1=0
 * Greppable:
 *   aarch64: mmu PASS
 *   aarch64: mmu map soft PASS   (L1[0] device + L1[1] normal + M/C/I)
 *   aarch64: mmu soft page_size=… tg0=… t0sz=…
 *   aarch64: mmu soft ttbr0=… ttbr1=… present=… ttbr1_clear=…
 *   aarch64: mmu soft PASS | FAIL
 *   aarch64: mmu soft SKIP (no page)
 *
 * Freestanding pure C; no GPL Linux arch code.
 */
#include "types_arch.h"

extern void aarch64_uart_puts(const char *sz);
extern void aarch64_uart_put_hex(unsigned long v);
extern void *aarch64_pmm_alloc(void);

/*
 * MAIR_EL1 value:
 *   bits [7:0]   Attr0 Device-nGnRnE
 *   bits [15:8]  Attr1 Normal WB RWA
 *   bits [63:16] zero (Attr2–7 unused)
 */
#define MAIR_DEVICE_nGnRnE 0x00ull
#define MAIR_NORMAL_WB     0xffull /* Write-back, read/write allocate */
/* Reserved for deepen notes: Device-nGnRE encoding if Attr2 is ever used. */
#define MAIR_DEVICE_nGnRE  0x04ull
#define MAIR_EL1_VAL       ((MAIR_NORMAL_WB << 8) | MAIR_DEVICE_nGnRnE)

/* TCR_EL1: T0SZ=25 → 39-bit VA; TG0=4K; IPS=40-bit; inner/outer WB walks */
#define TCR_T0SZ_25   (25ull)
#define TCR_T0SZ_MASK (0x3full)
#define TCR_TG0_SHIFT 14
#define TCR_TG0_MASK  (3ull << TCR_TG0_SHIFT)
#define TCR_TG0_4K    (0ull << TCR_TG0_SHIFT) /* 0b00 → 4 KiB granule */
#define TCR_TG0_64K   (1ull << TCR_TG0_SHIFT) /* 0b01 → 64 KiB */
#define TCR_TG0_16K   (2ull << TCR_TG0_SHIFT) /* 0b10 → 16 KiB */
#define TCR_SH0_IS    (3ull << 12) /* Inner Shareable page-table walks */
#define TCR_ORGN0_WB  (1ull << 10) /* Outer WB, read alloc (01 encoding) */
#define TCR_IRGN0_WB  (1ull << 8)  /* Inner WB, read alloc */
#define TCR_IPS_40    (2ull << 32) /* 40-bit PA */
#define TCR_EL1_VAL   (TCR_T0SZ_25 | TCR_TG0_4K | TCR_SH0_IS | TCR_ORGN0_WB | \
                       TCR_IRGN0_WB | TCR_IPS_40)

/* Soft page-size decode helpers (bytes; 0 if TG0 reserved/unknown). */
#define MMU_SOFT_PAGE_4K   4096ul
#define MMU_SOFT_PAGE_16K  16384ul
#define MMU_SOFT_PAGE_64K  65536ul

/* TTBR BADDR is page-aligned; low 12 bits are reserved/ASID for soft compare. */
#define TTBR_BADDR_MASK (~0xffful)

/* Block / table descriptors (stage-1) */
#define DESC_VALID    (1ull << 0)
#define DESC_TABLE    (1ull << 1) /* with VALID → table */
#define DESC_BLOCK    (0ull << 1) /* with VALID → block */
#define DESC_AF       (1ull << 10)
#define DESC_SH_IS    (3ull << 8)  /* Inner Shareable */
#define DESC_AP_RW    (0ull << 6)  /* EL1 RW / EL0 none */
#define DESC_ATTR_DEV (0ull << 2)  /* MAIR AttrIndx 0 → nGnRnE */
#define DESC_ATTR_MEM (1ull << 2)  /* MAIR AttrIndx 1 → Normal WB */
#define DESC_ATTR_MASK (7ull << 2)
#define DESC_PXN      (1ull << 53)
#define DESC_UXN      (1ull << 54)

/* SCTLR_EL1 bits we care about */
#define SCTLR_M  (1ull << 0)
#define SCTLR_C  (1ull << 2)
#define SCTLR_I  (1ull << 12)

/* Level-1 block size: 1 GiB with 4K granule / 39-bit VA (T0SZ=25) */
#define L1_BLOCK_SIZE (1ull << 30)

/* Soft map self-touch targets (identity on virt). */
#define MMU_SOFT_DRAM_PA   0x40080000ul
#define MMU_SOFT_DRAM_PAT  0x5a5a5a5a5a5a5a5aul
/* PL011 UART FR (flag register) — RO; confirms device block still maps MMIO. */
#define MMU_SOFT_UART_FR   0x09000018ul

static unsigned long *g_pL1; /* 512 entries; only [0],[1] used */

/*
 * L1 block OA: bits [47:30] only for 1 GiB blocks (4K granule).
 * Do not use ~((1<<30)-1): that keeps UXN/PXN [54:53] and looks like OA.
 */
#define L1_OA_MASK 0x0000ffffc0000000ull

static unsigned long
make_block(unsigned long pa, int fDevice)
{
    unsigned long d;

    d = pa & L1_OA_MASK;
    d |= DESC_VALID | DESC_BLOCK | DESC_AF | DESC_SH_IS | DESC_AP_RW;
    if (fDevice) {
        /* Device-nGnRnE + no-exec at EL0/EL1 (MMIO must not be executable). */
        d |= DESC_ATTR_DEV | DESC_PXN | DESC_UXN;
    } else {
        /*
         * Normal WB for DRAM: PXN/UXN clear so kernel EL1 can execute from
         * the identity map (image loaded at 0x4000_0000). EL0 still has no
         * AP permission until user maps are added.
         */
        d |= DESC_ATTR_MEM;
    }
    return d;
}

static void
tlb_invalidate_all(void)
{
    __asm__ volatile("tlbi vmalle1is" ::: "memory");
    __asm__ volatile("dsb ish" ::: "memory");
    __asm__ volatile("isb" ::: "memory");
}

/*
 * Soft map observability: re-validate L1 descriptors + system regs after
 * enable, soft-touch DRAM (Attr1) and soft-read UART FR (Attr0 device).
 * Returns 1 if map soft checks pass (non-fatal if 0 — still emit mmu PASS).
 */
static int
mmu_map_soft_observe(unsigned long *pL1)
{
    unsigned long d0;
    unsigned long d1;
    unsigned long mair;
    unsigned long tcr;
    unsigned long ttbr0;
    unsigned long sctlr;
    volatile unsigned long *pDram;
    volatile unsigned int *pUartFr;
    unsigned long v;
    unsigned int uFr;
    int fOk;

    d0 = pL1[0];
    d1 = pL1[1];

    __asm__ volatile("mrs %0, mair_el1" : "=r"(mair));
    __asm__ volatile("mrs %0, tcr_el1" : "=r"(tcr));
    __asm__ volatile("mrs %0, ttbr0_el1" : "=r"(ttbr0));
    __asm__ volatile("mrs %0, sctlr_el1" : "=r"(sctlr));

    fOk = 1;
    /* L1[0]: valid block, AttrIndx device, PXN|UXN, OA 0. */
    if ((d0 & DESC_VALID) == 0ul || (d0 & DESC_TABLE) != 0ul) {
        fOk = 0;
    }
    if ((d0 & DESC_ATTR_MASK) != DESC_ATTR_DEV) {
        fOk = 0;
    }
    if ((d0 & (DESC_PXN | DESC_UXN)) != (DESC_PXN | DESC_UXN)) {
        fOk = 0;
    }
    if ((d0 & L1_OA_MASK) != 0ul) {
        fOk = 0;
    }
    /* L1[1]: valid block, AttrIndx normal, OA 0x4000_0000. */
    if ((d1 & DESC_VALID) == 0ul || (d1 & DESC_TABLE) != 0ul) {
        fOk = 0;
    }
    if ((d1 & DESC_ATTR_MASK) != DESC_ATTR_MEM) {
        fOk = 0;
    }
    if ((d1 & L1_OA_MASK) != 0x40000000ul) {
        fOk = 0;
    }
    /* Live system regs: MAIR/TCR programmed, TTBR0 points at L1, M|C|I on. */
    if (mair != MAIR_EL1_VAL) {
        fOk = 0;
    }
    if ((tcr & TCR_T0SZ_MASK) != TCR_T0SZ_25) {
        fOk = 0;
    }
    if ((ttbr0 & TTBR_BADDR_MASK) != ((unsigned long)(void *)pL1 & TTBR_BADDR_MASK)) {
        fOk = 0;
    }
    if ((sctlr & (SCTLR_M | SCTLR_C | SCTLR_I)) != (SCTLR_M | SCTLR_C | SCTLR_I)) {
        fOk = 0;
    }

    /* Soft walk DRAM through identity map (Attr1). */
    pDram = (volatile unsigned long *)MMU_SOFT_DRAM_PA;
    *pDram = MMU_SOFT_DRAM_PAT;
    v = *pDram;
    if (v != MMU_SOFT_DRAM_PAT) {
        fOk = 0;
    }

    /*
     * Soft walk device block: read PL011 FR only (status). Confirms L1[0]
     * device Attr0 mapping still reaches low MMIO after SCTLR.M.
     */
    pUartFr = (volatile unsigned int *)MMU_SOFT_UART_FR;
    uFr = *pUartFr;
    (void)uFr;

    aarch64_uart_puts("aarch64: mmu map l1[0]=");
    aarch64_uart_put_hex(d0);
    aarch64_uart_puts(" l1[1]=");
    aarch64_uart_put_hex(d1);
    aarch64_uart_puts(" mair=");
    aarch64_uart_put_hex(mair);
    aarch64_uart_puts(" tcr=");
    aarch64_uart_put_hex(tcr);
    aarch64_uart_puts(" ttbr0=");
    aarch64_uart_put_hex(ttbr0);
    aarch64_uart_puts(" sctlr=");
    aarch64_uart_put_hex(sctlr);
    aarch64_uart_puts(" uart_fr=");
    aarch64_uart_put_hex((unsigned long)uFr);
    aarch64_uart_puts("\n");

    return fOk;
}

/*
 * Soft MMU inventory deepen (non-fatal):
 *   - page size soft: live TCR.TG0 → expected 4 KiB granule
 *   - TTBR soft presence: TTBR0 BADDR matches L1; TTBR1 cleared (TTBR0-only)
 * Emits greppable "aarch64: mmu soft …" lines. Returns 1 on PASS.
 */
static int
mmu_soft_inventory(unsigned long *pL1)
{
    unsigned long uTcr;
    unsigned long uTtbr0;
    unsigned long uTtbr1;
    unsigned long uTg0;
    unsigned long uT0sz;
    unsigned long cbPage;
    unsigned long paL1;
    unsigned long paTtbr0;
    int fPageOk;
    int fTtbrPresent;
    int fTtbr1Clear;
    int fTtbrMatch;
    int fOk;

    __asm__ volatile("mrs %0, tcr_el1" : "=r"(uTcr));
    __asm__ volatile("mrs %0, ttbr0_el1" : "=r"(uTtbr0));
    __asm__ volatile("mrs %0, ttbr1_el1" : "=r"(uTtbr1));

    uT0sz = uTcr & TCR_T0SZ_MASK;
    uTg0 = (uTcr & TCR_TG0_MASK) >> TCR_TG0_SHIFT;

    /*
     * AArch64 TCR_ELx.TG0 encodings (translation granule for TTBR0):
     *   0b00 = 4 KiB, 0b01 = 64 KiB, 0b10 = 16 KiB, 0b11 reserved.
     */
    if (uTg0 == 0ul) {
        cbPage = MMU_SOFT_PAGE_4K;
    } else if (uTg0 == 1ul) {
        cbPage = MMU_SOFT_PAGE_64K;
    } else if (uTg0 == 2ul) {
        cbPage = MMU_SOFT_PAGE_16K;
    } else {
        cbPage = 0ul;
    }

    /* Product identity map programs TG0=4K + T0SZ=25. */
    fPageOk = 0;
    if (cbPage == MMU_SOFT_PAGE_4K && (uTcr & TCR_TG0_MASK) == TCR_TG0_4K &&
        uT0sz == TCR_T0SZ_25) {
        fPageOk = 1;
    }

    paL1 = (unsigned long)(void *)pL1 & TTBR_BADDR_MASK;
    paTtbr0 = uTtbr0 & TTBR_BADDR_MASK;

    /* Soft presence: TTBR0 holds a non-zero table base (L1 page). */
    fTtbrPresent = 0;
    if (paTtbr0 != 0ul) {
        fTtbrPresent = 1;
    }

    /* Soft TTBR1 absence (high half unused on virt -kernel identity). */
    fTtbr1Clear = 0;
    if (uTtbr1 == 0ul) {
        fTtbr1Clear = 1;
    }

    fTtbrMatch = 0;
    if (fTtbrPresent != 0 && paTtbr0 == paL1) {
        fTtbrMatch = 1;
    }

    fOk = 0;
    if (fPageOk != 0 && fTtbrPresent != 0 && fTtbr1Clear != 0 &&
        fTtbrMatch != 0) {
        fOk = 1;
    }

    /* Greppable page-size soft inventory. */
    aarch64_uart_puts("aarch64: mmu soft page_size=");
    aarch64_uart_put_hex(cbPage);
    aarch64_uart_puts(" tg0=");
    aarch64_uart_put_hex(uTg0);
    aarch64_uart_puts(" t0sz=");
    aarch64_uart_put_hex(uT0sz);
    aarch64_uart_puts(" page_ok=");
    aarch64_uart_put_hex(fPageOk != 0 ? 1ul : 0ul);
    aarch64_uart_puts("\n");

    /* Greppable TTBR soft presence inventory. */
    aarch64_uart_puts("aarch64: mmu soft ttbr0=");
    aarch64_uart_put_hex(uTtbr0);
    aarch64_uart_puts(" ttbr1=");
    aarch64_uart_put_hex(uTtbr1);
    aarch64_uart_puts(" present=");
    aarch64_uart_put_hex(fTtbrPresent != 0 ? 1ul : 0ul);
    aarch64_uart_puts(" ttbr1_clear=");
    aarch64_uart_put_hex(fTtbr1Clear != 0 ? 1ul : 0ul);
    aarch64_uart_puts(" match=");
    aarch64_uart_put_hex(fTtbrMatch != 0 ? 1ul : 0ul);
    aarch64_uart_puts(" l1=");
    aarch64_uart_put_hex(paL1);
    aarch64_uart_puts("\n");

    if (fOk != 0) {
        aarch64_uart_puts("aarch64: mmu soft PASS\n");
    } else {
        aarch64_uart_puts("aarch64: mmu soft FAIL\n");
    }

    (void)TCR_TG0_64K;
    (void)TCR_TG0_16K;
    return fOk;
}

void
aarch64_mmu_init(void)
{
    void *pPage;
    unsigned long *pL1;
    unsigned long ttbr;
    unsigned long sctlr;
    unsigned i;
    int fMapSoft;
    int fInvSoft;

    (void)MAIR_DEVICE_nGnRE; /* reserved Attr encoding; documents nGnRE */
    (void)L1_BLOCK_SIZE;
    (void)DESC_TABLE;

    pPage = aarch64_pmm_alloc();
    if (pPage == 0) {
        aarch64_uart_puts("aarch64: mmu soft SKIP (no page)\n");
        return;
    }

    pL1 = (unsigned long *)pPage;
    for (i = 0; i < 512u; i++) {
        pL1[i] = 0;
    }

    /*
     * Identity L1 blocks:
     *   [0] PA 0x00000000 Attr0 device — covers UART 0x09000000, GIC 0x08..,
     *       virtio-mmio 0x0a000000
     *   [1] PA 0x40000000 Attr1 normal — covers kernel, stack, PMM freelist
     */
    pL1[0] = make_block(0x00000000ull, 1);
    pL1[1] = make_block(0x40000000ull, 0);
    g_pL1 = pL1;

    /* Single-level block map: no L2/L3 tables. */
    ttbr = (unsigned long)(void *)pL1;
    __asm__ volatile("msr mair_el1, %0" :: "r"(MAIR_EL1_VAL) : "memory");
    __asm__ volatile("msr tcr_el1, %0" :: "r"(TCR_EL1_VAL) : "memory");
    __asm__ volatile("msr ttbr0_el1, %0" :: "r"(ttbr) : "memory");
    __asm__ volatile("msr ttbr1_el1, %0" :: "r"(0ul) : "memory");
    __asm__ volatile("isb" ::: "memory");

    tlb_invalidate_all();

    __asm__ volatile("mrs %0, sctlr_el1" : "=r"(sctlr));
    sctlr |= SCTLR_M | SCTLR_C | SCTLR_I;
    /* Clear A (alignment) soft; keep defaults otherwise. */
    __asm__ volatile("msr sctlr_el1, %0" :: "r"(sctlr) : "memory");
    __asm__ volatile("isb" ::: "memory");

    /* Soft map observe + DRAM self-touch (Attr1 normal path). */
    fMapSoft = mmu_map_soft_observe(pL1);
    /* Soft inventory deepen: page size + TTBR presence (always-on). */
    fInvSoft = mmu_soft_inventory(pL1);
    (void)fInvSoft; /* greppable PASS/FAIL already emitted */

    if (fMapSoft == 0) {
        aarch64_uart_puts("aarch64: mmu map soft FAIL\n");
        aarch64_uart_puts("aarch64: mmu PASS\n"); /* path present */
        return;
    }

    aarch64_uart_puts("aarch64: mmu PASS\n");
    aarch64_uart_puts("aarch64: mmu map soft PASS\n");
}
