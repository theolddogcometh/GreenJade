/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room UDX skeleton — Intel PCH xHCI soft host path (G752VT 8086:a12f).
 *
 * WHAT THIS IS
 *   Userspace Linux-shaped driver host against libudx:
 *     match → probe (ioremap BAR0 + soft CAPLENGTH/HCIVERSION/HCSPARAMS1) →
 *     soft port enum (PORTSC snapshot) → soft BOT progress stub →
 *     quiesce → remove.
 *   Public register offsets only (xHCI Specification capability + port space).
 *   No GPL. No Linux xHCI source paste. No product BOT / MSC / stick claim.
 *
 * WHAT THIS IS NOT
 *   Not freestanding kernel/drv/xhci_msc.c (lab soft scaffold stays there —
 *   do not thrash that path from this dual-license host skeleton).
 *   Not a shipping USB host stack. Soft host inject ≠ real MMIO/IRQ/DMA grants.
 *   Soft BOT stage lamps ≠ CBW/CSW / READ CAPACITY / stick log product.
 *
 * HOST BUILD
 *   make -C user/drivers/xhci_udx
 *   ./user/drivers/xhci_udx/build/xhci_udx
 *   Grep: "xhci_udx: soft probe PASS" / "soft bot stage=" / "soft ports "
 *
 * License choice: MIT OR Apache-2.0 (dual). See repo LICENSE.
 */

#include <udx/udx.h>

/* ------------------------------------------------------------------ */
/* PCI identity — G752VT Intel 100 Series / C230 USB 3.0 xHCI.         */
/* Vendor/device from public PCI ID inventory (HCL / Linux lspci).    */
/* Class 0x0c0330 = Serial Bus / USB / xHCI (PCI class code, 24-bit).  */
/* ------------------------------------------------------------------ */
#define XHCI_UDX_VEND       0x8086u
#define XHCI_UDX_DEV        0xa12fu
#define XHCI_UDX_CLASS      0x0c0330u
#define XHCI_UDX_CLASS_MASK 0xffffffu

/* Soft host BDF for G752VT inventory: 0000:00:14.0 */
#define XHCI_UDX_BUS   0u
#define XHCI_UDX_SLOT  0x14u
#define XHCI_UDX_FUNC  0u
#define XHCI_UDX_IRQ   16

/* Intel PCH xHCI BAR0 is typically a 64 KiB memory window. */
#define XHCI_UDX_BAR0_LEN  0x10000ull

/*
 * Capability registers — xHCI Specification (public), base of BAR0 MMIO.
 *
 *   Offset 00h  CAPLENGTH  (RO)  bits 7:0  — length of capability regs
 *   Offset 00h  Rsvd       (RO)  bits 15:8
 *   Offset 02h  HCIVERSION (RO)  16-bit Interface Version Number
 *   Offset 04h  HCSPARAMS1 (RO)  MaxSlots[7:0] MaxIntrs[18:8] MaxPorts[31:24]
 *   Offset 08h  HCSPARAMS2 (RO)  — not programmed by this soft skeleton
 *   ...
 *   Operational registers base = BAR0 + CAPLENGTH
 *   PORTSC[n] (n=1..MaxPorts) at op + 0x400 + (n-1)*0x10  (xHCI 5.4.8)
 *
 * Soft host seeds CAP dword0, HCSPARAMS1, and one PORTSC so probe can
 * lamp soft params / ports / BOT stage progress. No op/runtime/doorbell
 * product program; no rings; no CBW/CSW.
 */
#define XHCI_CAP_CAPLENGTH   0x00ull /* 1-byte CAPLENGTH within dword 0 */
#define XHCI_CAP_HCIVERSION  0x02ull /* 16-bit HCIVERSION */
#define XHCI_CAP_HCSPARAMS1  0x04ull /* MaxSlots / MaxIntrs / MaxPorts */

/* Soft seed values (illustrative; not a claim of live silicon readout). */
#define XHCI_SOFT_CAPLENGTH  0x20u   /* common 32-byte capability region */
#define XHCI_SOFT_HCIVERSION 0x0100u /* xHCI Interface Version 1.0 */
#define XHCI_SOFT_MAX_SLOTS  32u
#define XHCI_SOFT_MAX_INTRS  1u
#define XHCI_SOFT_MAX_PORTS  4u      /* host demo seeds 4 soft root ports */

