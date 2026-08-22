/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * MMIO map + accessors. Host: window table from inject.
 * Freestanding: same API; ioremap uses static iomem pool + host window
 * table when a map grant is registered (no libc heap required).
 *
 * DDI bind path: after udx_host_bind_by_id / install_granted_pci, preferred
 * BAR phys is window-registered with the granted VA -- call
 * udx_ioremap(udx_pci_resource_start(pdev, bar), len) as usual.
 *
 * Soft API honesty (Soft!=product; dual MIT OR Apache-2.0; G-AC-1):
 *   soft path = window lookup + freestanding iomem pool + header accessors
 *   product   = MMIO_FRAME cap mint (OPEN; UDX_MMIO_SOFT_CAP_MINT=0)
 *   soft_ne_product=1  dual_dod agent_close=0  no .ko product AC
 *
 * Soft residual deepen (UDX product hosts; Soft!=product; G-AC-1; C2):
 *   ioremap residual  -- udx_ioremap via host_window_lookup
 *                         (granted PA -> VA); != product MMIO_FRAME mint
 *   iounmap residual  -- udx_iounmap clear + free / pool return;
 *                         != product Phase-A revoke / cap destroy
 *   readl / writel    -- range+natural-align volatile accessors
 *                         (header inlines); soft residual surface for
 *                         product hosts (rtl8168_udx / xhci_udx); not
 *                         product CAP program path
 *   write*_flush      -- write then CPU fence (posted-write intent);
 *                         product-host ISR/mask residual pattern
 *   posted_flush      -- fence + optional safe-reg readl drain intent
 *   query residual    -- is_mapped / len / phys / va observation helpers
 *   UC map honesty    -- soft VA expects host UC leaf attrs from
 *                         vmm_map_user_device / vmm_map_device_uc
 *                         (PCD|PWT intent); not WB identity rewrite;
 *                         UDX does not re-attr pages here (udx_reattr=0)
 *   freestanding thrash refuse -- no freestanding class re-enable /
 *                         pool expand / pool thrash from this unit;
 *                         product hosts = Linux-shaped userspace UDX
 *   map_chain residual -- C2 product path catalog only:
 *                         MAP_BAR grant -> window_register -> ioremap
 *                         lookup (soft window only; cap_mint=0);
 *                         Dual DoD A/B OPEN; agent!=close
 *   dual_dod residual -- DoD A (xhci_udx) / DoD B (rtl8168_udx) OPEN
 *                         constants + helpers; lamps never close DoD
 *   mint residual     -- MMIO_FRAME cap_mint honesty (always 0 today);
 *                         multi_server=0; confine=0; Soft!=product
 *
 * Product-host shape residual (Dual DoD A/B OPEN seed; Soft!=product):
 *   rtl8168_udx (DoD B): pref BAR0+BAR2 map + readl/w/b + writew +
 *                         mmio_flush / write*_flush residual
 *   xhci_udx    (DoD A): pref BAR0 cap map + readl/w/b residual
 *   both: map_chain MAP_BAR->window_register->ioremap;
 *         MMIO_FRAME mint OPEN; Dual DoD OPEN; agent_close=0
 *
 * greppable: udx: mmio soft residual lean
 * greppable: udx: mmio soft residual lean layout_ver=
 * greppable: udx: mmio soft residual lean UDX eng
 * greppable: udx: mmio soft api honesty
 * greppable: udx: mmio soft residual ioremap
 * greppable: udx: mmio soft residual iounmap
 * greppable: udx: mmio soft residual accessors
 * greppable: udx: mmio soft residual uc_map
 * greppable: udx: mmio soft residual freestanding thrash refuse
 * greppable: udx: mmio soft residual map_chain
 * greppable: udx: mmio soft residual product_host
 * greppable: udx: mmio soft residual query
 * greppable: udx: mmio soft residual write_flush
 * greppable: udx: mmio soft residual dual_dod
 * greppable: udx: mmio soft residual mint
 * greppable: udx: mmio soft product host
 * No stamp storms. No version stamp. No GJ_IMAGE_VERSION. Lean residual only.
 * Dual MIT OR Apache-2.0. Soft!=product. G-AC-1 (no .ko product AC).
 * Dual DoD A/B OPEN. No GPL.
 */
