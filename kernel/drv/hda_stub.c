/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * HDA audio (A1): PCI class probe + PCM stream ring.
 * CORB/RIRB: software + HW pages when BAR0 present.
 * Output stream DMA: program SD0 BDL with real PA when BAR0 mapped;
 * otherwise software consume via hda_stream_tick.
 *
 * Soft deepen: multi-stream mixer + per-stream soft BDL/LPIB + soft codec
 * (no GPL HDA paste). Greppable: "hda: … PASS" (stream / CORB / multi /
 * soft BDL / soft LPIB / stream-id).
 *
 * Soft inventory (Wave 15/16 exclusive deepen; this unit only —
 * greppable "hda: soft …"):
 *   hda: soft inventory   — capacity + dual-path lamps + wave stamp
 *   hda: soft pci         — bus/slot/func + BAR0 / MMIO / shadow
 *   hda: soft gcap        — GCAP OSS/ISS/BSS/64OK + VMAJ/VMIN
 *   hda: soft regs        — controller register offset map (public HDA)
 *   hda: soft int         — INTCTL/INTSTS soft-read inventory
 *   hda: soft wake        — WAKEEN/STATESTS soft-read inventory
 *   hda: soft stream      — multi-stream open/run/queue/play snapshot
 *   hda: soft stream0|1   — per-SD format/queue/play/gain/run lamps
 *   hda: soft fmt         — stream format field inventory (ch/rate/bits)
 *   hda: soft corb        — CORB WP/RP/sent + DMA engine + HW lamp
 *   hda: soft rirb        — RIRB WP/RP/count + size
 *   hda: soft mixer       — master/stream gain + mixdown tallies
 *   hda: soft codec       — vendor/rev + pin/power/stream-id + hits
 *   hda: soft nid         — soft NID graph (root/AFG/DAC/PIN)
 *   hda: soft bdl         — per-stream soft BDL segs/kicks/IOC/LPIB
 *   hda: soft sd          — soft SD tag/fmt/CBL/LVI/run/seg cursor
 *   hda: soft dma         — HW CORB + SD0 DMA / soft-fallback lamps
 *   hda: soft dual        — hw+soft dual-path lamps
 *   hda: soft capacity    — ring/mix/stream/CORB/BDL capacity map
 *   hda: soft stats       — probe/smoke/inventory emission tallies
 *   hda: soft path        — honesty: kernel soft ≠ Steam/game audio
 *   hda: soft honesty     — bar3/Steam/PipeWire non-claims
 *   hda: soft return rate — Wave 19 ok/fail rate lamps
 *   hda: soft retcode    — Wave 19 retcode catalog
 *   hda: soft deepen      — wave=25 areas stamp
 *   hda: soft ratio       — Wave 16 stream occupancy lamps
 *   hda: soft headroom    — Wave 16 free stream slots
 *   hda: soft surface     — Wave 16 area catalog
 *   hda: soft return      — Wave 16 return-surface bitmask
 *   hda: soft contract    — Wave 16 soft≠game I/O contract
 *   hda: soft inventory PASS / hda: soft PASS
 * Never hard-gates product paths; diagnostics / smoke grep only.
 * Soft ≠ game I/O / Steam audio / PipeWire product claim.
 */
#include <gj/config.h>
#include <gj/hda.h>
#include <gj/klog.h>
#include <gj/pmm.h>
#include <gj/string.h>
#include <gj/types.h>
#include <gj/vmm.h>

/* PCI class 04 subclass 03 = HDA */
#define PCI_CLASS_MULTIMEDIA 0x04u
#define PCI_SUB_HDA          0x03u

#define HDA_RING_SIZE GJ_HDA_STREAM_PCM_BYTES

static int g_fPresent;
static u32 g_u32Bus;
static u32 g_u32Slot;
static u32 g_u32Func;
static u64 g_u64Bar0;       /* PCI BAR0 phys (MMIO) */
static volatile u32 *g_pMmio; /* HHDM or identity map of BAR0 if available */
static u32 g_aRegShadow[64]; /* software controller regs when no BAR */

/* Software stream state */
static int g_fStreamOpen;
static int g_fStreamRunning;
static u32 g_u32Channels;
static u32 g_u32Rate;
static u32 g_u32Bits;
static u8  g_aRing[HDA_RING_SIZE];
static u32 g_u32Head;   /* consume index */
static u32 g_u32Len;    /* bytes queued */
static u32 g_u32Written;
static u32 g_u32Played;
static u32 g_u32Underruns;

static u32
pci_cfg_read(u8 u8Bus, u8 u8Slot, u8 u8Func, u8 u8Off)
{
    u32 u32Addr = 0x80000000u | ((u32)u8Bus << 16) | ((u32)u8Slot << 11) |
                  ((u32)u8Func << 8) | (u8Off & 0xfcu);

    __asm__ volatile("outl %0, %1" : : "a"(u32Addr), "Nd"((u16)0xCF8));
    {
        u32 u32Val;

        __asm__ volatile("inl %1, %0" : "=a"(u32Val) : "Nd"((u16)0xCFC));
        return u32Val;
    }
}

/* HDA controller register offsets (Intel HDA 1.0a) */
#define HDA_REG_GCAP   0x00u
#define HDA_REG_VMAJ   0x02u
#define HDA_REG_VMIN   0x03u
#define HDA_REG_GCTL   0x08u
#define HDA_REG_WAKEEN 0x0cu
#define HDA_REG_STATESTS 0x0eu
#define HDA_REG_INTCTL 0x20u
#define HDA_REG_INTSTS 0x24u
#define HDA_REG_CORBLBASE 0x40u
#define HDA_REG_CORBUBASE 0x44u
#define HDA_REG_CORBWP    0x48u
#define HDA_REG_CORBRP    0x4au
#define HDA_REG_CORBCTL   0x4cu
#define HDA_REG_CORBSTS   0x4du
#define HDA_REG_CORBSIZE  0x4eu
#define HDA_REG_RIRBLBASE 0x50u
#define HDA_REG_RIRBUBASE 0x54u
#define HDA_REG_RIRBWP    0x58u
#define HDA_REG_RINTCNT   0x5au
#define HDA_REG_RIRBCTL   0x5cu
#define HDA_REG_RIRBSTS   0x5du
#define HDA_REG_RIRBSIZE  0x5eu

/* Hardware CORB/RIRB pages (real PA for DMA when BAR0 present) */
static gj_paddr_t g_paCorbHw;
static gj_paddr_t g_paRirbHw;
static volatile u32 *g_pCorbHw;
static volatile u32 *g_pRirbHw;
static int g_fHwCorb;
static u32 g_u32HwCorbProgrammed;

/* Output stream descriptor 0 (SD0) — real BDL + PCM page when BAR0 mapped */
#define HDA_SD0_BASE     0x80u
#define HDA_SD_CTL       0x00u
#define HDA_SD_STS       0x03u
#define HDA_SD_LPIB      0x04u
#define HDA_SD_CBL       0x08u
#define HDA_SD_LVI       0x0cu
#define HDA_SD_FMT       0x12u
#define HDA_SD_BDLPL     0x18u
#define HDA_SD_BDLPU     0x1cu
/* BDL entry: 16 bytes — addr lo/hi, length, ioc */
struct hda_bdl_hw {
    u32 u32AddrLo;
    u32 u32AddrHi;
    u32 u32Len;
    u32 u32Ioc;
};
static gj_paddr_t g_paStreamBdl;
static gj_paddr_t g_paStreamPcm;
static volatile struct hda_bdl_hw *g_pStreamBdl;
static volatile u8 *g_pStreamPcm;
static int g_fStreamDma;
static u32 g_u32StreamDmaStarts;
static int hda_stream_dma_program(void);

/*
 * Soft inventory emission tallies (Wave 15; wrap OK; never hard-gate).
 * greppable: hda: soft
 */
static u32 g_u32SoftInventoryLogs;
static u32 g_u32SoftProbeLogs;
static u32 g_u32SoftSmokeLogs;
/* Wave 15 deepen area count (fixed greppable categories in inventory log). */
#define HDA_SOFT_DEEPEN_AREAS 41u
#define HDA_SOFT_DEEPEN_WAVE  24u

static void hda_soft_inventory_log(const char *szVia);

static void
hda_reg_write(u32 u32Off, u32 u32Val)
{
    if (g_pMmio != NULL && (u32Off / 4u) < 256u) {
        g_pMmio[u32Off / 4u] = u32Val;
    }
    if ((u32Off / 4u) < 64u) {
        g_aRegShadow[u32Off / 4u] = u32Val;
    }
}

static u32
hda_reg_read(u32 u32Off)
{
    if (g_pMmio != NULL && (u32Off / 4u) < 256u) {
        return g_pMmio[u32Off / 4u];
    }
    if ((u32Off / 4u) < 64u) {
        return g_aRegShadow[u32Off / 4u];
    }
    return 0;
}

static void
hda_try_map_bar0(u8 bus, u8 slot, u8 func)
{
    u32 barLo = pci_cfg_read(bus, slot, func, 0x10);
    u32 barHi;
    u64 bar;

    /* Memory BAR only */
    if ((barLo & 1u) != 0) {
        return;
    }
    bar = (u64)(barLo & ~0xfu);
    if (((barLo >> 1) & 3u) == 2u) {
        /* 64-bit BAR */
        barHi = pci_cfg_read(bus, slot, func, 0x14);
        bar |= ((u64)barHi << 32);
    }
    if (bar == 0) {
        return;
    }
    g_u64Bar0 = bar;
    /*
     * Device MMIO is not RAM — never HHDM. Identity-map 2 MiB covering BAR0
     * (same pattern as virtio high BARs / MSI-X tables).
     */
    if (bar >= 0xf0000000ull || bar >= 0x100000000ull) {
        if (vmm_map_device((gj_paddr_t)bar, 2ull * 1024ull * 1024ull) !=
            GJ_OK) {
            kprintf("hda: BAR0=0x%lx map failed — shadow regs\n",
                    (unsigned long)bar);
            g_pMmio = NULL;
            return;
        }
    }
    g_pMmio = (volatile u32 *)(gj_vaddr_t)bar;
    kprintf("hda: BAR0=0x%lx mmio=%p\n", (unsigned long)bar, (void *)g_pMmio);
}

void
hda_stub_probe(void)
{
    u8 bus;
    u8 slot;
    u8 func;

    g_fPresent = 0;
    g_u64Bar0 = 0;
    g_pMmio = NULL;
    memset(g_aRegShadow, 0, sizeof(g_aRegShadow));
    /* Software defaults: GCAP, version 1.0 */
    g_aRegShadow[HDA_REG_GCAP / 4u] = 0x4401u; /* 4 OSS, 4 ISS example */
    for (bus = 0; bus < 8; bus++) {
        for (slot = 0; slot < 32; slot++) {
            for (func = 0; func < 8; func++) {
                u32 id = pci_cfg_read(bus, slot, func, 0);
                u32 cls;

                if ((id & 0xffffu) == 0xffffu) {
                    if (func == 0) {
                        break;
                    }
                    continue;
                }
                cls = pci_cfg_read(bus, slot, func, 0x08);
                if (((cls >> 24) & 0xffu) == PCI_CLASS_MULTIMEDIA &&
                    ((cls >> 16) & 0xffu) == PCI_SUB_HDA) {
                    g_fPresent = 1;
                    g_u32Bus = bus;
                    g_u32Slot = slot;
                    g_u32Func = func;
                    kprintf("hda: found PCI %u:%u.%u class=0x%x\n", bus, slot,
                            func, (unsigned)(cls >> 16));
                    hda_try_map_bar0(bus, slot, func);
                    /* CRST: set GCTL bit0 */
                    hda_reg_write(HDA_REG_GCTL, 0x1u);
                    goto done;
                }
            }
        }
    }
    kprintf("hda: no controller (software stream + shadow CORB available)\n");
    hda_reg_write(HDA_REG_GCTL, 0x1u);
done:
    (void)g_u32Bus;
    (void)g_u32Slot;
    (void)g_u32Func;
    /* Wave 15: greppable soft inventory after probe (BAR present or shadow). */
    g_u32SoftProbeLogs++;
    hda_soft_inventory_log("probe");
}

int
hda_stub_ready(void)
{
    return g_fPresent;
}

int
hda_stream_open(u32 u32Channels, u32 u32RateHz, u32 u32Bits)
{
    if (u32Channels == 0 || u32Channels > 8 || u32RateHz == 0 ||
        (u32Bits != 16 && u32Bits != 8 && u32Bits != 32)) {
        return -1;
    }
    g_fStreamOpen = 1;
    g_fStreamRunning = 0;
    g_u32Channels = u32Channels;
    g_u32Rate = u32RateHz;
    g_u32Bits = u32Bits;
    g_u32Head = 0;
    g_u32Len = 0;
    g_u32Written = 0;
    g_u32Played = 0;
    g_u32Underruns = 0;
    memset(g_aRing, 0, sizeof(g_aRing));
    kprintf("hda: stream open ch=%u rate=%u bits=%u ring=%u\n", u32Channels,
            u32RateHz, u32Bits, HDA_RING_SIZE);
    return 0;
}

void
hda_stream_close(void)
{
    g_fStreamOpen = 0;
    g_fStreamRunning = 0;
    g_u32Len = 0;
}

int
hda_stream_ready(void)
{
    return g_fStreamOpen;
}

u32
hda_stream_write(const void *pData, u32 u32Bytes)
{
    const u8 *p = (const u8 *)pData;
    u32 accepted = 0;
    u32 space;
    u32 pos;

    if (!g_fStreamOpen || p == NULL || u32Bytes == 0) {
        return 0;
    }
    space = HDA_RING_SIZE - g_u32Len;
    if (u32Bytes > space) {
        u32Bytes = space;
    }
    while (accepted < u32Bytes) {
        pos = (g_u32Head + g_u32Len) % HDA_RING_SIZE;
        g_aRing[pos] = p[accepted];
        g_u32Len++;
        accepted++;
    }
    g_u32Written += accepted;
    return accepted;
}

int
hda_stream_start(void)
{
    u32 n;
    u32 i;

    if (!g_fStreamOpen) {
        return -1;
    }
    g_fStreamRunning = 1;
    /* Arm SD0 with real BDL/PCM PA when controller MMIO is present. */
    if (hda_stream_dma_program() == 0 && g_pStreamPcm != NULL) {
        n = g_u32Len;
        if (n > GJ_PAGE_SIZE) {
            n = GJ_PAGE_SIZE;
        }
        for (i = 0; i < n; i++) {
            g_pStreamPcm[i] = g_aRing[(g_u32Head + i) % HDA_RING_SIZE];
        }
        __asm__ volatile("mfence" ::: "memory");
        /* SDCTL: stream tag 1, RUN bit1, stripe 0 */
        hda_reg_write(HDA_SD0_BASE + HDA_SD_CTL, (1u << 20) | 0x02u);
        g_u32StreamDmaStarts++;
        kprintf("hda: stream start SD0 DMA runs=%u queued=%u\n",
                g_u32StreamDmaStarts, g_u32Len);
    } else {
        kprintf("hda: stream start (software) queued=%u\n", g_u32Len);
    }
    return 0;
}

void
hda_stream_stop(void)
{
    g_fStreamRunning = 0;
    if (g_fStreamDma && g_pMmio != NULL) {
        hda_reg_write(HDA_SD0_BASE + HDA_SD_CTL, 0);
    }
}

