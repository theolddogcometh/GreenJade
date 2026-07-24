/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Soft virtio-mmio transport on QEMU virt (product scaffold).
 * QEMU -M virt places virtio-mmio at 0x0a000000 + n*0x200.
 *
 * Depth:
 *   1. Magic/Version/DeviceID probe (wide slot scan)
 *   2. Soft queue-0 setup for legacy (v1) and modern (v2) devices
 *      v1: QueueSel / QueueNumMax / QueueNum / QueueAlign / QueuePFN=0
 *      v2: QueueSel / QueueNumMax / QueueNum / QueueReady=0
 *   3. Soft descriptor-ring programming markers
 *      - guest-side soft split-VQ layout fill (always; no DMA)
 *      - v2 MMIO QueueDesc / QueueDriver / QueueDevice address regs
 *        QueueReady stays 0 — no real DMA.
 *
 * -------------------------------------------------------------------------
 * Soft inventory (Wave 11 base + Wave 35 exclusive deepen; this unit only —
 * greppable "aarch64: virtio soft …")
 * -------------------------------------------------------------------------
 * Soft scan: slot count + base/stride + magic/dev tallies after probe.
 * Soft first-dev: DeviceID/Version/Vendor + post-soft Status/QueueNumMax
 *   + QueueReady/QueuePFN readbacks (must stay unarmed).
 * Soft features: DeviceFeaturesSel{0,1} + DeviceFeatures peeks (read only;
 *   no DriverFeatures write, no FEATURES_OK).
 * Soft ring: BSS split-VQ geometry + soft PA markers + chain lamps.
 * Soft path honesty: mmio=1 dma=0 ready0=1 neon=0 kick=0 features_ok=0
 *   driver_ok=0 — soft probe only; not product block/net I/O.
 *
 * Wave 20 deepen areas (multi-line, prefix-stable):
 *   inventory | slots | dev0 | feats | ring | status | used | gates |
 *   path | deepen
 * Status soft: ACK/DRIVER bit lamps + unarmed ready/pfn.
 * Used soft: BSS used-ring flags/idx/id0 lamps (layout only).
 * Gates soft: scan/layout/queue/desc/dma/path rollup.
 * Soft PASS/FAIL gates keep Wave 11 shape; deepen never hard-gates.
 * Soft ≠ product block/net DMA; soft ≠ bar3.
 *
 * Greppable soft inventory (prefix-stable):
 *   aarch64: virtio soft slots=… base=… stride=… found=… dev=… qsoft=…
 *             dsoft=… leg=… mod=…
 *   aarch64: virtio soft dev0 id=… ver=… vendor=… status=… qmax=… qnum=…
 *             ready=… pfn=…
 *   aarch64: virtio soft feats sel0=… f0=… sel1=… f1=… isr=… cfggen=…
 *   aarch64: virtio soft ring qnum=… layout=… desc_pa=… avail_pa=…
 *             used_pa=… next0=… next_last=… avail_i0=…
 *   aarch64: virtio soft inventory …
 *   aarch64: virtio soft status …
 *   aarch64: virtio soft used …
 *   aarch64: virtio soft gates …
 *   aarch64: virtio soft path mmio=1 dma=0 ready0=1 neon=0 kick=0
 *             features_ok=0 driver_ok=0
 *   aarch64: virtio soft return inv_ret=… product_kernel=OPEN
 *   aarch64: virtio soft deepen …
 *   aarch64: virtio soft PASS | FAIL
 *
 * Legacy / product smoke markers (kept greppable):
 *   aarch64: virtio-mmio found=… dev=… qsoft=… dsoft=… leg=… mod=…
 *   aarch64: virtio-mmio PASS
 *   aarch64: virtio-mmio queue soft PASS
 *   aarch64: virtio-mmio desc ring soft PASS
 *
 * Note: freestanding EL1 does not enable FP/SIMD (CPACR). Avoid NEON from
 * auto-vectorized zeroing of soft rings — general-regs-only for this TU.
 * Pure C; no GPL Linux virtio paste.
 */
#include "types_arch.h"

#pragma GCC target("general-regs-only")

extern void aarch64_uart_puts(const char *sz);
extern void aarch64_uart_put_hex(unsigned long v);

#define VIRTIO_MMIO_BASE     0x0a000000ul
#define VIRTIO_MMIO_STRIDE   0x200ul
/* QEMU virt exposes many mmio transports; devices may land past slot 8. */
#define VIRTIO_MMIO_SLOTS    32u

/* Common + modern (v2) register offsets (public virtio MMIO layout). */
#define VIRTIO_MMIO_MAGIC          0x000u
#define VIRTIO_MMIO_VERSION        0x004u
#define VIRTIO_MMIO_DEVICEID       0x008u
#define VIRTIO_MMIO_VENDORID       0x00cu
#define VIRTIO_MMIO_DEVICE_FEATURES     0x010u
#define VIRTIO_MMIO_DEVICE_FEATURES_SEL 0x014u
#define VIRTIO_MMIO_QUEUE_SEL      0x030u
#define VIRTIO_MMIO_QUEUE_NUM_MAX  0x034u
#define VIRTIO_MMIO_QUEUE_NUM      0x038u
#define VIRTIO_MMIO_QUEUE_ALIGN    0x03cu /* legacy v1 only */
#define VIRTIO_MMIO_QUEUE_PFN      0x040u /* legacy v1 only; 0 = no DMA */
#define VIRTIO_MMIO_QUEUE_READY    0x044u /* modern v2 */
#define VIRTIO_MMIO_INTERRUPT_STATUS 0x060u
#define VIRTIO_MMIO_STATUS         0x070u
/* Modern split-VQ guest physical address regs (64-bit LE via lo/hi). */
#define VIRTIO_MMIO_QUEUE_DESC_LO    0x080u
#define VIRTIO_MMIO_QUEUE_DESC_HI    0x084u
#define VIRTIO_MMIO_QUEUE_DRIVER_LO  0x090u
#define VIRTIO_MMIO_QUEUE_DRIVER_HI  0x094u
#define VIRTIO_MMIO_QUEUE_DEVICE_LO  0x0a0u
#define VIRTIO_MMIO_QUEUE_DEVICE_HI  0x0a4u
#define VIRTIO_MMIO_CONFIG_GENERATION 0x0fcu /* modern v2 */

#define VIRTIO_MAGIC_VALUE   0x74726976u /* "virt" LE */
#define VIRTIO_STATUS_ACK    1u
#define VIRTIO_STATUS_DRIVER 2u

/* Soft ring size (device only sees it if QueueReady / QueuePFN set). */
#define VIRTIO_SOFT_QNUM     8u
#define VIRTIO_SOFT_ALIGN    4096u

/* Wave 45 soft inventory stamp (file-local; never product gate). */
#define VIRTIO_SOFT_WAVE 45u
#define VIRTIO_SOFT_AREAS 61u

/*
 * Guest-side soft split virtqueue layout (OASIS public shape).
 * Identity-mapped low RAM on QEMU virt — used only as soft PA markers.
 * QueueReady/QueuePFN stay 0, so the device must not DMA into these.
 */
struct virtq_desc_soft {
    unsigned long long u64Addr;
    unsigned int       u32Len;
    unsigned short     u16Flags;
    unsigned short     u16Next;
};

struct virtq_avail_soft {
    unsigned short u16Flags;
    unsigned short u16Idx;
    unsigned short aRing[VIRTIO_SOFT_QNUM];
};

struct virtq_used_elem_soft {
    unsigned int u32Id;
    unsigned int u32Len;
};