#pragma once

#include <udx/types.h>

/*
 * Soft residual lean layout version (eng only; Soft!=host ABI; not stamp).
 * Bump when product-host MMIO surface grows (map/unmap/accessors/query/
 * write_flush/posted_flush/map_chain/dual_dod/mint). Not GJ_IMAGE_VERSION.
 * Not product ABI.
 * ver1: ioremap/iounmap + range+align accessors + bulk + flush +
 *       write*_flush + posted_flush + query helpers + product_host
 *       Dual DoD OPEN catalog + freestanding thrash refuse honesty.
 * ver2: map_chain residual (MAP_BAR->window_register->ioremap C2 path);
 *       Dual DoD A/B OPEN honesty constants + helpers; MMIO_FRAME
 *       cap_mint / multi_server / confine honesty (always 0 mint);
 *       product-host preferred BAR catalog + soft path-bit catalog;
 *       residual lean UDX eng + mint greppables; Soft!=product deepen.
 * Grep: udx: mmio soft residual lean layout_ver=
 */
#define UDX_MMIO_LAYOUT_VER          2u

/*
 * Soft freestanding iomem pool residual ceiling (matches UDX_FS_IOMEM_SLOTS
 * eng). Fixed pool only -- thrash refuse; no expand from this header.
 * Soft!=product quota.
 */
#define UDX_MMIO_FS_IOMEM_SLOTS      16u

/*
 * Soft residual Cap / Dual DoD honesty constants (eng only; Soft!=product).
 * Always-0 mint/server/confine flags match host soft residual lean spirit.
 * Dual DoD A/B remain OPEN (1) until USB path / interactive SSH login — header lamps
 * and soft inventory never flip these to closed. No version stamp. No GPL.
 * G-AC-1 (no .ko product AC). agent_close=0.
 * greppable: udx: mmio soft residual mint
 * greppable: udx: mmio soft residual dual_dod
 */
#define UDX_MMIO_SOFT_CAP_MINT       0u /* no MMIO_FRAME CNode mint today */
#define UDX_MMIO_SOFT_MULTI_SERVER   0u /* soft residual; not multi-server */
#define UDX_MMIO_SOFT_CONFINE        0u /* soft residual; not product confine */
#define UDX_MMIO_SOFT_DOD_A_OPEN     1u /* xhci_udx Dual DoD A still OPEN */
#define UDX_MMIO_SOFT_DOD_B_OPEN     1u /* rtl8168_udx Dual DoD B still OPEN */
#define UDX_MMIO_SOFT_AGENT_CLOSE    0u /* residual lamps never close Dual DoD */
#define UDX_MMIO_SOFT_UC_INTENT      1u /* soft VA expects host UC leaf attrs */
#define UDX_MMIO_SOFT_REATTR         0u /* UDX does not re-attr pages here */
#define UDX_MMIO_SOFT_NE_PRODUCT     1u /* Soft!=product honesty bit */

/*
 * Soft residual path-bit catalog (C2 observation; Soft!=product).
 * Hosts / eng may OR these into private soft path masks; UDX never
 * hard-gates Dual DoD from these bits.
 * greppable: udx: mmio soft residual lean
 */
#define UDX_MMIO_SOFT_PATH_IOREMAP         (1u << 0)
#define UDX_MMIO_SOFT_PATH_IOUNMAP         (1u << 1)
#define UDX_MMIO_SOFT_PATH_ACCESSORS       (1u << 2)
#define UDX_MMIO_SOFT_PATH_UC_MAP          (1u << 3)
#define UDX_MMIO_SOFT_PATH_THRASH_REFUSE   (1u << 4)
#define UDX_MMIO_SOFT_PATH_MAP_CHAIN       (1u << 5)
#define UDX_MMIO_SOFT_PATH_WRITE_FLUSH     (1u << 6)
#define UDX_MMIO_SOFT_PATH_QUERY           (1u << 7)
#define UDX_MMIO_SOFT_PATH_PRODUCT_HOST    (1u << 8)
#define UDX_MMIO_SOFT_PATH_DUAL_DOD        (1u << 9)
#define UDX_MMIO_SOFT_PATH_MINT            (1u << 10)