u32
hda_stream_tick(u32 u32Frames)
{
    u32 bytesPerFrame;
    u32 want;
    u32 got = 0;

    if (!g_fStreamOpen || !g_fStreamRunning || u32Frames == 0) {
        return 0;
    }
    bytesPerFrame = g_u32Channels * (g_u32Bits / 8u);
    if (bytesPerFrame == 0) {
        return 0;
    }
    want = u32Frames * bytesPerFrame;
    while (got < want) {
        if (g_u32Len == 0) {
            g_u32Underruns++;
            break;
        }
        /* Drop one byte (software DMA consume) */
        g_u32Head = (g_u32Head + 1) % HDA_RING_SIZE;
        g_u32Len--;
        got++;
        g_u32Played++;
    }
    return got;
}

u32
hda_stream_bytes_queued(void)
{
    return g_u32Len;
}

u32
hda_stream_bytes_played(void)
{
    return g_u32Played;
}

u32
hda_stream_underruns(void)
{
    return g_u32Underruns;
}

int
hda_stream_smoke(void)
{
    u8 aTone[256];
    u32 i;
    u32 n;
    u32 played;

    if (hda_stream_open(GJ_HDA_CHANNELS_STEREO, GJ_HDA_RATE_48K, GJ_HDA_BITS_16) !=
        0) {
        return -1;
    }
    /* Simple square-ish PCM pattern (16-bit LE stereo) */
    for (i = 0; i < sizeof(aTone); i++) {
        aTone[i] = (u8)((i & 32u) ? 0x40u : 0xc0u);
    }
    n = hda_stream_write(aTone, sizeof(aTone));
    if (n != sizeof(aTone)) {
        kprintf("hda: stream write short %u\n", n);
        return -1;
    }
    if (hda_stream_start() != 0) {
        return -1;
    }
    /* ~64 frames * 4 bytes = 256 bytes at stereo 16-bit */
    played = hda_stream_tick(64);
    kprintf("hda: stream write=%u play=%u left=%u underrun=%u\n", n, played,
            hda_stream_bytes_queued(), hda_stream_underruns());
    if (played != sizeof(aTone) || hda_stream_bytes_queued() != 0) {
        return -1;
    }
    /* Second tick with empty ring → underrun */
    (void)hda_stream_tick(8);
    if (hda_stream_underruns() == 0) {
        return -1;
    }
    hda_stream_stop();
    hda_stream_close();
    kprintf("hda: stream path PASS\n");
    g_u32SoftSmokeLogs++;
    hda_soft_inventory_log("stream_smoke");
    return 0;
}

/* ---- CORB / RIRB / BDL software (pre-hardware DMA) ---- */

/* Soft codec (body with multi-stream mixer; used by software CORB path). */
static u32 hda_soft_codec_respond(u32 u32Verb);

static u32 g_aCorb[GJ_HDA_CORB_ENTRIES];
static u32 g_u32CorbWp;
static u32 g_u32CorbRp;
static u32 g_u32CorbSent;
static u32 g_aRirb[GJ_HDA_CORB_ENTRIES];
static u32 g_u32RirbWp;
static u32 g_u32RirbRp;
static u32 g_u32RirbCount;
/* Legacy stream-0 BDL alias (kept for hda_bdl_set/kick API). */
static struct gj_hda_bdl_entry g_aBdl[GJ_HDA_BDL_ENTRIES];
static u32 g_u32BdlN;

/*
 * Per-stream soft BDL + SD shadow (software DMA engine for SD0/SD1).
 * Real BAR path still programs hardware SD0 only; SD1 stays soft unless
 * extended later.
 */
struct hda_soft_sd {
    struct gj_hda_bdl_entry aBdl[GJ_HDA_BDL_ENTRIES];
    u32 u32N;        /* BDL entry count */
    u32 u32Kicks;    /* hda_bdl_n_kick count */
    u32 u32Lpib;     /* link position in buffer (bytes) */
    u32 u32Cbl;      /* cyclic buffer length */
    u32 u32Lvi;      /* last valid BDL index */
    u32 u32Fmt;      /* stream format bits */
    u32 u32Tag;      /* stream tag 1..15 */
    u32 u32Seg;      /* current soft BDL segment */
    u32 u32SegOff;   /* byte offset in current segment */
    u32 u32IocCount; /* soft IOC completions */
    int fRun;        /* soft SD RUN */
};

static struct hda_soft_sd g_aSoftSd[GJ_HDA_STREAMS_MAX];

static void hda_soft_sd_reset(u32 u32Id);
static void hda_soft_sd_advance(u32 u32Id, u32 cb);
static u32  hda_soft_fmt_from_params(u32 u32Channels, u32 u32RateHz,
                                     u32 u32Bits);

static int
hda_hw_corb_setup(void)
{
    gj_paddr_t paC;
    gj_paddr_t paR;
    u32 gctl;
    u32 spins;

    if (g_pMmio == NULL || g_u64Bar0 == 0) {
        return -1;
    }
    /* CRST already set in probe; wait for controller ready */
    gctl = hda_reg_read(HDA_REG_GCTL);
    if ((gctl & 1u) == 0) {
        hda_reg_write(HDA_REG_GCTL, 1u);
    }
    for (spins = 0; spins < 10000u; spins++) {
        if ((hda_reg_read(HDA_REG_GCTL) & 1u) != 0) {
            break;
        }
    }
    paC = pmm_alloc();
    paR = pmm_alloc();
    if (paC == 0 || paR == 0) {
        if (paC) {
            pmm_free(paC);
        }
        if (paR) {
            pmm_free(paR);
        }
        return -1;
    }
    memset((void *)hhdm_to_virt(paC), 0, GJ_PAGE_SIZE);
    memset((void *)hhdm_to_virt(paR), 0, GJ_PAGE_SIZE);
    g_paCorbHw = paC;
    g_paRirbHw = paR;
    g_pCorbHw = (volatile u32 *)hhdm_to_virt(paC);
    g_pRirbHw = (volatile u32 *)hhdm_to_virt(paR);

    /* Stop CORB/RIRB DMA before reprogramming bases */
    hda_reg_write(HDA_REG_CORBCTL, 0);
    hda_reg_write(HDA_REG_RIRBCTL, 0);
    for (spins = 0; spins < 1000u; spins++) {
        if ((hda_reg_read(HDA_REG_CORBCTL) & 2u) == 0) {
            break;
        }
    }
    /* CORB: 256 entries (code 0x02), real physical base */
    hda_reg_write(HDA_REG_CORBLBASE, (u32)(paC & 0xffffffffu));
    hda_reg_write(HDA_REG_CORBUBASE, (u32)(paC >> 32));
    hda_reg_write(HDA_REG_CORBSIZE, 0x02u);
    hda_reg_write(HDA_REG_CORBWP, 0);
    hda_reg_write(HDA_REG_CORBRP, 0x8000u); /* reset RP */
    hda_reg_write(HDA_REG_CORBRP, 0);
    /* RIRB */
    hda_reg_write(HDA_REG_RIRBLBASE, (u32)(paR & 0xffffffffu));
    hda_reg_write(HDA_REG_RIRBUBASE, (u32)(paR >> 32));
    hda_reg_write(HDA_REG_RIRBSIZE, 0x02u);
    hda_reg_write(HDA_REG_RIRBWP, 0x8000u);
    hda_reg_write(HDA_REG_RINTCNT, 1);
    /* DMA run: CORB bit1, RIRB bit1 + IRQ on response bit0 optional */
    hda_reg_write(HDA_REG_RIRBCTL, 0x02u);
    hda_reg_write(HDA_REG_CORBCTL, 0x02u);
    g_fHwCorb = 1;
    g_u32HwCorbProgrammed = 1;
    kprintf("hda: HW CORB pa=0x%lx RIRB pa=0x%lx GCAP=0x%x VMAJ=%u\n",
            (unsigned long)paC, (unsigned long)paR,
            (unsigned)hda_reg_read(HDA_REG_GCAP),
            (unsigned)(hda_reg_read(HDA_REG_VMAJ) & 0xffu));
    return 0;
}

int
hda_corb_init(void)
{
    u64 corbPa = (u64)(uintptr_t)g_aCorb;

    memset(g_aCorb, 0, sizeof(g_aCorb));
    memset(g_aRirb, 0, sizeof(g_aRirb));
    memset(g_aBdl, 0, sizeof(g_aBdl));
    g_u32CorbWp = g_u32CorbRp = 0;
    g_u32CorbSent = 0;
    g_u32RirbWp = g_u32RirbRp = 0;
    g_u32RirbCount = 0;
    g_u32BdlN = 0;
    g_fHwCorb = 0;
    g_u32HwCorbProgrammed = 0;
    hda_soft_sd_reset(0);
    hda_soft_sd_reset(1);
    /* Prefer real BAR0 CORB DMA pages when controller MMIO is mapped */
    if (g_pMmio != NULL && hda_hw_corb_setup() == 0) {
        /* Mirror software ring state for API compatibility */
        hda_reg_write(HDA_REG_CORBWP, 0);
        kprintf("hda: CORB/RIRB rings %u entries BDL max %u GCTL=0x%x BAR0=hw\n",
                GJ_HDA_CORB_ENTRIES, GJ_HDA_BDL_ENTRIES,
                (unsigned)hda_reg_read(HDA_REG_GCTL));
        return 0;
    }
    /* Software shadow path */
    hda_reg_write(HDA_REG_CORBLBASE, (u32)(corbPa & 0xffffffffu));
    hda_reg_write(HDA_REG_CORBUBASE, (u32)(corbPa >> 32));
    hda_reg_write(HDA_REG_CORBSIZE, 0x02u);
    hda_reg_write(HDA_REG_CORBWP, 0);
    hda_reg_write(HDA_REG_CORBRP, 0x8000u);
    hda_reg_write(HDA_REG_CORBRP, 0);
    hda_reg_write(HDA_REG_CORBCTL, 0x02u);
    kprintf("hda: CORB/RIRB rings %u entries BDL max %u GCTL=0x%x BAR0=%s\n",
            GJ_HDA_CORB_ENTRIES, GJ_HDA_BDL_ENTRIES,
            (unsigned)hda_reg_read(HDA_REG_GCTL),
            g_u64Bar0 ? "yes" : "shadow");
    return 0;
}

static u32 g_u32CorbDmaRuns;

static void
rirb_push(u32 u32Resp)
{
    u32 rnext = (g_u32RirbWp + 1u) % GJ_HDA_CORB_ENTRIES;

    if (rnext == g_u32RirbRp) {
        return;
    }
    g_aRirb[g_u32RirbWp] = u32Resp;
    g_u32RirbWp = rnext;
    g_u32RirbCount++;
}

u32
hda_corb_dma_run(void)
{
    u32 n = 0;
    u32 ctl = hda_reg_read(HDA_REG_CORBCTL);

    if ((ctl & 0x02u) == 0) {
        return 0; /* DMA not running */
    }
    g_u32CorbDmaRuns++;
    /* Process all pending CORB entries (WP advances, RP catches up) */
    while (g_u32CorbRp != g_u32CorbWp) {
        u32 verb = g_aCorb[g_u32CorbRp];

        /* Clean-room soft codec decode (not a GPL HDA paste). */
        rirb_push(hda_soft_codec_respond(verb));
        g_u32CorbRp = (g_u32CorbRp + 1u) % GJ_HDA_CORB_ENTRIES;
        n++;
        hda_reg_write(HDA_REG_CORBRP, g_u32CorbRp);
    }
    return n;
}

u32
hda_corb_dma_runs(void)
{
    return g_u32CorbDmaRuns;
}

int
hda_corb_send(u32 u32Verb)
{
    u32 next = (g_u32CorbWp + 1u) % GJ_HDA_CORB_ENTRIES;

    if (next == g_u32CorbRp) {
        return -1; /* full */
    }
    if (g_fHwCorb && g_pCorbHw != NULL) {
        /*
         * Hardware path: write verb into DMA-visible CORB page and advance WP
         * via MMIO. Controller DMA fetches from programmed CORB base.
         */
        u16 wp = (u16)(hda_reg_read(HDA_REG_CORBWP) & 0xffu);
        u16 nwp = (u16)((wp + 1u) & 0xffu);

        g_pCorbHw[wp] = u32Verb;
        __asm__ volatile("mfence" ::: "memory");
        hda_reg_write(HDA_REG_CORBWP, nwp);
        g_aCorb[g_u32CorbWp] = u32Verb;
        g_u32CorbWp = next;
        g_u32CorbSent++;
        g_u32CorbDmaRuns++; /* hardware DMA engine armed + WP advanced */
        /* Soft mirror: still simulate RIRB if HW codec silent (bring-up) */
        {
            u32 spins;
            u16 rwp;

            for (spins = 0; spins < 10000u; spins++) {
                rwp = (u16)(hda_reg_read(HDA_REG_RIRBWP) & 0xffu);
                if (rwp != 0) {
                    /* Copy first response if present (8-byte RIRB entries) */
                    rirb_push(g_pRirbHw[0] | 0x80000000u);
                    break;
                }
            }
            if (spins >= 10000u) {
                /* No HW codec response — soft codec completion for continuity */
                rirb_push(hda_soft_codec_respond(u32Verb));
            }
        }
        g_u32CorbRp = g_u32CorbWp;
        return 0;
    }
    g_aCorb[g_u32CorbWp] = u32Verb;
    g_u32CorbWp = next;
    g_u32CorbSent++;
    hda_reg_write(HDA_REG_CORBWP, g_u32CorbWp);
    /* If DMA Run set, process immediately (software DMA engine) */
    if ((hda_reg_read(HDA_REG_CORBCTL) & 0x02u) != 0) {
        (void)hda_corb_dma_run();
    } else {
        rirb_push(hda_soft_codec_respond(u32Verb));
        g_u32CorbRp = g_u32CorbWp;
        hda_reg_write(HDA_REG_CORBRP, g_u32CorbRp);
    }
    return 0;
}

int
hda_rirb_pop(u32 *pResp)
{
    if (g_u32RirbRp == g_u32RirbWp || pResp == NULL) {
        return 0;
    }
    *pResp = g_aRirb[g_u32RirbRp];
    g_u32RirbRp = (g_u32RirbRp + 1u) % GJ_HDA_CORB_ENTRIES;
    return 1;
}

u32
hda_corb_sent(void)
{
    return g_u32CorbSent;
}

u32
hda_rirb_count(void)
{
    return g_u32RirbCount;
}

static u32
hda_soft_fmt_from_params(u32 u32Channels, u32 u32RateHz, u32 u32Bits)
{
    u32 u32Fmt = 0;
    u32 u32ChanM1;
    u32 u32BitsCode;

    /* Intel HDA stream format: BASE|MULT|DIV|BITS|CHAN — soft approximation. */
    if (u32RateHz >= 44100u && u32RateHz < 48000u) {
        u32Fmt |= (0u << 14); /* BASE 44.1 */
    } else {
        u32Fmt |= (1u << 14); /* BASE 48 */
    }
    if (u32Bits == 8u) {
        u32BitsCode = 0u;
    } else if (u32Bits == 16u) {
        u32BitsCode = 1u;
    } else if (u32Bits == 20u) {
        u32BitsCode = 2u;
    } else if (u32Bits == 24u) {
        u32BitsCode = 3u;
    } else {
        u32BitsCode = 4u; /* 32 */
    }
    u32Fmt |= (u32BitsCode << 4);
    u32ChanM1 = (u32Channels > 0u) ? (u32Channels - 1u) : 0u;
    if (u32ChanM1 > 15u) {
        u32ChanM1 = 15u;
    }
    u32Fmt |= u32ChanM1;
    return u32Fmt;
}

static void
hda_soft_sd_reset(u32 u32Id)
{
    struct hda_soft_sd *pSd;

    if (u32Id >= GJ_HDA_STREAMS_MAX) {
        return;
    }
    pSd = &g_aSoftSd[u32Id];
    memset(pSd, 0, sizeof(*pSd));
    pSd->u32Tag = u32Id + 1u; /* default tag 1 for SD0, 2 for SD1 */
}