/* Soft BOT progress stages (lamps only; never product MSC). */
#define XHCI_SOFT_BOT_ST_CAP     1u /* CAPLENGTH + HCIVERSION ok */
#define XHCI_SOFT_BOT_ST_PARAMS  2u /* HCSPARAMS1 MaxPorts snapshot */
#define XHCI_SOFT_BOT_ST_PORTS   3u /* soft PORTSC walk / CCS count */
#define XHCI_SOFT_BOT_ST_STUB    4u /* BOT catalog lamp — no CBW/DMA */

/*
 * Soft port bound for walk (cap MaxPorts may be larger on silicon;
 * skeleton only soft-walks a small table so host inject stays tiny).
 */
#define XHCI_SOFT_PORT_WALK_MAX  8u

/* Packed dword0: CAPLENGTH | (0 << 8) | (HCIVERSION << 16) */
#define XHCI_SOFT_CAP_DWORD0 \
    ((u32)XHCI_SOFT_CAPLENGTH | ((u32)XHCI_SOFT_HCIVERSION << 16))

/* HCSPARAMS1: MaxSlots | MaxIntrs<<8 | MaxPorts<<24 */
#define XHCI_SOFT_HCSPARAMS1 \
    ((u32)XHCI_SOFT_MAX_SLOTS | \
     ((u32)XHCI_SOFT_MAX_INTRS << 8) | \
     ((u32)XHCI_SOFT_MAX_PORTS << 24))

/*
 * PORTSC soft bits (xHCI 5.4.8 public names) — snapshot only.
 * Host seeds one CCS+PED+PP SuperSpeed-looking port for stage=3.
 */
#define XHCI_PORTSC_CCS   (1u << 0)
#define XHCI_PORTSC_PED   (1u << 1)
#define XHCI_PORTSC_PP    (1u << 9)
#define XHCI_PORTSC_SPEED_SHIFT 10u
#define XHCI_PORTSC_SPEED_MASK  0xfu
#define XHCI_SOFT_PORT_SPEED_SS 4u /* SuperSpeed field value (public) */

/* Soft PORTSC seed: CCS|PED|PP|speed=SS */
#define XHCI_SOFT_PORTSC_SEED \
    (XHCI_PORTSC_CCS | XHCI_PORTSC_PED | XHCI_PORTSC_PP | \
     ((u32)XHCI_SOFT_PORT_SPEED_SS << XHCI_PORTSC_SPEED_SHIFT))

/* Optional freestanding bind (may be absent on host libudx). */
#if !defined(UDX_HOST_LIBC)
udx_status_t udx_host_bind_by_id(u16 u16Vendor, u16 u16Device,
                                 struct udx_pci_dev **ppOut)
    __attribute__((weak));
#endif

/*
 * Per-device soft state (Linux: private struct / dev_get_drvdata).
 * Static pool so freestanding hosts need no heap for this skeleton.
 */
struct xhci_udx_soft {
    struct udx_pci_dev *pPdev;
    struct udx_iomem   *pCap;      /* BAR0 capability / MMIO window */
    u8                  u8CapLen;  /* CAPLENGTH soft snapshot */
    u16                 u16HciVer; /* HCIVERSION soft snapshot */
    u8                  u8MaxSlots;
    u8                  u8MaxPorts;
    u16                 u16MaxIntrs;
    u8                  u8PortCcs; /* soft PORTSC CCS count */
    u8                  u8FirstCcs;/* 1-based first CCS port; 0=none */
    u8                  u8FirstSpd;/* PORTSC speed of first CCS */
    u8                  u8BotStage;/* soft BOT progress 1..4 */
    u32                 u32FQuiesced;
    u32                 u32FProbeOk;
    u32                 u32FBotStub; /* soft BOT stub ran (not product) */
};

static struct xhci_udx_soft s_soft;

/**
 * Soft BOT progress lamp — greppable stage path toward stick access.
 * Soft ≠ product: never claims CBW/CSW, rings, or MSC capacity.
 *
 * greppable: xhci_udx: soft bot stage=
 */
static void
xhci_udx_soft_bot_lamp(u8 u8Stage, const char *szExtra)
{
    if (szExtra != NULL && szExtra[0] != '\0') {
        udx_printk("xhci_udx: soft bot stage=%u %s soft≠product\n",
                   (unsigned)u8Stage, szExtra);
    } else {
        udx_printk("xhci_udx: soft bot stage=%u soft≠product\n",
                   (unsigned)u8Stage);
    }
}