/*
 * G752 / product-host preferred BAR residual for map_chain (Soft!=product).
 * Matches pci.h / host bind preferred-BAR spirit (local catalog only):
 *   rtl8168_udx (DoD B): BAR0 (DDI grant) + BAR2 (silicon-common MMIO)
 *   xhci_udx    (DoD A): BAR0 (HC MMIO)
 * greppable: udx: mmio soft residual map_chain
 * greppable: udx: mmio soft residual product_host
 */
#define UDX_MMIO_RTL_PREF_BAR0       0
#define UDX_MMIO_RTL_PREF_BAR2       2
#define UDX_MMIO_XHCI_PREF_BAR0      0
#define UDX_MMIO_RTL_PREF_BAR_MASK   ((1u << UDX_MMIO_RTL_PREF_BAR0) | \
                                      (1u << UDX_MMIO_RTL_PREF_BAR2))
#define UDX_MMIO_XHCI_PREF_BAR_MASK  (1u << UDX_MMIO_XHCI_PREF_BAR0)

/* Opaque MMIO mapping (≈ ioremap result). Soft residual cookie only. */
struct udx_iomem {
    volatile void *pVa;   /* host UC-intent VA (window grant); soft only */
    u64            u64Len;
    u64            u64Phys; /* optional bookkeeping (BAR / grant PA) */
};

/*
 * Map BAR / granted MMIO into host process.
 * Soft residual: resolves udx_host_window_lookup only (UC VA already
 * granted by DDI MAP_BAR / inject). Product MMIO_FRAME cap mint OPEN
 * (UDX_MMIO_SOFT_CAP_MINT=0). C2 map_chain residual:
 *   MAP_BAR grant -> window_register -> this ioremap lookup.
 * Dual DoD A/B OPEN; agent_close=0; Soft!=product.
 * Full product: uses process MMIO/FRAME caps from devmgr (not this path).
 * greppable residual (impl): udx: mmio soft residual ioremap
 * greppable residual: udx: mmio soft residual map_chain
 * greppable residual: udx: mmio soft residual mint
 */
struct udx_iomem *udx_ioremap(u64 u64Phys, u64 u64Len);

/*
 * Unmap soft residual iomem bookkeeping.
 * Soft residual: clear fields then host free / freestanding pool return.
 * != product Phase-A revoke / MMIO_FRAME cap destroy (cap_mint stays 0).
 * greppable residual (impl): udx: mmio soft residual iounmap
 * greppable residual: udx: mmio soft residual mint
 */
void              udx_iounmap(struct udx_iomem *pIo);

/* ---- Soft residual range / align gates (fail-closed; Soft!=product) -- */

/**
 * Soft residual range gate for accessors (readl/writel family + bulk).
 * Rejects null map, zero width, and out-of-span offsets.
 * Never hard-gates product DoD; observation + fail-closed read/write.
 */
static inline int
udx_mmio_in_range(const struct udx_iomem *pIo, u64 u64Off, u64 u64Width)
{
    if (pIo == NULL || pIo->pVa == NULL || u64Width == 0) {
        return 0;
    }
    if (u64Off > pIo->u64Len) {
        return 0;
    }
    if (u64Width > pIo->u64Len - u64Off) {
        return 0;
    }
    return 1;
}

/**
 * Soft residual natural-align gate for fixed-width accessors.
 * Power-of-two u64Width only (1/2/4/8): requires u64Off % width == 0.
 * aarch64 Device memory may fault on unaligned; x86_64 UC is soft.
 * Bulk byte paths keep udx_mmio_in_range (no natural-align demand).
 * greppable residual: udx: mmio soft residual accessors
 */