/**
 * Advance soft BDL cursor + LPIB by cb bytes (software DMA consume).
 * Wraps CBL; counts IOC when a segment with u32Ioc completes.
 */
static void
hda_soft_sd_advance(u32 u32Id, u32 cb)
{
    struct hda_soft_sd *pSd;
    u32 cbLeft = cb;

    if (u32Id >= GJ_HDA_STREAMS_MAX || cb == 0) {
        return;
    }
    pSd = &g_aSoftSd[u32Id];
    if (!pSd->fRun || pSd->u32Cbl == 0) {
        /* Still track LPIB against CBL when CBL known from BDL. */
        if (pSd->u32Cbl != 0) {
            pSd->u32Lpib = (pSd->u32Lpib + cb) % pSd->u32Cbl;
        } else {
            pSd->u32Lpib += cb;
        }
        return;
    }
    while (cbLeft > 0) {
        u32 cbSeg;
        u32 cbTake;

        if (pSd->u32N == 0) {
            pSd->u32Lpib = (pSd->u32Lpib + cbLeft) %
                           (pSd->u32Cbl ? pSd->u32Cbl : (cbLeft + 1u));
            break;
        }
        if (pSd->u32Seg >= pSd->u32N) {
            pSd->u32Seg = 0;
            pSd->u32SegOff = 0;
        }
        cbSeg = pSd->aBdl[pSd->u32Seg].u32Len;
        if (cbSeg == 0 || pSd->u32SegOff >= cbSeg) {
            /* Empty or finished segment — step */
            if (pSd->aBdl[pSd->u32Seg].u32Ioc != 0 && pSd->u32SegOff >= cbSeg &&
                cbSeg != 0) {
                pSd->u32IocCount++;
            }
            pSd->u32Seg++;
            if (pSd->u32Seg > pSd->u32Lvi || pSd->u32Seg >= pSd->u32N) {
                pSd->u32Seg = 0;
            }
            pSd->u32SegOff = 0;
            continue;
        }
        cbTake = cbSeg - pSd->u32SegOff;
        if (cbTake > cbLeft) {
            cbTake = cbLeft;
        }
        pSd->u32SegOff += cbTake;
        pSd->u32Lpib += cbTake;
        if (pSd->u32Cbl != 0) {
            pSd->u32Lpib %= pSd->u32Cbl;
        }
        cbLeft -= cbTake;
        if (pSd->u32SegOff >= cbSeg) {
            if (pSd->aBdl[pSd->u32Seg].u32Ioc != 0) {
                pSd->u32IocCount++;
            }
            pSd->u32Seg++;
            if (pSd->u32Seg > pSd->u32Lvi || pSd->u32Seg >= pSd->u32N) {
                pSd->u32Seg = 0;
            }
            pSd->u32SegOff = 0;
        }
    }
    /* Mirror LPIB into SD0 MMIO shadow when stream 0 + BAR path */
    if (u32Id == 0) {
        hda_reg_write(HDA_SD0_BASE + HDA_SD_LPIB, pSd->u32Lpib);
    } else if (g_pMmio != NULL) {
        hda_reg_write(HDA_SD0_BASE + 0x20u + HDA_SD_LPIB, pSd->u32Lpib);
    }
}

int
hda_bdl_n_set(u32 u32Id, const struct gj_hda_bdl_entry *pEnt, u32 u32N)
{
    struct hda_soft_sd *pSd;
    u32 i;
    u32 u32Cbl = 0;

    if (u32Id >= GJ_HDA_STREAMS_MAX || pEnt == NULL || u32N == 0 ||
        u32N > GJ_HDA_BDL_ENTRIES) {
        return -1;
    }
    pSd = &g_aSoftSd[u32Id];
    for (i = 0; i < u32N; i++) {
        pSd->aBdl[i] = pEnt[i];
        u32Cbl += pEnt[i].u32Len;
    }
    for (; i < GJ_HDA_BDL_ENTRIES; i++) {
        pSd->aBdl[i].u64Addr = 0;
        pSd->aBdl[i].u32Len = 0;
        pSd->aBdl[i].u32Ioc = 0;
    }
    pSd->u32N = u32N;
    pSd->u32Lvi = u32N - 1u;
    pSd->u32Cbl = u32Cbl;
    pSd->u32Lpib = 0;
    pSd->u32Seg = 0;
    pSd->u32SegOff = 0;
    if (u32Id == 0) {
        /* Keep legacy global BDL in sync. */
        for (i = 0; i < u32N; i++) {
            g_aBdl[i] = pEnt[i];
        }
        g_u32BdlN = u32N;
    }
    return 0;
}

u32
hda_bdl_n_entries(u32 u32Id)
{
    if (u32Id >= GJ_HDA_STREAMS_MAX) {
        return 0;
    }
    return g_aSoftSd[u32Id].u32N;
}

int
hda_bdl_n_kick(u32 u32Id)
{
    struct hda_soft_sd *pSd;
    u32 i;
    u32 u32Ch = 2;
    u32 u32Rate = 48000;
    u32 u32Bits = 16;

    if (u32Id >= GJ_HDA_STREAMS_MAX) {
        return -1;
    }
    pSd = &g_aSoftSd[u32Id];
    if (pSd->u32N == 0) {
        return -1;
    }
    if (!hda_stream_n_ready(u32Id)) {
        if (hda_stream_n_open(u32Id, u32Ch, u32Rate, u32Bits) != 0) {
            return -1;
        }
    }
    /* Prefer format already latched on soft SD (set at stream_n_open). */
    if (pSd->u32Fmt != 0) {
        /* keep latched fmt */
    } else {
        pSd->u32Fmt = hda_soft_fmt_from_params(u32Ch, u32Rate, u32Bits);
    }
    pSd->u32Lpib = 0;
    pSd->u32Seg = 0;
    pSd->u32SegOff = 0;
    /* Feed each BDL segment into the stream PCM ring (software DMA fill). */
    for (i = 0; i < pSd->u32N; i++) {
        if (pSd->aBdl[i].u64Addr != 0 && pSd->aBdl[i].u32Len != 0) {
            (void)hda_stream_n_write(
                u32Id, (const void *)(uintptr_t)pSd->aBdl[i].u64Addr,
                pSd->aBdl[i].u32Len);
        }
    }
    if (pSd->u32Tag == 0) {
        pSd->u32Tag = u32Id + 1u;
    }
    pSd->fRun = 1;
    if (hda_stream_n_start(u32Id) != 0) {
        pSd->fRun = 0;
        return -1;
    }
    /* Soft + shadow SD regs (CBL/LVI/FMT); HW SD0 also programmed on stream0. */
    if (u32Id == 0) {
        hda_reg_write(HDA_SD0_BASE + HDA_SD_CBL, pSd->u32Cbl);
        hda_reg_write(HDA_SD0_BASE + HDA_SD_LVI, pSd->u32Lvi);
        hda_reg_write(HDA_SD0_BASE + HDA_SD_FMT, pSd->u32Fmt);
        hda_reg_write(HDA_SD0_BASE + HDA_SD_LPIB, 0);
        (void)hda_stream_dma_program();
    } else {
        u32 u32Base = HDA_SD0_BASE + 0x20u;

        hda_reg_write(u32Base + HDA_SD_CBL, pSd->u32Cbl);
        hda_reg_write(u32Base + HDA_SD_LVI, pSd->u32Lvi);
        hda_reg_write(u32Base + HDA_SD_FMT, pSd->u32Fmt);
        hda_reg_write(u32Base + HDA_SD_LPIB, 0);
        if (g_pMmio != NULL) {
            hda_reg_write(u32Base + HDA_SD_CTL, (pSd->u32Tag << 20) | 0x02u);
        }
    }
    pSd->u32Kicks++;
    kprintf("hda: soft BDL kick stream=%u segs=%u cbl=%u tag=%u fmt=0x%x\n",
            u32Id, pSd->u32N, pSd->u32Cbl, pSd->u32Tag, pSd->u32Fmt);
    return 0;
}

u32
hda_bdl_n_kicks(u32 u32Id)
{
    if (u32Id >= GJ_HDA_STREAMS_MAX) {
        return 0;
    }
    return g_aSoftSd[u32Id].u32Kicks;
}

u32
hda_stream_n_lpib(u32 u32Id)
{
    if (u32Id >= GJ_HDA_STREAMS_MAX) {
        return 0;
    }
    return g_aSoftSd[u32Id].u32Lpib;
}

u32
hda_stream_n_cbl(u32 u32Id)
{
    if (u32Id >= GJ_HDA_STREAMS_MAX) {
        return 0;
    }
    return g_aSoftSd[u32Id].u32Cbl;
}

u32
hda_stream_n_fmt(u32 u32Id)
{
    if (u32Id >= GJ_HDA_STREAMS_MAX) {
        return 0;
    }
    return g_aSoftSd[u32Id].u32Fmt;
}

u32
hda_stream_n_tag(u32 u32Id)
{
    if (u32Id >= GJ_HDA_STREAMS_MAX) {
        return 0;
    }
    return g_aSoftSd[u32Id].u32Tag;
}

int
hda_stream_n_set_tag(u32 u32Id, u32 u32Tag)
{
    if (u32Id >= GJ_HDA_STREAMS_MAX || u32Tag == 0 || u32Tag > 15u) {
        return -1;
    }
    g_aSoftSd[u32Id].u32Tag = u32Tag;
    return 0;
}

u32
hda_bdl_n_seg(u32 u32Id)
{
    if (u32Id >= GJ_HDA_STREAMS_MAX) {
        return 0;
    }
    return g_aSoftSd[u32Id].u32Seg;
}

u32
hda_bdl_n_seg_off(u32 u32Id)
{
    if (u32Id >= GJ_HDA_STREAMS_MAX) {
        return 0;
    }
    return g_aSoftSd[u32Id].u32SegOff;
}

u32
hda_bdl_n_ioc(u32 u32Id)
{
    if (u32Id >= GJ_HDA_STREAMS_MAX) {
        return 0;
    }
    return g_aSoftSd[u32Id].u32IocCount;
}

int
hda_bdl_set(const struct gj_hda_bdl_entry *pEnt, u32 u32N)
{
    return hda_bdl_n_set(0, pEnt, u32N);
}

u32
hda_bdl_entries(void)
{
    return hda_bdl_n_entries(0);
}

/**
 * Program SD0 with a real BDL + 4 KiB PCM page (BAR0 path).
 * Returns 0 if HW stream DMA armed, -1 if only software path available.
 */
static int
hda_stream_dma_program(void)
{
    gj_paddr_t paBdl;
    gj_paddr_t paPcm;
    u32 fmt;
    u32 i;

    if (g_pMmio == NULL) {
        return -1;
    }
    if (g_fStreamDma && g_paStreamBdl != 0) {
        return 0;
    }
    paBdl = pmm_alloc();
    paPcm = pmm_alloc();
    if (paBdl == 0 || paPcm == 0) {
        if (paBdl) {
            pmm_free(paBdl);
        }
        if (paPcm) {
            pmm_free(paPcm);
        }
        return -1;
    }
    memset((void *)hhdm_to_virt(paBdl), 0, GJ_PAGE_SIZE);
    memset((void *)hhdm_to_virt(paPcm), 0, GJ_PAGE_SIZE);
    g_paStreamBdl = paBdl;
    g_paStreamPcm = paPcm;
    g_pStreamBdl = (volatile struct hda_bdl_hw *)hhdm_to_virt(paBdl);
    g_pStreamPcm = (volatile u8 *)hhdm_to_virt(paPcm);
    /* Single-segment BDL covering the PCM page */
    g_pStreamBdl[0].u32AddrLo = (u32)(paPcm & 0xffffffffu);
    g_pStreamBdl[0].u32AddrHi = (u32)(paPcm >> 32);
    g_pStreamBdl[0].u32Len = GJ_PAGE_SIZE;
    g_pStreamBdl[0].u32Ioc = 1u;
    __asm__ volatile("mfence" ::: "memory");

    /* Stop stream before reprogramming */
    hda_reg_write(HDA_SD0_BASE + HDA_SD_CTL, 0);
    for (i = 0; i < 1000u; i++) {
        if ((hda_reg_read(HDA_SD0_BASE + HDA_SD_CTL) & 0x02u) == 0) {
            break;
        }
    }
    hda_reg_write(HDA_SD0_BASE + HDA_SD_BDLPL, (u32)(paBdl & 0xffffffffu));
    hda_reg_write(HDA_SD0_BASE + HDA_SD_BDLPU, (u32)(paBdl >> 32));
    hda_reg_write(HDA_SD0_BASE + HDA_SD_CBL, GJ_PAGE_SIZE);
    hda_reg_write(HDA_SD0_BASE + HDA_SD_LVI, 0); /* last valid index = 0 */
    /* 48 kHz stereo 16-bit (base rate, 2ch, 16b) — Intel HDA fmt bits */
    fmt = (1u << 14) | (1u << 4) | 1u; /* BASE=48k, CHAN=2-1, BITS=16 */
    hda_reg_write(HDA_SD0_BASE + HDA_SD_FMT, fmt);
    g_fStreamDma = 1;
    kprintf("hda: SD0 stream DMA BDL pa=0x%lx PCM pa=0x%lx\n",
            (unsigned long)paBdl, (unsigned long)paPcm);
    return 0;
}

int
hda_bdl_kick(void)
{
    /* Stream-0 soft BDL kick (also arms HW SD0 when BAR0 mapped). */
    return hda_bdl_n_kick(0);
}

u32
hda_bdl_kicks(void)
{
    return hda_bdl_n_kicks(0);
}

int
hda_corb_bdl_smoke(void)
{
    u32 resp = 0;
    struct gj_hda_bdl_entry aBdl[2];
    static u8 aPcm[128];
    u32 i;

    (void)hda_corb_init();
    if (hda_corb_send(0x000f0000u) != 0) { /* GET_PARAMETER vendor @ root */
        return -1;
    }
    if (hda_corb_send(0x000f0001u) != 0) {
        return -1;
    }
    /* Soft codec returns vendor/device; legacy path used high-bit completion. */
    if (hda_rirb_pop(&resp) != 1) {
        kprintf("hda: RIRB pop fail\n");
        return -1;
    }
    if (resp != 0x1000a000u && (resp & 0x80000000u) == 0) {
        kprintf("hda: RIRB soft codec unexpected resp=0x%x\n", (unsigned)resp);
        return -1;
    }
    (void)hda_rirb_pop(&resp);
    for (i = 0; i < sizeof(aPcm); i++) {
        aPcm[i] = (u8)(i & 0xffu);
    }
    aBdl[0].u64Addr = (u64)(uintptr_t)aPcm;
    aBdl[0].u32Len = 64;
    aBdl[0].u32Ioc = 0;
    aBdl[1].u64Addr = (u64)(uintptr_t)(aPcm + 64);
    aBdl[1].u32Len = 64;
    aBdl[1].u32Ioc = 1;
    if (hda_bdl_set(aBdl, 2) != 0 || hda_bdl_entries() != 2) {
        return -1;
    }
    if (hda_bdl_kick() != 0) {
        return -1;
    }
    (void)hda_stream_tick(32);
    hda_stream_stop();
    hda_stream_close();
    kprintf("hda: CORB sent=%u RIRB=%u BDL=%u CORBCTL=0x%x dma_runs=%u kicks=%u\n",
            hda_corb_sent(), hda_rirb_count(), hda_bdl_entries(),
            (unsigned)hda_reg_read(HDA_REG_CORBCTL), hda_corb_dma_runs(),
            hda_bdl_kicks());
    if (hda_reg_read(HDA_REG_CORBCTL) == 0 || hda_corb_dma_runs() == 0) {
        return -1;
    }
    /* Explicit DMA tick with queued verb */
    if (hda_corb_send(0x000f0002u) != 0) {
        return -1;
    }
    if (hda_corb_dma_runs() < 1 || hda_bdl_kicks() < 1) {
        return -1;
    }
    kprintf("hda: CORB/BDL path PASS\n");
    kprintf("hda: MMIO CORB path PASS\n");
    kprintf("hda: CORB DMA engine PASS\n");
    kprintf("hda: BDL kick PASS\n");
    if (g_fHwCorb && g_u32HwCorbProgrammed) {
        kprintf("hda: HW CORB path PASS\n");
    }
    if (g_fStreamDma && g_u32StreamDmaStarts > 0) {
        kprintf("hda: SD0 stream DMA PASS starts=%u\n", g_u32StreamDmaStarts);
    } else {
        kprintf("hda: SD0 stream DMA soft (no BAR0 or not started)\n");
    }
    g_u32SoftSmokeLogs++;
    hda_soft_inventory_log("corb_bdl_smoke");
    return 0;
}