/**
 * Soft-read HCSPARAMS1 and lamp stage=2 params.
 * Returns 0 on empty/implausible MaxPorts; non-zero when params usable.
 */
static int
xhci_udx_soft_params(struct xhci_udx_soft *pSoft)
{
    u32 u32Hcs1;
    u8  u8Slots;
    u8  u8Ports;
    u16 u16Intrs;

    if (pSoft == NULL || pSoft->pCap == NULL) {
        return 0;
    }

    u32Hcs1  = udx_readl(pSoft->pCap, XHCI_CAP_HCSPARAMS1);
    u8Slots  = (u8)(u32Hcs1 & 0xffu);
    u16Intrs = (u16)((u32Hcs1 >> 8) & 0x7ffu);
    u8Ports  = (u8)((u32Hcs1 >> 24) & 0xffu);

    pSoft->u8MaxSlots  = u8Slots;
    pSoft->u16MaxIntrs = u16Intrs;
    pSoft->u8MaxPorts  = u8Ports;
    pSoft->u8BotStage  = XHCI_SOFT_BOT_ST_PARAMS;

    udx_printk("xhci_udx: soft params hcs1=0x%08x maxslots=%u maxintrs=%u "
               "maxports=%u\n",
               (unsigned)u32Hcs1, (unsigned)u8Slots, (unsigned)u16Intrs,
               (unsigned)u8Ports);

    xhci_udx_soft_bot_lamp(XHCI_SOFT_BOT_ST_PARAMS, "params");

    /* Soft honesty: zero MaxPorts → cannot soft-walk ports (unseeded). */
    if (u8Ports == 0u) {
        udx_printk("xhci_udx: soft params empty (no MaxPorts seed / no HC)\n");
        return 0;
    }
    return 1;
}

/**
 * Soft PORTSC walk — operational base = BAR0 + CAPLENGTH.
 * Counts CCS; records first connected port. Soft ≠ live port reset/enum.
 *
 * greppable: xhci_udx: soft ports
 */
static int
xhci_udx_soft_ports(struct xhci_udx_soft *pSoft)
{
    u64 u64OpBase;
    u32 u32Walk;
    u32 u32Port;
    u8  u8Ccs;
    u8  u8First;
    u8  u8Spd;

    if (pSoft == NULL || pSoft->pCap == NULL || pSoft->u8CapLen == 0u) {
        return 0;
    }
    if (pSoft->u8MaxPorts == 0u) {
        return 0;
    }

    u64OpBase = (u64)pSoft->u8CapLen;
    u32Walk   = (u32)pSoft->u8MaxPorts;
    if (u32Walk > XHCI_SOFT_PORT_WALK_MAX) {
        u32Walk = XHCI_SOFT_PORT_WALK_MAX;
    }

    u8Ccs   = 0;
    u8First = 0;
    u8Spd   = 0;

    for (u32Port = 1u; u32Port <= u32Walk; u32Port++) {
        u64 u64Off;
        u32 u32Ps;
        u8  u8PortSpd;

        /* PORTSC[n] = op + 0x400 + (n-1)*0x10 (xHCI Spec 5.4.8). */
        u64Off = u64OpBase + 0x400ull + ((u64)(u32Port - 1u) * 0x10ull);
        if (u64Off + 4ull > XHCI_UDX_BAR0_LEN) {
            break;
        }
        u32Ps = udx_readl(pSoft->pCap, u64Off);
        u8PortSpd = (u8)((u32Ps >> XHCI_PORTSC_SPEED_SHIFT) &
                         XHCI_PORTSC_SPEED_MASK);

        if ((u32Ps & XHCI_PORTSC_CCS) != 0u) {
            if (u8Ccs < 0xffu) {
                u8Ccs++;
            }
            if (u8First == 0u) {
                u8First = (u8)u32Port;
                u8Spd   = u8PortSpd;
            }
            udx_printk("xhci_udx: soft port %u ccs=1 ped=%u pp=%u spd=%u "
                       "ps=0x%08x\n",
                       (unsigned)u32Port,
                       (u32Ps & XHCI_PORTSC_PED) != 0u ? 1u : 0u,
                       (u32Ps & XHCI_PORTSC_PP) != 0u ? 1u : 0u,
                       (unsigned)u8PortSpd,
                       (unsigned)u32Ps);
        }
    }

    pSoft->u8PortCcs  = u8Ccs;
    pSoft->u8FirstCcs = u8First;
    pSoft->u8FirstSpd = u8Spd;
    pSoft->u8BotStage = XHCI_SOFT_BOT_ST_PORTS;

    udx_printk("xhci_udx: soft ports n=%u walk=%u ccs=%u first=%u spd=%u "
               "soft≠product\n",
               (unsigned)pSoft->u8MaxPorts, (unsigned)u32Walk,
               (unsigned)u8Ccs, (unsigned)u8First, (unsigned)u8Spd);

    xhci_udx_soft_bot_lamp(XHCI_SOFT_BOT_ST_PORTS, "ports");
    return 1;
}