static inline int
udx_mmio_in_range_nat(const struct udx_iomem *pIo, u64 u64Off, u64 u64Width)
{
    if (!udx_mmio_in_range(pIo, u64Off, u64Width)) {
        return 0;
    }
    /* Width must be power-of-two for natural-align mask. */
    if ((u64Width & (u64Width - 1u)) != 0) {
        return 0;
    }
    if ((u64Off & (u64Width - 1u)) != 0) {
        return 0;
    }
    return 1;
}

/* ---- Soft residual map queries (observation only; Soft!=product) ----- */

/**
 * Non-zero when soft iomem cookie has a live UC-intent VA and non-zero len.
 * greppable residual: udx: mmio soft residual query
 */
static inline int
udx_mmio_is_mapped(const struct udx_iomem *pIo)
{
    return (pIo != NULL && pIo->pVa != NULL && pIo->u64Len != 0) ? 1 : 0;
}

/** Soft residual: mapped span length (0 if unmapped / null). */
static inline u64
udx_mmio_len(const struct udx_iomem *pIo)
{
    return (pIo != NULL) ? pIo->u64Len : 0;
}

/** Soft residual: bookkeeping grant / BAR PA (0 if null). */
static inline u64
udx_mmio_phys(const struct udx_iomem *pIo)
{
    return (pIo != NULL) ? pIo->u64Phys : 0;
}

/**
 * Soft residual: host UC-intent VA, or NULL.
 * Soft!=product: not a product MMIO_FRAME cap handle.
 */
static inline volatile void *
udx_mmio_va(const struct udx_iomem *pIo)
{
    return (pIo != NULL) ? pIo->pVa : NULL;
}

/*
 * Soft residual accessors (Linux readl/writel spirit).
 * Volatile load/store through granted VA -- assumes host UC map honesty
 * (no software cache ops here; UC leaf attrs are host/VMM responsibility;
 * UDX_MMIO_SOFT_REATTR=0). Fixed-width paths use natural-align range gate
 * (fail-closed). Out-of-range / misaligned: read returns all-ones; write
 * is a no-op. Soft!=product; not product CAP program / MMIO_FRAME mint.
 * Product hosts: rtl8168_udx / xhci_udx register/BAR soft paths (C2 Dual
 * DoD A/B OPEN residual only; agent_close=0).
 * greppable residual (impl): udx: mmio soft residual accessors
 * greppable residual (impl): udx: mmio soft residual uc_map
 * greppable residual: udx: mmio soft residual map_chain
 */

static inline u32
udx_readl(const struct udx_iomem *pIo, u64 u64Off)
{
    const volatile u32 *p;

    if (!udx_mmio_in_range_nat(pIo, u64Off, 4)) {
        return 0xffffffffu;
    }
    p = (const volatile u32 *)((const volatile u8 *)pIo->pVa + u64Off);
    return *p;
}

static inline void
udx_writel(struct udx_iomem *pIo, u64 u64Off, u32 u32Val)
{
    volatile u32 *p;

    if (!udx_mmio_in_range_nat(pIo, u64Off, 4)) {
        return;
    }
    p = (volatile u32 *)((volatile u8 *)pIo->pVa + u64Off);
    *p = u32Val;
}

static inline u16
udx_readw(const struct udx_iomem *pIo, u64 u64Off)
{
    const volatile u16 *p;

    if (!udx_mmio_in_range_nat(pIo, u64Off, 2)) {
        return 0xffffu;
    }
    p = (const volatile u16 *)((const volatile u8 *)pIo->pVa + u64Off);
    return *p;
}

static inline void
udx_writew(struct udx_iomem *pIo, u64 u64Off, u16 u16Val)
{
    volatile u16 *p;

    if (!udx_mmio_in_range_nat(pIo, u64Off, 2)) {
        return;
    }
    p = (volatile u16 *)((volatile u8 *)pIo->pVa + u64Off);
    *p = u16Val;
}

static inline u8
udx_readb(const struct udx_iomem *pIo, u64 u64Off)
{
    const volatile u8 *p;

    if (!udx_mmio_in_range_nat(pIo, u64Off, 1)) {
        return 0xffu;
    }
    p = (const volatile u8 *)pIo->pVa + u64Off;
    return *p;
}