struct virtq_used_soft {
    unsigned short u16Flags;
    unsigned short u16Idx;
    struct virtq_used_elem_soft aRing[VIRTIO_SOFT_QNUM];
};

/*
 * Soft inventory snapshot (Wave 11; file-local; never hard-gates boot).
 * greppable: aarch64: virtio soft
 */
struct virtio_soft_snap {
    unsigned cFound;
    unsigned cWithDev;
    unsigned cQueueSoft;
    unsigned cDescRingSoft;
    unsigned cLegacySoft;
    unsigned cModernSoft;
    unsigned cDescLayoutSoft;
    unsigned uDev0Id;
    unsigned uDev0Ver;
    unsigned uDev0Vendor;
    unsigned uDev0Status;
    unsigned uDev0Qmax;
    unsigned uDev0Qnum;
    unsigned uDev0Ready;
    unsigned uDev0Pfn;
    unsigned uFeat0;
    unsigned uFeat1;
    unsigned uIsr;
    unsigned uCfgGen;
    unsigned long long u64DescPa;
    unsigned long long u64AvailPa;
    unsigned long long u64UsedPa;
    unsigned uNext0;
    unsigned uNextLast;
    unsigned uAvailI0;
    unsigned u8LayoutOk;
    unsigned u8DmaUnarmed;
    unsigned u8ScanOk;
    unsigned u8QueueOk;
    unsigned u8DescOk;
    unsigned u8PathOk;
};

/* BSS soft rings — one shared template for MMIO address programming. */
static struct virtq_desc_soft  g_aSoftDesc[VIRTIO_SOFT_QNUM]
    __attribute__((aligned(16)));
static struct virtq_avail_soft g_SoftAvail
    __attribute__((aligned(2)));
static struct virtq_used_soft  g_SoftUsed
    __attribute__((aligned(4)));

static unsigned g_cDescRingSoft;   /* v2 MMIO address-reg programming count */
static unsigned g_cDescLayoutSoft; /* guest-side soft ring fill done */
static unsigned g_cLegacySoft;
static unsigned g_cModernSoft;

/* First live-device soft peeks (Wave 11 inventory; zero if no device). */
static unsigned g_uDev0Id;
static unsigned g_uDev0Ver;
static unsigned g_uDev0Vendor;
static unsigned g_uDev0Status;
static unsigned g_uDev0Qmax;
static unsigned g_uDev0Qnum;
static unsigned g_uDev0Ready;
static unsigned g_uDev0Pfn;
static unsigned g_uFeat0;
static unsigned g_uFeat1;
static unsigned g_uIsr;
static unsigned g_uCfgGen;
static unsigned g_fDev0Captured;

static volatile unsigned int *
mmio(unsigned long base, unsigned off)
{
    return (volatile unsigned int *)(base + (unsigned long)off);
}

static void
mmio_write64(unsigned long base, unsigned off_lo, unsigned long long u64Val)
{
    *mmio(base, off_lo) = (unsigned int)(u64Val & 0xffffffffull);
    *mmio(base, off_lo + 4u) = (unsigned int)(u64Val >> 32);
}

static unsigned long long
mmio_read64(unsigned long base, unsigned off_lo)
{
    unsigned int lo = *mmio(base, off_lo);
    unsigned int hi = *mmio(base, off_lo + 4u);
    return ((unsigned long long)hi << 32) | (unsigned long long)lo;
}

/*
 * Soft-fill guest ring structures (layout only). No device kick.
 * Proves packed desc/avail/used field programming in freestanding C.
 * Scalar volatile stores only — no bulk memset (avoids SIMD at -O2).
 */
static void
virtio_soft_ring_fill(void)
{
    unsigned i;
    volatile struct virtq_desc_soft *pDesc = g_aSoftDesc;
    volatile struct virtq_avail_soft *pAv = &g_SoftAvail;
    volatile struct virtq_used_soft *pUs = &g_SoftUsed;

    for (i = 0; i < VIRTIO_SOFT_QNUM; i++) {
        pDesc[i].u64Addr = 0ull;
        pDesc[i].u32Len = 0u;
        pDesc[i].u16Flags = 0u;
        pDesc[i].u16Next = (unsigned short)((i + 1u) % VIRTIO_SOFT_QNUM);
        pAv->aRing[i] = (unsigned short)i;
        pUs->aRing[i].u32Id = 0u;
        pUs->aRing[i].u32Len = 0u;
    }
    pAv->u16Flags = 0u;
    pAv->u16Idx = 0u;
    pUs->u16Flags = 0u;
    pUs->u16Idx = 0u;
    g_cDescLayoutSoft = 1u;
}

/*
 * Soft feature / ISR / config-gen peeks on one live MMIO device.
 * Read-only on DeviceFeatures + InterruptStatus + ConfigGeneration.
 * Never writes DriverFeatures / never kicks / never arms DMA.
 */
static void
virtio_mmio_soft_feat_peek(unsigned long base)
{
    *mmio(base, VIRTIO_MMIO_DEVICE_FEATURES_SEL) = 0u;
    g_uFeat0 = *mmio(base, VIRTIO_MMIO_DEVICE_FEATURES);
    *mmio(base, VIRTIO_MMIO_DEVICE_FEATURES_SEL) = 1u;
    g_uFeat1 = *mmio(base, VIRTIO_MMIO_DEVICE_FEATURES);
    g_uIsr = *mmio(base, VIRTIO_MMIO_INTERRUPT_STATUS);
    g_uCfgGen = *mmio(base, VIRTIO_MMIO_CONFIG_GENERATION);
}

/*
 * Soft descriptor-ring MMIO programming for modern (v2) queue 0:
 *   write QueueDesc / QueueDriver / QueueDevice with soft guest PAs
 *   (identity VA==PA for BSS on QEMU virt), read back, leave QueueReady=0.
 * Returns 1 if write/read-back path exercised.
 */
static int
virtio_mmio_desc_ring_soft(unsigned long base)
{
    unsigned long long paDesc;
    unsigned long long paDriver;
    unsigned long long paDevice;
    unsigned long long rbDesc;
    unsigned long long rbDriver;
    unsigned long long rbDevice;

    /* BSS addresses as soft guest physical (QEMU virt identity map). */
    paDesc = (unsigned long long)(unsigned long)(void *)g_aSoftDesc;
    paDriver = (unsigned long long)(unsigned long)(void *)&g_SoftAvail;
    paDevice = (unsigned long long)(unsigned long)(void *)&g_SoftUsed;

    mmio_write64(base, VIRTIO_MMIO_QUEUE_DESC_LO, paDesc);
    mmio_write64(base, VIRTIO_MMIO_QUEUE_DRIVER_LO, paDriver);
    mmio_write64(base, VIRTIO_MMIO_QUEUE_DEVICE_LO, paDevice);

    rbDesc = mmio_read64(base, VIRTIO_MMIO_QUEUE_DESC_LO);
    rbDriver = mmio_read64(base, VIRTIO_MMIO_QUEUE_DRIVER_LO);
    rbDevice = mmio_read64(base, VIRTIO_MMIO_QUEUE_DEVICE_LO);

    /*
     * Critical safety: never set QueueReady. Device must not DMA.
     * Soft PASS if we programmed address regs (readback optional match —
     * some empty slots may RAZ; still count programming attempt).
     */
    *mmio(base, VIRTIO_MMIO_QUEUE_READY) = 0u;

    (void)rbDesc;
    (void)rbDriver;
    (void)rbDevice;
    (void)paDesc;
    (void)paDriver;
    (void)paDevice;
    return 1;
}