/**
 * Soft BOT progress stub — lamps the dual-license path toward stick access
 * without programming rings, CBW/CSW, or claiming MSC capacity.
 *
 * Stages already advanced: 1=cap 2=params 3=ports; this stamps stage=4.
 * Product BOT/MSC remains OPEN (docs/LAPTOP_LINUX_DRIVER_HOST.md §2.2).
 *
 * greppable: xhci_udx: soft bot stage=4
 * greppable: xhci_udx: soft bot stub PASS
 */
static void
xhci_udx_soft_bot_stub(struct xhci_udx_soft *pSoft)
{
    if (pSoft == NULL) {
        return;
    }

    pSoft->u8BotStage  = XHCI_SOFT_BOT_ST_STUB;
    pSoft->u32FBotStub = 1u;

    /*
     * Catalog only — what product BOT would touch next (not executed):
     *   Enable Slot → Address Device → GET_DESCRIPTOR → SET_CONFIGURATION →
     *   MSC BOT CBW / CSW / READ CAPACITY. Soft host path stops here.
     */
    if (pSoft->u8FirstCcs != 0u) {
        udx_printk("xhci_udx: soft bot would_try_port=%u spd=%u "
                   "next=slot/addr/enum/bot OPEN soft≠product\n",
                   (unsigned)pSoft->u8FirstCcs,
                   (unsigned)pSoft->u8FirstSpd);
    } else {
        udx_printk("xhci_udx: soft bot no_ccs next=wait_port "
                   "OPEN soft≠product\n");
    }

    xhci_udx_soft_bot_lamp(XHCI_SOFT_BOT_ST_STUB, "stub");
    udx_printk("xhci_udx: soft bot stub PASS stage=%u ccs=%u gpl=0 "
               "soft≠product\n",
               (unsigned)pSoft->u8BotStage, (unsigned)pSoft->u8PortCcs);
}