static inline void
udx_writeb(struct udx_iomem *pIo, u64 u64Off, u8 u8Val)
{
    volatile u8 *p;

    if (!udx_mmio_in_range_nat(pIo, u64Off, 1)) {
        return;
    }
    p = (volatile u8 *)pIo->pVa + u64Off;
    *p = u8Val;
}

static inline u64
udx_readq(const struct udx_iomem *pIo, u64 u64Off)
{
    const volatile u64 *p;

    if (!udx_mmio_in_range_nat(pIo, u64Off, 8)) {
        return 0xffffffffffffffffull;
    }
    p = (const volatile u64 *)((const volatile u8 *)pIo->pVa + u64Off);
    return *p;
}

static inline void
udx_writeq(struct udx_iomem *pIo, u64 u64Off, u64 u64Val)
{
    volatile u64 *p;

    if (!udx_mmio_in_range_nat(pIo, u64Off, 8)) {
        return;
    }
    p = (volatile u64 *)((volatile u8 *)pIo->pVa + u64Off);
    *p = u64Val;
}

/* Linux ioread/iowrite name aliases (same soft residual path). */
static inline u32 udx_ioread32(const struct udx_iomem *pIo, u64 u64Off)
{
    return udx_readl(pIo, u64Off);
}
static inline void udx_iowrite32(struct udx_iomem *pIo, u64 u64Off, u32 u32Val)
{
    udx_writel(pIo, u64Off, u32Val);
}
static inline u16 udx_ioread16(const struct udx_iomem *pIo, u64 u64Off)
{
    return udx_readw(pIo, u64Off);
}
static inline void udx_iowrite16(struct udx_iomem *pIo, u64 u64Off, u16 u16Val)
{
    udx_writew(pIo, u64Off, u16Val);
}
static inline u8 udx_ioread8(const struct udx_iomem *pIo, u64 u64Off)
{
    return udx_readb(pIo, u64Off);
}
static inline void udx_iowrite8(struct udx_iomem *pIo, u64 u64Off, u8 u8Val)
{
    udx_writeb(pIo, u64Off, u8Val);
}
static inline u64 udx_ioread64(const struct udx_iomem *pIo, u64 u64Off)
{
    return udx_readq(pIo, u64Off);
}
static inline void udx_iowrite64(struct udx_iomem *pIo, u64 u64Off, u64 u64Val)
{
    udx_writeq(pIo, u64Off, u64Val);
}

/**
 * Soft residual RMW helpers (cmd/status bit set/clear spirit).
 * Fail-closed via readl/writel natural-align gate. Soft!=product.
 * greppable residual: udx: mmio soft residual accessors
 */
static inline void
udx_mmio_setl(struct udx_iomem *pIo, u64 u64Off, u32 u32Bits)
{
    u32 u32Cur;

    u32Cur = udx_readl(pIo, u64Off);
    udx_writel(pIo, u64Off, u32Cur | u32Bits);
}

static inline void
udx_mmio_clearl(struct udx_iomem *pIo, u64 u64Off, u32 u32Bits)
{
    u32 u32Cur;

    u32Cur = udx_readl(pIo, u64Off);
    udx_writel(pIo, u64Off, u32Cur & ~u32Bits);
}

/**
 * Bulk MMIO helpers (Linux memcpy_fromio / toio / memset_io spirit).
 * Host soft: plain byte copy through volatile-mapped VA (UC honesty).
 * Soft residual accessors family; fail-closed on out-of-span (no align).
 */
static inline void
udx_memcpy_fromio(void *pDst, const struct udx_iomem *pIo, u64 u64Off,
                  size_t cb)
{
    const volatile u8 *pSrc;
    u8 *pOut;
    size_t iByte;

    if (pDst == NULL || cb == 0 || !udx_mmio_in_range(pIo, u64Off, (u64)cb)) {
        return;
    }
    pSrc = (const volatile u8 *)pIo->pVa + u64Off;
    pOut = (u8 *)pDst;
    for (iByte = 0; iByte < cb; iByte++) {
        pOut[iByte] = pSrc[iByte];
    }
}