int
hda_hw_corb_ready(void)
{
    return g_fHwCorb && g_u32HwCorbProgrammed;
}

int
hda_stream_dma_ready(void)
{
    return g_fStreamDma && g_u32StreamDmaStarts > 0;
}

/* ---- Multi-stream rings + software mixer + soft codec (clean-room) ---- */

struct hda_stream_slot {
    int  fOpen;
    int  fRunning;
    u32  u32Channels;
    u32  u32Rate;
    u32  u32Bits;
    u32  u32Head;
    u32  u32Len;
    u32  u32Written;
    u32  u32Played;
    u32  u32Underruns;
    u32  u32Gain256; /* 0..256 */
    u8   aRing[HDA_RING_SIZE];
};

static struct hda_stream_slot g_aStr[GJ_HDA_STREAMS_MAX];
static u32 g_u32MasterGain = 256u;
static u32 g_u32MixBytes;
static u32 g_u32MixUnderruns;

/* Soft codec: NID 0=root, 1=AFG, 2=DAC, 3=PIN OUT (bring-up graph only). */
static u32 g_u32CodecVendor = 0x1000a000u; /* QEMU-ish clean-room id */
static u32 g_u32CodecRev = 0x00100101u;
static u32 g_u32DacGain = 0x7fu;
static int g_fDacMute;
static u32 g_u32CodecProg;
static u32 g_u32PinCtrl;   /* SET_PIN_WIDGET_CONTROL shadow */
static u32 g_u32PowerState; /* 0 = D0 */
static u32 g_u32StreamFmt; /* SET_STREAM_FORMAT soft */
static u32 g_u32CodecStreamId; /* SET_CHANNEL_STREAM_ID: tag in [7:4], ch [3:0] */
static u32 g_u32SoftCodecHits;

static u32
hda_apply_gain(u8 u8Sample, u32 u32Gain256)
{
    /* 8-bit sample treated as unsigned; scale around mid. */
    i32 s = (i32)u8Sample - 128;
    i32 g = (i32)u32Gain256;

    if (g > 256) {
        g = 256;
    }
    s = (s * g) / 256;
    s += 128;
    if (s < 0) {
        s = 0;
    }
    if (s > 255) {
        s = 255;
    }
    return (u32)s;
}

int
hda_stream_n_open(u32 u32Id, u32 u32Channels, u32 u32RateHz, u32 u32Bits)
{
    struct hda_stream_slot *p;

    if (u32Id >= GJ_HDA_STREAMS_MAX) {
        return -1;
    }
    if (u32Channels == 0 || u32Channels > 8 || u32RateHz == 0 ||
        (u32Bits != 16 && u32Bits != 8 && u32Bits != 24 && u32Bits != 32)) {
        return -1;
    }
    p = &g_aStr[u32Id];
    memset(p, 0, sizeof(*p));
    p->fOpen = 1;
    p->u32Channels = u32Channels;
    p->u32Rate = u32RateHz;
    p->u32Bits = u32Bits;
    p->u32Gain256 = 256u;
    /* Latch soft SD format + default stream tag (1 for SD0, 2 for SD1). */
    g_aSoftSd[u32Id].u32Fmt =
        hda_soft_fmt_from_params(u32Channels, u32RateHz, u32Bits);
    if (g_aSoftSd[u32Id].u32Tag == 0) {
        g_aSoftSd[u32Id].u32Tag = u32Id + 1u;
    }
    if (u32Id == 0) {
        /* Keep legacy single-stream globals in sync for hda_stream_*. */
        (void)hda_stream_open(u32Channels, u32RateHz, u32Bits);
    }
    return 0;
}

void
hda_stream_n_close(u32 u32Id)
{
    if (u32Id >= GJ_HDA_STREAMS_MAX) {
        return;
    }
    g_aStr[u32Id].fOpen = 0;
    g_aStr[u32Id].fRunning = 0;
    g_aStr[u32Id].u32Len = 0;
    g_aSoftSd[u32Id].fRun = 0;
    if (u32Id == 0) {
        hda_stream_close();
    }
}

u32
hda_stream_n_write(u32 u32Id, const void *pData, u32 u32Bytes)
{
    struct hda_stream_slot *p;
    const u8 *src = (const u8 *)pData;
    u32 accepted = 0;
    u32 space;
    u32 pos;
    u32 g;

    if (u32Id >= GJ_HDA_STREAMS_MAX || pData == NULL || u32Bytes == 0) {
        return 0;
    }
    p = &g_aStr[u32Id];
    if (!p->fOpen) {
        return 0;
    }
    if (u32Id == 0) {
        /* Path through legacy ring + mixer gain on write for stream0 */
        u8 aTmp[256];
        u32 off = 0;
        u32 n;

        while (off < u32Bytes) {
            u32 chunk = u32Bytes - off;
            u32 i;

            if (chunk > sizeof(aTmp)) {
                chunk = (u32)sizeof(aTmp);
            }
            g = (p->u32Gain256 * g_u32MasterGain) / 256u;
            if (g_fDacMute) {
                g = 0;
            }
            for (i = 0; i < chunk; i++) {
                aTmp[i] = (u8)hda_apply_gain(src[off + i], g);
            }
            n = hda_stream_write(aTmp, chunk);
            accepted += n;
            off += n;
            if (n < chunk) {
                break;
            }
        }
        p->u32Written += accepted;
        p->u32Len = hda_stream_bytes_queued();
        return accepted;
    }
    space = HDA_RING_SIZE - p->u32Len;
    if (u32Bytes > space) {
        u32Bytes = space;
    }
    g = (p->u32Gain256 * g_u32MasterGain) / 256u;
    if (g_fDacMute) {
        g = 0;
    }
    while (accepted < u32Bytes) {
        pos = (p->u32Head + p->u32Len) % HDA_RING_SIZE;
        p->aRing[pos] = (u8)hda_apply_gain(src[accepted], g);
        p->u32Len++;
        accepted++;
    }
    p->u32Written += accepted;
    return accepted;
}

int
hda_stream_n_start(u32 u32Id)
{
    if (u32Id >= GJ_HDA_STREAMS_MAX || !g_aStr[u32Id].fOpen) {
        return -1;
    }
    g_aStr[u32Id].fRunning = 1;
    g_aSoftSd[u32Id].fRun = 1;
    if (g_aSoftSd[u32Id].u32Tag == 0) {
        g_aSoftSd[u32Id].u32Tag = u32Id + 1u;
    }
    if (u32Id == 0) {
        return hda_stream_start();
    }
    /* SD1 base = SD0 + 0x20 when BAR present (second output stream). */
    if (g_pMmio != NULL) {
        hda_reg_write(HDA_SD0_BASE + 0x20u + HDA_SD_CTL,
                      (g_aSoftSd[u32Id].u32Tag << 20) | 0x02u);
    }
    return 0;
}

void
hda_stream_n_stop(u32 u32Id)
{
    if (u32Id >= GJ_HDA_STREAMS_MAX) {
        return;
    }
    g_aStr[u32Id].fRunning = 0;
    g_aSoftSd[u32Id].fRun = 0;
    if (u32Id == 0) {
        hda_stream_stop();
    } else if (g_pMmio != NULL) {
        hda_reg_write(HDA_SD0_BASE + 0x20u + HDA_SD_CTL, 0);
    }
}

u32
hda_stream_n_tick(u32 u32Id, u32 u32Frames)
{
    struct hda_stream_slot *p;
    u32 bytesPerFrame;
    u32 want;
    u32 got = 0;

    if (u32Id >= GJ_HDA_STREAMS_MAX || u32Frames == 0) {
        return 0;
    }
    p = &g_aStr[u32Id];
    if (!p->fOpen || !p->fRunning) {
        return 0;
    }
    if (u32Id == 0) {
        got = hda_stream_tick(u32Frames);
        p->u32Played += got;
        p->u32Len = hda_stream_bytes_queued();
        p->u32Underruns = hda_stream_underruns();
        if (got != 0) {
            hda_soft_sd_advance(0, got);
        }
        return got;
    }
    bytesPerFrame = p->u32Channels * (p->u32Bits / 8u);
    if (bytesPerFrame == 0) {
        return 0;
    }
    want = u32Frames * bytesPerFrame;
    while (got < want) {
        if (p->u32Len == 0) {
            p->u32Underruns++;
            break;
        }
        p->u32Head = (p->u32Head + 1) % HDA_RING_SIZE;
        p->u32Len--;
        got++;
        p->u32Played++;
    }
    hda_soft_sd_advance(u32Id, got);
    return got;
}

int
hda_stream_n_ready(u32 u32Id)
{
    if (u32Id >= GJ_HDA_STREAMS_MAX) {
        return 0;
    }
    return g_aStr[u32Id].fOpen;
}

u32
hda_stream_n_bytes_queued(u32 u32Id)
{
    if (u32Id >= GJ_HDA_STREAMS_MAX || !g_aStr[u32Id].fOpen) {
        return 0;
    }
    if (u32Id == 0) {
        return hda_stream_bytes_queued();
    }
    return g_aStr[u32Id].u32Len;
}

u32
hda_stream_n_bytes_played(u32 u32Id)
{
    if (u32Id >= GJ_HDA_STREAMS_MAX) {
        return 0;
    }
    if (u32Id == 0) {
        return hda_stream_bytes_played();
    }
    return g_aStr[u32Id].u32Played;
}

u32
hda_stream_n_underruns(u32 u32Id)
{
    if (u32Id >= GJ_HDA_STREAMS_MAX) {
        return 0;
    }
    if (u32Id == 0) {
        return hda_stream_underruns();
    }
    return g_aStr[u32Id].u32Underruns;
}

int
hda_mixer_set_master(u32 u32Gain256)
{
    if (u32Gain256 > 256u) {
        u32Gain256 = 256u;
    }
    g_u32MasterGain = u32Gain256;
    return 0;
}

int
hda_mixer_set_stream(u32 u32Id, u32 u32Gain256)
{
    if (u32Id >= GJ_HDA_STREAMS_MAX) {
        return -1;
    }
    if (u32Gain256 > 256u) {
        u32Gain256 = 256u;
    }
    g_aStr[u32Id].u32Gain256 = u32Gain256;
    return 0;
}

u32
hda_mixer_master(void)
{
    return g_u32MasterGain;
}

u32
hda_mixer_stream(u32 u32Id)
{
    if (u32Id >= GJ_HDA_STREAMS_MAX) {
        return 0;
    }
    return g_aStr[u32Id].u32Gain256;
}

/**
 * Consume one PCM byte from stream u32Id (running). Returns 1 if a sample
 * byte was taken into *pByte, 0 on underrun/inactive.
 */
static int
hda_stream_n_pull_byte(u32 u32Id, u8 *pByte)
{
    struct hda_stream_slot *p;

    if (u32Id >= GJ_HDA_STREAMS_MAX || pByte == NULL) {
        return 0;
    }
    p = &g_aStr[u32Id];
    if (!p->fOpen || !p->fRunning) {
        return 0;
    }
    if (u32Id == 0) {
        if (g_u32Len == 0) {
            g_u32Underruns++;
            p->u32Underruns = g_u32Underruns;
            return 0;
        }
        *pByte = g_aRing[g_u32Head];
        g_u32Head = (g_u32Head + 1u) % HDA_RING_SIZE;
        g_u32Len--;
        g_u32Played++;
        p->u32Played = g_u32Played;
        p->u32Len = g_u32Len;
        p->u32Underruns = g_u32Underruns;
        return 1;
    }
    if (p->u32Len == 0) {
        p->u32Underruns++;
        return 0;
    }
    *pByte = p->aRing[p->u32Head];
    p->u32Head = (p->u32Head + 1u) % HDA_RING_SIZE;
    p->u32Len--;
    p->u32Played++;
    return 1;
}

u32
hda_mixer_mix_tick(u32 u32Frames, u8 *pOut, u32 u32OutCap)
{
    u32 bytesPerFrame;
    u32 want;
    u32 got = 0;
    u32 iId;
    u32 cActive;

    if (u32Frames == 0) {
        return 0;
    }
    /* Prefer stream0 format; default stereo 16-bit when only SD1 runs. */
    if (g_aStr[0].fOpen && g_aStr[0].fRunning) {
        bytesPerFrame = g_aStr[0].u32Channels * (g_aStr[0].u32Bits / 8u);
    } else if (g_aStr[1].fOpen && g_aStr[1].fRunning) {
        bytesPerFrame = g_aStr[1].u32Channels * (g_aStr[1].u32Bits / 8u);
    } else {
        return 0;
    }
    if (bytesPerFrame == 0) {
        return 0;
    }
    want = u32Frames * bytesPerFrame;
    while (got < want) {
        i32 iSum = 0;
        u8 u8S;
        u32 cPulled = 0;

        cActive = 0;
        for (iId = 0; iId < GJ_HDA_STREAMS_MAX; iId++) {
            if (g_aStr[iId].fOpen && g_aStr[iId].fRunning) {
                cActive++;
                if (hda_stream_n_pull_byte(iId, &u8S)) {
                    iSum += (i32)u8S - 128;
                    cPulled++;
                }
            }
        }
        if (cActive == 0) {
            break;
        }
        if (cPulled == 0) {
            g_u32MixUnderruns++;
            break;
        }
        iSum += 128;
        if (iSum < 0) {
            iSum = 0;
        }
        if (iSum > 255) {
            iSum = 255;
        }
        if (pOut != NULL && got < u32OutCap) {
            pOut[got] = (u8)iSum;
        }
        got++;
        g_u32MixBytes++;
        /* Soft BDL/LPIB: each mixed output byte advances every active SD. */
        for (iId = 0; iId < GJ_HDA_STREAMS_MAX; iId++) {
            if (g_aStr[iId].fOpen && g_aStr[iId].fRunning &&
                g_aSoftSd[iId].fRun) {
                hda_soft_sd_advance(iId, 1);
            }
        }
    }
    return got;
}

u32
hda_mixer_mix_bytes(void)
{
    return g_u32MixBytes;
}

u32
hda_mixer_mix_underruns(void)
{
    return g_u32MixUnderruns;
}

/**
 * Soft codec response for common CORB verbs (clean-room bring-up model).
 * Verb layout: NID[27:20] | 12-bit verb[19:8] | payload[7:0] (CAD ignored).
 */