/* ------------------------------------------------------------------ */
/* probe — enable, map BAR0, soft-read public cap + port + BOT stub.   */
/* ------------------------------------------------------------------ */
static int
xhci_udx_probe(struct udx_pci_dev *pPdev, const struct udx_pci_device_id *pId)
{
    struct xhci_udx_soft *pSoft;
    u64 u64Bar;
    u64 u64Len;
    u8  u8Cap;
    u16 u16Ver;
    u32 u32Dword0;

    if (pPdev == NULL || pPdev->pDev == NULL) {
        return UDX_ERR_INVAL;
    }

    (void)pId;

    if (udx_pci_enable(pPdev) != UDX_OK) {
        return UDX_ERR_IO;
    }
    if (udx_pci_request_regions(pPdev, "xhci_udx") != UDX_OK) {
        udx_pci_disable(pPdev);
        return UDX_ERR_BUSY;
    }

    u64Bar = udx_pci_resource_start(pPdev, 0);
    u64Len = udx_pci_resource_len(pPdev, 0);
    if (u64Bar == 0 || u64Len < 8ull) {
        udx_pci_release_regions(pPdev);
        udx_pci_disable(pPdev);
        return UDX_ERR_IO;
    }
    if (!udx_pci_resource_is_mem(pPdev, 0)) {
        udx_printk("xhci_udx: BAR0 not memory\n");
        udx_pci_release_regions(pPdev);
        udx_pci_disable(pPdev);
        return UDX_ERR_IO;
    }

    pSoft = &s_soft;
    pSoft->pPdev       = pPdev;
    pSoft->pCap        = NULL;
    pSoft->u8CapLen    = 0;
    pSoft->u16HciVer   = 0;
    pSoft->u8MaxSlots  = 0;
    pSoft->u8MaxPorts  = 0;
    pSoft->u16MaxIntrs = 0;
    pSoft->u8PortCcs   = 0;
    pSoft->u8FirstCcs  = 0;
    pSoft->u8FirstSpd  = 0;
    pSoft->u8BotStage  = 0;
    pSoft->u32FQuiesced = 0;
    pSoft->u32FProbeOk = 0;
    pSoft->u32FBotStub = 0;

    pSoft->pCap = udx_ioremap(u64Bar, u64Len);
    if (pSoft->pCap == NULL) {
        udx_pci_release_regions(pPdev);
        udx_pci_disable(pPdev);
        return UDX_ERR_NOMEM;
    }

    /*
     * Soft capability snapshot — public offsets only.
     * Prefer byte/word accessors matching the spec field widths;
     * also read dword0 once for a consistent soft view.
     */
    u32Dword0 = udx_readl(pSoft->pCap, XHCI_CAP_CAPLENGTH);
    u8Cap     = udx_readb(pSoft->pCap, XHCI_CAP_CAPLENGTH);
    u16Ver    = udx_readw(pSoft->pCap, XHCI_CAP_HCIVERSION);

    /* Cross-check: CAPLENGTH is low byte of dword0; version is high half. */
    if (u8Cap != (u8)(u32Dword0 & 0xffu)) {
        u8Cap = (u8)(u32Dword0 & 0xffu);
    }
    if (u16Ver != (u16)((u32Dword0 >> 16) & 0xffffu)) {
        u16Ver = (u16)((u32Dword0 >> 16) & 0xffffu);
    }

    pSoft->u8CapLen  = u8Cap;
    pSoft->u16HciVer = u16Ver;

    udx_printk("xhci_udx: soft cap ver=0x%04x caplen=0x%02x bar0=%llx\n",
               (unsigned)u16Ver, (unsigned)u8Cap,
               (unsigned long long)u64Bar);

    /*
     * Soft success gate for stage=1: non-zero CAPLENGTH and a plausible
     * Interface Version. Host demo seeds 0x20 / 0x0100; live silicon would
     * also be non-zero for a present controller. Zero/zero → soft FAIL.
     */
    if (u8Cap == 0u || u16Ver == 0u) {
        udx_printk("xhci_udx: soft cap empty (no seed / no HC)\n");
        udx_iounmap(pSoft->pCap);
        pSoft->pCap = NULL;
        udx_pci_release_regions(pPdev);
        udx_pci_disable(pPdev);
        return UDX_ERR_IO;
    }

    pSoft->u8BotStage = XHCI_SOFT_BOT_ST_CAP;
    xhci_udx_soft_bot_lamp(XHCI_SOFT_BOT_ST_CAP, "cap");

    /* Soft deepen: HCSPARAMS1 → PORTSC walk → BOT progress stub. */
    if (xhci_udx_soft_params(pSoft) != 0) {
        (void)xhci_udx_soft_ports(pSoft);
    }
    xhci_udx_soft_bot_stub(pSoft);

    pSoft->u32FProbeOk = 1;
    udx_set_drvdata(pPdev->pDev, pSoft);
    udx_printk("xhci_udx: soft probe PASS dev=%s id=%04x:%04x class=%06x "
               "bot_stage=%u soft≠product\n",
               udx_dev_name(pPdev->pDev),
               (unsigned)pPdev->u16Vendor,
               (unsigned)pPdev->u16Device,
               (unsigned)(pPdev->u32Class & 0x00ffffffu),
               (unsigned)pSoft->u8BotStage);
    return UDX_OK;
}

/* ------------------------------------------------------------------ */
/* quiesce — stop further soft activity before remove / crash path.    */
/* ------------------------------------------------------------------ */
static void
xhci_udx_quiesce(struct udx_pci_dev *pPdev)
{
    struct xhci_udx_soft *pSoft;

    if (pPdev == NULL || pPdev->pDev == NULL) {
        return;
    }
    pSoft = (struct xhci_udx_soft *)udx_get_drvdata(pPdev->pDev);
    if (pSoft == NULL) {
        return;
    }
    pSoft->u32FQuiesced = 1;
    /* Soft skeleton: no bus-master, no IRQ, no rings — flag only. */
    udx_printk("xhci_udx: soft quiesce bot_stage=%u\n",
               (unsigned)pSoft->u8BotStage);
}