static inline void
udx_memcpy_toio(struct udx_iomem *pIo, u64 u64Off, const void *pSrc,
                size_t cb)
{
    volatile u8 *pDst;
    const u8 *pIn;
    size_t iByte;

    if (pSrc == NULL || cb == 0 || !udx_mmio_in_range(pIo, u64Off, (u64)cb)) {
        return;
    }
    pDst = (volatile u8 *)pIo->pVa + u64Off;
    pIn = (const u8 *)pSrc;
    for (iByte = 0; iByte < cb; iByte++) {
        pDst[iByte] = pIn[iByte];
    }
}

static inline void
udx_memset_io(struct udx_iomem *pIo, u64 u64Off, u8 u8Val, size_t cb)
{
    volatile u8 *pDst;
    size_t iByte;

    if (cb == 0 || !udx_mmio_in_range(pIo, u64Off, (u64)cb)) {
        return;
    }
    pDst = (volatile u8 *)pIo->pVa + u64Off;
    for (iByte = 0; iByte < cb; iByte++) {
        pDst[iByte] = u8Val;
    }
}

/**
 * Compiler/CPU fence after a batch of MMIO writes (posted-write flush intent).
 * Soft residual: does not change map attrs; UC leaf still host/VMM owned.
 * Product hosts: rtl8168_udx IntrStatus / IntrMask write then flush.
 * greppable residual (impl): udx: mmio soft residual accessors
 * greppable residual (impl): udx: mmio soft residual uc_map
 */
static inline void
udx_mmio_flush(const struct udx_iomem *pIo)
{
    (void)pIo;
#if defined(__x86_64__) || defined(__i386__)
    __asm__ volatile("mfence" ::: "memory");
#elif defined(__aarch64__)
    __asm__ volatile("dmb sy" ::: "memory");
#else
    __atomic_thread_fence(__ATOMIC_SEQ_CST);
#endif
}

/**
 * Soft residual write-then-fence helpers (product-host ISR/mask pattern).
 * Soft!=product; does not re-attr UC; MMIO_FRAME mint remains OPEN
 * (UDX_MMIO_SOFT_CAP_MINT=0). Dual DoD A/B OPEN; agent_close=0.
 * greppable residual: udx: mmio soft residual write_flush
 * greppable residual: udx: mmio soft residual product_host
 * greppable residual: udx: mmio soft residual mint
 */
static inline void
udx_writel_flush(struct udx_iomem *pIo, u64 u64Off, u32 u32Val)
{
    udx_writel(pIo, u64Off, u32Val);
    udx_mmio_flush(pIo);
}

static inline void
udx_writew_flush(struct udx_iomem *pIo, u64 u64Off, u16 u16Val)
{
    udx_writew(pIo, u64Off, u16Val);
    udx_mmio_flush(pIo);
}

static inline void
udx_writeb_flush(struct udx_iomem *pIo, u64 u64Off, u8 u8Val)
{
    udx_writeb(pIo, u64Off, u8Val);
    udx_mmio_flush(pIo);
}

static inline void
udx_writeq_flush(struct udx_iomem *pIo, u64 u64Off, u64 u64Val)
{
    udx_writeq(pIo, u64Off, u64Val);
    udx_mmio_flush(pIo);
}

/**
 * Soft residual PCI posted-write drain intent:
 *   CPU fence then fail-closed readl at a caller-chosen safe offset
 *   (side-effect-free status/id reg preferred). Soft!=product; not a
 *   product bridge/CFG flush claim. UC leaf still host/VMM owned.
 * greppable residual: udx: mmio soft residual write_flush
 * greppable residual: udx: mmio soft residual uc_map
 */
static inline void
udx_mmio_posted_flush(struct udx_iomem *pIo, u64 u64Off)
{
    udx_mmio_flush(pIo);
    (void)udx_readl(pIo, u64Off);
}

/* ---- Soft residual queries / shape (observation only; Soft!=product) - */