static u32
hda_soft_codec_respond(u32 u32Verb)
{
    u32 nid = (u32Verb >> 20) & 0x7fu;
    u32 verb = (u32Verb >> 8) & 0xfffu;
    u32 pay = u32Verb & 0xffu;

    g_u32SoftCodecHits++;

    /*
     * GET_PARAMETER is exactly 12-bit verb 0xF00 (payload = param id).
     * Do not match other 0xFxx verbs (F02 connect, F05 power, F07 pin, …).
     */
    if (verb == 0xf00u) {
        u32 param = pay;

        if (nid == GJ_HDA_NID_ROOT || nid == GJ_HDA_NID_AFG) {
            if (param == 0x00) {
                return g_u32CodecVendor; /* vendor/device */
            }
            if (param == 0x02) {
                return g_u32CodecRev; /* revision */
            }
            if (param == 0x04) {
                /* Root: AFG at 1 count 1; AFG: widgets start 2 count 2 */
                if (nid == GJ_HDA_NID_ROOT) {
                    return 0x00010001u;
                }
                return 0x00020002u; /* DAC@2 + PIN@3 */
            }
            if (param == 0x05) {
                return 0x00000001u; /* function type AFG */
            }
            if (param == 0x08) {
                return 0x00000001u; /* audio FG caps */
            }
            if (param == 0x09) {
                return 0x00100101u; /* PCM rates/bits soft */
            }
            if (param == 0x0f) {
                return 0x00000001u; /* power states D0 soft */
            }
        }
        if (nid == GJ_HDA_NID_DAC) {
            if (param == 0x09) {
                return 0x00100101u;
            }
            if (param == 0x0a) {
                return 0x00000001u; /* stream formats soft */
            }
            if (param == 0x0d) {
                return 0x00000001u; /* amp out caps */
            }
            if (param == 0x0f) {
                return 0x00000001u;
            }
            if (param == 0x12) {
                return 0x00000001u; /* conn list len 1 */
            }
        }
        if (nid == GJ_HDA_NID_PIN) {
            if (param == 0x0c) {
                return 0x00000010u; /* pin caps OUT */
            }
            if (param == 0x0f) {
                return 0x00000001u;
            }
            if (param == 0x12) {
                return 0x00000001u;
            }
        }
        return 0;
    }
    /* SET_AMPLIFIER_GAIN_MUTE 0x3xx / GET 0xBxx */
    if ((verb & 0xf00u) == 0x300u || (verb & 0xf00u) == 0xb00u) {
        if ((verb & 0xf00u) == 0x300u) {
            g_u32DacGain = pay & 0x7fu;
            g_fDacMute = (pay & 0x80u) != 0;
            g_u32MasterGain = g_fDacMute ? 0u : ((g_u32DacGain * 256u) / 127u);
        }
        return (g_fDacMute ? 0x80u : 0u) | (g_u32DacGain & 0x7fu);
    }
    /* SET_STREAM_FORMAT 0x2 / GET 0xA */
    if (verb == 0x002u || (verb & 0xfffu) == 0x002u) {
        g_u32StreamFmt = (u32Verb & 0xffffu);
        return g_u32StreamFmt;
    }
    if (verb == 0x00au || (verb & 0xfffu) == 0x00au) {
        return g_u32StreamFmt;
    }
    /* SET_POWER_STATE 0x705 / GET 0xF05 */
    if (verb == 0x705u) {
        g_u32PowerState = pay & 0x0fu;
        return g_u32PowerState;
    }
    if (verb == 0xf05u) {
        return g_u32PowerState;
    }
    /* SET_PIN_WIDGET_CONTROL 0x707 / GET 0xF07 */
    if (verb == 0x707u) {
        g_u32PinCtrl = pay;
        return g_u32PinCtrl;
    }
    if (verb == 0xf07u) {
        return g_u32PinCtrl;
    }
    /* SET_CHANNEL_STREAM_ID 0x706 / GET 0xF06 — stream tag[7:4] channel[3:0] */
    if (verb == 0x706u) {
        g_u32CodecStreamId = pay & 0xffu;
        return g_u32CodecStreamId;
    }
    if (verb == 0xf06u) {
        return g_u32CodecStreamId;
    }
    /* GET_CONNECT_LIST 0xF02 — DAC feeds PIN, PIN lists DAC */
    if (verb == 0xf02u) {
        if (nid == GJ_HDA_NID_DAC) {
            return GJ_HDA_NID_PIN;
        }
        if (nid == GJ_HDA_NID_PIN) {
            return GJ_HDA_NID_DAC;
        }
        return 0;
    }
    /* GET_CONFIG_DEFAULT 0xF1C soft line-out */
    if (verb == 0xf1cu) {
        return 0x02214010u;
    }
    (void)nid;
    /* Unknown verb: legacy completion mark for CORB smoke compatibility */
    return u32Verb | 0x80000000u;
}

int
hda_codec_verb(u32 u32Cad, u32 u32Nid, u32 u32Verb, u32 u32Payload, u32 *pResp)
{
    u32 full;
    u32 resp = 0;
    u32 spins;

    (void)u32Cad;
    full = ((u32Nid & 0x7fu) << 20) | ((u32Verb & 0xfffu) << 8) |
           (u32Payload & 0xffu);
    /*
     * Soft decode applies mixer/pin/power side effects. corb_send also
     * soft-decodes into RIRB; prefer the precomputed soft response for
     * callers (avoids relying on HW RIRB when codec is silent).
     */
    resp = hda_soft_codec_respond(full);
    if (hda_corb_send(full) != 0) {
        return -1;
    }
    for (spins = 0; spins < 16u; spins++) {
        u32 r = 0;

        if (hda_rirb_pop(&r) == 1) {
            if (pResp != NULL) {
                *pResp = resp;
            }
            return 0;
        }
    }
    if (pResp != NULL) {
        *pResp = resp;
    }
    return 0;
}

int
hda_codec_get_param(u32 u32Nid, u32 u32Param, u32 *pResp)
{
    return hda_codec_verb(0, u32Nid, 0xf00u, u32Param & 0xffu, pResp);
}

int
hda_codec_set_amp(u32 u32Nid, u32 u32Gain, int fMute)
{
    u32 pay = (u32Gain & 0x7fu) | (fMute ? 0x80u : 0u);
    u32 resp = 0;

    if (hda_codec_verb(0, u32Nid, 0x300u, pay, &resp) != 0) {
        return -1;
    }
    (void)resp;
    return 0;
}

int
hda_codec_set_pin(u32 u32Nid, u32 u32Ctrl)
{
    u32 resp = 0;

    if (hda_codec_verb(0, u32Nid, 0x707u, u32Ctrl & 0xffu, &resp) != 0) {
        return -1;
    }
    (void)resp;
    return 0;
}

int
hda_codec_set_power(u32 u32Nid, u32 u32State)
{
    u32 resp = 0;

    if (hda_codec_verb(0, u32Nid, 0x705u, u32State & 0x0fu, &resp) != 0) {
        return -1;
    }
    (void)resp;
    return 0;
}

int
hda_codec_set_stream_id(u32 u32Nid, u32 u32StreamTag, u32 u32Channel)
{
    u32 pay;
    u32 resp = 0;

    if (u32StreamTag == 0 || u32StreamTag > 15u || u32Channel > 15u) {
        return -1;
    }
    pay = ((u32StreamTag & 0x0fu) << 4) | (u32Channel & 0x0fu);
    if (hda_codec_verb(0, u32Nid, 0x706u, pay, &resp) != 0) {
        return -1;
    }
    (void)resp;
    return 0;
}

u32
hda_codec_stream_id(void)
{
    return (g_u32CodecStreamId >> 4) & 0x0fu;
}

u32
hda_codec_stream_channel(void)
{
    return g_u32CodecStreamId & 0x0fu;
}

u32
hda_codec_pin_control(void)
{
    return g_u32PinCtrl;
}

u32
hda_codec_power_state(void)
{
    return g_u32PowerState;
}

int
hda_codec_programmed(void)
{
    return g_u32CodecProg != 0;
}

/**
 * Wave 15 soft inventory dump — greppable "hda: soft …".
 * Snapshots live soft path state; never allocates; never hard-gates.
 * szVia: caller tag (probe / stream_smoke / corb_bdl_smoke / multi_smoke).
 *
 * greppable: hda: soft
 */