/* ------------------------------------------------------------------ */
/* remove — reverse probe: iounmap → release regions → disable.        */
/* ------------------------------------------------------------------ */
static void
xhci_udx_remove(struct udx_pci_dev *pPdev)
{
    struct xhci_udx_soft *pSoft;

    if (pPdev == NULL || pPdev->pDev == NULL) {
        return;
    }
    pSoft = (struct xhci_udx_soft *)udx_get_drvdata(pPdev->pDev);
    if (pSoft != NULL) {
        if (pSoft->pCap != NULL) {
            udx_iounmap(pSoft->pCap);
            pSoft->pCap = NULL;
        }
        pSoft->u32FProbeOk = 0;
        pSoft->u32FBotStub = 0;
        udx_set_drvdata(pPdev->pDev, NULL);
    }
    udx_pci_release_regions(pPdev);
    udx_pci_disable(pPdev);
    udx_printk("xhci_udx: soft remove\n");
}

/* ID table: exact G752VT function + optional any-xHCI class row. */
static const struct udx_pci_device_id xhci_udx_ids[] = {
    UDX_PCI_DEVICE(XHCI_UDX_VEND, XHCI_UDX_DEV),
    UDX_PCI_DEVICE_CLASS(XHCI_UDX_CLASS, XHCI_UDX_CLASS_MASK),
    { 0, } /* sentinel */
};

static struct udx_pci_driver xhci_udx_driver = {
    .szName     = "xhci_udx",
    .pIdTable   = xhci_udx_ids,
    .pfnProbe   = xhci_udx_probe,
    .pfnRemove  = xhci_udx_remove,
    .pfnQuiesce = xhci_udx_quiesce,
};

#if defined(UDX_HOST_LIBC)

/*
 * Host soft demo main.
 *
 * Order matters: inject + seed CAP / HCSPARAMS1 / PORTSC *before* register
 * so probe sees non-zero cap/version/ports (register_driver rescans).
 */