/**
 * Soft residual lean layout version (UDX_MMIO_LAYOUT_VER).
 * Grep: udx: mmio soft residual lean layout_ver=
 * Grep: udx: mmio soft residual lean UDX eng
 * Not GJ_IMAGE_VERSION. Not product ABI stamp.
 */
static inline u32
udx_mmio_layout_ver(void)
{
    return UDX_MMIO_LAYOUT_VER;
}

/**
 * Non-zero when soft product-host MMIO surface is present at layout_ver:
 * ioremap/iounmap + range/align accessors + bulk + flush + write*_flush +
 * posted_flush + query helpers + map_chain residual + Dual DoD OPEN catalog
 * + mint honesty constants (ver >= 2).
 * Soft!=product; eng residual only. Never hard-gates Dual DoD.
 * greppable: udx: mmio soft residual product_host
 * greppable: udx: mmio soft residual dual_dod
 * greppable: udx: mmio soft product host
 */
static inline int
udx_mmio_shape_ok(void)
{
    return (UDX_MMIO_LAYOUT_VER >= 2u) ? 1 : 0;
}

/**
 * Soft residual freestanding thrash-refuse honesty (fixed pool ceiling).
 * Soft!=product: not a live freestanding slot tally (see mmio.c inventory).
 * greppable residual: udx: mmio soft residual freestanding thrash refuse
 */
static inline u32
udx_mmio_fs_iomem_slots(void)
{
    return UDX_MMIO_FS_IOMEM_SLOTS;
}

/* ---- Soft residual mint / Dual DoD / map_chain (C2; Soft!=product) --- */

/**
 * Soft residual MMIO_FRAME cap mint honesty (always 0 today).
 * Soft!=product: window lookup never claims CNode MMIO_FRAME mint.
 * greppable residual: udx: mmio soft residual mint
 * greppable residual: udx: mmio soft api honesty
 */
static inline u32
udx_mmio_cap_mint(void)
{
    return UDX_MMIO_SOFT_CAP_MINT;
}

/**
 * Soft residual Soft!=product honesty bit (always 1).
 * greppable residual: udx: mmio soft residual lean
 * greppable residual: udx: mmio soft api honesty
 */
static inline u32
udx_mmio_soft_ne_product(void)
{
    return UDX_MMIO_SOFT_NE_PRODUCT;
}

/**
 * Dual DoD A OPEN residual (xhci_udx). Always 1 until host USB path.
 * Lamps / shape_ok never close Dual DoD. Soft!=product; G-AC-1.
 * greppable residual: udx: mmio soft residual dual_dod
 * greppable residual: udx: mmio soft residual product_host
 */
static inline u32
udx_mmio_dod_a_open(void)
{
    return UDX_MMIO_SOFT_DOD_A_OPEN;
}

/**
 * Dual DoD B OPEN residual (rtl8168_udx). Always 1 until interactive SSH login.
 * greppable residual: udx: mmio soft residual dual_dod
 * greppable residual: udx: mmio soft residual product_host
 */
static inline u32
udx_mmio_dod_b_open(void)
{
    return UDX_MMIO_SOFT_DOD_B_OPEN;
}

/**
 * Soft residual agent_close honesty (always 0). Residual never closes DoD.
 * greppable residual: udx: mmio soft residual dual_dod
 */
static inline u32
udx_mmio_agent_close(void)
{
    return UDX_MMIO_SOFT_AGENT_CLOSE;
}

/**
 * Soft residual multi_server honesty (always 0 today).
 * greppable residual: udx: mmio soft residual mint
 */
static inline u32
udx_mmio_multi_server(void)
{
    return UDX_MMIO_SOFT_MULTI_SERVER;
}

/**
 * Soft residual confine honesty (always 0 today).
 * greppable residual: udx: mmio soft residual mint
 */
static inline u32
udx_mmio_confine(void)
{
    return UDX_MMIO_SOFT_CONFINE;
}

/**
 * Soft residual UC map honesty: non-zero when soft expects host UC leaf
 * attrs and UDX does not re-attr pages (uc_intent=1, reattr=0).
 * greppable residual: udx: mmio soft residual uc_map
 */