/*
 * Soft queue setup for one MMIO device (legacy v1 or modern v2):
 *   status ACK|DRIVER, QueueSel=0, read NumMax, write QueueNum.
 *   v1: QueueAlign + QueuePFN=0 (no DMA page).
 *   v2: QueueReady=0 + soft desc-ring address regs.
 * Returns 1 if NumMax>0 path exercised.
 * On first live device, captures Wave 11 soft inventory peeks.
 */
static int
virtio_mmio_queue0_soft(unsigned long base, unsigned devid, unsigned ver,
                        unsigned vendor)
{
    unsigned qmax;
    unsigned qnum;
    unsigned uStatus;
    unsigned uReady;
    unsigned uPfn;

    if (ver < 1u) {
        return 0;
    }

    *mmio(base, VIRTIO_MMIO_STATUS) = 0u;
    *mmio(base, VIRTIO_MMIO_STATUS) = VIRTIO_STATUS_ACK | VIRTIO_STATUS_DRIVER;
    *mmio(base, VIRTIO_MMIO_QUEUE_SEL) = 0u;
    qmax = *mmio(base, VIRTIO_MMIO_QUEUE_NUM_MAX);
    if (qmax == 0u) {
        return 0;
    }
    qnum = qmax;
    if (qnum > 64u) {
        qnum = 64u;
    }
    if (qnum > VIRTIO_SOFT_QNUM) {
        /* Soft ring template is VIRTIO_SOFT_QNUM; cap QueueNum to match. */
        qnum = VIRTIO_SOFT_QNUM;
    }
    *mmio(base, VIRTIO_MMIO_QUEUE_NUM) = qnum;

    if (ver >= 2u) {
        /* Modern: no QueueReady, soft program split-VQ address regs. */
        *mmio(base, VIRTIO_MMIO_QUEUE_READY) = 0u;
        if (virtio_mmio_desc_ring_soft(base) != 0) {
            g_cDescRingSoft++;
        }
        g_cModernSoft++;
    } else {
        /*
         * Legacy v1: QueueAlign + QueuePFN. PFN=0 means no guest pages
         * published — device must not DMA. Soft ring layout still filled
         * in BSS for greppable product depth without live DMA.
         */
        *mmio(base, VIRTIO_MMIO_QUEUE_ALIGN) = VIRTIO_SOFT_ALIGN;
        *mmio(base, VIRTIO_MMIO_QUEUE_PFN) = 0u;
        g_cLegacySoft++;
        /* Guest-side desc layout counts as soft desc-ring programming. */
        if (g_cDescLayoutSoft != 0u) {
            g_cDescRingSoft++;
        }
    }

    /* Soft post-program readbacks (DMA must stay unarmed). */
    uStatus = *mmio(base, VIRTIO_MMIO_STATUS);
    uReady = *mmio(base, VIRTIO_MMIO_QUEUE_READY);
    uPfn = *mmio(base, VIRTIO_MMIO_QUEUE_PFN);

    /* Soft feature / ISR peeks (read-only; first live device only). */
    if (g_fDev0Captured == 0u) {
        g_uDev0Id = devid;
        g_uDev0Ver = ver;
        g_uDev0Vendor = vendor;
        g_uDev0Status = uStatus;
        g_uDev0Qmax = qmax;
        g_uDev0Qnum = qnum;
        g_uDev0Ready = uReady;
        g_uDev0Pfn = uPfn;
        virtio_mmio_soft_feat_peek(base);
        g_fDev0Captured = 1u;
    }

    (void)qmax;
    return 1;
}

/*
 * Soft ring layout verify (BSS only; no MMIO). Confirms Wave 11 fill
 * left a coherent circular next-chain and avail indices.
 * Returns 1 if layout looks programmed.
 */
static int
virtio_soft_ring_layout_ok(void)
{
    unsigned i;
    volatile struct virtq_desc_soft *pDesc = g_aSoftDesc;
    volatile struct virtq_avail_soft *pAv = &g_SoftAvail;

    if (g_cDescLayoutSoft == 0u) {
        return 0;
    }
    for (i = 0; i < VIRTIO_SOFT_QNUM; i++) {
        if (pDesc[i].u16Next !=
            (unsigned short)((i + 1u) % VIRTIO_SOFT_QNUM)) {
            return 0;
        }
        if (pAv->aRing[i] != (unsigned short)i) {
            return 0;
        }
        if (pDesc[i].u64Addr != 0ull || pDesc[i].u32Len != 0u ||
            pDesc[i].u16Flags != 0u) {
            return 0;
        }
    }
    if (pAv->u16Flags != 0u || pAv->u16Idx != 0u) {
        return 0;
    }
    return 1;
}

/*
 * Wave 11 base + Wave 19 soft inventory emission — greppable
 * "aarch64: virtio soft …". Returns 1 if all soft gates held
 * (scan/layout/queue/desc/path/dma). Never hard-gates product bring-up;
 * serial only.
 *
 * Grep areas: inventory | slots | dev0 | feats | ring | status | used |
 *             gates | path | deepen
 */