int
main(int argc, char **argv)
{
    struct udx_pci_dev *pPdev;
    u64 aBarLen[6];
    u8  aBarMem[6];
    int iBar;
    udx_status_t st;
    int fPass;
    struct xhci_udx_soft *pSoft;
    u64 u64Port1Off;

    (void)argc;
    (void)argv;
    fPass = 0;
    pPdev = NULL;

    if (udx_init() != UDX_OK) {
        return 1;
    }

    for (iBar = 0; iBar < 6; iBar++) {
        aBarLen[iBar] = 0;
        aBarMem[iBar] = 1;
    }
    aBarLen[0] = XHCI_UDX_BAR0_LEN;

    /*
     * Inject Intel PCH xHCI soft function (class 0x0c0330) without a
     * driver bound yet — BAR host memory is zero-filled by UDX.
     */
    st = udx_host_inject_pci_ex(XHCI_UDX_VEND, XHCI_UDX_DEV,
                                0xffffu, 0xffffu,
                                XHCI_UDX_CLASS,
                                (u8)XHCI_UDX_BUS,
                                udx_pci_devfn((u8)XHCI_UDX_SLOT,
                                              (u8)XHCI_UDX_FUNC),
                                XHCI_UDX_IRQ,
                                aBarLen, aBarMem, &pPdev);
    if (st != UDX_OK || pPdev == NULL) {
        udx_printk("xhci_udx: inject FAIL %d\n", (int)st);
        udx_exit();
        return 1;
    }

    /* Seed public capability fields in soft BAR0 (host-only poke). */
    st = udx_host_bar_writel(pPdev, 0, XHCI_CAP_CAPLENGTH, XHCI_SOFT_CAP_DWORD0);
    if (st != UDX_OK) {
        udx_printk("xhci_udx: soft cap seed FAIL %d\n", (int)st);
        (void)udx_host_remove_pci(pPdev);
        udx_exit();
        return 1;
    }

    /* Soft deepen seed: HCSPARAMS1 MaxSlots/MaxIntrs/MaxPorts. */
    st = udx_host_bar_writel(pPdev, 0, XHCI_CAP_HCSPARAMS1, XHCI_SOFT_HCSPARAMS1);
    if (st != UDX_OK) {
        udx_printk("xhci_udx: soft params seed FAIL %d\n", (int)st);
        (void)udx_host_remove_pci(pPdev);
        udx_exit();
        return 1;
    }

    /*
     * Soft port seed: PORTSC[1] at op+0x400 with CCS|PED|PP|SS.
     * op base = CAPLENGTH (0x20) → offset 0x420.
     */
    u64Port1Off = (u64)XHCI_SOFT_CAPLENGTH + 0x400ull;
    st = udx_host_bar_writel(pPdev, 0, u64Port1Off, XHCI_SOFT_PORTSC_SEED);
    if (st != UDX_OK) {
        udx_printk("xhci_udx: soft port seed FAIL %d\n", (int)st);
        (void)udx_host_remove_pci(pPdev);
        udx_exit();
        return 1;
    }

    if (udx_pci_register_driver(&xhci_udx_driver) != UDX_OK) {
        udx_printk("xhci_udx: register FAIL\n");
        (void)udx_host_remove_pci(pPdev);
        udx_exit();
        return 1;
    }

    if (pPdev->pDev == NULL || !udx_dev_has_drvdata(pPdev->pDev)) {
        udx_printk("xhci_udx: soft probe FAIL (no drvdata)\n");
        udx_pci_unregister_driver(&xhci_udx_driver);
        (void)udx_host_remove_pci(pPdev);
        udx_exit();
        return 1;
    }

    pSoft = (struct xhci_udx_soft *)udx_get_drvdata(pPdev->pDev);
    if (pSoft != NULL &&
        pSoft->u32FProbeOk != 0u &&
        pSoft->u8CapLen == XHCI_SOFT_CAPLENGTH &&
        pSoft->u16HciVer == XHCI_SOFT_HCIVERSION &&
        pSoft->u8MaxPorts == XHCI_SOFT_MAX_PORTS &&
        pSoft->u8PortCcs >= 1u &&
        pSoft->u8BotStage == XHCI_SOFT_BOT_ST_STUB &&
        pSoft->u32FBotStub != 0u) {
        fPass = 1;
    }

    /* Teardown: unregister runs quiesce then remove. */
    udx_pci_unregister_driver(&xhci_udx_driver);
    (void)udx_host_remove_pci(pPdev);
    udx_exit();

    if (fPass) {
        udx_printk("xhci_udx: host soft demo done bot_stub=1 soft≠product\n");
        return 0;
    }
    udx_printk("xhci_udx: soft probe FAIL\n");
    return 1;
}

#else /* !UDX_HOST_LIBC */

/*
 * Freestanding registration + optional DDI bind for 8086:a12f.
 * When udx_host_bind_by_id is linked (GJ_SYS_DDI SCAN/OPEN/MAP_BAR path),
 * probe runs against the granted BAR0 and soft-lamps BOT stages from live
 * or zeroed MMIO (zero → params/ports soft-empty; cap still gates probe).
 * No product BOT claim. Does not touch kernel/drv/xhci_msc.c.
 */
int
xhci_udx_freestanding_register(void)
{
    udx_status_t st;

    if (udx_init() != UDX_OK) {
        return UDX_ERR_IO;
    }
    st = udx_pci_register_driver(&xhci_udx_driver);
    if (st != UDX_OK) {
        udx_printk("xhci_udx: soft SKIP (register %d)\n", (int)st);
        return st;
    }

    if (udx_host_bind_by_id != NULL) {
        st = udx_host_bind_by_id((u16)XHCI_UDX_VEND, (u16)XHCI_UDX_DEV, NULL);
        if (st != UDX_OK) {
            udx_printk("xhci_udx: soft SKIP (bind_by_id %d) — "
                       "inject 8086:a12f on host path soft≠product\n",
                       (int)st);
            return st;
        }
        udx_printk("xhci_udx: soft freestanding bind PASS soft≠product\n");
        return UDX_OK;
    }

    udx_printk("xhci_udx: soft SKIP (no udx_host_bind_by_id; "
               "host-inject 8086:a12f BAR0 for lab test)\n");
    return UDX_ERR_NOSYS;
}

#endif /* UDX_HOST_LIBC */