static inline int
udx_mmio_uc_map_honesty(void)
{
    return (UDX_MMIO_SOFT_UC_INTENT != 0u &&
            UDX_MMIO_SOFT_REATTR == 0u) ? 1 : 0;
}

/**
 * Soft residual map_chain shape present (C2 product path catalog):
 * MAP_BAR -> window_register -> ioremap lookup; cap_mint=0; Dual DoD OPEN.
 * Soft!=product; never hard-gates Dual DoD A/B.
 * greppable residual: udx: mmio soft residual map_chain
 * greppable residual: udx: mmio soft residual lean UDX eng
 */
static inline int
udx_mmio_map_chain_shape_ok(void)
{
    return (UDX_MMIO_LAYOUT_VER >= 2u &&
            UDX_MMIO_SOFT_CAP_MINT == 0u &&
            UDX_MMIO_SOFT_DOD_A_OPEN != 0u &&
            UDX_MMIO_SOFT_DOD_B_OPEN != 0u &&
            UDX_MMIO_SOFT_AGENT_CLOSE == 0u &&
            UDX_MMIO_SOFT_NE_PRODUCT != 0u) ? 1 : 0;
}

/**
 * Soft residual product-host preferred BAR mask catalog (observation only).
 * nHost: 0 = rtl8168_udx (DoD B), 1 = xhci_udx (DoD A); else 0.
 * greppable residual: udx: mmio soft residual map_chain
 * greppable residual: udx: mmio soft residual product_host
 */
static inline u32
udx_mmio_pref_bar_mask(u32 u32Host)
{
    if (u32Host == 0u) {
        return UDX_MMIO_RTL_PREF_BAR_MASK;
    }
    if (u32Host == 1u) {
        return UDX_MMIO_XHCI_PREF_BAR_MASK;
    }
    return 0u;
}

/**
 * Soft residual path-bit catalog seed for product-host MMIO surface.
 * OR of residual path bits present at this layout_ver (observation only).
 * Soft!=product; Dual DoD never closed from this mask.
 * greppable residual: udx: mmio soft residual lean
 * greppable residual: udx: mmio soft residual lean UDX eng
 */
static inline u32
udx_mmio_soft_path_mask(void)
{
    return (UDX_MMIO_SOFT_PATH_IOREMAP |
            UDX_MMIO_SOFT_PATH_IOUNMAP |
            UDX_MMIO_SOFT_PATH_ACCESSORS |
            UDX_MMIO_SOFT_PATH_UC_MAP |
            UDX_MMIO_SOFT_PATH_THRASH_REFUSE |
            UDX_MMIO_SOFT_PATH_MAP_CHAIN |
            UDX_MMIO_SOFT_PATH_WRITE_FLUSH |
            UDX_MMIO_SOFT_PATH_QUERY |
            UDX_MMIO_SOFT_PATH_PRODUCT_HOST |
            UDX_MMIO_SOFT_PATH_DUAL_DOD |
            UDX_MMIO_SOFT_PATH_MINT);
}

/**
 * Soft residual Dual DoD + mint honesty bundle (all must hold for C2 seed).
 * Non-zero when: cap_mint=0, multi_server=0, confine=0, DoD A/B OPEN,
 * agent_close=0, Soft!=product, map_chain shape ok.
 * Never closes Dual DoD. greppable: udx: mmio soft residual dual_dod
 * greppable: udx: mmio soft residual mint
 * greppable: udx: mmio soft residual map_chain
 */
static inline int
udx_mmio_dual_dod_open_ok(void)
{
    return (UDX_MMIO_SOFT_CAP_MINT == 0u &&
            UDX_MMIO_SOFT_MULTI_SERVER == 0u &&
            UDX_MMIO_SOFT_CONFINE == 0u &&
            UDX_MMIO_SOFT_DOD_A_OPEN != 0u &&
            UDX_MMIO_SOFT_DOD_B_OPEN != 0u &&
            UDX_MMIO_SOFT_AGENT_CLOSE == 0u &&
            UDX_MMIO_SOFT_NE_PRODUCT != 0u &&
            udx_mmio_map_chain_shape_ok()) ? 1 : 0;
}