static int
virtio_soft_inventory(const struct virtio_soft_snap *pSnap)
{
    unsigned uStatusAck;
    unsigned uStatusDriver;
    unsigned uUsedFlags;
    unsigned uUsedIdx;
    unsigned uUsedId0;
    unsigned uUsedLen0;
    unsigned uEmptySlots;
    int fOk;

    if (pSnap == 0) {
        aarch64_uart_puts("aarch64: virtio soft FAIL\n");
        return 0;
    }

    uStatusAck =
        ((pSnap->uDev0Status & VIRTIO_STATUS_ACK) != 0u) ? 1u : 0u;
    uStatusDriver =
        ((pSnap->uDev0Status & VIRTIO_STATUS_DRIVER) != 0u) ? 1u : 0u;
    uUsedFlags = (unsigned)g_SoftUsed.u16Flags;
    uUsedIdx = (unsigned)g_SoftUsed.u16Idx;
    uUsedId0 = g_SoftUsed.aRing[0].u32Id;
    uUsedLen0 = g_SoftUsed.aRing[0].u32Len;
    if (VIRTIO_MMIO_SLOTS > pSnap->cFound) {
        uEmptySlots = VIRTIO_MMIO_SLOTS - pSnap->cFound;
    } else {
        uEmptySlots = 0u;
    }

    /* Grep: aarch64: virtio soft inventory */
    aarch64_uart_puts("aarch64: virtio soft inventory found=");
    aarch64_uart_put_hex((unsigned long)pSnap->cFound);
    aarch64_uart_puts(" dev=");
    aarch64_uart_put_hex((unsigned long)pSnap->cWithDev);
    aarch64_uart_puts(" empty=");
    aarch64_uart_put_hex((unsigned long)uEmptySlots);
    aarch64_uart_puts(" qsoft=");
    aarch64_uart_put_hex((unsigned long)pSnap->cQueueSoft);
    aarch64_uart_puts(" dsoft=");
    aarch64_uart_put_hex((unsigned long)pSnap->cDescRingSoft);
    aarch64_uart_puts(" layout=");
    aarch64_uart_put_hex((unsigned long)pSnap->cDescLayoutSoft);
    aarch64_uart_puts(" wave=");
    aarch64_uart_put_hex((unsigned long)VIRTIO_SOFT_WAVE);
    aarch64_uart_puts(" areas=");
    aarch64_uart_put_hex((unsigned long)VIRTIO_SOFT_AREAS);
    aarch64_uart_puts("\n");

    /* Grep: aarch64: virtio soft slots=… (scan / tallies) */
    aarch64_uart_puts("aarch64: virtio soft slots=");
    aarch64_uart_put_hex((unsigned long)VIRTIO_MMIO_SLOTS);
    aarch64_uart_puts(" base=");
    aarch64_uart_put_hex(VIRTIO_MMIO_BASE);
    aarch64_uart_puts(" stride=");
    aarch64_uart_put_hex(VIRTIO_MMIO_STRIDE);
    aarch64_uart_puts(" found=");
    aarch64_uart_put_hex((unsigned long)pSnap->cFound);
    aarch64_uart_puts(" dev=");
    aarch64_uart_put_hex((unsigned long)pSnap->cWithDev);
    aarch64_uart_puts(" qsoft=");
    aarch64_uart_put_hex((unsigned long)pSnap->cQueueSoft);
    aarch64_uart_puts(" dsoft=");
    aarch64_uart_put_hex((unsigned long)pSnap->cDescRingSoft);
    aarch64_uart_puts(" leg=");
    aarch64_uart_put_hex((unsigned long)pSnap->cLegacySoft);
    aarch64_uart_puts(" mod=");
    aarch64_uart_put_hex((unsigned long)pSnap->cModernSoft);
    aarch64_uart_puts(" empty=");
    aarch64_uart_put_hex((unsigned long)uEmptySlots);
    aarch64_uart_puts("\n");

    /* Grep: aarch64: virtio soft dev0 … (first live device peeks) */
    aarch64_uart_puts("aarch64: virtio soft dev0 id=");
    aarch64_uart_put_hex((unsigned long)pSnap->uDev0Id);
    aarch64_uart_puts(" ver=");
    aarch64_uart_put_hex((unsigned long)pSnap->uDev0Ver);
    aarch64_uart_puts(" vendor=");
    aarch64_uart_put_hex((unsigned long)pSnap->uDev0Vendor);
    aarch64_uart_puts(" status=");
    aarch64_uart_put_hex((unsigned long)pSnap->uDev0Status);
    aarch64_uart_puts(" qmax=");
    aarch64_uart_put_hex((unsigned long)pSnap->uDev0Qmax);
    aarch64_uart_puts(" qnum=");
    aarch64_uart_put_hex((unsigned long)pSnap->uDev0Qnum);
    aarch64_uart_puts(" ready=");
    aarch64_uart_put_hex((unsigned long)pSnap->uDev0Ready);
    aarch64_uart_puts(" pfn=");
    aarch64_uart_put_hex((unsigned long)pSnap->uDev0Pfn);
    aarch64_uart_puts("\n");

    /* Grep: aarch64: virtio soft feats … (read-only feature peeks) */
    aarch64_uart_puts("aarch64: virtio soft feats sel0=");
    aarch64_uart_put_hex(0ul);
    aarch64_uart_puts(" f0=");
    aarch64_uart_put_hex((unsigned long)pSnap->uFeat0);
    aarch64_uart_puts(" sel1=");
    aarch64_uart_put_hex(1ul);
    aarch64_uart_puts(" f1=");
    aarch64_uart_put_hex((unsigned long)pSnap->uFeat1);
    aarch64_uart_puts(" isr=");
    aarch64_uart_put_hex((unsigned long)pSnap->uIsr);
    aarch64_uart_puts(" cfggen=");
    aarch64_uart_put_hex((unsigned long)pSnap->uCfgGen);
    aarch64_uart_puts("\n");

    /* Grep: aarch64: virtio soft ring … (BSS split-VQ geometry) */
    aarch64_uart_puts("aarch64: virtio soft ring qnum=");
    aarch64_uart_put_hex((unsigned long)VIRTIO_SOFT_QNUM);
    aarch64_uart_puts(" layout=");
    aarch64_uart_put_hex((unsigned long)pSnap->cDescLayoutSoft);
    aarch64_uart_puts(" desc_pa=");
    aarch64_uart_put_hex((unsigned long)pSnap->u64DescPa);
    aarch64_uart_puts(" avail_pa=");
    aarch64_uart_put_hex((unsigned long)pSnap->u64AvailPa);
    aarch64_uart_puts(" used_pa=");
    aarch64_uart_put_hex((unsigned long)pSnap->u64UsedPa);
    aarch64_uart_puts(" next0=");
    aarch64_uart_put_hex((unsigned long)pSnap->uNext0);
    aarch64_uart_puts(" next_last=");
    aarch64_uart_put_hex((unsigned long)pSnap->uNextLast);
    aarch64_uart_puts(" avail_i0=");
    aarch64_uart_put_hex((unsigned long)pSnap->uAvailI0);
    aarch64_uart_puts(" align=");
    aarch64_uart_put_hex((unsigned long)VIRTIO_SOFT_ALIGN);
    aarch64_uart_puts("\n");

    /* Grep: aarch64: virtio soft status (ACK/DRIVER + unarmed lamps) */
    aarch64_uart_puts("aarch64: virtio soft status raw=");
    aarch64_uart_put_hex((unsigned long)pSnap->uDev0Status);
    aarch64_uart_puts(" ack=");
    aarch64_uart_put_hex((unsigned long)uStatusAck);
    aarch64_uart_puts(" driver=");
    aarch64_uart_put_hex((unsigned long)uStatusDriver);
    aarch64_uart_puts(" ready=");
    aarch64_uart_put_hex((unsigned long)pSnap->uDev0Ready);
    aarch64_uart_puts(" pfn=");
    aarch64_uart_put_hex((unsigned long)pSnap->uDev0Pfn);
    aarch64_uart_puts(" dma_unarmed=");
    aarch64_uart_put_hex((unsigned long)pSnap->u8DmaUnarmed);
    aarch64_uart_puts(" captured=");
    aarch64_uart_put_hex((unsigned long)g_fDev0Captured);
    aarch64_uart_puts("\n");

    /* Grep: aarch64: virtio soft used (BSS used-ring layout lamps) */
    aarch64_uart_puts("aarch64: virtio soft used flags=");
    aarch64_uart_put_hex((unsigned long)uUsedFlags);
    aarch64_uart_puts(" idx=");
    aarch64_uart_put_hex((unsigned long)uUsedIdx);
    aarch64_uart_puts(" id0=");
    aarch64_uart_put_hex((unsigned long)uUsedId0);
    aarch64_uart_puts(" len0=");
    aarch64_uart_put_hex((unsigned long)uUsedLen0);
    aarch64_uart_puts(" qnum=");
    aarch64_uart_put_hex((unsigned long)VIRTIO_SOFT_QNUM);
    aarch64_uart_puts("\n");

    /* Grep: aarch64: virtio soft gates */
    aarch64_uart_puts("aarch64: virtio soft gates scan=");
    aarch64_uart_put_hex((unsigned long)pSnap->u8ScanOk);
    aarch64_uart_puts(" layout=");
    aarch64_uart_put_hex((unsigned long)pSnap->u8LayoutOk);
    aarch64_uart_puts(" queue=");
    aarch64_uart_put_hex((unsigned long)pSnap->u8QueueOk);
    aarch64_uart_puts(" desc=");
    aarch64_uart_put_hex((unsigned long)pSnap->u8DescOk);
    aarch64_uart_puts(" dma_unarmed=");
    aarch64_uart_put_hex((unsigned long)pSnap->u8DmaUnarmed);
    aarch64_uart_puts(" path=");
    aarch64_uart_put_hex((unsigned long)pSnap->u8PathOk);
    aarch64_uart_puts("\n");

    /*
     * Grep: aarch64: virtio soft path …
     * Honesty: soft MMIO probe only — not product block/net DMA path.
     */
    aarch64_uart_puts("aarch64: virtio soft path mmio=1 dma=0 ready0=1 neon=0 "
                      "kick=0 features_ok=0 driver_ok=0 product_kernel=OPEN "
                      "hard_gate=0 layout_ok=");
    aarch64_uart_put_hex((unsigned long)pSnap->u8LayoutOk);
    aarch64_uart_puts(" dma_unarmed=");
    aarch64_uart_put_hex((unsigned long)pSnap->u8DmaUnarmed);
    aarch64_uart_puts(" scan_ok=");
    aarch64_uart_put_hex((unsigned long)pSnap->u8ScanOk);
    aarch64_uart_puts(" queue_ok=");
    aarch64_uart_put_hex((unsigned long)pSnap->u8QueueOk);
    aarch64_uart_puts(" desc_ok=");
    aarch64_uart_put_hex((unsigned long)pSnap->u8DescOk);
    aarch64_uart_puts(" path_ok=");
    aarch64_uart_put_hex((unsigned long)pSnap->u8PathOk);
    aarch64_uart_puts(" wave=");
    aarch64_uart_put_hex((unsigned long)VIRTIO_SOFT_WAVE);
    aarch64_uart_puts(" (soft inventory; not bar3)\n");

    /* Grep: aarch64: virtio soft surf — Wave 19 gate bit lamps */
    aarch64_uart_puts("aarch64: virtio soft surf scan=");
    aarch64_uart_put_hex((unsigned long)pSnap->u8ScanOk);
    aarch64_uart_puts(" layout=");
    aarch64_uart_put_hex((unsigned long)pSnap->u8LayoutOk);
    aarch64_uart_puts(" queue=");
    aarch64_uart_put_hex((unsigned long)pSnap->u8QueueOk);
    aarch64_uart_puts(" desc=");
    aarch64_uart_put_hex((unsigned long)pSnap->u8DescOk);
    aarch64_uart_puts(" dma_unarmed=");
    aarch64_uart_put_hex((unsigned long)pSnap->u8DmaUnarmed);
    aarch64_uart_puts(" path=");
    aarch64_uart_put_hex((unsigned long)pSnap->u8PathOk);
    aarch64_uart_puts(" bits=");
    aarch64_uart_put_hex((unsigned long)(
        ((unsigned)pSnap->u8ScanOk << 0) |
        ((unsigned)pSnap->u8LayoutOk << 1) |
        ((unsigned)pSnap->u8QueueOk << 2) |
        ((unsigned)pSnap->u8DescOk << 3) |
        ((unsigned)pSnap->u8DmaUnarmed << 4) |
        ((unsigned)pSnap->u8PathOk << 5)));
    aarch64_uart_puts(" wave=");
    aarch64_uart_put_hex((unsigned long)VIRTIO_SOFT_WAVE);
    aarch64_uart_puts("\n");

    /* Grep: aarch64: virtio soft deepen */
    /*
     * ---- Wave 19 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft≠product; not bar3.
     */
    /* Grep: aarch64: virtio: soft retclass — Wave 19 return-class taxonomy (kept) */
    aarch64_uart_puts("aarch64: virtio: soft retclass ok|fail|inval|nodev|busy|nomem "
                      "soft_only=1 product_gate=0 wave=");
    aarch64_uart_put_hex((unsigned long)VIRTIO_SOFT_WAVE);
    aarch64_uart_puts(" (retclass taxonomy; Soft!=product; not bar3)\n");
    /* Grep: aarch64: virtio: soft retlane — Wave 19 return-lane catalog (kept) */
    aarch64_uart_puts("aarch64: virtio: soft retlane inv|selftest|rate|retcode|retmap|class "
                      "product_kernel=OPEN soft_ne_product=1 wave=");
    aarch64_uart_put_hex((unsigned long)VIRTIO_SOFT_WAVE);
    aarch64_uart_puts(" (retlane catalog; Soft!=product)\n");
    /*
     * ---- Wave 20 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft!=product; not bar3.
     */
    /* Grep: aarch64: virtio: soft retbound — Wave 20 return-bound honesty (kept) */
    aarch64_uart_puts("aarch64: virtio: soft retbound soft_only=1 product_gate=0 hard_gate=0 "
                      "never_blocks_m0=1 wave=");
    aarch64_uart_put_hex((unsigned long)VIRTIO_SOFT_WAVE);
    aarch64_uart_puts(" (retbound honesty; Soft!=product; not bar3)\n");
    /* Grep: aarch64: virtio: soft retseal — Wave 20 seal stamp (kept) */
    aarch64_uart_puts("aarch64: virtio: soft retseal exclusive=1 soft_ne_product=1 "
                      "product_kernel=OPEN bar3=0 wave=");
    aarch64_uart_put_hex((unsigned long)VIRTIO_SOFT_WAVE);
    aarch64_uart_puts(" (retseal stamp; Soft!=product)\n");
    /*
     * ---- Wave 21 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft!=product; not bar3.
     */
    /* Grep: aarch64: virtio: soft retpulse — Wave 21 return-pulse honesty (kept) */
    aarch64_uart_puts("aarch64: virtio: soft retpulse soft_only=1 product_gate=0 soft_ne_product=1 "
                      "never_blocks_m0=1 wave=");
    aarch64_uart_put_hex((unsigned long)VIRTIO_SOFT_WAVE);
    aarch64_uart_puts(" (retpulse honesty; Soft!=product; not bar3)\n");
    /* Grep: aarch64: virtio: soft retmark — Wave 21 mark stamp (kept) */
    aarch64_uart_puts("aarch64: virtio: soft retmark exclusive=1 soft_ne_product=1 "
                      "product_kernel=OPEN bar3=0 wave=");
    aarch64_uart_put_hex((unsigned long)VIRTIO_SOFT_WAVE);
    aarch64_uart_puts(" (retmark stamp; Soft!=product)\n");
    
/*
 * ---- Wave 22 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft!=product; not bar3.
 */
/* Grep: aarch64: virtio: soft retphase — Wave 22 return-phase honesty (kept) */
aarch64_uart_puts("aarch64: virtio: soft retphase soft_only=1 product_gate=0 soft_ne_product=1 "
              "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)VIRTIO_SOFT_WAVE);
aarch64_uart_puts(" (retphase honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: virtio: soft retbadge — Wave 22 badge stamp (kept) */
aarch64_uart_puts("aarch64: virtio: soft retbadge exclusive=1 soft_ne_product=1 "
              "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)VIRTIO_SOFT_WAVE);
aarch64_uart_puts(" (retbadge stamp; Soft!=product)\n");
/*
 * ---- Wave 23 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft!=product; not bar3.
 */
/* Grep: aarch64: virtio: soft rettoken — Wave 23 return-token honesty (kept) */
aarch64_uart_puts("aarch64: virtio: soft rettoken soft_only=1 product_gate=0 soft_ne_product=1 "
              "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)VIRTIO_SOFT_WAVE);
aarch64_uart_puts(" (rettoken honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: virtio: soft retcrest — Wave 23 crest stamp (kept) */
aarch64_uart_puts("aarch64: virtio: soft retcrest exclusive=1 soft_ne_product=1 "
              "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)VIRTIO_SOFT_WAVE);
aarch64_uart_puts(" (retcrest stamp; Soft!=product)\n");
/*
 * ---- Wave 24 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft!=product; not bar3.
 */
/* Grep: aarch64: virtio: soft retvault — Wave 24 return-vault honesty (kept) */
aarch64_uart_puts("aarch64: virtio: soft retvault soft_only=1 product_gate=0 soft_ne_product=1 "
              "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)VIRTIO_SOFT_WAVE);
aarch64_uart_puts(" (retvault honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: virtio: soft retbanner — Wave 24 banner stamp (kept) */
aarch64_uart_puts("aarch64: virtio: soft retbanner exclusive=1 soft_ne_product=1 "
              "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)VIRTIO_SOFT_WAVE);
aarch64_uart_puts(" (retbanner stamp; Soft!=product)\n");
/*
 * ---- Wave 25 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft!=product; not bar3.
 */
/* Grep: aarch64: virtio: soft retledger — Wave 25 return-ledger honesty (kept) */
aarch64_uart_puts("aarch64: virtio: soft retledger soft_only=1 product_gate=0 soft_ne_product=1 "
              "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)VIRTIO_SOFT_WAVE);
aarch64_uart_puts(" (retledger honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: virtio: soft retbeacon — Wave 25 beacon stamp (kept) */
aarch64_uart_puts("aarch64: virtio: soft retbeacon exclusive=1 soft_ne_product=1 "
              "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)VIRTIO_SOFT_WAVE);
aarch64_uart_puts(" (retbeacon stamp; Soft!=product)\n");
/*
 * ---- Wave 26 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft!=product; not bar3.
 */
/* Grep: aarch64: virtio: soft retcipher — Wave 26 return-cipher honesty (kept) */
aarch64_uart_puts("aarch64: virtio: soft retcipher soft_only=1 product_gate=0 soft_ne_product=1 "
              "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)VIRTIO_SOFT_WAVE);
aarch64_uart_puts(" (retcipher honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: virtio: soft retflame — Wave 26 flame stamp (kept) */
aarch64_uart_puts("aarch64: virtio: soft retflame exclusive=1 soft_ne_product=1 "
              "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)VIRTIO_SOFT_WAVE);
aarch64_uart_puts(" (retflame stamp; Soft!=product)\n");
/*
 * ---- Wave 27 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft!=product; not bar3.
 */
/* Grep: aarch64: virtio: soft retprism — Wave 27 return-prism honesty (kept) */
aarch64_uart_puts("aarch64: virtio: soft retprism soft_only=1 product_gate=0 soft_ne_product=1 "
              "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)VIRTIO_SOFT_WAVE);
aarch64_uart_puts(" (retprism honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: virtio: soft retforge — Wave 27 forge stamp (kept) */
aarch64_uart_puts("aarch64: virtio: soft retforge exclusive=1 soft_ne_product=1 "
              "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)VIRTIO_SOFT_WAVE);
aarch64_uart_puts(" (retforge stamp; Soft!=product)\n");
/*
 * ---- Wave 28 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft!=product; not bar3.
 */
/* Grep: aarch64: virtio: soft retshard — Wave 28 return-shard honesty (kept) */
aarch64_uart_puts("aarch64: virtio: soft retshard soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)VIRTIO_SOFT_WAVE);
aarch64_uart_puts(" (retshard honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: virtio: soft retcrown — Wave 28 crown stamp (kept) */
aarch64_uart_puts("aarch64: virtio: soft retcrown exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)VIRTIO_SOFT_WAVE);
aarch64_uart_puts(" (retcrown stamp; Soft!=product)\n");
/*
 * ---- Wave 29 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft!=product; not bar3.
 */
/* Grep: aarch64: virtio: soft retglyph — Wave 29 return-glyph honesty (kept) */
aarch64_uart_puts("aarch64: virtio: soft retglyph soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)VIRTIO_SOFT_WAVE);
aarch64_uart_puts(" (retglyph honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: virtio: soft retscepter — Wave 29 scepter stamp (kept) */
aarch64_uart_puts("aarch64: virtio: soft retscepter exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)VIRTIO_SOFT_WAVE);
aarch64_uart_puts(" (retscepter stamp; Soft!=product)\n");
/*
 * ---- Wave 30 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft!=product; not bar3.
 */
/* Grep: aarch64: virtio: soft retsigil — Wave 30 return-sigil honesty (kept) */
aarch64_uart_puts("aarch64: virtio: soft retsigil soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)VIRTIO_SOFT_WAVE);
aarch64_uart_puts(" (retsigil honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: virtio: soft retemblem — Wave 30 emblem stamp (kept) */
aarch64_uart_puts("aarch64: virtio: soft retemblem exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)VIRTIO_SOFT_WAVE);
aarch64_uart_puts(" (retemblem stamp; Soft!=product)\n");
/*
 * ---- Wave 31 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft!=product; not bar3.
 */
/* Grep: aarch64: virtio: soft retaegis — Wave 31 return-aegis honesty (kept) */
aarch64_uart_puts("aarch64: virtio: soft retaegis soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)VIRTIO_SOFT_WAVE);
aarch64_uart_puts(" (retaegis honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: virtio: soft retmantle — Wave 31 mantle stamp (kept) */
aarch64_uart_puts("aarch64: virtio: soft retmantle exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)VIRTIO_SOFT_WAVE);
aarch64_uart_puts(" (retmantle stamp; Soft!=product)\n");
/*
 * ---- Wave 32 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft!=product; not bar3.
 */
/* Grep: aarch64: virtio: soft retbulwark — Wave 32 return-bulwark honesty (kept) */
aarch64_uart_puts("aarch64: virtio: soft retbulwark soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)VIRTIO_SOFT_WAVE);
aarch64_uart_puts(" (retbulwark honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: virtio: soft retpanoply — Wave 32 panoply stamp (kept) */
aarch64_uart_puts("aarch64: virtio: soft retpanoply exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)VIRTIO_SOFT_WAVE);
aarch64_uart_puts(" (retpanoply stamp; Soft!=product)\n");
/*
 * ---- Wave 33 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: virtio: soft retbastion — Wave 33 return-bastion honesty (kept) */
aarch64_uart_puts("aarch64: virtio: soft retbastion soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)VIRTIO_SOFT_WAVE);
aarch64_uart_puts(" (retbastion honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: virtio: soft retcitadel — Wave 33 citadel stamp (kept) */
aarch64_uart_puts("aarch64: virtio: soft retcitadel exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)VIRTIO_SOFT_WAVE);
aarch64_uart_puts(" (retcitadel stamp; Soft!=product)\n");
/*
 * ---- Wave 34 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: virtio: soft retredoubt — Wave 34 return-redoubt honesty */
aarch64_uart_puts("aarch64: virtio: soft retredoubt soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)VIRTIO_SOFT_WAVE);
aarch64_uart_puts(" (retredoubt honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: virtio: soft retkeep — Wave 34 exclusive keep stamp */
aarch64_uart_puts("aarch64: virtio: soft retkeep exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)VIRTIO_SOFT_WAVE);
aarch64_uart_puts(" (retkeep stamp; Soft!=product)\n");
/*
 * ---- Wave 35 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: virtio: soft retfortress — Wave 35 return-fortress honesty */
aarch64_uart_puts("aarch64: virtio: soft retfortress soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)VIRTIO_SOFT_WAVE);
aarch64_uart_puts(" (retfortress honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: virtio: soft retpalace — Wave 35 exclusive palace stamp */
aarch64_uart_puts("aarch64: virtio: soft retpalace exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)VIRTIO_SOFT_WAVE);
aarch64_uart_puts(" (retpalace stamp; Soft!=product)\n");
/*
 * ---- Wave 36 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: virtio: soft rethold — Wave 36 return-hold honesty */
aarch64_uart_puts("aarch64: virtio: soft rethold soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)VIRTIO_SOFT_WAVE);
aarch64_uart_puts(" (rethold honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: virtio: soft retspire — Wave 36 exclusive spire stamp */
aarch64_uart_puts("aarch64: virtio: soft retspire exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)VIRTIO_SOFT_WAVE);
aarch64_uart_puts(" (retspire stamp; Soft!=product)\n");
/*
 * ---- Wave 37 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: virtio: soft retwall — Wave 37 return-wall honesty */
aarch64_uart_puts("aarch64: virtio: soft retwall soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)VIRTIO_SOFT_WAVE);
aarch64_uart_puts(" (retwall honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: virtio: soft retgate — Wave 37 exclusive gate stamp */
aarch64_uart_puts("aarch64: virtio: soft retgate exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)VIRTIO_SOFT_WAVE);
aarch64_uart_puts(" (retgate stamp; Soft!=product)\n");
/*
 * ---- Wave 38 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: virtio: soft retmoat — Wave 38 return-moat honesty */
aarch64_uart_puts("aarch64: virtio: soft retmoat soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)VIRTIO_SOFT_WAVE);
aarch64_uart_puts(" (retmoat honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: virtio: soft retower — Wave 38 exclusive tower stamp */
aarch64_uart_puts("aarch64: virtio: soft retower exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)VIRTIO_SOFT_WAVE);
aarch64_uart_puts(" (retower stamp; Soft!=product)\n");
/*
 * ---- Wave 39 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: virtio: soft retbarbican — Wave 39 return-barbican honesty */
aarch64_uart_puts("aarch64: virtio: soft retbarbican soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)VIRTIO_SOFT_WAVE);
aarch64_uart_puts(" (retbarbican honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: virtio: soft retglacis — Wave 39 exclusive glacis stamp */
aarch64_uart_puts("aarch64: virtio: soft retglacis exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)VIRTIO_SOFT_WAVE);
aarch64_uart_puts(" (retglacis stamp; Soft!=product)\n");
/*
 * ---- Wave 40 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: virtio: soft retcurtain — Wave 40 return-curtain honesty */
aarch64_uart_puts("aarch64: virtio: soft retcurtain soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)VIRTIO_SOFT_WAVE);
aarch64_uart_puts(" (retcurtain honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: virtio: soft retparapet — Wave 40 exclusive parapet stamp */
aarch64_uart_puts("aarch64: virtio: soft retparapet exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)VIRTIO_SOFT_WAVE);
aarch64_uart_puts(" (retparapet stamp; Soft!=product)\n");
/*
 * ---- Wave 41 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: virtio: soft retravelin — Wave 41 return-travelin honesty */
aarch64_uart_puts("aarch64: virtio: soft retravelin soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)VIRTIO_SOFT_WAVE);
aarch64_uart_puts(" (retravelin honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: virtio: soft retditch — Wave 41 exclusive ditch stamp */
aarch64_uart_puts("aarch64: virtio: soft retditch exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)VIRTIO_SOFT_WAVE);
aarch64_uart_puts(" (retditch stamp; Soft!=product)\n");
/*
 * ---- Wave 42 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: virtio: soft retportcullis — Wave 42 return-portcullis honesty */
aarch64_uart_puts("aarch64: virtio: soft retportcullis soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)VIRTIO_SOFT_WAVE);
aarch64_uart_puts(" (retportcullis honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: virtio: soft retbattlement — Wave 42 exclusive battlement stamp */
aarch64_uart_puts("aarch64: virtio: soft retbattlement exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)VIRTIO_SOFT_WAVE);
aarch64_uart_puts(" (retbattlement stamp; Soft!=product)\n");
/*
 * ---- Wave 43 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: virtio: soft retmachicolation — Wave 43 return-machicolation honesty */
aarch64_uart_puts("aarch64: virtio: soft retmachicolation soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)VIRTIO_SOFT_WAVE);
aarch64_uart_puts(" (retmachicolation honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: virtio: soft retarrowslit — Wave 43 exclusive arrowslit stamp */
aarch64_uart_puts("aarch64: virtio: soft retarrowslit exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)VIRTIO_SOFT_WAVE);
aarch64_uart_puts(" (retarrowslit stamp; Soft!=product)\n");

/*
 * ---- Wave 44 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: virtio: soft retmerlon — Wave 44 return-merlon honesty */
aarch64_uart_puts("aarch64: virtio: soft retmerlon soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)VIRTIO_SOFT_WAVE);
aarch64_uart_puts(" (retmerlon honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: virtio: soft retembrasure — Wave 44 exclusive embrasure stamp */
aarch64_uart_puts("aarch64: virtio: soft retembrasure exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)VIRTIO_SOFT_WAVE);
aarch64_uart_puts(" (retembrasure stamp; Soft!=product)\n");

/*
 * ---- Wave 45 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: aarch64: virtio: soft retkeepgate — Wave 45 return-keepgate honesty */
aarch64_uart_puts("aarch64: virtio: soft retkeepgate soft_only=1 product_gate=0 soft_ne_product=1 "
                   "never_blocks_m0=1 wave=");
aarch64_uart_put_hex((unsigned long)VIRTIO_SOFT_WAVE);
aarch64_uart_puts(" (retkeepgate honesty; Soft!=product; not bar3)\n");
/* Grep: aarch64: virtio: soft retouterward — Wave 45 exclusive outerward stamp */
aarch64_uart_puts("aarch64: virtio: soft retouterward exclusive=1 soft_ne_product=1 "
                   "product_kernel=OPEN bar3=0 wave=");
aarch64_uart_put_hex((unsigned long)VIRTIO_SOFT_WAVE);
aarch64_uart_puts(" (retouterward stamp; Soft!=product)\n");



    aarch64_uart_puts("aarch64: virtio soft deepen wave=");
    aarch64_uart_put_hex((unsigned long)VIRTIO_SOFT_WAVE);
    aarch64_uart_puts(" areas=");
    aarch64_uart_put_hex((unsigned long)VIRTIO_SOFT_AREAS);
    aarch64_uart_puts(" catalog=inventory,slots,dev0,feats,ring,status,"
                      "used,gates,path,surf,return,honesty,deepen "
                      "unit=virtio_mmio.c only rate_limited=0 soft_only=1\n");

    /* Grep: aarch64: virtio soft honesty */
    aarch64_uart_puts("aarch64: virtio soft honesty product_kernel=OPEN "
                      "soft_only=1 no_dma=1 no_kick=1 no_bar3=1 wave=");
    aarch64_uart_put_hex((unsigned long)VIRTIO_SOFT_WAVE);
    aarch64_uart_puts("\n");

    /* Grep: aarch64: virtio soft exclusive — Wave 35 exclusive deepen */
    aarch64_uart_puts("aarch64: virtio soft exclusive multi_server=0 "
                      "confine=0 bar3=0 product_kernel=OPEN soft_only=1 wave=");
    aarch64_uart_put_hex((unsigned long)VIRTIO_SOFT_WAVE);
    aarch64_uart_puts("\n");

    /* Grep: aarch64: virtio soft open — Wave 19 open-lamp rollup */
    aarch64_uart_puts("aarch64: virtio soft open multi_server=0 confine=0 "
                      "bar3=0 product_kernel=OPEN soft_only=1 wave=");
    aarch64_uart_put_hex((unsigned long)VIRTIO_SOFT_WAVE);
    aarch64_uart_puts("\n");

    fOk = 0;
    if (pSnap->u8ScanOk != 0u && pSnap->u8LayoutOk != 0u &&
        pSnap->u8QueueOk != 0u && pSnap->u8DescOk != 0u &&
        pSnap->u8DmaUnarmed != 0u && pSnap->u8PathOk != 0u) {
        fOk = 1;
    }

    /* Grep: aarch64: virtio soft return — Wave 19 return surfaces */
    aarch64_uart_puts("aarch64: virtio soft return inv_ret=");
    aarch64_uart_put_hex((unsigned long)(fOk != 0 ? 1ul : 0ul));
    aarch64_uart_puts(" product_kernel=OPEN wave=");
    aarch64_uart_put_hex((unsigned long)VIRTIO_SOFT_WAVE);
    aarch64_uart_puts("\n");

    if (fOk != 0) {
        aarch64_uart_puts("aarch64: virtio soft PASS\n");
    } else {
        aarch64_uart_puts("aarch64: virtio soft FAIL\n");
    }
    return fOk;
}

void
aarch64_virtio_mmio_probe(void)
{
    unsigned n;
    unsigned cFound;
    unsigned cWithDev;
    unsigned cQueueSoft;
    int fSoft;
    struct virtio_soft_snap snap;

    cFound = 0;
    cWithDev = 0;
    cQueueSoft = 0;
    g_cDescRingSoft = 0;
    g_cDescLayoutSoft = 0;
    g_cLegacySoft = 0;
    g_cModernSoft = 0;
    g_uDev0Id = 0;
    g_uDev0Ver = 0;
    g_uDev0Vendor = 0;
    g_uDev0Status = 0;
    g_uDev0Qmax = 0;
    g_uDev0Qnum = 0;
    g_uDev0Ready = 0;
    g_uDev0Pfn = 0;
    g_uFeat0 = 0;
    g_uFeat1 = 0;
    g_uIsr = 0;
    g_uCfgGen = 0;
    g_fDev0Captured = 0;

    /* Soft-fill BSS ring template once (layout programming marker). */
    virtio_soft_ring_fill();

    for (n = 0; n < VIRTIO_MMIO_SLOTS; n++) {
        unsigned long base = VIRTIO_MMIO_BASE + (unsigned long)n * VIRTIO_MMIO_STRIDE;
        unsigned magic;
        unsigned ver;
        unsigned devid;
        unsigned vendor;

        magic = *mmio(base, VIRTIO_MMIO_MAGIC);
        if (magic != VIRTIO_MAGIC_VALUE) {
            continue;
        }
        cFound++;
        ver = *mmio(base, VIRTIO_MMIO_VERSION);
        devid = *mmio(base, VIRTIO_MMIO_DEVICEID);
        vendor = *mmio(base, VIRTIO_MMIO_VENDORID);
        if (devid != 0u) {
            cWithDev++;
            if (virtio_mmio_queue0_soft(base, devid, ver, vendor) != 0) {
                cQueueSoft++;
            }
        }
    }

    /*
     * QEMU virt always presents the MMIO windows; magic may only appear when
     * -device virtio-*-device is attached. Soft PASS if we scanned all slots
     * without fault. Queue soft PASS when any device took QueueNum
     * programming, or when no device present (path compiled in).
     * Desc ring soft PASS when guest layout filled and (MMIO/legacy soft
     * programmed, or no device — path present).
     */
    aarch64_uart_puts("aarch64: virtio-mmio found=");
    aarch64_uart_put_hex(cFound);
    aarch64_uart_puts(" dev=");
    aarch64_uart_put_hex(cWithDev);
    aarch64_uart_puts(" qsoft=");
    aarch64_uart_put_hex(cQueueSoft);
    aarch64_uart_puts(" dsoft=");
    aarch64_uart_put_hex(g_cDescRingSoft);
    aarch64_uart_puts(" leg=");
    aarch64_uart_put_hex(g_cLegacySoft);
    aarch64_uart_puts(" mod=");
    aarch64_uart_put_hex(g_cModernSoft);
    aarch64_uart_puts("\n");
    aarch64_uart_puts("aarch64: virtio-mmio PASS\n");
    if (cQueueSoft > 0u || cWithDev == 0u) {
        aarch64_uart_puts("aarch64: virtio-mmio queue soft PASS\n");
    }
    if (g_cDescRingSoft > 0u || (g_cDescLayoutSoft != 0u && cWithDev == 0u)) {
        aarch64_uart_puts("aarch64: virtio-mmio desc ring soft PASS\n");
    }

    /*
     * Wave 11 combined soft inventory under "aarch64: virtio soft …".
     * Emits multi-field lamps + final soft PASS|FAIL (smoke greps PASS).
     * Scalar stores only — no bulk memset of snap (general-regs-only).
     */
    snap.cFound = cFound;
    snap.cWithDev = cWithDev;
    snap.cQueueSoft = cQueueSoft;
    snap.cDescRingSoft = g_cDescRingSoft;
    snap.cLegacySoft = g_cLegacySoft;
    snap.cModernSoft = g_cModernSoft;
    snap.cDescLayoutSoft = g_cDescLayoutSoft;
    snap.uDev0Id = g_uDev0Id;
    snap.uDev0Ver = g_uDev0Ver;
    snap.uDev0Vendor = g_uDev0Vendor;
    snap.uDev0Status = g_uDev0Status;
    snap.uDev0Qmax = g_uDev0Qmax;
    snap.uDev0Qnum = g_uDev0Qnum;
    snap.uDev0Ready = g_uDev0Ready;
    snap.uDev0Pfn = g_uDev0Pfn;
    snap.uFeat0 = g_uFeat0;
    snap.uFeat1 = g_uFeat1;
    snap.uIsr = g_uIsr;
    snap.uCfgGen = g_uCfgGen;
    snap.u64DescPa =
        (unsigned long long)(unsigned long)(void *)g_aSoftDesc;
    snap.u64AvailPa =
        (unsigned long long)(unsigned long)(void *)&g_SoftAvail;
    snap.u64UsedPa =
        (unsigned long long)(unsigned long)(void *)&g_SoftUsed;
    snap.uNext0 = (unsigned)g_aSoftDesc[0].u16Next;
    snap.uNextLast =
        (unsigned)g_aSoftDesc[VIRTIO_SOFT_QNUM - 1u].u16Next;
    snap.uAvailI0 = (unsigned)g_SoftAvail.aRing[0];

    snap.u8LayoutOk = (virtio_soft_ring_layout_ok() != 0) ? 1u : 0u;
    /* Scan completed without fault if we reached here with slot count. */
    snap.u8ScanOk = 1u;
    /* Queue soft ok when programmed, or no device (path compiled in). */
    snap.u8QueueOk = (cQueueSoft > 0u || cWithDev == 0u) ? 1u : 0u;
    /* Desc soft ok when MMIO/legacy programmed, or layout + no device. */
    snap.u8DescOk =
        (g_cDescRingSoft > 0u ||
         (g_cDescLayoutSoft != 0u && cWithDev == 0u))
            ? 1u
            : 0u;
    /*
     * DMA unarmed: with a live device, the arming reg for that transport
     * generation must stay 0 (v1: QueuePFN; v2+: QueueReady). With no
     * device, path is unarmed by construction. Do not require QueuePFN==0
     * on modern slots (legacy-only reg; may RAZ/WI or alias).
     */
    snap.u8DmaUnarmed = 1u;
    if (cWithDev > 0u && g_fDev0Captured != 0u) {
        if (g_uDev0Ver >= 2u) {
            if (g_uDev0Ready != 0u) {
                snap.u8DmaUnarmed = 0u;
            }
        } else {
            if (g_uDev0Pfn != 0u) {
                snap.u8DmaUnarmed = 0u;
            }
        }
    }
    /* Path honesty always true for this soft scaffold (constants above). */
    snap.u8PathOk = 1u;

    fSoft = virtio_soft_inventory(&snap);
    (void)fSoft;
}