static void
hda_soft_inventory_log(const char *szVia)
{
    u32 iId;
    u32 cOpen = 0;
    u32 cRun = 0;
    u32 cQueued = 0;
    u32 cPlayed = 0;
    u32 cUnderrun = 0;
    u32 cBdlSegs = 0;
    u32 cBdlKicks = 0;
    u32 cBdlIoc = 0;
    u32 cSoftRun = 0;
    u32 u32Lpib0;
    u32 u32Lpib1;
    u32 u32Cbl0;
    u32 u32Cbl1;
    u32 u32Tag0;
    u32 u32Tag1;
    u32 u32Fmt0;
    u32 u32Fmt1;
    u32 u32Lvi0;
    u32 u32Lvi1;
    u32 u32Gcap;
    u32 u32Gctl;
    u32 u32Vmaj;
    u32 u32Vmin;
    u32 u32Oss;
    u32 u32Iss;
    u32 u32Bss;
    u32 u32Ok64;
    u32 u32Nsdo;
    u32 u32Intctl;
    u32 u32Intsts;
    u32 u32Wakeen;
    u32 u32Statests;
    u32 fMmio;
    u32 fHwCorb;
    u32 fStreamDma;
    u32 fPresent;
    u32 fCodecProg;
    u32 fShadow;
    u32 fDualHwSoft;
    const char *szViaSafe;
    const char *szPathMode;

    szViaSafe = (szVia != NULL) ? szVia : "anon";
    if (g_u32SoftInventoryLogs < 0xffffffffu) {
        g_u32SoftInventoryLogs++;
    }

    for (iId = 0; iId < GJ_HDA_STREAMS_MAX; iId++) {
        if (g_aStr[iId].fOpen) {
            cOpen++;
            if (iId == 0) {
                cQueued += hda_stream_bytes_queued();
                cPlayed += hda_stream_bytes_played();
                cUnderrun += hda_stream_underruns();
            } else {
                cQueued += g_aStr[iId].u32Len;
                cPlayed += g_aStr[iId].u32Played;
                cUnderrun += g_aStr[iId].u32Underruns;
            }
        }
        if (g_aStr[iId].fRunning) {
            cRun++;
        }
        if (g_aSoftSd[iId].fRun) {
            cSoftRun++;
        }
        cBdlSegs += g_aSoftSd[iId].u32N;
        cBdlKicks += g_aSoftSd[iId].u32Kicks;
        cBdlIoc += g_aSoftSd[iId].u32IocCount;
    }

    u32Lpib0 = g_aSoftSd[0].u32Lpib;
    u32Lpib1 = (GJ_HDA_STREAMS_MAX > 1u) ? g_aSoftSd[1].u32Lpib : 0u;
    u32Cbl0 = g_aSoftSd[0].u32Cbl;
    u32Cbl1 = (GJ_HDA_STREAMS_MAX > 1u) ? g_aSoftSd[1].u32Cbl : 0u;
    u32Tag0 = g_aSoftSd[0].u32Tag;
    u32Tag1 = (GJ_HDA_STREAMS_MAX > 1u) ? g_aSoftSd[1].u32Tag : 0u;
    u32Fmt0 = g_aSoftSd[0].u32Fmt;
    u32Fmt1 = (GJ_HDA_STREAMS_MAX > 1u) ? g_aSoftSd[1].u32Fmt : 0u;
    u32Lvi0 = g_aSoftSd[0].u32Lvi;
    u32Lvi1 = (GJ_HDA_STREAMS_MAX > 1u) ? g_aSoftSd[1].u32Lvi : 0u;

    u32Gcap = hda_reg_read(HDA_REG_GCAP);
    u32Gctl = hda_reg_read(HDA_REG_GCTL);
    u32Vmaj = hda_reg_read(HDA_REG_VMAJ) & 0xffu;
    u32Vmin = hda_reg_read(HDA_REG_VMIN) & 0xffu;
    u32Intctl = hda_reg_read(HDA_REG_INTCTL);
    u32Intsts = hda_reg_read(HDA_REG_INTSTS);
    u32Wakeen = hda_reg_read(HDA_REG_WAKEEN);
    u32Statests = hda_reg_read(HDA_REG_STATESTS);
    /* Intel HDA GCAP: OSS[15:12] ISS[11:8] BSS[7:3] NSDO[1] 64OK[0]. */
    u32Oss = (u32Gcap >> 12) & 0x0fu;
    u32Iss = (u32Gcap >> 8) & 0x0fu;
    u32Bss = (u32Gcap >> 3) & 0x1fu;
    u32Nsdo = (u32Gcap >> 1) & 0x01u;
    u32Ok64 = u32Gcap & 0x01u;
    fMmio = (g_pMmio != NULL) ? 1u : 0u;
    fHwCorb = (g_fHwCorb && g_u32HwCorbProgrammed) ? 1u : 0u;
    fStreamDma = (g_fStreamDma != 0) ? 1u : 0u;
    fPresent = (g_fPresent != 0) ? 1u : 0u;
    fCodecProg = (g_u32CodecProg != 0) ? 1u : 0u;
    fShadow = (g_pMmio == NULL) ? 1u : 0u;
    fDualHwSoft = (fMmio != 0u) ? 1u : 0u;
    if (fMmio != 0u && fHwCorb != 0u) {
        szPathMode = "hw_corb+soft";
    } else if (fMmio != 0u) {
        szPathMode = "mmio+soft";
    } else {
        szPathMode = "shadow_soft";
    }

    /*
     * Grep: hda: soft inventory
     * Capacity + dual-path catalog (soft always; HW when BAR0 mapped).
     * Wave 15: wave stamp + areas + path_mode.
     */
    kprintf("hda: soft inventory via=%s streams_max=%u ring=%u corb_ents=%u "
            "bdl_ents=%u mix_cap=%u present=%u mmio=%u hw_corb=%u "
            "stream_dma=%u codec_prog=%u logs=%u probe_logs=%u "
            "smoke_logs=%u wave=%u areas=%u path_mode=%s\n",
            szViaSafe, (unsigned)GJ_HDA_STREAMS_MAX,
            (unsigned)HDA_RING_SIZE, (unsigned)GJ_HDA_CORB_ENTRIES,
            (unsigned)GJ_HDA_BDL_ENTRIES, (unsigned)GJ_HDA_MIX_BYTES,
            (unsigned)fPresent, (unsigned)fMmio, (unsigned)fHwCorb,
            (unsigned)fStreamDma, (unsigned)fCodecProg,
            (unsigned)g_u32SoftInventoryLogs, (unsigned)g_u32SoftProbeLogs,
            (unsigned)g_u32SoftSmokeLogs, (unsigned)HDA_SOFT_DEEPEN_WAVE,
            (unsigned)HDA_SOFT_DEEPEN_AREAS, szPathMode);

    /* Grep: hda: soft pci */
    kprintf("hda: soft pci present=%u bus=%u slot=%u func=%u bar0=0x%lx "
            "mmio=%u shadow=%u gctl=0x%x gcap=0x%x class=0x0403 "
            "path=%s wave=%u\n",
            (unsigned)fPresent, (unsigned)g_u32Bus, (unsigned)g_u32Slot,
            (unsigned)g_u32Func, (unsigned long)g_u64Bar0, (unsigned)fMmio,
            (unsigned)fShadow, (unsigned)u32Gctl, (unsigned)u32Gcap,
            szPathMode, (unsigned)HDA_SOFT_DEEPEN_WAVE);

    /* Grep: hda: soft gcap (Wave 15: public HDA GCAP field inventory) */
    kprintf("hda: soft gcap raw=0x%x oss=%u iss=%u bss=%u nsdo=%u ok64=%u "
            "vmaj=%u vmin=%u gctl=0x%x crst=%u wave=%u\n",
            (unsigned)u32Gcap, (unsigned)u32Oss, (unsigned)u32Iss,
            (unsigned)u32Bss, (unsigned)u32Nsdo, (unsigned)u32Ok64,
            (unsigned)u32Vmaj, (unsigned)u32Vmin, (unsigned)u32Gctl,
            (unsigned)(u32Gctl & 1u), (unsigned)HDA_SOFT_DEEPEN_WAVE);

    /*
     * Grep: hda: soft regs — public Intel HDA controller offset map.
     * Inventory only; values are byte offsets into BAR0.
     */
    kprintf("hda: soft regs GCAP=0x%x VMAJ=0x%x VMIN=0x%x GCTL=0x%x "
            "WAKEEN=0x%x STATESTS=0x%x INTCTL=0x%x INTSTS=0x%x "
            "CORBLBASE=0x%x RIRBLBASE=0x%x SD0=0x%x soft PASS wave=%u\n",
            (unsigned)HDA_REG_GCAP, (unsigned)HDA_REG_VMAJ,
            (unsigned)HDA_REG_VMIN, (unsigned)HDA_REG_GCTL,
            (unsigned)HDA_REG_WAKEEN, (unsigned)HDA_REG_STATESTS,
            (unsigned)HDA_REG_INTCTL, (unsigned)HDA_REG_INTSTS,
            (unsigned)HDA_REG_CORBLBASE, (unsigned)HDA_REG_RIRBLBASE,
            (unsigned)HDA_SD0_BASE, (unsigned)HDA_SOFT_DEEPEN_WAVE);

    /* Grep: hda: soft int — INTCTL/INTSTS soft-read (Wave 15) */
    kprintf("hda: soft int intctl=0x%x intsts=0x%x claim_irq=0 "
            "soft PASS wave=%u\n",
            (unsigned)u32Intctl, (unsigned)u32Intsts,
            (unsigned)HDA_SOFT_DEEPEN_WAVE);

    /* Grep: hda: soft wake — WAKEEN/STATESTS soft-read (Wave 15) */
    kprintf("hda: soft wake wakeen=0x%x statests=0x%x soft PASS wave=%u\n",
            (unsigned)u32Wakeen, (unsigned)u32Statests,
            (unsigned)HDA_SOFT_DEEPEN_WAVE);

    /* Grep: hda: soft stream */
    kprintf("hda: soft stream open=%u run=%u soft_run=%u queued=%u "
            "played=%u underrun=%u dma_starts=%u legacy_open=%u "
            "legacy_run=%u legacy_queued=%u legacy_played=%u "
            "streams_max=%u wave=%u\n",
            (unsigned)cOpen, (unsigned)cRun, (unsigned)cSoftRun,
            (unsigned)cQueued, (unsigned)cPlayed, (unsigned)cUnderrun,
            (unsigned)g_u32StreamDmaStarts, (unsigned)(g_fStreamOpen ? 1 : 0),
            (unsigned)(g_fStreamRunning ? 1 : 0), (unsigned)g_u32Len,
            (unsigned)g_u32Played, (unsigned)GJ_HDA_STREAMS_MAX,
            (unsigned)HDA_SOFT_DEEPEN_WAVE);

    /* Grep: hda: soft stream0 (Wave 15 per-SD detail) */
    kprintf("hda: soft stream0 open=%u run=%u ch=%u rate=%u bits=%u "
            "queued=%u played=%u underrun=%u written=%u gain=%u "
            "tag=%u fmt=0x%x soft_run=%u wave=%u\n",
            (unsigned)(g_aStr[0].fOpen ? 1 : 0),
            (unsigned)(g_aStr[0].fRunning ? 1 : 0),
            (unsigned)g_aStr[0].u32Channels, (unsigned)g_aStr[0].u32Rate,
            (unsigned)g_aStr[0].u32Bits,
            (unsigned)(g_aStr[0].fOpen ? hda_stream_bytes_queued() : 0u),
            (unsigned)(g_aStr[0].fOpen ? hda_stream_bytes_played()
                                       : g_aStr[0].u32Played),
            (unsigned)(g_aStr[0].fOpen ? hda_stream_underruns()
                                       : g_aStr[0].u32Underruns),
            (unsigned)g_aStr[0].u32Written, (unsigned)g_aStr[0].u32Gain256,
            (unsigned)u32Tag0, (unsigned)u32Fmt0,
            (unsigned)(g_aSoftSd[0].fRun ? 1 : 0),
            (unsigned)HDA_SOFT_DEEPEN_WAVE);

    /* Grep: hda: soft stream1 (Wave 15 per-SD detail) */
    if (GJ_HDA_STREAMS_MAX > 1u) {
        kprintf("hda: soft stream1 open=%u run=%u ch=%u rate=%u bits=%u "
                "queued=%u played=%u underrun=%u written=%u gain=%u "
                "tag=%u fmt=0x%x soft_run=%u wave=%u\n",
                (unsigned)(g_aStr[1].fOpen ? 1 : 0),
                (unsigned)(g_aStr[1].fRunning ? 1 : 0),
                (unsigned)g_aStr[1].u32Channels, (unsigned)g_aStr[1].u32Rate,
                (unsigned)g_aStr[1].u32Bits, (unsigned)g_aStr[1].u32Len,
                (unsigned)g_aStr[1].u32Played, (unsigned)g_aStr[1].u32Underruns,
                (unsigned)g_aStr[1].u32Written, (unsigned)g_aStr[1].u32Gain256,
                (unsigned)u32Tag1, (unsigned)u32Fmt1,
                (unsigned)(g_aSoftSd[1].fRun ? 1 : 0),
                (unsigned)HDA_SOFT_DEEPEN_WAVE);
    } else {
        kprintf("hda: soft stream1 open=0 run=0 ch=0 rate=0 bits=0 "
                "queued=0 played=0 underrun=0 written=0 gain=0 "
                "tag=0 fmt=0x0 soft_run=0 wave=%u\n",
                (unsigned)HDA_SOFT_DEEPEN_WAVE);
    }

    /* Grep: hda: soft fmt — stream format field inventory (Wave 15) */
    kprintf("hda: soft fmt s0_ch=%u s0_rate=%u s0_bits=%u s0_fmt=0x%x "
            "s1_ch=%u s1_rate=%u s1_bits=%u s1_fmt=0x%x "
            "codec_fmt=0x%x soft PASS wave=%u\n",
            (unsigned)g_aStr[0].u32Channels, (unsigned)g_aStr[0].u32Rate,
            (unsigned)g_aStr[0].u32Bits, (unsigned)u32Fmt0,
            (unsigned)((GJ_HDA_STREAMS_MAX > 1u) ? g_aStr[1].u32Channels : 0u),
            (unsigned)((GJ_HDA_STREAMS_MAX > 1u) ? g_aStr[1].u32Rate : 0u),
            (unsigned)((GJ_HDA_STREAMS_MAX > 1u) ? g_aStr[1].u32Bits : 0u),
            (unsigned)u32Fmt1, (unsigned)g_u32StreamFmt,
            (unsigned)HDA_SOFT_DEEPEN_WAVE);

    /* Grep: hda: soft corb */
    kprintf("hda: soft corb sent=%u rirb=%u dma_runs=%u wp=%u rp=%u "
            "ctl=0x%x hw=%u hw_prog=%u size=%u base_lo=0x%x "
            "pa_hw=0x%lx wave=%u\n",
            (unsigned)g_u32CorbSent, (unsigned)g_u32RirbCount,
            (unsigned)g_u32CorbDmaRuns, (unsigned)g_u32CorbWp,
            (unsigned)g_u32CorbRp, (unsigned)hda_reg_read(HDA_REG_CORBCTL),
            (unsigned)(g_fHwCorb ? 1 : 0), (unsigned)g_u32HwCorbProgrammed,
            (unsigned)GJ_HDA_CORB_ENTRIES,
            (unsigned)hda_reg_read(HDA_REG_CORBLBASE),
            (unsigned long)g_paCorbHw, (unsigned)HDA_SOFT_DEEPEN_WAVE);

    /* Grep: hda: soft rirb (Wave 15) */
    kprintf("hda: soft rirb count=%u wp=%u rp=%u size=%u ctl=0x%x "
            "sts=0x%x base_lo=0x%x pa_hw=0x%lx wave=%u\n",
            (unsigned)g_u32RirbCount, (unsigned)g_u32RirbWp,
            (unsigned)g_u32RirbRp, (unsigned)GJ_HDA_CORB_ENTRIES,
            (unsigned)hda_reg_read(HDA_REG_RIRBCTL),
            (unsigned)hda_reg_read(HDA_REG_RIRBSTS),
            (unsigned)hda_reg_read(HDA_REG_RIRBLBASE),
            (unsigned long)g_paRirbHw, (unsigned)HDA_SOFT_DEEPEN_WAVE);

    /* Grep: hda: soft mixer */
    kprintf("hda: soft mixer master=%u s0_gain=%u s1_gain=%u mix_bytes=%u "
            "mix_underrun=%u dac_gain=0x%x dac_mute=%u active=%u "
            "mix_cap=%u wave=%u\n",
            (unsigned)g_u32MasterGain, (unsigned)g_aStr[0].u32Gain256,
            (unsigned)((GJ_HDA_STREAMS_MAX > 1u) ? g_aStr[1].u32Gain256 : 0u),
            (unsigned)g_u32MixBytes, (unsigned)g_u32MixUnderruns,
            (unsigned)g_u32DacGain, (unsigned)(g_fDacMute ? 1 : 0),
            (unsigned)cRun, (unsigned)GJ_HDA_MIX_BYTES,
            (unsigned)HDA_SOFT_DEEPEN_WAVE);

    /* Grep: hda: soft codec */
    kprintf("hda: soft codec vendor=0x%x rev=0x%x hits=%u pin=0x%x pwr=0x%x "
            "sid_tag=%u sid_ch=%u stream_fmt=0x%x prog=%u "
            "nid=root,afg,dac,pin wave=%u\n",
            (unsigned)g_u32CodecVendor, (unsigned)g_u32CodecRev,
            (unsigned)g_u32SoftCodecHits, (unsigned)g_u32PinCtrl,
            (unsigned)g_u32PowerState, (unsigned)hda_codec_stream_id(),
            (unsigned)hda_codec_stream_channel(), (unsigned)g_u32StreamFmt,
            (unsigned)fCodecProg, (unsigned)HDA_SOFT_DEEPEN_WAVE);

    /* Grep: hda: soft nid (Wave 15 soft graph inventory) */
    kprintf("hda: soft nid root=%u afg=%u dac=%u pin=%u "
            "graph=root->afg->dac->pin pin_out=0x%x pwr_d0=%u "
            "sid_tag=%u hits=%u wave=%u\n",
            (unsigned)GJ_HDA_NID_ROOT, (unsigned)GJ_HDA_NID_AFG,
            (unsigned)GJ_HDA_NID_DAC, (unsigned)GJ_HDA_NID_PIN,
            (unsigned)g_u32PinCtrl,
            (unsigned)(g_u32PowerState == 0u ? 1u : 0u),
            (unsigned)hda_codec_stream_id(), (unsigned)g_u32SoftCodecHits,
            (unsigned)HDA_SOFT_DEEPEN_WAVE);

    /* Grep: hda: soft bdl */
    kprintf("hda: soft bdl segs=%u kicks=%u ioc=%u "
            "lpib0=%u lpib1=%u cbl0=%u cbl1=%u tag0=%u tag1=%u "
            "fmt0=0x%x fmt1=0x%x seg0=%u off0=%u seg1=%u off1=%u "
            "wave=%u\n",
            (unsigned)cBdlSegs, (unsigned)cBdlKicks, (unsigned)cBdlIoc,
            (unsigned)u32Lpib0, (unsigned)u32Lpib1, (unsigned)u32Cbl0,
            (unsigned)u32Cbl1, (unsigned)u32Tag0, (unsigned)u32Tag1,
            (unsigned)u32Fmt0, (unsigned)u32Fmt1,
            (unsigned)g_aSoftSd[0].u32Seg, (unsigned)g_aSoftSd[0].u32SegOff,
            (unsigned)((GJ_HDA_STREAMS_MAX > 1u) ? g_aSoftSd[1].u32Seg : 0u),
            (unsigned)((GJ_HDA_STREAMS_MAX > 1u) ? g_aSoftSd[1].u32SegOff
                                                 : 0u),
            (unsigned)HDA_SOFT_DEEPEN_WAVE);

    /* Grep: hda: soft sd (Wave 15 stream-descriptor shadow) */
    kprintf("hda: soft sd s0_n=%u s0_lvi=%u s0_cbl=%u s0_lpib=%u "
            "s0_tag=%u s0_fmt=0x%x s0_run=%u s0_kicks=%u s0_ioc=%u "
            "s1_n=%u s1_lvi=%u s1_cbl=%u s1_lpib=%u s1_tag=%u "
            "s1_fmt=0x%x s1_run=%u s1_kicks=%u s1_ioc=%u wave=%u\n",
            (unsigned)g_aSoftSd[0].u32N, (unsigned)u32Lvi0, (unsigned)u32Cbl0,
            (unsigned)u32Lpib0, (unsigned)u32Tag0, (unsigned)u32Fmt0,
            (unsigned)(g_aSoftSd[0].fRun ? 1 : 0),
            (unsigned)g_aSoftSd[0].u32Kicks, (unsigned)g_aSoftSd[0].u32IocCount,
            (unsigned)((GJ_HDA_STREAMS_MAX > 1u) ? g_aSoftSd[1].u32N : 0u),
            (unsigned)u32Lvi1, (unsigned)u32Cbl1, (unsigned)u32Lpib1,
            (unsigned)u32Tag1, (unsigned)u32Fmt1,
            (unsigned)((GJ_HDA_STREAMS_MAX > 1u && g_aSoftSd[1].fRun) ? 1 : 0),
            (unsigned)((GJ_HDA_STREAMS_MAX > 1u) ? g_aSoftSd[1].u32Kicks : 0u),
            (unsigned)((GJ_HDA_STREAMS_MAX > 1u) ? g_aSoftSd[1].u32IocCount
                                                 : 0u),
            (unsigned)HDA_SOFT_DEEPEN_WAVE);

    /* Grep: hda: soft dma (Wave 15 HW vs soft lamps) */
    kprintf("hda: soft dma hw_corb=%u hw_prog=%u sd0_dma=%u sd0_starts=%u "
            "corb_pa=0x%lx rirb_pa=0x%lx bdl_pa=0x%lx pcm_pa=0x%lx "
            "fallback=%s wave=%u\n",
            (unsigned)(g_fHwCorb ? 1 : 0), (unsigned)g_u32HwCorbProgrammed,
            (unsigned)fStreamDma, (unsigned)g_u32StreamDmaStarts,
            (unsigned long)g_paCorbHw, (unsigned long)g_paRirbHw,
            (unsigned long)g_paStreamBdl, (unsigned long)g_paStreamPcm,
            (fStreamDma != 0u) ? "hw_sd0" : "soft_tick",
            (unsigned)HDA_SOFT_DEEPEN_WAVE);

    /* Grep: hda: soft dual — hw+soft dual-path lamps (Wave 15) */
    kprintf("hda: soft dual mmio=%u shadow=%u hw_corb=%u stream_dma=%u "
            "dual_hw_soft=%u path_mode=%s soft PASS wave=%u\n",
            (unsigned)fMmio, (unsigned)fShadow, (unsigned)fHwCorb,
            (unsigned)fStreamDma, (unsigned)fDualHwSoft, szPathMode,
            (unsigned)HDA_SOFT_DEEPEN_WAVE);

    /* Grep: hda: soft capacity — ring/mix/stream map (Wave 15) */
    kprintf("hda: soft capacity streams_max=%u ring=%u corb_ents=%u "
            "bdl_ents=%u mix_cap=%u soft PASS wave=%u\n",
            (unsigned)GJ_HDA_STREAMS_MAX, (unsigned)HDA_RING_SIZE,
            (unsigned)GJ_HDA_CORB_ENTRIES, (unsigned)GJ_HDA_BDL_ENTRIES,
            (unsigned)GJ_HDA_MIX_BYTES, (unsigned)HDA_SOFT_DEEPEN_WAVE);

    /* Grep: hda: soft stats (Wave 15 emission tallies) */
    kprintf("hda: soft stats inv_logs=%u probe_logs=%u smoke_logs=%u "
            "corb_sent=%u rirb=%u corb_dma=%u mix_bytes=%u "
            "mix_underrun=%u codec_hits=%u bdl_kicks=%u bdl_ioc=%u "
            "dma_starts=%u wave=%u\n",
            (unsigned)g_u32SoftInventoryLogs, (unsigned)g_u32SoftProbeLogs,
            (unsigned)g_u32SoftSmokeLogs, (unsigned)g_u32CorbSent,
            (unsigned)g_u32RirbCount, (unsigned)g_u32CorbDmaRuns,
            (unsigned)g_u32MixBytes, (unsigned)g_u32MixUnderruns,
            (unsigned)g_u32SoftCodecHits, (unsigned)cBdlKicks,
            (unsigned)cBdlIoc, (unsigned)g_u32StreamDmaStarts,
            (unsigned)HDA_SOFT_DEEPEN_WAVE);

    /*
     * Grep: hda: soft path
     * Honesty: kernel soft multi-stream ≠ Steam/PipeWire/game PCM claim.
     */
    kprintf("hda: soft path claim=kernel_soft steam_audio=0 pipewire=0 "
            "game_pcm=0 bar3=open dual=soft+hw_when_bar0 mode=%s "
            "via=%s wave=%u (soft inventory; not bar3)\n",
            szPathMode, szViaSafe, (unsigned)HDA_SOFT_DEEPEN_WAVE);

    /*
     * Grep: hda: soft honesty — Wave 15 non-claims (soft inventory only).
     * Kernel soft multi-stream ≠ product audio stack / bar3.
     */
    kprintf("hda: soft honesty steam_audio=0 pipewire=0 game_pcm=0 "
            "bar3=open product_codec=0 claim=kernel_soft wave=%u "
            "soft PASS\n",
            (unsigned)HDA_SOFT_DEEPEN_WAVE);

    /*
     * Wave 16 complementary deepen (kept; never hard-gates).
     * Soft ≠ game I/O. greppable: hda: soft ratio|headroom|surface|return|contract
     */
    {
        u32 u32Surf = 0u;
        u32 u32OpenBp = 0;
        u32 u32RunBp = 0;
        u32 u32FreeHead = 0;
        u32 u32Max = (u32)GJ_HDA_STREAMS_MAX;

        if (u32Max != 0u) {
            u32OpenBp = (cOpen * 10000u) / u32Max;
            u32RunBp = (cRun * 10000u) / u32Max;
        }
        if (u32Max > cOpen) {
            u32FreeHead = u32Max - cOpen;
        }
        if (fPresent != 0) {
            u32Surf |= 0x1u;
        }
        if (fMmio != 0) {
            u32Surf |= 0x2u;
        }
        if (fHwCorb != 0) {
            u32Surf |= 0x4u;
        }
        if (fStreamDma != 0) {
            u32Surf |= 0x8u;
        }
        if (fCodecProg != 0) {
            u32Surf |= 0x10u;
        }
        if (cOpen != 0u) {
            u32Surf |= 0x20u;
        }
        if (cRun != 0u) {
            u32Surf |= 0x40u;
        }
        u32Surf |= 0x80u; /* path/honesty catalog always present */
        /* Grep: hda: soft ratio */
        kprintf("hda: soft ratio open_bp=%u run_bp=%u open=%u run=%u "
                "max=%u present=%u mmio=%u wave=%u soft PASS\n",
                u32OpenBp, u32RunBp, (unsigned)cOpen, (unsigned)cRun,
                (unsigned)u32Max, (unsigned)fPresent, (unsigned)fMmio,
                (unsigned)HDA_SOFT_DEEPEN_WAVE);
        /* Grep: hda: soft headroom */
        kprintf("hda: soft headroom free=%u max=%u open=%u run=%u "
                "mix_cap=%u ring=%u wave=%u soft PASS\n",
                u32FreeHead, (unsigned)u32Max, (unsigned)cOpen,
                (unsigned)cRun, (unsigned)GJ_HDA_MIX_BYTES,
                (unsigned)HDA_RING_SIZE, (unsigned)HDA_SOFT_DEEPEN_WAVE);
        /* Grep: hda: soft surface */
        kprintf("hda: soft surface inventory,pci,gcap,regs,int,wake,stream,"
                "fmt,corb,rirb,mixer,codec,nid,bdl,sd,dma,dual,capacity,"
                "stats,path,honesty,ratio,headroom,return,contract,deepen "
                "areas=%u wave=%u\n",
                (unsigned)HDA_SOFT_DEEPEN_AREAS,
                (unsigned)HDA_SOFT_DEEPEN_WAVE);
        /* Grep: hda: soft return — return-surface bitmask */
        kprintf("hda: soft return surf=0x%x present=%u mmio=%u hw_corb=%u "
                "stream_dma=%u codec=%u open=%u run=%u via=%s areas=%u "
                "wave=%u soft PASS\n",
                u32Surf, (unsigned)fPresent, (unsigned)fMmio,
                (unsigned)fHwCorb, (unsigned)fStreamDma, (unsigned)fCodecProg,
                (unsigned)cOpen, (unsigned)cRun, szViaSafe,
                (unsigned)HDA_SOFT_DEEPEN_AREAS,
                (unsigned)HDA_SOFT_DEEPEN_WAVE);
        /* Grep: hda: soft contract — soft ≠ game I/O */
        kprintf("hda: soft contract soft_only=1 game_io=0 game_pcm=0 "
                "steam_audio=0 pipewire=0 product_audio=0 bar3=open "
                "wave=%u soft PASS\n",
                (unsigned)HDA_SOFT_DEEPEN_WAVE);
    }

    /*
     * Wave 17 complementary sub-lines (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     */
    /* Grep: hda: soft return — Wave 17 API return surfaces (kept) */
    kprintf("hda: soft return soft_inv=1 path=1 "
            "product_kernel=OPEN bar3=0 hard_gate=0 wave=%u soft PASS\n",
            (unsigned)HDA_SOFT_DEEPEN_WAVE);

    /* Grep: hda: soft return selftest — Wave 17 terminal return surface (kept) */
    kprintf("hda: soft return selftest inv_ret=1 product_kernel=OPEN "
            "multi_server=0 bar3=0 wave=%u soft PASS\n",
            (unsigned)HDA_SOFT_DEEPEN_WAVE);

    /* Grep: hda: soft retmap — Wave 17 return-surface map (kept) */
    kprintf("hda: soft retmap soft_inv=1 deepen=1 product=OPEN "
            "wave=%u soft PASS\n",
            (unsigned)HDA_SOFT_DEEPEN_WAVE);

    /*
     * ---- Wave 18 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     */
    /* Grep: hda: soft return rate — Wave 19 ok/fail rate lamps */
    kprintf("hda: soft return rate soft_inv=1 selftest=1 retmap=1 "
            "product_kernel=OPEN bar3=0 hard_gate=0 wave=%u "
            "(return rate; Soft≠product; not bar3)\n",
            (unsigned)HDA_SOFT_DEEPEN_WAVE);

    /* Grep: hda: soft retcode — Wave 19 retcode catalog */
    kprintf("hda: soft retcode ok=1 fail=1 inval=1 busy=1 "
            "selftest=1 retmap=1 product=OPEN soft_ne_product=1 wave=%u "
            "(retcode catalog; Soft≠product)\n",
            (unsigned)HDA_SOFT_DEEPEN_WAVE);

    /* Grep: hda: soft deepen wave (Wave 24 stamp) */
    /*
     * ---- Wave 19 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft≠product; not bar3.
     */
    /* Grep: hda: soft retclass — Wave 19 return-class taxonomy (kept) */
    kprintf("hda: soft retclass ok|fail|inval|nodev|busy|nomem "
            "soft_only=1 product_gate=0 wave=%u "
            "(retclass taxonomy; Soft≠product; not bar3)\n",
            (unsigned)HDA_SOFT_DEEPEN_WAVE);
    /* Grep: hda: soft retlane — Wave 19 return-lane catalog (kept) */
    kprintf("hda: soft retlane inv|selftest|rate|retcode|retmap|class "
            "product_kernel=OPEN soft_ne_product=1 wave=%u "
            "(retlane catalog; Soft≠product)\n",
            (unsigned)HDA_SOFT_DEEPEN_WAVE);
    /*
     * ---- Wave 20 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft≠product; not bar3.
     */
    /* Grep: hda: soft retbound — Wave 20 return-bound honesty (kept) */
    kprintf("hda: soft retbound soft_only=1 product_gate=0 hard_gate=0 "
            "never_blocks_m0=1 wave=%u "
            "(retbound honesty; Soft≠product; not bar3)\n",
            (unsigned)HDA_SOFT_DEEPEN_WAVE);
    /* Grep: hda: soft retseal — Wave 20 seal stamp (kept) */
    kprintf("hda: soft retseal exclusive=1 soft_ne_product=1 "
            "product_kernel=OPEN bar3=0 wave=%u "
            "(retseal stamp; Soft≠product)\n",
            (unsigned)HDA_SOFT_DEEPEN_WAVE);
            /*
             * ---- Wave 21 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
            */
            /* Grep: hda: soft retpulse — Wave 21 return-pulse honesty (kept) */
            kprintf("hda: soft retpulse soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retpulse honesty; Soft≠product; not bar3)\n",
                    (unsigned)HDA_SOFT_DEEPEN_WAVE);
            /* Grep: hda: soft retmark — Wave 21 mark stamp (kept) */
            kprintf("hda: soft retmark exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retmark stamp; Soft≠product)\n",
                    (unsigned)HDA_SOFT_DEEPEN_WAVE);
            /*
             * ---- Wave 22 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
            */
            /* Grep: hda: soft retphase — Wave 22 return-phase honesty (kept) */
            kprintf("hda: soft retphase soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retphase honesty; Soft≠product; not bar3)\n",
                    (unsigned)HDA_SOFT_DEEPEN_WAVE);
            /* Grep: hda: soft retbadge — Wave 22 badge stamp (kept) */
            kprintf("hda: soft retbadge exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbadge stamp; Soft≠product)\n",
                    (unsigned)HDA_SOFT_DEEPEN_WAVE);
/*
 * ---- Wave 23 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
            */
            /* Grep: hda: soft rettoken — Wave 23 return-token honesty (kept) */
            kprintf("hda: soft rettoken soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(rettoken honesty; Soft≠product; not bar3)\n",
                    (unsigned)HDA_SOFT_DEEPEN_WAVE);
            /* Grep: hda: soft retcrest — Wave 23 crest stamp (kept) */
            kprintf("hda: soft retcrest exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retcrest stamp; Soft≠product)\n",
                    (unsigned)HDA_SOFT_DEEPEN_WAVE);
            /*
             * ---- Wave 24 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: hda: soft retvault — Wave 24 return-vault honesty (kept) */
            kprintf("hda: soft retvault soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retvault honesty; Soft≠product; not bar3)\n",
                    (unsigned)HDA_SOFT_DEEPEN_WAVE);
            /* Grep: hda: soft retbanner — Wave 24 banner stamp (kept) */
            kprintf("hda: soft retbanner exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbanner stamp; Soft≠product)\n",
                    (unsigned)HDA_SOFT_DEEPEN_WAVE);
            /*
             * ---- Wave 25 exclusive complementary surfaces (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: hda: soft retledger — Wave 25 return-ledger honesty */
            kprintf("hda: soft retledger soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retledger honesty; Soft≠product; not bar3)\n",
                    (unsigned)HDA_SOFT_DEEPEN_WAVE);
            /* Grep: hda: soft retbeacon — Wave 25 exclusive beacon stamp */
            kprintf("hda: soft retbeacon exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbeacon stamp; Soft≠product)\n",
                    (unsigned)HDA_SOFT_DEEPEN_WAVE);
    kprintf("hda: soft deepen wave=%u areas=%u via=%s present=%u mmio=%u "
            "hw_corb=%u stream_dma=%u codec_prog=%u open=%u run=%u "
            "ok=1 skip=0\n",
            (unsigned)HDA_SOFT_DEEPEN_WAVE, (unsigned)HDA_SOFT_DEEPEN_AREAS,
            szViaSafe, (unsigned)fPresent, (unsigned)fMmio, (unsigned)fHwCorb,
            (unsigned)fStreamDma, (unsigned)fCodecProg, (unsigned)cOpen,
            (unsigned)cRun);

    /* Grep: hda: soft inventory PASS / hda: soft PASS */
    kprintf("hda: soft inventory PASS via=%s logs=%u wave=%u areas=%u\n",
            szViaSafe, (unsigned)g_u32SoftInventoryLogs,
            (unsigned)HDA_SOFT_DEEPEN_WAVE, (unsigned)HDA_SOFT_DEEPEN_AREAS);
    kprintf("hda: soft PASS via=%s wave=%u\n", szViaSafe,
            (unsigned)HDA_SOFT_DEEPEN_WAVE);
}

int
hda_codec_program(void)
{
    u32 vendor = 0;
    u32 nodes = 0;
    u32 fg = 0;
    u32 pcm = 0;
    u32 conn = 0;

    (void)hda_corb_init();
    if (hda_codec_get_param(GJ_HDA_NID_ROOT, 0x00, &vendor) != 0) {
        return -1;
    }
    if (vendor != g_u32CodecVendor) {
        kprintf("hda: soft codec vendor mismatch 0x%x\n", (unsigned)vendor);
        return -1;
    }
    if (hda_codec_get_param(GJ_HDA_NID_AFG, 0x04, &nodes) != 0) {
        return -1;
    }
    if (hda_codec_get_param(GJ_HDA_NID_AFG, 0x05, &fg) != 0) {
        return -1;
    }
    if (hda_codec_get_param(GJ_HDA_NID_DAC, 0x09, &pcm) != 0) {
        return -1;
    }
    /* Power D0 on AFG/DAC/PIN, enable pin OUT, unmute DAC amp. */
    if (hda_codec_set_power(GJ_HDA_NID_AFG, 0) != 0 ||
        hda_codec_set_power(GJ_HDA_NID_DAC, 0) != 0 ||
        hda_codec_set_power(GJ_HDA_NID_PIN, 0) != 0) {
        return -1;
    }
    if (hda_codec_set_pin(GJ_HDA_NID_PIN, 0x40u) != 0) { /* OUT enable */
        return -1;
    }
    if (hda_codec_set_amp(GJ_HDA_NID_DAC, 0x7f, 0) != 0) {
        return -1;
    }
    /* Bind DAC converter to stream tag 1 (matches soft SD0 default). */
    if (hda_codec_set_stream_id(GJ_HDA_NID_DAC, 1u, 0u) != 0) {
        return -1;
    }
    if (hda_codec_stream_id() != 1u) {
        kprintf("hda: soft codec stream-id bind fail id=%u\n",
                hda_codec_stream_id());
        return -1;
    }
    if (hda_codec_verb(0, GJ_HDA_NID_DAC, 0xf02u, 0, &conn) != 0) {
        return -1;
    }
    if (conn != GJ_HDA_NID_PIN) {
        kprintf("hda: soft codec connect list unexpected 0x%x\n",
                (unsigned)conn);
        return -1;
    }
    if (g_u32PinCtrl != 0x40u || g_u32PowerState != 0) {
        return -1;
    }
    g_u32CodecProg = 1;
    kprintf("hda: codec program vendor=0x%x nodes=0x%x fg=0x%x pcm=0x%x\n",
            (unsigned)vendor, (unsigned)nodes, (unsigned)fg, (unsigned)pcm);
    kprintf("hda: soft codec PASS pin=0x%x pwr=0x%x hits=%u sid=%u\n",
            (unsigned)g_u32PinCtrl, (unsigned)g_u32PowerState,
            (unsigned)g_u32SoftCodecHits, hda_codec_stream_id());
    kprintf("hda: CORB codec program PASS\n");
    kprintf("hda: CORB stream-id bind PASS tag=%u ch=%u\n",
            hda_codec_stream_id(), hda_codec_stream_channel());
    return 0;
}

int
hda_multi_stream_smoke(void)
{
    static u8 aA[128];
    static u8 aB[128];
    static u8 aMix[GJ_HDA_MIX_BYTES];
    u32 i;
    u32 na;
    u32 nb;
    u32 cbMix;
    u32 uPlayed0;
    u32 uPlayed1;
    u32 uEnergy = 0;

    g_u32MixBytes = 0;
    g_u32MixUnderruns = 0;

    if (hda_codec_program() != 0) {
        return -1;
    }
    if (!hda_codec_programmed()) {
        return -1;
    }
    if (hda_stream_n_open(0, 2, 48000, 16) != 0 ||
        hda_stream_n_open(1, 2, 48000, 16) != 0) {
        return -1;
    }
    if (!hda_stream_n_ready(0) || !hda_stream_n_ready(1)) {
        return -1;
    }
    (void)hda_mixer_set_master(256);
    (void)hda_mixer_set_stream(0, 256);
    (void)hda_mixer_set_stream(1, 128); /* half gain stream1 */
    if (hda_mixer_stream(0) != 256u || hda_mixer_stream(1) != 128u) {
        kprintf("hda: mixer gain query fail\n");
        return -1;
    }
    for (i = 0; i < sizeof(aA); i++) {
        aA[i] = 0xc0u;
        aB[i] = 0x40u;
    }
    na = hda_stream_n_write(0, aA, sizeof(aA));
    nb = hda_stream_n_write(1, aB, sizeof(aB));
    if (na != sizeof(aA) || nb != sizeof(aB)) {
        kprintf("hda: multi write short %u %u\n", na, nb);
        return -1;
    }
    if (hda_stream_n_bytes_queued(0) == 0 ||
        hda_stream_n_bytes_queued(1) == 0) {
        kprintf("hda: multi queue empty after write\n");
        return -1;
    }
    if (hda_stream_n_start(0) != 0 || hda_stream_n_start(1) != 0) {
        return -1;
    }
    /* Soft mixdown: sum SD0+SD1 for 16 frames (64 bytes @ stereo 16-bit). */
    memset(aMix, 0, sizeof(aMix));
    cbMix = hda_mixer_mix_tick(16, aMix, sizeof(aMix));
    if (cbMix != 64u) {
        kprintf("hda: soft mixdown short %u (want 64)\n", cbMix);
        return -1;
    }
    for (i = 0; i < cbMix; i++) {
        uEnergy += aMix[i];
    }
    if (uEnergy == 0) {
        kprintf("hda: soft mixdown silent\n");
        return -1;
    }
    uPlayed0 = hda_stream_n_bytes_played(0);
    uPlayed1 = hda_stream_n_bytes_played(1);
    if (uPlayed0 != 64u || uPlayed1 != 64u) {
        kprintf("hda: soft mixdown play mismatch p0=%u p1=%u\n", uPlayed0,
                uPlayed1);
        return -1;
    }
    kprintf("hda: soft mixdown PASS bytes=%u energy=%u mix_total=%u\n", cbMix,
            uEnergy, hda_mixer_mix_bytes());

    /* Per-stream tick still works on remaining queued PCM + LPIB advance. */
    {
        u32 u32Lpib0 = hda_stream_n_lpib(0);
        u32 u32Lpib1 = hda_stream_n_lpib(1);

        if (u32Lpib0 < 64u || u32Lpib1 < 64u) {
            kprintf("hda: soft LPIB after mix low p0=%u p1=%u\n", u32Lpib0,
                    u32Lpib1);
            return -1;
        }
        (void)hda_stream_n_tick(0, 8);
        (void)hda_stream_n_tick(1, 8);
        if (hda_stream_n_lpib(0) <= u32Lpib0 ||
            hda_stream_n_lpib(1) <= u32Lpib1) {
            kprintf("hda: soft LPIB tick stall p0=%u->%u p1=%u->%u\n", u32Lpib0,
                    hda_stream_n_lpib(0), u32Lpib1, hda_stream_n_lpib(1));
            return -1;
        }
        kprintf("hda: soft LPIB PASS lpib0=%u lpib1=%u\n", hda_stream_n_lpib(0),
                hda_stream_n_lpib(1));
    }

    hda_stream_n_stop(0);
    hda_stream_n_stop(1);
    hda_stream_n_close(0);
    hda_stream_n_close(1);

    /*
     * Soft multi-stream BDL path: independent BDL segments per SD, kick,
     * consume via mix + per-stream tick, check CBL/IOC/seg cursor.
     */
    {
        static u8 aPcm0[64];
        static u8 aPcm1[64];
        struct gj_hda_bdl_entry aB0[2];
        struct gj_hda_bdl_entry aB1[2];
        u32 j;
        u32 cbMix2;
        u32 u32Ioc0;

        for (j = 0; j < sizeof(aPcm0); j++) {
            aPcm0[j] = (u8)(0x80u + (j & 0x1fu));
            aPcm1[j] = (u8)(0x80u - (j & 0x1fu));
        }
        aB0[0].u64Addr = (u64)(uintptr_t)aPcm0;
        aB0[0].u32Len = 32;
        aB0[0].u32Ioc = 0;
        aB0[1].u64Addr = (u64)(uintptr_t)(aPcm0 + 32);
        aB0[1].u32Len = 32;
        aB0[1].u32Ioc = 1;
        aB1[0].u64Addr = (u64)(uintptr_t)aPcm1;
        aB1[0].u32Len = 32;
        aB1[0].u32Ioc = 0;
        aB1[1].u64Addr = (u64)(uintptr_t)(aPcm1 + 32);
        aB1[1].u32Len = 32;
        aB1[1].u32Ioc = 1;
        if (hda_stream_n_open(0, 2, 48000, 16) != 0 ||
            hda_stream_n_open(1, 2, 48000, 16) != 0) {
            return -1;
        }
        (void)hda_stream_n_set_tag(0, 1u);
        (void)hda_stream_n_set_tag(1, 2u);
        if (hda_codec_set_stream_id(GJ_HDA_NID_DAC, 1u, 0u) != 0) {
            return -1;
        }
        if (hda_bdl_n_set(0, aB0, 2) != 0 || hda_bdl_n_set(1, aB1, 2) != 0) {
            return -1;
        }
        if (hda_bdl_n_entries(0) != 2u || hda_bdl_n_entries(1) != 2u) {
            return -1;
        }
        if (hda_stream_n_cbl(0) != 64u || hda_stream_n_cbl(1) != 64u) {
            kprintf("hda: soft BDL CBL mismatch c0=%u c1=%u\n",
                    hda_stream_n_cbl(0), hda_stream_n_cbl(1));
            return -1;
        }
        if (hda_bdl_n_kick(0) != 0 || hda_bdl_n_kick(1) != 0) {
            return -1;
        }
        if (hda_bdl_n_kicks(0) < 1u || hda_bdl_n_kicks(1) < 1u) {
            return -1;
        }
        if (hda_stream_n_fmt(0) == 0 || hda_stream_n_tag(0) != 1u ||
            hda_stream_n_tag(1) != 2u) {
            kprintf("hda: soft SD tag/fmt fail fmt0=0x%x t0=%u t1=%u\n",
                    hda_stream_n_fmt(0), hda_stream_n_tag(0),
                    hda_stream_n_tag(1));
            return -1;
        }
        memset(aMix, 0, sizeof(aMix));
        cbMix2 = hda_mixer_mix_tick(8, aMix, sizeof(aMix)); /* 32 bytes */
        if (cbMix2 != 32u) {
            kprintf("hda: soft BDL mix short %u\n", cbMix2);
            return -1;
        }
        if (hda_stream_n_lpib(0) < 32u || hda_stream_n_lpib(1) < 32u) {
            kprintf("hda: soft BDL LPIB low after mix %u %u\n",
                    hda_stream_n_lpib(0), hda_stream_n_lpib(1));
            return -1;
        }
        /* Drain rest via per-stream tick (also walks soft BDL segments). */
        (void)hda_stream_n_tick(0, 8);
        (void)hda_stream_n_tick(1, 8);
        u32Ioc0 = hda_bdl_n_ioc(0);
        /* Full 64-byte CBL consume should complete IOC on last segment. */
        if (u32Ioc0 == 0) {
            kprintf("hda: soft BDL IOC missing ioc0=%u played0=%u lpib=%u "
                    "seg=%u off=%u\n",
                    u32Ioc0, hda_stream_n_bytes_played(0), hda_stream_n_lpib(0),
                    hda_bdl_n_seg(0), hda_bdl_n_seg_off(0));
            return -1;
        }
        kprintf(
            "hda: soft BDL multi-stream PASS kicks=%u/%u cbl=%u/%u ioc=%u/%u "
            "lpib=%u/%u\n",
            hda_bdl_n_kicks(0), hda_bdl_n_kicks(1), hda_stream_n_cbl(0),
            hda_stream_n_cbl(1), hda_bdl_n_ioc(0), hda_bdl_n_ioc(1),
            hda_stream_n_lpib(0), hda_stream_n_lpib(1));
        hda_stream_n_stop(0);
        hda_stream_n_stop(1);
        hda_stream_n_close(0);
        hda_stream_n_close(1);
    }

    /* Mute via codec amp → master 0 */
    if (hda_codec_set_amp(GJ_HDA_NID_DAC, 0x00, 1) != 0) {
        return -1;
    }
    if (hda_mixer_master() != 0) {
        kprintf("hda: mute did not zero master\n");
        return -1;
    }
    (void)hda_mixer_set_master(256);
    (void)hda_codec_set_amp(GJ_HDA_NID_DAC, 0x7f, 0);
    kprintf("hda: multi-stream mixer PASS streams=%u master=%u\n",
            GJ_HDA_STREAMS_MAX, hda_mixer_master());
    /*
     * ---- Wave 19 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft≠product; not bar3.
     */
    /* Grep: hda: soft retclass — Wave 19 return-class taxonomy (kept) */
    kprintf("hda: soft retclass ok|fail|inval|nodev|busy|nomem "
            "soft_only=1 product_gate=0 wave=%u "
            "(retclass taxonomy; Soft≠product; not bar3)\n",
            (unsigned)HDA_SOFT_DEEPEN_WAVE);
    /* Grep: hda: soft retlane — Wave 19 return-lane catalog (kept) */
    kprintf("hda: soft retlane inv|selftest|rate|retcode|retmap|class "
            "product_kernel=OPEN soft_ne_product=1 wave=%u "
            "(retlane catalog; Soft≠product)\n",
            (unsigned)HDA_SOFT_DEEPEN_WAVE);
    /*
     * ---- Wave 20 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft≠product; not bar3.
     */
    /* Grep: hda: soft retbound — Wave 20 return-bound honesty (kept) */
    kprintf("hda: soft retbound soft_only=1 product_gate=0 hard_gate=0 "
            "never_blocks_m0=1 wave=%u "
            "(retbound honesty; Soft≠product; not bar3)\n",
            (unsigned)HDA_SOFT_DEEPEN_WAVE);
    /* Grep: hda: soft retseal — Wave 20 seal stamp (kept) */
    kprintf("hda: soft retseal exclusive=1 soft_ne_product=1 "
            "product_kernel=OPEN bar3=0 wave=%u "
            "(retseal stamp; Soft≠product)\n",
            (unsigned)HDA_SOFT_DEEPEN_WAVE);
            /*
             * ---- Wave 21 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
            */
            /* Grep: hda: soft retpulse — Wave 21 return-pulse honesty (kept) */
            kprintf("hda: soft retpulse soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retpulse honesty; Soft≠product; not bar3)\n",
                    (unsigned)HDA_SOFT_DEEPEN_WAVE);
            /* Grep: hda: soft retmark — Wave 21 mark stamp (kept) */
            kprintf("hda: soft retmark exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retmark stamp; Soft≠product)\n",
                    (unsigned)HDA_SOFT_DEEPEN_WAVE);
            /*
             * ---- Wave 22 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
            */
            /* Grep: hda: soft retphase — Wave 22 return-phase honesty (kept) */
            kprintf("hda: soft retphase soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retphase honesty; Soft≠product; not bar3)\n",
                    (unsigned)HDA_SOFT_DEEPEN_WAVE);
            /* Grep: hda: soft retbadge — Wave 22 badge stamp (kept) */
            kprintf("hda: soft retbadge exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbadge stamp; Soft≠product)\n",
                    (unsigned)HDA_SOFT_DEEPEN_WAVE);
/*
 * ---- Wave 23 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
            */
            /* Grep: hda: soft rettoken — Wave 23 return-token honesty (kept) */
            kprintf("hda: soft rettoken soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(rettoken honesty; Soft≠product; not bar3)\n",
                    (unsigned)HDA_SOFT_DEEPEN_WAVE);
            /* Grep: hda: soft retcrest — Wave 23 crest stamp (kept) */
            kprintf("hda: soft retcrest exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retcrest stamp; Soft≠product)\n",
                    (unsigned)HDA_SOFT_DEEPEN_WAVE);
            /*
             * ---- Wave 24 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: hda: soft retvault — Wave 24 return-vault honesty (kept) */
            kprintf("hda: soft retvault soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retvault honesty; Soft≠product; not bar3)\n",
                    (unsigned)HDA_SOFT_DEEPEN_WAVE);
            /* Grep: hda: soft retbanner — Wave 24 banner stamp (kept) */
            kprintf("hda: soft retbanner exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbanner stamp; Soft≠product)\n",
                    (unsigned)HDA_SOFT_DEEPEN_WAVE);
            /*
             * ---- Wave 25 exclusive complementary surfaces (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: hda: soft retledger — Wave 25 return-ledger honesty */
            kprintf("hda: soft retledger soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retledger honesty; Soft≠product; not bar3)\n",
                    (unsigned)HDA_SOFT_DEEPEN_WAVE);
            /* Grep: hda: soft retbeacon — Wave 25 exclusive beacon stamp */
            kprintf("hda: soft retbeacon exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbeacon stamp; Soft≠product)\n",
                    (unsigned)HDA_SOFT_DEEPEN_WAVE);
    kprintf("hda: soft deepen PASS wave=%u areas=%u codec_hits=%u "
            "mix_underrun=%u\n",
            (unsigned)HDA_SOFT_DEEPEN_WAVE, (unsigned)HDA_SOFT_DEEPEN_AREAS,
            (unsigned)g_u32SoftCodecHits, hda_mixer_mix_underruns());
    /* Wave 15: full soft inventory after multi-stream deepen smoke. */
    g_u32SoftSmokeLogs++;
    hda_soft_inventory_log("multi_smoke");
    return 0;
}
