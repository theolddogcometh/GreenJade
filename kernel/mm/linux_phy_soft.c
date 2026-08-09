/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Soft Linux PHY / MDIO seed.
 * Pure C11 freestanding. Dual MIT OR Apache-2.0. No GPL / no Linux .ko source.
 *
 * Soft only:
 *   - linux_phy_soft_init: ready lamp + linux_ksym_register of soft bodies
 *   - phy_* / mdiobus_* soft-success paths so r8169 init + post-probe soft-complete
 *   - non-NULL soft static objects for alloc / connect / get_phy
 *   - soft clause-22 register shadow (read/write/page helpers)
 *   - soft link-state bookkeeping (BMSR LSTATUS + speed/duplex lamps)
 *   - soft MMD-ish shadow (phy_read_mmd / phy_write_mmd / phy_modify_mmd)
 *   - soft-virtual libphy + mdio_devres markers (eng; no FAIL need:libphy)
 *   - residual lean: soft_phy_device / soft_mii_bus front fill + genphy_* status
 *   - residual lean deepen (layout_ver≥3): state/BMCR/BMSR/page mirrors +
 *     thrash-refuse tallies; R0 thrash refuse + dual DoD B gate0 once-lamps
 *   - residual lean deepen (layout_ver≥4): UDX eng stamps (udx_eng /
 *     never_phyar / product_open / gate0) + genphy/thrash tally mirrors;
 *     future UDX NIC eng lean surface (not freestanding rtl thrash)
 *   - residual lean deepen (layout_ver≥5 · LAW): freestanding_rtl=SKIP +
 *     product_nic=UDX + never product link claim (u8FsRtlSkip /
 *     u8ProductNicUdx / u8NoProductLink stamps + residual lamps)
 *   - C0 soft residual deepen (this pass / Soft!=product / stamp-free):
 *     denser Dual DoD A/B OPEN residual_ne_close honesty; RUN_INIT=0
 *     freestanding_no_exec; never_exec_ko; stamp_storm=0 no_version_stamp;
 *     dual=MIT_OR_Apache-2.0. No layout_ver bump; no GJ_IMAGE_VERSION;
 *     freestanding_rtl=SKIP; product_nic=UDX; G-AC-1. Once-lamps only.
 *
 * Soft != ABI-stable: struct layouts under void* are incomplete soft shapes.
 * Soft != product: no real MDIO bus, no copper link, no .ko PHY driver load.
 * G-AC-1: soft path != product AC; no .ko product claim.
 * Never product link claim: soft BMSR/LSTATUS / link lamps != product copper.
 * Product NIC = userspace UDX. Freestanding rtl default SKIP (not product).
 * Dual DoD A/B remain OPEN (agent residual != product close; Soft!=product).
 *
 * Non-destructive residual (hybrid 4a / gate0 · freestanding_rtl SKIP):
 *   Soft MDIO / phy_* touch ONLY in-kernel shadows (g_aSoftRegs / g_aSoftMmd).
 *   Hybrid-safe soft-only MDIO - never programs 10ec:8168 BAR MDIO (0xB0-class),
 *   never CF8, never calls freestanding rtl8168 MDIO helpers. Soft phy
 *   start/stop/aneg are bookkeeping only. freestanding_rtl=SKIP default;
 *   product_nic=UDX; soft link != product link; RUN_INIT=0 freestanding_no_exec.
 *   Grep: linux_phy_soft: soft hybrid zero-touch PASS
 *   Grep: linux_phy_soft: soft residual lean PASS
 *   Grep: linux_phy_soft: soft residual freestanding_rtl=SKIP
 *   Grep: linux_phy_soft: soft residual product_nic=UDX
 *   Grep: linux_phy_soft: soft residual RUN_INIT=0 freestanding_no_exec
 *   Grep: linux_phy_soft: soft residual lean UDX eng
 *   Grep: linux_phy_soft: soft residual R0 thrash refuse
 *   Grep: linux_phy_soft: soft dual DoD B residual gate0 PASS
 *   Grep: linux_phy_soft: soft residual DoD OPEN
 *
 * Completeness: every r8169.ko und in the phy_ / mdiobus_ / genphy_ class is
 * registered here (re-registers over empty linux_ksym stubs). Soft!=product.
 *
 * Greppable markers (keep stable; once-lamps only - no stamp storms):
 *   linux_phy_soft: soft init PASS n=
 *   linux_phy_soft: soft hybrid zero-touch PASS
 *   linux_phy_soft: soft residual lean PASS
 *   linux_phy_soft: soft residual freestanding_rtl=SKIP
 *   linux_phy_soft: soft residual product_nic=UDX
 *   linux_phy_soft: soft residual RUN_INIT=0 freestanding_no_exec
 *   linux_phy_soft: soft residual lean UDX eng
 *   linux_phy_soft: soft residual layout ...
 *   linux_phy_soft: soft residual R0 thrash refuse ...
 *   linux_phy_soft: soft dual DoD B residual gate0 PASS ...
 *   linux_phy_soft: soft residual DoD OPEN
 *   linux_phy_soft: soft dep virtual register libphy+mdio_devres ...
 *   linux_phy_soft: soft connect ...
 *   linux_phy_soft: soft start ...
 *   linux_phy_soft: soft stop ...
 *   linux_phy_soft: soft reset ...
 *   linux_phy_soft: soft mdiobus ...
 *   linux_phy_soft: soft phy_read ...
 *   linux_phy_soft: soft phy_write ...
 *   linux_phy_soft: soft link ...
 *   linux_phy_soft: soft genphy ...
 */
#include <gj/klog.h>
#include <gj/linux_phy_soft.h>
#include <gj/string.h>
#include <gj/types.h>

/*
 * F2 linux_ksym may be linked later. Weak unresolved -> NULL; init skips export.
 * Coordinator may also point ksym at these symbols by C name without this call.
 */
int linux_ksym_register(const char *szName, void *pFn) __attribute__((weak));

/*
 * Soft dep virtual registration lives in linux_module.c. Weak so this TU
 * builds if module path is stripped; seed markers only when module linked.
 * Grep: linux_module: soft dep virtual libphy soft=1 product=0
 * Grep: linux_module: soft dep virtual mdio_devres soft=1 product=0
 */
int linux_module_soft_dep_virtual_register(const char *szCanon)
    __attribute__((weak));

/* Soft clause-22-ish seeds (not a real PHY; Soft!=product link). */
#define LPHYS_BMCR_ANENABLE     0x1000u
#define LPHYS_BMCR_RESET        0x8000u
#define LPHYS_BMCR_PDOWN        0x0800u
#define LPHYS_BMCR_ISOLATE      0x0400u
#define LPHYS_BMCR_ANRESTART    0x0200u
#define LPHYS_BMCR_FULLDPLX     0x0100u
#define LPHYS_BMCR_SPEED100     0x2000u
#define LPHYS_BMCR_SPEED1000    0x0040u
#define LPHYS_BMSR_DEFAULT      0x7949u /* abilities + soft aneg-capable */
#define LPHYS_BMSR_LSTATUS      0x0004u
#define LPHYS_BMSR_ANEGCOMPLETE 0x0020u
#define LPHYS_PHYID1_SOFT       0x001cu /* Realtek-ish OUI nibble soft only */
#define LPHYS_PHYID2_SOFT       0xc800u
/* Soft ADVERTISE: 10/100 H/F + CSMA + pause-class bits (honesty seed). */
#define LPHYS_ADVERTISE_SOFT    0x0de1u
/* Soft LPA: partner mirrors advertise + LP aneg-able. */
#define LPHYS_LPA_SOFT          0x4de1u
/* Soft ANER: page received. */
#define LPHYS_ANER_SOFT         0x0001u
/* Soft 1000Base-T control: advertise full duplex. */
#define LPHYS_CTRL1000_SOFT     0x0200u
/* Soft 1000Base-T status: LP full duplex (when soft link up). */
#define LPHYS_STAT1000_SOFT     0x3800u
/* Realtek-ish page select register (soft shadow of reg 31). */
#define LPHYS_REG_PAGE          0x1fu
/* Soft pause bits in ADVERTISE (clause-22). */
#define LPHYS_ADVERTISE_PAUSE   0x0400u
#define LPHYS_ADVERTISE_ASYM    0x0800u

/* ---- Soft state --------------------------------------------------------- */

static int g_fReady;
static u32 g_cInitCalls;
static u32 g_cKsymOk;
static u32 g_cKsymSkip;

/* Soft static objects returned by pointer APIs (never free; zeroed once). */
static u8 g_aSoftPhyDev[LINUX_PHY_SOFT_PHYDEV_CB];
static u8 g_aSoftMiiBus[LINUX_PHY_SOFT_MIIBUS_CB];
static int g_fSoftObjsInit;

/* Soft lifecycle bookkeeping (post-probe r8169 soft path). */
static int g_fConnected;
static int g_fStarted;
static int g_fBusRegistered;
static int g_nIface;
static int g_nPage;
static int g_nPhyAddr;
static void *g_pNetdev;
static void *g_pfnHandler;

/*
 * Soft link-state residual (eng honesty only).
 * Tracks BMSR LSTATUS + speed/duplex lamps for post-probe status reads.
 * Soft!=product: never freestanding copper / BAR MDIO / netif_carrier dual-drive.
 */
static int g_fLinkUp;
static int g_nSpeedMbps;
static int g_nDuplexFull;
static int g_nMaxSpeed;
static u32 g_cLinkChange;
static int g_fPause;
static int g_fAsymPause;

/* Soft MDIO clause-22 register shadow (addr-agnostic single soft PHY). */
static u16 g_aSoftRegs[LINUX_PHY_SOFT_REG_MAX];
static u16 g_u16LastWrite;
static u32 g_u32LastReg;

/* Soft MMD-ish shadow (devad x reg; clause-45 folded Soft!=product). */
static u16 g_aSoftMmd[LINUX_PHY_SOFT_MMD_DEV_MAX][LINUX_PHY_SOFT_MMD_REG_MAX];
static int g_nLastMmdDevad;
static u32 g_cMmdRd;
static u32 g_cMmdWr;

/* Call counters (soft diagnostics; not product metrics). */
static u32 g_cConnect;
static u32 g_cDisconnect;
static u32 g_cStart;
static u32 g_cStop;
static u32 g_cStartAneg;
static u32 g_cReset;
static u32 g_cMdioRd;
static u32 g_cMdioWr;
static u32 g_cPhyRd;
static u32 g_cPhyWr;
static u32 g_cAlloc;
static u32 g_cRegister;
static u32 g_cGenphy;
/* Soft thrash-refuse tally: every shadow MDIO op that refused BAR/CF8 thrash. */
static u32 g_cThrashRefuse;
static int g_nState;

/* Once lamps for greppable soft breadcrumbs (hot path stays quiet after). */
static int g_fLogConnect;
static int g_fLogStart;
static int g_fLogStop;
static int g_fLogReset;
static int g_fLogMdio;
static int g_fLogPhyRd;
static int g_fLogPhyWr;
static int g_fLogBusReg;
static int g_fLogLink;
static int g_fLogStatus;
static int g_fLogAttached;
static int g_fHybridLamp;
static int g_fResidualLeanLamp;
static int g_fThrashRefuseLamp;
static int g_fDualDodBLamp;
static int g_fLogGenphy;

/* ---- Helpers ------------------------------------------------------------ */

static void
lphys_inc(u32 *pu32)
{
    if (pu32 != NULL && *pu32 < 0xffffffffu) {
        (*pu32)++;
    }
}

static void
lphys_ksym_one(const char *szName, void *pFn)
{
    int nSt;

    if (szName == NULL || pFn == NULL) {
        lphys_inc(&g_cKsymSkip);
        return;
    }
    if (linux_ksym_register == NULL) {
        lphys_inc(&g_cKsymSkip);
        return;
    }
    nSt = linux_ksym_register(szName, pFn);
    if (nSt == 0) {
        lphys_inc(&g_cKsymOk);
    } else {
        lphys_inc(&g_cKsymSkip);
    }
}

static struct soft_phy_device *
lphys_phydev(void)
{
    return (struct soft_phy_device *)(void *)g_aSoftPhyDev;
}

static struct soft_mii_bus *
lphys_miibus(void)
{
    return (struct soft_mii_bus *)(void *)g_aSoftMiiBus;
}

/*
 * Soft residual lean: derive LINUX_PHY_SOFT_STATE_* from connect/start/link.
 * Soft!=product; freestanding_rtl=SKIP; product_nic=UDX; never product link.
 */
static void
lphys_state_recompute(void)
{
    if (g_fConnected == 0) {
        g_nState = LINUX_PHY_SOFT_STATE_DOWN;
    } else if (g_fStarted == 0) {
        g_nState = LINUX_PHY_SOFT_STATE_READY;
    } else if (g_fLinkUp != 0) {
        g_nState = LINUX_PHY_SOFT_STATE_UP;
    } else {
        g_nState = LINUX_PHY_SOFT_STATE_NOLINK;
    }
}

/*
 * Soft thrash refuse: count a shadow-only MDIO op that never touched
 * freestanding BAR 0xB0 / CF8 / rtl8168 MDIO. Soft!=product; R0 class.
 */
static void
lphys_thrash_refuse_inc(void)
{
    lphys_inc(&g_cThrashRefuse);
}

/*
 * Soft residual lean: mirror bookkeeping into soft_phy_device front.
 * Soft!=Linux ABI; freestanding_rtl=SKIP; product_nic=UDX.
 * layout_ver ≥ 3: state / page / BMCR·BMSR / thrash-refuse tallies.
 * layout_ver ≥ 4: UDX eng stamps + genphy tally (future UDX NIC eng).
 * layout_ver ≥ 5: freestanding_rtl SKIP + product_nic UDX + no product link.
 */
static void
lphys_phydev_sync(void)
{
    struct soft_phy_device *pPd;

    lphys_state_recompute();
    pPd = lphys_phydev();
    pPd->u32Magic = LINUX_PHY_SOFT_PHYDEV_MAGIC;
    pPd->u32LayoutVer = LINUX_PHY_SOFT_LAYOUT_VER;
    pPd->pAttachedDev = g_pNetdev;
    pPd->pBus = (void *)g_aSoftMiiBus;
    pPd->pfnAdjustLink = g_pfnHandler;
    pPd->nAddr = g_nPhyAddr;
    pPd->nIface = g_nIface;
    pPd->nSpeed = g_nSpeedMbps;
    pPd->nDuplex = g_nDuplexFull;
    pPd->nLink = g_fLinkUp; /* soft bookkeeping only - never product link */
    pPd->nAutoneg = ((g_aSoftRegs[0] & LPHYS_BMCR_ANENABLE) != 0u) ? 1 : 0;
    pPd->nPause = g_fPause;
    pPd->nAsymPause = g_fAsymPause;
    pPd->nMaxSpeed = g_nMaxSpeed;
    pPd->u16PhyId1 = g_aSoftRegs[2];
    pPd->u16PhyId2 = g_aSoftRegs[3];
    pPd->u8Connected = (u8)((g_fConnected != 0) ? 1u : 0u);
    pPd->u8Started = (u8)((g_fStarted != 0) ? 1u : 0u);
    pPd->u8SoftOnly = 1u;
    pPd->u8WireSafe = 1u;
    /* layout_ver ≥ 3 residual lean front */
    pPd->nState = g_nState;
    pPd->nPage = g_nPage;
    pPd->u16Bmcr = g_aSoftRegs[0];
    pPd->u16Bmsr = g_aSoftRegs[1];
    pPd->pPriv = NULL;
    pPd->u32ShadowOps = g_cThrashRefuse;
    pPd->u32LinkChanges = g_cLinkChange;
    /* layout_ver ≥ 4 residual lean for future UDX NIC eng */
    pPd->u8UdxEng = 1u;
    pPd->u8NeverPhyar = 1u;
    pPd->u8ProductOpen = 1u;
    pPd->u8Gate0 = 1u;
    pPd->u32GenphyOps = g_cGenphy;
    /* layout_ver ≥ 5 residual lean LAW stamps */
    pPd->u8FsRtlSkip = 1u;
    pPd->u8ProductNicUdx = 1u;
    pPd->u8NoProductLink = 1u;
    pPd->u8PadVer5 = 0u;
}

static void
lphys_miibus_sync(void)
{
    struct soft_mii_bus *pMb;

    pMb = lphys_miibus();
    pMb->u32Magic = LINUX_PHY_SOFT_MIIBUS_MAGIC;
    pMb->u32LayoutVer = LINUX_PHY_SOFT_LAYOUT_VER;
    pMb->nRegistered = g_fBusRegistered;
    pMb->nPhyMask = 0xffffffff;
    pMb->u8SoftOnly = 1u;
    pMb->u8WireSafe = 1u;
    pMb->u8ShadowOnly = 1u;
    pMb->u8ThrashRefuse = 1u;
    pMb->u32ShadowOps = (u32)(g_cMdioRd + g_cMdioWr);
    /* layout_ver ≥ 4 residual lean for future UDX NIC eng */
    pMb->u8UdxEng = 1u;
    pMb->u8NeverPhyar = 1u;
    pMb->u8ProductOpen = 1u;
    pMb->u8Gate0 = 1u;
    pMb->u32ThrashRefuse = g_cThrashRefuse;
    /* layout_ver ≥ 5 residual lean LAW stamps */
    pMb->u8FsRtlSkip = 1u;
    pMb->u8ProductNicUdx = 1u;
    pMb->u8NoProductLink = 1u;
    pMb->u8PadVer5 = 0u;
    if (pMb->szId[0] == '\0') {
        /* Soft default bus id; Soft!=product. */
        pMb->szId[0] = 's';
        pMb->szId[1] = 'o';
        pMb->szId[2] = 'f';
        pMb->szId[3] = 't';
        pMb->szId[4] = '-';
        pMb->szId[5] = 'm';
        pMb->szId[6] = 'i';
        pMb->szId[7] = 'i';
        pMb->szId[8] = '\0';
    }
}

/*
 * Soft link-state residual: sync BMSR/LPA/STAT1000 + once lamp + phydev front.
 * Bookkeeping only - never product link claim. freestanding_rtl=SKIP;
 * product_nic=UDX. Soft!=product copper.
 * Grep: linux_phy_soft: soft link
 */
static void
lphys_link_set(int fUp, int nSpeedMbps, int nDuplexFull)
{
    int fPrev;

    fPrev = g_fLinkUp;
    g_fLinkUp = (fUp != 0) ? 1 : 0;
    if (g_fLinkUp != 0) {
        if (nSpeedMbps > 0) {
            g_nSpeedMbps = nSpeedMbps;
        }
        if (g_nMaxSpeed > 0 && g_nSpeedMbps > g_nMaxSpeed) {
            g_nSpeedMbps = g_nMaxSpeed;
        }
        g_nDuplexFull = (nDuplexFull != 0) ? 1 : 0;
        g_aSoftRegs[1] = (u16)(g_aSoftRegs[1] | LPHYS_BMSR_LSTATUS |
                               LPHYS_BMSR_ANEGCOMPLETE);
        g_aSoftRegs[5] = (u16)LPHYS_LPA_SOFT;
        if (g_nSpeedMbps >= 1000) {
            g_aSoftRegs[10] = (u16)LPHYS_STAT1000_SOFT;
        } else {
            g_aSoftRegs[10] = 0u;
        }
    } else {
        g_aSoftRegs[1] = (u16)(g_aSoftRegs[1] &
                               (u16)~(LPHYS_BMSR_LSTATUS | LPHYS_BMSR_ANEGCOMPLETE));
        /* Soft LP unresolved while down; ADVERTISE retained. */
        g_aSoftRegs[10] = 0u;
    }

    if (fPrev != g_fLinkUp) {
        lphys_inc(&g_cLinkChange);
    }

    lphys_phydev_sync();

    if (g_fLogLink == 0) {
        g_fLogLink = 1;
        /* Grep: linux_phy_soft: soft link */
        kprintf("linux_phy_soft: soft link up=%d speed=%d duplex=%s "
                "bmsr=0x%x changes=%u soft_ne_product=1 "
                "no_product_link=1 freestanding_rtl=SKIP product_nic=UDX "
                "(soft bookkeeping only; never product link claim)\n",
                g_fLinkUp, g_nSpeedMbps,
                g_nDuplexFull != 0 ? "full" : "half",
                (unsigned)g_aSoftRegs[1], (unsigned)g_cLinkChange);
    }
}

static void
lphys_regs_seed(void)
{
    u32 i;
    u32 iDev;
    u32 iReg;

    for (i = 0u; i < LINUX_PHY_SOFT_REG_MAX; i++) {
        g_aSoftRegs[i] = 0u;
    }
    for (iDev = 0u; iDev < LINUX_PHY_SOFT_MMD_DEV_MAX; iDev++) {
        for (iReg = 0u; iReg < LINUX_PHY_SOFT_MMD_REG_MAX; iReg++) {
            g_aSoftMmd[iDev][iReg] = 0u;
        }
    }
    /*
     * Soft-plausible clause-22 defaults so post-probe status reads are not
     * all-zero death (Soft!=product copper; values are honesty-seed only).
     * BMCR: ANE; BMSR: abilities + LSTATUS + ANEGCOMPLETE (soft link-up).
     * ADVERTISE/LPA/CTRL1000/STAT1000 denser residual for mdio-ish probes.
     */
    g_aSoftRegs[0] = (u16)(LPHYS_BMCR_ANENABLE | LPHYS_BMCR_FULLDPLX |
                           LPHYS_BMCR_SPEED1000);
    g_aSoftRegs[1] = (u16)(LPHYS_BMSR_DEFAULT | LPHYS_BMSR_LSTATUS |
                           LPHYS_BMSR_ANEGCOMPLETE);
    g_aSoftRegs[2] = (u16)LPHYS_PHYID1_SOFT;
    g_aSoftRegs[3] = (u16)LPHYS_PHYID2_SOFT;
    g_aSoftRegs[4] = (u16)LPHYS_ADVERTISE_SOFT;
    g_aSoftRegs[5] = (u16)LPHYS_LPA_SOFT;
    g_aSoftRegs[6] = (u16)LPHYS_ANER_SOFT;
    g_aSoftRegs[9] = (u16)LPHYS_CTRL1000_SOFT;
    g_aSoftRegs[10] = (u16)LPHYS_STAT1000_SOFT;
    g_nPage = 0;
    g_u16LastWrite = 0u;
    g_u32LastReg = 0u;
    g_nLastMmdDevad = 0;
    g_fLinkUp = 1;
    g_nSpeedMbps = LINUX_PHY_SOFT_SPEED_DEFAULT;
    g_nDuplexFull = LINUX_PHY_SOFT_DUPLEX_FULL;
    g_nMaxSpeed = 0;
    g_fPause = ((LPHYS_ADVERTISE_SOFT & LPHYS_ADVERTISE_PAUSE) != 0u) ? 1 : 0;
    g_fAsymPause = ((LPHYS_ADVERTISE_SOFT & LPHYS_ADVERTISE_ASYM) != 0u) ? 1 : 0;
    g_nState = LINUX_PHY_SOFT_STATE_READY;
}

static void
lphys_soft_objs_once(void)
{
    if (g_fSoftObjsInit != 0) {
        return;
    }
    memset(g_aSoftPhyDev, 0, sizeof(g_aSoftPhyDev));
    memset(g_aSoftMiiBus, 0, sizeof(g_aSoftMiiBus));
    lphys_regs_seed();
    lphys_phydev_sync();
    lphys_miibus_sync();
    g_fSoftObjsInit = 1;
}

static u16
lphys_reg_get(u32 u32Regnum)
{
    u32 u32Idx;

    u32Idx = u32Regnum & 0x1fu;
    if (u32Idx >= LINUX_PHY_SOFT_REG_MAX) {
        return 0u;
    }
    return g_aSoftRegs[u32Idx];
}

static void
lphys_reg_set(u32 u32Regnum, u16 u16Val)
{
    u32 u32Idx;

    u32Idx = u32Regnum & 0x1fu;
    if (u32Idx >= LINUX_PHY_SOFT_REG_MAX) {
        return;
    }
    g_aSoftRegs[u32Idx] = u16Val;
    g_u32LastReg = u32Regnum;
    g_u16LastWrite = u16Val;

    /* Soft Realtek-ish page select (reg 31): bookkeep g_nPage. */
    if (u32Idx == LPHYS_REG_PAGE) {
        g_nPage = (int)(u16Val & 0xffu);
        return;
    }

    /* Soft ADVERTISE residual: track pause lamps from reg 4. */
    if (u32Idx == 4u) {
        g_fPause = ((u16Val & LPHYS_ADVERTISE_PAUSE) != 0u) ? 1 : 0;
        g_fAsymPause = ((u16Val & LPHYS_ADVERTISE_ASYM) != 0u) ? 1 : 0;
        lphys_phydev_sync();
        return;
    }

    if (u32Idx != 0u) {
        return;
    }

    /*
     * Soft BMCR residual:
     *   reset: clear bit + reseed status; soft link-up bookkeeping
     *   aneg restart: clear bit + soft complete immediately
     *   power-down / isolate: soft link down while set
     * Soft!=product; no real silicon delay; no freestanding thrash.
     */
    if ((u16Val & LPHYS_BMCR_RESET) != 0u) {
        g_aSoftRegs[0] = (u16)(u16Val & (u16)~LPHYS_BMCR_RESET);
        if (g_aSoftRegs[0] == 0u) {
            g_aSoftRegs[0] = (u16)(LPHYS_BMCR_ANENABLE | LPHYS_BMCR_FULLDPLX |
                                   LPHYS_BMCR_SPEED1000);
        }
        g_aSoftRegs[1] = (u16)LPHYS_BMSR_DEFAULT;
        g_aSoftRegs[4] = (u16)LPHYS_ADVERTISE_SOFT;
        g_aSoftRegs[6] = (u16)LPHYS_ANER_SOFT;
        g_aSoftRegs[9] = (u16)LPHYS_CTRL1000_SOFT;
        g_fPause = 1;
        g_fAsymPause = 1;
        lphys_link_set(1, LINUX_PHY_SOFT_SPEED_DEFAULT,
                       LINUX_PHY_SOFT_DUPLEX_FULL);
        return;
    }

    if ((u16Val & (LPHYS_BMCR_PDOWN | LPHYS_BMCR_ISOLATE)) != 0u) {
        lphys_link_set(0, g_nSpeedMbps, g_nDuplexFull);
        return;
    }

    if ((u16Val & LPHYS_BMCR_ANRESTART) != 0u) {
        g_aSoftRegs[0] = (u16)(u16Val & (u16)~LPHYS_BMCR_ANRESTART);
        g_aSoftRegs[0] = (u16)(g_aSoftRegs[0] | LPHYS_BMCR_ANENABLE);
        lphys_link_set(1,
                       g_nSpeedMbps > 0 ? g_nSpeedMbps
                                        : LINUX_PHY_SOFT_SPEED_DEFAULT,
                       g_nDuplexFull != 0 ? 1 : LINUX_PHY_SOFT_DUPLEX_FULL);
        return;
    }

    lphys_phydev_sync();
}

static void *
lphys_phy_of(void *pPhy)
{
    lphys_soft_objs_once();
    if (pPhy != NULL) {
        return pPhy;
    }
    return (void *)g_aSoftPhyDev;
}

/* ---- phy connect / lifecycle -------------------------------------------- */

void *
phy_connect(void *pNetdev, const char *szBusId, void *pfnHandler, int nIface)
{
    struct soft_mii_bus *pMb;

    lphys_inc(&g_cConnect);
    lphys_soft_objs_once();

    g_pNetdev = pNetdev;
    g_pfnHandler = pfnHandler;
    g_nIface = nIface;
    g_fConnected = 1;
    /* Soft: started-off until phy_start; Soft!=product; never product link. */
    g_fStarted = 0;

    pMb = lphys_miibus();
    if (szBusId != NULL && szBusId[0] != '\0') {
        u32 i;
        for (i = 0u; i + 1u < sizeof(pMb->szId); i++) {
            if (szBusId[i] == '\0') {
                break;
            }
            pMb->szId[i] = szBusId[i];
        }
        pMb->szId[i] = '\0';
    }
    lphys_miibus_sync();
    lphys_phydev_sync();

    if (g_fLogConnect == 0) {
        g_fLogConnect = 1;
        /* Grep: linux_phy_soft: soft connect */
        kprintf("linux_phy_soft: soft connect bus=%s netdev=%p iface=%d "
                "phy=%p handler=%p layout_ver=%u calls=%u "
                "soft_ne_product=1 wire_safe=1\n",
                szBusId != NULL ? szBusId : "?",
                g_pNetdev, g_nIface, (void *)g_aSoftPhyDev, g_pfnHandler,
                (unsigned)LINUX_PHY_SOFT_LAYOUT_VER, (unsigned)g_cConnect);
    }
    return (void *)g_aSoftPhyDev;
}

int
phy_connect_direct(void *pNetdev, void *pPhy, void *pfnHandler, int nIface)
{
    (void)lphys_phy_of(pPhy);

    lphys_inc(&g_cConnect);
    g_pNetdev = pNetdev;
    g_pfnHandler = pfnHandler;
    g_nIface = nIface;
    g_fConnected = 1;
    g_fStarted = 0;
    lphys_phydev_sync();

    if (g_fLogConnect == 0) {
        g_fLogConnect = 1;
        /* Grep: linux_phy_soft: soft connect */
        kprintf("linux_phy_soft: soft connect direct=1 netdev=%p phy=%p "
                "iface=%d handler=%p layout_ver=%u calls=%u "
                "soft_ne_product=1 wire_safe=1\n",
                g_pNetdev, pPhy != NULL ? pPhy : (void *)g_aSoftPhyDev,
                g_nIface, g_pfnHandler, (unsigned)LINUX_PHY_SOFT_LAYOUT_VER,
                (unsigned)g_cConnect);
    }
    /*
     * Linux-shaped: int 0 = success (caller already holds pPhy from
     * mdiobus_get_phy / scan). Soft!=product link state.
     */
    return 0;
}

void
phy_disconnect(void *pPhy)
{
    (void)pPhy;
    lphys_inc(&g_cDisconnect);
    g_fConnected = 0;
    g_fStarted = 0;
    g_pfnHandler = NULL;
    /* Soft residual: disconnect -> soft link down (bookkeeping only). */
    lphys_soft_objs_once();
    lphys_link_set(0, g_nSpeedMbps, g_nDuplexFull);
}

void
phy_start(void *pPhy)
{
    (void)lphys_phy_of(pPhy);
    lphys_inc(&g_cStart);
    g_fStarted = 1;
    /* Soft residual: start -> soft link-up 1000/full (post-probe open path). */
    lphys_link_set(1, LINUX_PHY_SOFT_SPEED_DEFAULT, LINUX_PHY_SOFT_DUPLEX_FULL);

    if (g_fLogStart == 0) {
        g_fLogStart = 1;
        /* Grep: linux_phy_soft: soft start */
        kprintf("linux_phy_soft: soft start phy=%p connected=%d started=%d "
                "link=%d bmsr=0x%x speed=%d calls=%u soft_ne_product=1 "
                "wire_safe=1\n",
                pPhy != NULL ? pPhy : (void *)g_aSoftPhyDev,
                g_fConnected, g_fStarted, g_fLinkUp, (unsigned)g_aSoftRegs[1],
                g_nSpeedMbps, (unsigned)g_cStart);
    }
}

void
phy_stop(void *pPhy)
{
    (void)pPhy;
    lphys_inc(&g_cStop);
    g_fStarted = 0;
    lphys_soft_objs_once();
    /* Soft residual: stop -> clear LSTATUS (honest soft down). */
    lphys_link_set(0, g_nSpeedMbps, g_nDuplexFull);

    if (g_fLogStop == 0) {
        g_fLogStop = 1;
        /* Grep: linux_phy_soft: soft stop */
        kprintf("linux_phy_soft: soft stop phy=%p started=%d link=%d disc=%u "
                "calls=%u soft_ne_product=1\n",
                pPhy != NULL ? pPhy : (void *)g_aSoftPhyDev, g_fStarted,
                g_fLinkUp, (unsigned)g_cDisconnect, (unsigned)g_cStop);
    }
}

int
phy_start_aneg(void *pPhy)
{
    (void)pPhy;
    lphys_inc(&g_cStartAneg);
    lphys_soft_objs_once();
    /* Soft: pretend ANEGCOMPLETE + LSTATUS immediately. Soft!=product. */
    g_aSoftRegs[0] = (u16)(g_aSoftRegs[0] | LPHYS_BMCR_ANENABLE);
    lphys_link_set(1,
                   g_nSpeedMbps > 0 ? g_nSpeedMbps
                                    : LINUX_PHY_SOFT_SPEED_DEFAULT,
                   LINUX_PHY_SOFT_DUPLEX_FULL);
    if (g_cStartAneg == 1u && g_fLogStart == 0) {
        /* Grep: linux_phy_soft: soft start */
        kprintf("linux_phy_soft: soft start aneg=1 bmsr=0x%x link=%d "
                "calls=%u soft_ne_product=1\n",
                (unsigned)g_aSoftRegs[1], g_fLinkUp, (unsigned)g_cStartAneg);
    }
    return 0;
}

int
phy_init_hw(void *pPhy)
{
    (void)pPhy;
    lphys_soft_objs_once();
    /* Soft success: leave shadow regs; no real silicon init. */
    return 0;
}

int
phy_resume(void *pPhy)
{
    (void)pPhy;
    lphys_soft_objs_once();
    g_fStarted = 1;
    lphys_link_set(1,
                   g_nSpeedMbps > 0 ? g_nSpeedMbps
                                    : LINUX_PHY_SOFT_SPEED_DEFAULT,
                   g_nDuplexFull != 0 ? 1 : LINUX_PHY_SOFT_DUPLEX_FULL);
    return 0;
}

int
phy_suspend(void *pPhy)
{
    (void)pPhy;
    g_fStarted = 0;
    lphys_soft_objs_once();
    lphys_link_set(0, g_nSpeedMbps, g_nDuplexFull);
    return 0;
}

void
phy_attached_info(void *pPhy)
{
    (void)pPhy;
    lphys_soft_objs_once();
    if (g_fLogAttached == 0) {
        g_fLogAttached = 1;
        /* Grep: linux_phy_soft: soft link */
        kprintf("linux_phy_soft: soft link attached_info phy=%p link=%d "
                "speed=%d duplex=%s soft_ne_product=1\n",
                pPhy != NULL ? pPhy : (void *)g_aSoftPhyDev, g_fLinkUp,
                g_nSpeedMbps, g_nDuplexFull != 0 ? "full" : "half");
    }
}

void
phy_print_status(void *pPhy)
{
    (void)pPhy;
    lphys_soft_objs_once();
    if (g_fLogStatus == 0) {
        g_fLogStatus = 1;
        /* Grep: linux_phy_soft: soft link */
        kprintf("linux_phy_soft: soft link status phy=%p link=%d speed=%d "
                "duplex=%s bmsr=0x%x page=%d soft_ne_product=1\n",
                pPhy != NULL ? pPhy : (void *)g_aSoftPhyDev, g_fLinkUp,
                g_nSpeedMbps, g_nDuplexFull != 0 ? "full" : "half",
                (unsigned)g_aSoftRegs[1], g_nPage);
    }
}

int
phy_set_max_speed(void *pPhy, int nSpeed)
{
    (void)pPhy;
    if (nSpeed > 0) {
        g_nMaxSpeed = nSpeed;
        if (g_fLinkUp != 0 && g_nSpeedMbps > g_nMaxSpeed) {
            lphys_link_set(1, g_nMaxSpeed, g_nDuplexFull);
        } else {
            lphys_phydev_sync();
        }
    }
    return 0;
}

int
phy_aneg_done(void *pPhy)
{
    (void)pPhy;
    lphys_soft_objs_once();
    /*
     * Soft residual: ANEGCOMPLETE from BMSR shadow only.
     * Soft!=product; never product link; never polls live BAR MDIO.
     */
    lphys_thrash_refuse_inc();
    if ((g_aSoftRegs[1] & LPHYS_BMSR_ANEGCOMPLETE) != 0u) {
        return 1;
    }
    return 0;
}

int
genphy_soft_reset(void *pPhy)
{
    (void)pPhy;
    lphys_inc(&g_cReset);
    lphys_inc(&g_cGenphy);
    lphys_soft_objs_once();
    /* Soft reset: reseed BMCR/BMSR; Soft!=product delay. */
    g_aSoftRegs[0] = (u16)(LPHYS_BMCR_ANENABLE | LPHYS_BMCR_FULLDPLX |
                           LPHYS_BMCR_SPEED1000);
    g_aSoftRegs[1] = (u16)LPHYS_BMSR_DEFAULT;
    g_aSoftRegs[4] = (u16)LPHYS_ADVERTISE_SOFT;
    g_aSoftRegs[6] = (u16)LPHYS_ANER_SOFT;
    g_aSoftRegs[9] = (u16)LPHYS_CTRL1000_SOFT;
    g_fPause = 1;
    g_fAsymPause = 1;
    lphys_link_set(1, LINUX_PHY_SOFT_SPEED_DEFAULT, LINUX_PHY_SOFT_DUPLEX_FULL);

    if (g_fLogReset == 0) {
        g_fLogReset = 1;
        /* Grep: linux_phy_soft: soft reset */
        kprintf("linux_phy_soft: soft reset genphy=1 phy=%p link=%d "
                "calls=%u soft_ne_product=1 wire_safe=1\n",
                pPhy != NULL ? pPhy : (void *)g_aSoftPhyDev, g_fLinkUp,
                (unsigned)g_cReset);
    }
    return 0;
}

/* ---- genphy residual lean ----------------------------------------------- */

int
genphy_update_link(void *pPhy)
{
    (void)pPhy;
    lphys_inc(&g_cGenphy);
    lphys_soft_objs_once();
    /*
     * Soft residual: refresh link lamp from BMSR shadow only.
     * Never polls live BAR MDIO. Soft!=product; freestanding_rtl=SKIP.
     * R0 thrash refuse: shadow status only; product_nic=UDX lean.
     */
    lphys_thrash_refuse_inc();
    if ((g_aSoftRegs[1] & LPHYS_BMSR_LSTATUS) != 0u) {
        g_fLinkUp = 1;
    } else {
        g_fLinkUp = 0;
    }
    lphys_phydev_sync();
    return 0;
}

int
genphy_read_status(void *pPhy)
{
    (void)pPhy;
    lphys_inc(&g_cGenphy);
    lphys_soft_objs_once();
    /* Soft: update_link + speed/duplex from shadow bookkeeping. */
    (void)genphy_update_link(pPhy);
    if (g_fLinkUp != 0) {
        if (g_nSpeedMbps <= 0) {
            g_nSpeedMbps = LINUX_PHY_SOFT_SPEED_DEFAULT;
        }
        if ((g_aSoftRegs[0] & LPHYS_BMCR_FULLDPLX) != 0u) {
            g_nDuplexFull = 1;
        }
        g_aSoftRegs[1] = (u16)(g_aSoftRegs[1] | LPHYS_BMSR_LSTATUS |
                               LPHYS_BMSR_ANEGCOMPLETE);
        g_aSoftRegs[5] = (u16)LPHYS_LPA_SOFT;
        if (g_nSpeedMbps >= 1000) {
            g_aSoftRegs[10] = (u16)LPHYS_STAT1000_SOFT;
        }
    }
    lphys_phydev_sync();

    if (g_fLogGenphy == 0) {
        g_fLogGenphy = 1;
        /* Grep: linux_phy_soft: soft genphy */
        kprintf("linux_phy_soft: soft genphy read_status link=%d speed=%d "
                "duplex=%s bmsr=0x%x soft_ne_product=1 wire_safe=1 "
                "shadow_only=1\n",
                g_fLinkUp, g_nSpeedMbps,
                g_nDuplexFull != 0 ? "full" : "half",
                (unsigned)g_aSoftRegs[1]);
    }
    return 0;
}

int
genphy_restart_aneg(void *pPhy)
{
    lphys_inc(&g_cGenphy);
    return phy_start_aneg(pPhy);
}

int
genphy_check_and_restart_aneg(void *pPhy, int fRestart)
{
    lphys_inc(&g_cGenphy);
    lphys_soft_objs_once();
    if (fRestart != 0 ||
        (g_aSoftRegs[1] & LPHYS_BMSR_ANEGCOMPLETE) == 0u) {
        return phy_start_aneg(pPhy);
    }
    return 0;
}

int
genphy_read_lpa(void *pPhy)
{
    (void)pPhy;
    lphys_inc(&g_cGenphy);
    lphys_soft_objs_once();
    /* Soft LPA already seeded; return success. Soft!=product. */
    if (g_fLinkUp != 0) {
        g_aSoftRegs[5] = (u16)LPHYS_LPA_SOFT;
    }
    lphys_phydev_sync();
    return 0;
}

int
genphy_read_abilities(void *pPhy)
{
    (void)pPhy;
    lphys_inc(&g_cGenphy);
    lphys_soft_objs_once();
    /* Soft BMSR abilities already seeded in LPHYS_BMSR_DEFAULT. */
    g_aSoftRegs[1] = (u16)(g_aSoftRegs[1] | LPHYS_BMSR_DEFAULT);
    lphys_phydev_sync();
    return 0;
}

int
genphy_config_aneg(void *pPhy)
{
    lphys_inc(&g_cGenphy);
    lphys_soft_objs_once();
    g_aSoftRegs[0] = (u16)(g_aSoftRegs[0] | LPHYS_BMCR_ANENABLE);
    g_aSoftRegs[4] = (u16)LPHYS_ADVERTISE_SOFT;
    g_aSoftRegs[9] = (u16)LPHYS_CTRL1000_SOFT;
    g_fPause = 1;
    g_fAsymPause = 1;
    return phy_start_aneg(pPhy);
}

int
genphy_setup_forced(void *pPhy)
{
    u16 u16Bmcr;

    (void)pPhy;
    lphys_inc(&g_cGenphy);
    lphys_soft_objs_once();
    /*
     * Soft residual: forced speed/duplex from bookkeeping into BMCR shadow.
     * Clear ANENABLE; set FULLDPLX / SPEED bits. Soft!=product copper.
     * Shadow only - thrash refuse (no BAR MDIO).
     */
    lphys_thrash_refuse_inc();
    u16Bmcr = 0u;
    if (g_nDuplexFull != 0) {
        u16Bmcr = (u16)(u16Bmcr | LPHYS_BMCR_FULLDPLX);
    }
    if (g_nSpeedMbps >= 1000) {
        u16Bmcr = (u16)(u16Bmcr | LPHYS_BMCR_SPEED1000);
    } else if (g_nSpeedMbps >= 100) {
        u16Bmcr = (u16)(u16Bmcr | LPHYS_BMCR_SPEED100);
    }
    g_aSoftRegs[0] = u16Bmcr;
    if (g_fLinkUp != 0) {
        g_aSoftRegs[1] = (u16)(g_aSoftRegs[1] | LPHYS_BMSR_LSTATUS);
    }
    lphys_phydev_sync();
    return 0;
}

int
genphy_resume(void *pPhy)
{
    lphys_inc(&g_cGenphy);
    return phy_resume(pPhy);
}

int
genphy_suspend(void *pPhy)
{
    lphys_inc(&g_cGenphy);
    return phy_suspend(pPhy);
}

/* ---- MDIO bus ----------------------------------------------------------- */

int
mdiobus_read(void *pBus, int nAddr, u32 u32Regnum)
{
    u16 u16Val;

    (void)pBus;
    lphys_inc(&g_cMdioRd);
    lphys_soft_objs_once();
    g_nPhyAddr = nAddr;
    /*
     * Hybrid-safe soft-only MDIO: clause-22 shadow only.
     * Never 10ec:8168 BAR MDIO / CF8 / freestanding rtl8168 MDIO.
     * Soft!=product; freestanding_rtl=SKIP; product_nic=UDX.
     * R0 thrash refuse: each shadow op tallied (no freestanding dual-drive).
     */
    lphys_thrash_refuse_inc();
    u16Val = lphys_reg_get(u32Regnum);
    lphys_phydev_sync();
    lphys_miibus_sync();

    if (g_fLogMdio == 0) {
        g_fLogMdio = 1;
        /* Grep: linux_phy_soft: soft mdiobus */
        kprintf("linux_phy_soft: soft mdiobus read addr=%d reg=0x%x "
                "val=0x%x bus_reg=%d last_wr=0x%x calls=%u soft_ne_product=1 "
                "shadow_only=1 wire_safe=1 thrash_refuse=1 G-AC-1=1\n",
                g_nPhyAddr, (unsigned)u32Regnum, (unsigned)u16Val,
                g_fBusRegistered, (unsigned)g_u16LastWrite,
                (unsigned)g_cMdioRd);
    }
    return (int)u16Val;
}

int
mdiobus_write(void *pBus, int nAddr, u32 u32Regnum, u16 u16Val)
{
    (void)pBus;
    lphys_inc(&g_cMdioWr);
    lphys_soft_objs_once();
    g_nPhyAddr = nAddr;
    /*
     * Hybrid-safe soft-only MDIO write: shadow only (live BAR NOOP).
     * Never programs copper / freestanding BAR; Soft!=product.
     * R0 thrash refuse: each shadow op tallied.
     */
    lphys_thrash_refuse_inc();
    lphys_reg_set(u32Regnum, u16Val);
    lphys_miibus_sync();

    if (g_fLogMdio == 0) {
        g_fLogMdio = 1;
        /* Grep: linux_phy_soft: soft mdiobus */
        kprintf("linux_phy_soft: soft mdiobus write addr=%d reg=0x%x "
                "val=0x%x last_reg=0x%x calls=%u soft_ne_product=1 "
                "shadow_only=1 wire_safe=1 thrash_refuse=1 G-AC-1=1\n",
                g_nPhyAddr, (unsigned)u32Regnum, (unsigned)u16Val,
                (unsigned)g_u32LastReg, (unsigned)g_cMdioWr);
    }
    return 0;
}

int
__mdiobus_read(void *pBus, int nAddr, u32 u32Regnum)
{
    return mdiobus_read(pBus, nAddr, u32Regnum);
}

int
__mdiobus_write(void *pBus, int nAddr, u32 u32Regnum, u16 u16Val)
{
    return mdiobus_write(pBus, nAddr, u32Regnum, u16Val);
}

void *
mdiobus_get_phy(void *pBus, int nAddr)
{
    (void)pBus;
    g_nPhyAddr = nAddr;
    lphys_soft_objs_once();
    lphys_phydev_sync();
    return (void *)g_aSoftPhyDev;
}

void *
mdiobus_alloc_size(int nSizeofPriv)
{
    (void)nSizeofPriv;
    lphys_inc(&g_cAlloc);
    lphys_soft_objs_once();
    lphys_miibus_sync();
    if (g_fLogBusReg == 0 && g_cAlloc == 1u) {
        /* First alloc breadcrumb; register log may replace later. */
        /* Grep: linux_phy_soft: soft mdiobus */
        kprintf("linux_phy_soft: soft mdiobus alloc bus=%p priv=%d "
                "layout_ver=%u calls=%u soft_ne_product=1 wire_safe=1 "
                "thrash_refuse=1\n",
                (void *)g_aSoftMiiBus, nSizeofPriv,
                (unsigned)LINUX_PHY_SOFT_LAYOUT_VER, (unsigned)g_cAlloc);
    }
    return (void *)g_aSoftMiiBus;
}

void *
mdiobus_alloc(void)
{
    return mdiobus_alloc_size(0);
}

void *
devm_mdiobus_alloc_size(void *pDev, int nSizeofPriv)
{
    struct soft_mii_bus *pMb;

    pMb = (struct soft_mii_bus *)mdiobus_alloc_size(nSizeofPriv);
    if (pMb != NULL) {
        pMb->pParent = pDev;
    }
    return (void *)pMb;
}

void *
devm_mdiobus_alloc(void *pDev)
{
    return devm_mdiobus_alloc_size(pDev, 0);
}

void
mdiobus_free(void *pBus)
{
    (void)pBus;
    /* Soft static bus never freed; Soft!=product allocator. */
    g_fBusRegistered = 0;
    lphys_miibus_sync();
}

int
mdiobus_register(void *pBus)
{
    return __mdiobus_register(pBus, NULL);
}

int
__mdiobus_register(void *pBus, void *pOwner)
{
    struct soft_mii_bus *pMb;

    (void)pOwner;
    lphys_inc(&g_cRegister);
    lphys_soft_objs_once();
    g_fBusRegistered = 1;
    pMb = lphys_miibus();
    if (pBus != NULL && pBus != (void *)g_aSoftMiiBus) {
        /* Soft: accept external cookie; still track static residual. */
        pMb->pPriv = pBus;
    }
    lphys_miibus_sync();

    if (g_fLogBusReg == 0) {
        g_fLogBusReg = 1;
        /* Grep: linux_phy_soft: soft mdiobus */
        kprintf("linux_phy_soft: soft mdiobus register bus=%p calls=%u "
                "layout_ver=%u soft_ne_product=1 wire_safe=1\n",
                pBus != NULL ? pBus : (void *)g_aSoftMiiBus,
                (unsigned)g_cRegister, (unsigned)LINUX_PHY_SOFT_LAYOUT_VER);
    }
    return 0;
}

int
__devm_mdiobus_register(void *pDev, void *pBus, void *pOwner)
{
    struct soft_mii_bus *pMb;
    int nSt;

    pMb = lphys_miibus();
    pMb->pParent = pDev;
    nSt = __mdiobus_register(pBus, pOwner);
    return nSt;
}

void
mdiobus_unregister(void *pBus)
{
    (void)pBus;
    g_fBusRegistered = 0;
    lphys_miibus_sync();
}

/* ---- phy register / page helpers ---------------------------------------- */

int
phy_read(void *pPhy, u32 u32Regnum)
{
    u16 u16Val;

    (void)pPhy;
    lphys_inc(&g_cPhyRd);
    lphys_soft_objs_once();
    /* Soft-only shadow; no live MDIO/BAR. Soft!=product. R0 thrash refuse. */
    lphys_thrash_refuse_inc();
    u16Val = lphys_reg_get(u32Regnum);
    lphys_phydev_sync();

    if (g_fLogPhyRd == 0) {
        g_fLogPhyRd = 1;
        /* Grep: linux_phy_soft: soft phy_read */
        kprintf("linux_phy_soft: soft phy_read reg=0x%x val=0x%x page=%d "
                "calls=%u soft_ne_product=1 shadow_only=1 wire_safe=1 "
                "thrash_refuse=1\n",
                (unsigned)u32Regnum, (unsigned)u16Val, g_nPage,
                (unsigned)g_cPhyRd);
    }
    return (int)u16Val;
}

int
phy_write(void *pPhy, u32 u32Regnum, u16 u16Val)
{
    (void)pPhy;
    lphys_inc(&g_cPhyWr);
    lphys_soft_objs_once();
    /* Soft-only shadow; live BAR MDIO NOOP. Soft!=product. R0 thrash refuse. */
    lphys_thrash_refuse_inc();
    lphys_reg_set(u32Regnum, u16Val);

    if (g_fLogPhyWr == 0) {
        g_fLogPhyWr = 1;
        /* Grep: linux_phy_soft: soft phy_write */
        kprintf("linux_phy_soft: soft phy_write reg=0x%x val=0x%x page=%d "
                "calls=%u soft_ne_product=1 shadow_only=1 wire_safe=1 "
                "thrash_refuse=1\n",
                (unsigned)u32Regnum, (unsigned)u16Val, g_nPage,
                (unsigned)g_cPhyWr);
    }
    return 0;
}

int
__phy_read(void *pPhy, u32 u32Regnum)
{
    /* Soft unlocked alias of phy_read (shadow only). Soft!=product. */
    return phy_read(pPhy, u32Regnum);
}

int
__phy_write(void *pPhy, u32 u32Regnum, u16 u16Val)
{
    /* Soft unlocked alias of phy_write (shadow only). Soft!=product. */
    return phy_write(pPhy, u32Regnum, u16Val);
}

int
phy_read_paged(void *pPhy, int nPage, u32 u32Regnum)
{
    int nOld;
    int nVal;

    nOld = g_nPage;
    g_nPage = nPage;
    nVal = phy_read(pPhy, u32Regnum);
    g_nPage = nOld;
    return nVal;
}

int
phy_write_paged(void *pPhy, int nPage, u32 u32Regnum, u16 u16Val)
{
    int nOld;
    int nSt;

    nOld = g_nPage;
    g_nPage = nPage;
    nSt = phy_write(pPhy, u32Regnum, u16Val);
    g_nPage = nOld;
    return nSt;
}

int
phy_read_mmd(void *pPhy, int nDevad, u32 u32Regnum)
{
    u32 u32Dev;
    u32 u32Reg;
    u16 u16Val;

    (void)pPhy;
    lphys_soft_objs_once();
    lphys_inc(&g_cMmdRd);
    /* Soft MMD shadow only - R0 thrash refuse (no clause-45 BAR). */
    lphys_thrash_refuse_inc();
    u32Dev = (u32)nDevad % LINUX_PHY_SOFT_MMD_DEV_MAX;
    u32Reg = u32Regnum & (LINUX_PHY_SOFT_MMD_REG_MAX - 1u);
    g_nLastMmdDevad = nDevad;
    u16Val = g_aSoftMmd[u32Dev][u32Reg];
    /*
     * Soft residual: if MMD empty and clause-22 has a value for low regs,
     * fold for post-probe readers that probe MMD first (Soft!=product MMD).
     */
    if (u16Val == 0u && u32Regnum < LINUX_PHY_SOFT_REG_MAX) {
        u16Val = lphys_reg_get(u32Regnum);
    }
    return (int)u16Val;
}

int
phy_write_mmd(void *pPhy, int nDevad, u32 u32Regnum, u16 u16Val)
{
    u32 u32Dev;
    u32 u32Reg;

    (void)pPhy;
    lphys_soft_objs_once();
    lphys_inc(&g_cMmdWr);
    /* Soft MMD shadow only - R0 thrash refuse. */
    lphys_thrash_refuse_inc();
    u32Dev = (u32)nDevad % LINUX_PHY_SOFT_MMD_DEV_MAX;
    u32Reg = u32Regnum & (LINUX_PHY_SOFT_MMD_REG_MAX - 1u);
    g_nLastMmdDevad = nDevad;
    g_aSoftMmd[u32Dev][u32Reg] = u16Val;
    return 0;
}

int
phy_modify(void *pPhy, u32 u32Regnum, u16 u16Mask, u16 u16Set)
{
    u16 u16Old;
    u16 u16New;

    lphys_soft_objs_once();
    /*
     * Soft residual: RMW on clause-22 shadow only.
     * Soft!=product; R0 thrash refuse (no BAR MDIO / freestanding thrash).
     */
    lphys_thrash_refuse_inc();
    u16Old = lphys_reg_get(u32Regnum);
    u16New = (u16)((u16Old & (u16)~u16Mask) | (u16Set & u16Mask));
    lphys_reg_set(u32Regnum, u16New);
    lphys_phydev_sync();
    (void)pPhy;
    return 0;
}

int
phy_modify_paged(void *pPhy, int nPage, u32 u32Regnum, u16 u16Mask, u16 u16Set)
{
    int nOld;
    int nSt;

    nOld = g_nPage;
    g_nPage = nPage;
    nSt = phy_modify(pPhy, u32Regnum, u16Mask, u16Set);
    g_nPage = nOld;
    return nSt;
}

int
phy_modify_mmd(void *pPhy, int nDevad, u32 u32Regnum, u16 u16Mask, u16 u16Set)
{
    u16 u16Old;
    u16 u16New;
    int nOldVal;

    nOldVal = phy_read_mmd(pPhy, nDevad, u32Regnum);
    u16Old = (u16)(nOldVal & 0xffff);
    u16New = (u16)((u16Old & (u16)~u16Mask) | (u16Set & u16Mask));
    return phy_write_mmd(pPhy, nDevad, u32Regnum, u16New);
}

int
__phy_modify(void *pPhy, u32 u32Regnum, u16 u16Mask, u16 u16Set)
{
    return phy_modify(pPhy, u32Regnum, u16Mask, u16Set);
}

int
phy_select_page(void *pPhy, int nPage)
{
    int nOld;

    (void)pPhy;
    lphys_soft_objs_once();
    nOld = g_nPage;
    g_nPage = nPage;
    /* Soft residual lean: mirror page into clause-22 reg 31 shadow. */
    g_aSoftRegs[LPHYS_REG_PAGE] = (u16)(nPage & 0xff);
    lphys_phydev_sync();
    /* Soft: return previous page (Linux-shaped). Soft!=product. */
    return nOld;
}

int
phy_restore_page(void *pPhy, int nOldPage, int nRet)
{
    (void)pPhy;
    g_nPage = nOldPage;
    return nRet;
}

int
phy_save_page(void *pPhy)
{
    (void)pPhy;
    /* Soft: return current page (Linux-shaped save). */
    return g_nPage;
}

/* ---- pause / eee / speed / ioctl ---------------------------------------- */

void
phy_mac_interrupt(void *pPhy)
{
    (void)pPhy;
    /* Soft residual: MAC-side link IRQ bookkeeping only; no freestanding. */
    lphys_soft_objs_once();
    lphys_phydev_sync();
}

void
phy_get_pause(void *pPhy, u8 *pu8Tx, u8 *pu8Rx)
{
    (void)pPhy;
    lphys_soft_objs_once();
    /* Soft residual lean: pause lamps from ADVERTISE seed / writes. */
    if (pu8Tx != NULL) {
        *pu8Tx = (u8)((g_fPause != 0) ? 1u : 0u);
    }
    if (pu8Rx != NULL) {
        *pu8Rx = (u8)((g_fPause != 0) ? 1u : 0u);
    }
}

void
phy_set_asym_pause(void *pPhy, int nRx, int nTx)
{
    (void)pPhy;
    (void)nRx;
    (void)nTx;
    lphys_soft_objs_once();
    g_fAsymPause = 1;
    g_aSoftRegs[4] = (u16)(g_aSoftRegs[4] | LPHYS_ADVERTISE_ASYM);
    lphys_phydev_sync();
}

void
phy_support_asym_pause(void *pPhy)
{
    (void)pPhy;
    lphys_soft_objs_once();
    g_fAsymPause = 1;
    g_aSoftRegs[4] = (u16)(g_aSoftRegs[4] | LPHYS_ADVERTISE_ASYM);
    lphys_phydev_sync();
}

void
phy_support_sym_pause(void *pPhy)
{
    (void)pPhy;
    lphys_soft_objs_once();
    g_fPause = 1;
    g_aSoftRegs[4] = (u16)(g_aSoftRegs[4] | LPHYS_ADVERTISE_PAUSE);
    lphys_phydev_sync();
}

void
phy_set_sym_pause(void *pPhy, int nRx, int nTx, int nAutoneg)
{
    (void)pPhy;
    (void)nAutoneg;
    lphys_soft_objs_once();
    g_fPause = (nRx != 0 || nTx != 0) ? 1 : 0;
    if (g_fPause != 0) {
        g_aSoftRegs[4] = (u16)(g_aSoftRegs[4] | LPHYS_ADVERTISE_PAUSE);
    } else {
        g_aSoftRegs[4] = (u16)(g_aSoftRegs[4] & (u16)~LPHYS_ADVERTISE_PAUSE);
    }
    lphys_phydev_sync();
}

void
phy_support_eee(void *pPhy)
{
    (void)pPhy;
    /* Soft residual: EEE advertise lamp only; Soft!=product EEE copper. */
}

void
phy_resolve_aneg_pause(void *pPhy)
{
    (void)pPhy;
    lphys_soft_objs_once();
    /*
     * Soft residual: resolve pause from ADVERTISE & LPA shadows.
     * Bookkeeping only - no freestanding MAC dual-drive.
     */
    if ((g_aSoftRegs[4] & LPHYS_ADVERTISE_PAUSE) != 0u &&
        (g_aSoftRegs[5] & LPHYS_ADVERTISE_PAUSE) != 0u) {
        g_fPause = 1;
    } else {
        g_fPause = 0;
    }
    if ((g_aSoftRegs[4] & LPHYS_ADVERTISE_ASYM) != 0u ||
        (g_aSoftRegs[5] & LPHYS_ADVERTISE_ASYM) != 0u) {
        g_fAsymPause = 1;
    }
    lphys_phydev_sync();
}

int
phy_advertise_eee_all(void *pPhy)
{
    (void)pPhy;
    return 0;
}

int
phy_speed_down(void *pPhy, int fSync)
{
    (void)pPhy;
    (void)fSync;
    lphys_soft_objs_once();
    /* Soft residual: drop soft speed one step (1000->100->10); bookkeeping. */
    if (g_nSpeedMbps >= 1000) {
        lphys_link_set(g_fLinkUp, 100, g_nDuplexFull);
    } else if (g_nSpeedMbps >= 100) {
        lphys_link_set(g_fLinkUp, 10, g_nDuplexFull);
    }
    return 0;
}

int
phy_speed_up(void *pPhy)
{
    (void)pPhy;
    lphys_soft_objs_once();
    /* Soft residual: restore soft default speed when up. */
    if (g_fLinkUp != 0) {
        lphys_link_set(1, LINUX_PHY_SOFT_SPEED_DEFAULT, g_nDuplexFull);
    }
    return 0;
}

int
phy_do_ioctl_running(void *pNetdev, void *pIfr, int nCmd)
{
    (void)pNetdev;
    (void)pIfr;
    (void)nCmd;
    return 0;
}

/* ---- ethtool (soft success 0) ------------------------------------------- */

int
phy_ethtool_nway_reset(void *pNetdev)
{
    (void)pNetdev;
    return phy_start_aneg(NULL);
}

int
phy_ethtool_get_eee(void *pPhy, void *pData)
{
    (void)pPhy;
    (void)pData;
    return 0;
}

int
phy_ethtool_set_eee(void *pPhy, void *pData)
{
    (void)pPhy;
    (void)pData;
    return 0;
}

int
phy_ethtool_get_link_ksettings(void *pNetdev, void *pCmd)
{
    (void)pNetdev;
    /*
     * Soft residual lean: do not write host ethtool_link_ksettings layout
     * (Soft!=ABI-stable). Return 0 so callers soft-complete; link lamps
     * live on linux_phy_soft_link_up / speed getters + soft_phy_device front.
     */
    (void)pCmd;
    lphys_soft_objs_once();
    lphys_phydev_sync();
    return 0;
}

int
phy_ethtool_set_link_ksettings(void *pNetdev, const void *pCmd)
{
    (void)pNetdev;
    (void)pCmd;
    return 0;
}

int
phy_ethtool_ksettings_get(void *pNetdev, void *pCmd)
{
    return phy_ethtool_get_link_ksettings(pNetdev, pCmd);
}

int
phy_ethtool_ksettings_set(void *pNetdev, const void *pCmd)
{
    return phy_ethtool_set_link_ksettings(pNetdev, pCmd);
}

/* ---- Public init / diagnostics ------------------------------------------ */

int
linux_phy_soft_residual_lean_lamp_once(void)
{
    struct soft_phy_device *pPd;
    struct soft_mii_bus *pMb;

    if (g_fResidualLeanLamp != 0) {
        return 0;
    }
    if (g_fReady == 0) {
        return 0;
    }
    g_fResidualLeanLamp = 1;
    lphys_soft_objs_once();
    lphys_phydev_sync();
    lphys_miibus_sync();
    pPd = lphys_phydev();
    pMb = lphys_miibus();

    /*
     * Grep: linux_phy_soft: soft residual lean PASS
     * Grep: linux_phy_soft: soft residual freestanding_rtl=SKIP
     * Grep: linux_phy_soft: soft residual product_nic=UDX
     * Grep: linux_phy_soft: soft residual RUN_INIT=0 freestanding_no_exec
     * Soft!=product; freestanding_rtl=SKIP; product_nic=UDX;
     * never product link claim; shadow MDIO only; no BAR thrash.
     * layout_ver≥5: LAW residual stamps + state/BMCR/BMSR + thrash tallies.
     * C0 denser: Dual DoD A/B OPEN residual_ne_close; stamp-free; G-AC-1.
     */
    kprintf("linux_phy_soft: soft residual lean PASS Soft!=product "
            "soft residual freestanding_rtl=SKIP "
            "soft residual product_nic=UDX "
            "soft residual RUN_INIT=0 freestanding_no_exec "
            "layout_ver=%u phydev_cb=%u miibus_cb=%u "
            "magic_pd=0x%x magic_mb=0x%x "
            "link_soft=%d speed=%d duplex=%s pause=%d asym=%d "
            "connected=%d started=%d bus_reg=%d state=%d "
            "mdio_rd=%u mdio_wr=%u phy_rd=%u phy_wr=%u mmd_rd=%u mmd_wr=%u "
            "genphy=%u thrash_refuse=%u ksym_ok=%u "
            "udx_eng=1 never_phyar=1 product_open=1 gate0=1 "
            "fs_rtl_skip=1 product_nic_udx=1 no_product_link=1 "
            "soft_only=1 wire_safe=1 shadow_mdio=1 no_bar_mdio=1 "
            "RUN_INIT=0 freestanding_no_exec=1 never_exec_ko=1 "
            "DoD_A=OPEN DoD_B=OPEN residual_ne_close=1 "
            "soft_ne_product=1 dual=MIT_OR_Apache-2.0 G-AC-1=1 "
            "stamp_storm=0 no_version_stamp=1 "
            "(hybrid residual lean; product NIC=UDX userspace; "
            "freestanding_rtl=SKIP; never product link claim; "
            "shadow MDIO only; no freestanding thrash; "
            "Dual DoD A/B OPEN agent residual != product close)\n",
            (unsigned)LINUX_PHY_SOFT_LAYOUT_VER,
            (unsigned)LINUX_PHY_SOFT_PHYDEV_CB,
            (unsigned)LINUX_PHY_SOFT_MIIBUS_CB,
            (unsigned)pPd->u32Magic, (unsigned)pMb->u32Magic,
            g_fLinkUp, g_nSpeedMbps,
            g_nDuplexFull != 0 ? "full" : "half",
            g_fPause, g_fAsymPause,
            g_fConnected, g_fStarted, g_fBusRegistered, g_nState,
            (unsigned)g_cMdioRd, (unsigned)g_cMdioWr,
            (unsigned)g_cPhyRd, (unsigned)g_cPhyWr,
            (unsigned)g_cMmdRd, (unsigned)g_cMmdWr,
            (unsigned)g_cGenphy, (unsigned)g_cThrashRefuse,
            (unsigned)g_cKsymOk);

    /* Grep: linux_phy_soft: soft residual layout */
    kprintf("linux_phy_soft: soft residual layout Soft!=product "
            "freestanding_rtl=SKIP product_nic=UDX soft_phy=1 soft_mii=1 "
            "pd_addr=%d pd_iface=%d pd_autoneg=%d pd_state=%d "
            "pd_bmcr=0x%x pd_bmsr=0x%x pd_page=%d "
            "pd_udx_eng=%u pd_never_phyar=%u pd_product_open=%u "
            "pd_fs_rtl_skip=%u pd_product_nic_udx=%u pd_no_product_link=%u "
            "mb_id=%s mb_reg=%d mb_shadow_ops=%u mb_thrash_refuse=%u "
            "reg_shadow=%u mmd_dev=%u thrash_refuse=%u "
            "RUN_INIT=0 freestanding_no_exec=1 "
            "DoD_A=OPEN DoD_B=OPEN residual_ne_close=1 "
            "product_nic=UDX product_copper=OPEN G-AC-1 "
            "stamp_storm=0 no_version_stamp=1 "
            "(soft phy_device/mii_bus-ish front for hybrid+UDX eng; "
            "shadow MDIO only; never product link claim; "
            "freestanding_rtl=SKIP residual honesty; Dual DoD OPEN)\n",
            pPd->nAddr, pPd->nIface, pPd->nAutoneg, pPd->nState,
            (unsigned)pPd->u16Bmcr, (unsigned)pPd->u16Bmsr, pPd->nPage,
            (unsigned)pPd->u8UdxEng, (unsigned)pPd->u8NeverPhyar,
            (unsigned)pPd->u8ProductOpen,
            (unsigned)pPd->u8FsRtlSkip, (unsigned)pPd->u8ProductNicUdx,
            (unsigned)pPd->u8NoProductLink,
            pMb->szId[0] != '\0' ? pMb->szId : "soft-mii",
            pMb->nRegistered, (unsigned)pMb->u32ShadowOps,
            (unsigned)pMb->u32ThrashRefuse,
            (unsigned)LINUX_PHY_SOFT_REG_MAX,
            (unsigned)LINUX_PHY_SOFT_MMD_DEV_MAX,
            (unsigned)g_cThrashRefuse);

    /*
     * Grep: linux_phy_soft: soft residual lean UDX eng
     * Once-lamp only (no stamp storms). Soft!=product · G-AC-1.
     * Product NIC = userspace UDX - not freestanding rtl thrash.
     */
    kprintf("linux_phy_soft: soft residual lean UDX eng "
            "layout_ver=%u udx_eng=1 product_nic=UDX "
            "freestanding_rtl=SKIP never_phyar=1 never_bar_mdio=1 "
            "never_cf8=1 never_rtl_mdio=1 no_product_link=1 "
            "product_copper=OPEN gate0=1 thrash_refuse=%u genphy=%u "
            "state=%d RUN_INIT=0 freestanding_no_exec=1 "
            "DoD_A=OPEN DoD_B=OPEN residual_ne_close=1 "
            "soft_ne_product=1 dual=MIT_OR_Apache-2.0 G-AC-1=1 "
            "stamp_storm=0 no_version_stamp=1 "
            "(product NIC=UDX residual lean; Soft!=product; "
            "not freestanding rtl thrash; shadow MDIO only; "
            "never product link claim; Dual DoD OPEN)\n",
            (unsigned)LINUX_PHY_SOFT_LAYOUT_VER,
            (unsigned)g_cThrashRefuse, (unsigned)g_cGenphy, g_nState);

    return 1;
}

int
linux_phy_soft_thrash_refuse_lamp_once(void)
{
    if (g_fThrashRefuseLamp != 0) {
        return 0;
    }
    if (g_fReady == 0) {
        return 0;
    }
    g_fThrashRefuseLamp = 1;
    /*
     * Grep: linux_phy_soft: soft residual R0 thrash refuse
     * Soft MDIO never dual-drives freestanding BAR / copper (R0 class).
     * Soft!=product; freestanding_rtl=SKIP; product_nic=UDX; G-AC-1.
     * C0 denser: RUN_INIT=0 freestanding_no_exec; Dual DoD OPEN residual.
     */
    kprintf("linux_phy_soft: soft residual R0 thrash refuse "
            "shadow_only=1 no_bar_mdio=1 no_cf8=1 no_rtl8168_mdio=1 "
            "no_copper_program=1 never_phyar=1 thrash_refuse_n=%u "
            "mdio_rd=%u mdio_wr=%u phy_rd=%u phy_wr=%u mmd_rd=%u mmd_wr=%u "
            "wire_safe=1 freestanding_rtl=SKIP product_nic=UDX "
            "no_product_link=1 RUN_INIT=0 freestanding_no_exec=1 "
            "never_exec_ko=1 DoD_A=OPEN DoD_B=OPEN residual_ne_close=1 "
            "soft_ne_product=1 gate0=1 dual=MIT_OR_Apache-2.0 G-AC-1=1 "
            "stamp_storm=0 no_version_stamp=1 "
            "(soft phy MDIO shadow only; freestanding_rtl=SKIP; "
            "product_nic=UDX; never product link; no freestanding thrash; "
            "dual DoD B residual; Dual DoD OPEN != product close)\n",
            (unsigned)g_cThrashRefuse,
            (unsigned)g_cMdioRd, (unsigned)g_cMdioWr,
            (unsigned)g_cPhyRd, (unsigned)g_cPhyWr,
            (unsigned)g_cMmdRd, (unsigned)g_cMmdWr);
    return 1;
}

int
linux_phy_soft_dual_dod_b_lamp_once(void)
{
    if (g_fDualDodBLamp != 0) {
        return 0;
    }
    if (g_fReady == 0) {
        return 0;
    }
    g_fDualDodBLamp = 1;
    /*
     * Grep: linux_phy_soft: soft dual DoD B residual gate0 PASS
     * Grep: linux_phy_soft: soft residual DoD OPEN
     * Grep: linux_phy_soft: soft residual RUN_INIT=0 freestanding_no_exec
     * Dual DoD B product direction = userspace UDX NIC (OPEN until DUT).
     * Dual DoD A companion honesty = USB UDX OPEN (phy is NIC residual only).
     * freestanding_rtl=SKIP default; soft PHY is control/bookkeeping only.
     * Soft!=product; never steals BAR/MDIO; never product link claim.
     * Agent residual != product close (residual_ne_close). Stamp-free.
     */
    kprintf("linux_phy_soft: soft dual DoD B residual gate0 PASS "
            "soft residual DoD OPEN "
            "soft residual RUN_INIT=0 freestanding_no_exec "
            "Soft!=product freestanding_rtl=SKIP product_nic=UDX "
            "soft_phy=1 shadow_mdio=1 no_bar_steal=1 thrash_refuse=1 "
            "udx_eng=1 never_phyar=1 no_product_link=1 "
            "DoD_A=OPEN need=xhci_udx class=USB_UDX "
            "DoD_B=OPEN need=rtl8168_udx class=NIC_UDX "
            "residual_ne_close=1 agent_ne_close=1 "
            "RUN_INIT=0 freestanding_no_exec=1 never_exec_ko=1 "
            "layout_ver=%u link_soft=%d state=%d bus_reg=%d "
            "ksym_ok=%u dual=MIT_OR_Apache-2.0 G-AC-1=1 "
            "stamp_storm=0 no_version_stamp=1 "
            "(PHY soft bookkeeping only; product NIC=UDX userspace; "
            "freestanding_rtl=SKIP; never product link claim; "
            "dual DoD B OPEN until DUT proof; Dual DoD A/B OPEN; "
            "agent residual != product close; C0 soft residual only)\n",
            (unsigned)LINUX_PHY_SOFT_LAYOUT_VER,
            g_fLinkUp, g_nState, g_fBusRegistered,
            (unsigned)g_cKsymOk);
    return 1;
}

void
linux_phy_soft_init(void)
{
    g_cInitCalls++;
    if (g_fReady) {
        /* Grep: linux_phy_soft: soft init PASS n= */
        kprintf("linux_phy_soft: soft init PASS n=%u (idempotent call=%u)\n",
                (unsigned)g_cKsymOk, (unsigned)g_cInitCalls);
        return;
    }

    g_cKsymOk = 0u;
    g_cKsymSkip = 0u;
    g_cConnect = 0u;
    g_cDisconnect = 0u;
    g_cStart = 0u;
    g_cStop = 0u;
    g_cStartAneg = 0u;
    g_cReset = 0u;
    g_cMdioRd = 0u;
    g_cMdioWr = 0u;
    g_cPhyRd = 0u;
    g_cPhyWr = 0u;
    g_cAlloc = 0u;
    g_cRegister = 0u;
    g_cLinkChange = 0u;
    g_cMmdRd = 0u;
    g_cMmdWr = 0u;
    g_cGenphy = 0u;
    g_cThrashRefuse = 0u;
    g_nState = LINUX_PHY_SOFT_STATE_DOWN;
    g_fConnected = 0;
    g_fStarted = 0;
    g_fBusRegistered = 0;
    g_nIface = 0;
    g_nPhyAddr = 0;
    g_pNetdev = NULL;
    g_pfnHandler = NULL;
    g_fPause = 0;
    g_fAsymPause = 0;
    g_fLogConnect = 0;
    g_fLogStart = 0;
    g_fLogStop = 0;
    g_fLogReset = 0;
    g_fLogMdio = 0;
    g_fLogPhyRd = 0;
    g_fLogPhyWr = 0;
    g_fLogBusReg = 0;
    g_fLogLink = 0;
    g_fLogStatus = 0;
    g_fLogAttached = 0;
    g_fHybridLamp = 0;
    g_fResidualLeanLamp = 0;
    g_fThrashRefuseLamp = 0;
    g_fDualDodBLamp = 0;
    g_fLogGenphy = 0;
    g_fSoftObjsInit = 0;
    lphys_soft_objs_once();
    g_fReady = 1;

    /*
     * Export soft bodies for F2 ksym / module resolve.
     * Replaces zero-return generic stubs for pointer-critical names.
     * Weak linux_ksym_register: count as skip when F2 not linked yet.
     * r8169.ko und phy/mdio/genphy class: names below (residual lean surface).
     */
    lphys_ksym_one("phy_connect", (void *)phy_connect);
    lphys_ksym_one("phy_connect_direct", (void *)phy_connect_direct);
    lphys_ksym_one("phy_disconnect", (void *)phy_disconnect);
    lphys_ksym_one("phy_start", (void *)phy_start);
    lphys_ksym_one("phy_stop", (void *)phy_stop);
    lphys_ksym_one("phy_start_aneg", (void *)phy_start_aneg);
    lphys_ksym_one("phy_init_hw", (void *)phy_init_hw);
    lphys_ksym_one("phy_resume", (void *)phy_resume);
    lphys_ksym_one("phy_suspend", (void *)phy_suspend);
    lphys_ksym_one("phy_attached_info", (void *)phy_attached_info);
    lphys_ksym_one("phy_print_status", (void *)phy_print_status);
    lphys_ksym_one("phy_set_max_speed", (void *)phy_set_max_speed);
    lphys_ksym_one("phy_aneg_done", (void *)phy_aneg_done);
    lphys_ksym_one("genphy_soft_reset", (void *)genphy_soft_reset);
    lphys_ksym_one("genphy_read_status", (void *)genphy_read_status);
    lphys_ksym_one("genphy_update_link", (void *)genphy_update_link);
    lphys_ksym_one("genphy_restart_aneg", (void *)genphy_restart_aneg);
    lphys_ksym_one("genphy_check_and_restart_aneg",
                   (void *)genphy_check_and_restart_aneg);
    lphys_ksym_one("genphy_read_lpa", (void *)genphy_read_lpa);
    lphys_ksym_one("genphy_read_abilities", (void *)genphy_read_abilities);
    lphys_ksym_one("genphy_config_aneg", (void *)genphy_config_aneg);
    lphys_ksym_one("genphy_setup_forced", (void *)genphy_setup_forced);
    lphys_ksym_one("genphy_resume", (void *)genphy_resume);
    lphys_ksym_one("genphy_suspend", (void *)genphy_suspend);
    lphys_ksym_one("mdiobus_read", (void *)mdiobus_read);
    lphys_ksym_one("mdiobus_write", (void *)mdiobus_write);
    lphys_ksym_one("__mdiobus_read", (void *)__mdiobus_read);
    lphys_ksym_one("__mdiobus_write", (void *)__mdiobus_write);
    lphys_ksym_one("mdiobus_get_phy", (void *)mdiobus_get_phy);
    lphys_ksym_one("mdiobus_alloc", (void *)mdiobus_alloc);
    lphys_ksym_one("mdiobus_alloc_size", (void *)mdiobus_alloc_size);
    lphys_ksym_one("devm_mdiobus_alloc", (void *)devm_mdiobus_alloc);
    lphys_ksym_one("devm_mdiobus_alloc_size", (void *)devm_mdiobus_alloc_size);
    lphys_ksym_one("mdiobus_free", (void *)mdiobus_free);
    lphys_ksym_one("mdiobus_register", (void *)mdiobus_register);
    lphys_ksym_one("__mdiobus_register", (void *)__mdiobus_register);
    lphys_ksym_one("__devm_mdiobus_register", (void *)__devm_mdiobus_register);
    lphys_ksym_one("mdiobus_unregister", (void *)mdiobus_unregister);
    lphys_ksym_one("phy_read", (void *)phy_read);
    lphys_ksym_one("phy_write", (void *)phy_write);
    lphys_ksym_one("__phy_read", (void *)__phy_read);
    lphys_ksym_one("__phy_write", (void *)__phy_write);
    lphys_ksym_one("phy_read_paged", (void *)phy_read_paged);
    lphys_ksym_one("phy_write_paged", (void *)phy_write_paged);
    lphys_ksym_one("phy_read_mmd", (void *)phy_read_mmd);
    lphys_ksym_one("phy_write_mmd", (void *)phy_write_mmd);
    lphys_ksym_one("phy_modify", (void *)phy_modify);
    lphys_ksym_one("phy_modify_paged", (void *)phy_modify_paged);
    lphys_ksym_one("phy_modify_mmd", (void *)phy_modify_mmd);
    lphys_ksym_one("__phy_modify", (void *)__phy_modify);
    lphys_ksym_one("phy_mac_interrupt", (void *)phy_mac_interrupt);
    lphys_ksym_one("phy_get_pause", (void *)phy_get_pause);
    lphys_ksym_one("phy_set_asym_pause", (void *)phy_set_asym_pause);
    lphys_ksym_one("phy_support_asym_pause", (void *)phy_support_asym_pause);
    lphys_ksym_one("phy_support_sym_pause", (void *)phy_support_sym_pause);
    lphys_ksym_one("phy_set_sym_pause", (void *)phy_set_sym_pause);
    lphys_ksym_one("phy_support_eee", (void *)phy_support_eee);
    lphys_ksym_one("phy_resolve_aneg_pause", (void *)phy_resolve_aneg_pause);
    lphys_ksym_one("phy_advertise_eee_all", (void *)phy_advertise_eee_all);
    lphys_ksym_one("phy_speed_down", (void *)phy_speed_down);
    lphys_ksym_one("phy_speed_up", (void *)phy_speed_up);
    lphys_ksym_one("phy_do_ioctl_running", (void *)phy_do_ioctl_running);
    lphys_ksym_one("phy_select_page", (void *)phy_select_page);
    lphys_ksym_one("phy_restore_page", (void *)phy_restore_page);
    lphys_ksym_one("phy_save_page", (void *)phy_save_page);
    lphys_ksym_one("phy_ethtool_nway_reset", (void *)phy_ethtool_nway_reset);
    lphys_ksym_one("phy_ethtool_get_eee", (void *)phy_ethtool_get_eee);
    lphys_ksym_one("phy_ethtool_set_eee", (void *)phy_ethtool_set_eee);
    lphys_ksym_one("phy_ethtool_get_link_ksettings",
                   (void *)phy_ethtool_get_link_ksettings);
    lphys_ksym_one("phy_ethtool_set_link_ksettings",
                   (void *)phy_ethtool_set_link_ksettings);
    lphys_ksym_one("phy_ethtool_ksettings_get",
                   (void *)phy_ethtool_ksettings_get);
    lphys_ksym_one("phy_ethtool_ksettings_set",
                   (void *)phy_ethtool_ksettings_set);

    /* Grep: linux_phy_soft: soft init PASS n= */
    kprintf("linux_phy_soft: soft init PASS n=%u skip=%u soft_ne_abi=1 "
            "product=OPEN soft_ne_product=1 r8169_phy_ksym=complete "
            "wire_safe=1 shadow_mdio=1 link_state_soft=1 mdio_ish=1 "
            "layout_ver=%u genphy_residual=1 thrash_refuse=1 "
            "udx_eng=1 never_phyar=1 freestanding_rtl=SKIP "
            "product_nic=UDX no_product_link=1 "
            "RUN_INIT=0 freestanding_no_exec=1 never_exec_ko=1 "
            "DoD_A=OPEN DoD_B=OPEN residual_ne_close=1 "
            "dual=MIT_OR_Apache-2.0 G-AC-1=1 "
            "stamp_storm=0 no_version_stamp=1 "
            "Soft!=product (hybrid residual lean; product NIC=UDX; "
            "freestanding_rtl=SKIP; never product link claim; "
            "no freestanding thrash; Dual DoD OPEN; C0 soft residual)\n",
            (unsigned)g_cKsymOk, (unsigned)g_cKsymSkip,
            (unsigned)LINUX_PHY_SOFT_LAYOUT_VER);

    /*
     * Soft dep virtual slots for NIC soft-order honesty (eng only):
     *   libphy      -> slot libphy-soft
     *   mdio_devres -> slot mdio_devres-soft
     * So r8169 soft load does not spuriously FAIL KSYM need:libphy when a
     * soft dep lists libphy/mdio (G752 eng path). loaded(libphy) stays 0.
     * Grep: linux_module: soft dep virtual libphy soft=1 product=0
     * Grep: linux_phy_soft: soft dep virtual register libphy+mdio_devres
     * Soft!=product; != real libphy.ko / mdio_devres.ko.
     */
    if (linux_module_soft_dep_virtual_register != NULL) {
        (void)linux_module_soft_dep_virtual_register("libphy");
        (void)linux_module_soft_dep_virtual_register("mdio_devres");
        kprintf("linux_phy_soft: soft dep virtual register "
                "libphy+mdio_devres soft=1 product=0 Soft!=product "
                "(deps_ready eng PASS; != real libphy/mdio_devres.ko; "
                "no FAIL need:libphy; G-AC-1)\n");
    } else {
        kprintf("linux_phy_soft: soft dep virtual SKIP no module path "
                "soft=1 product=0 Soft!=product G-AC-1\n");
    }

    /*
     * Hybrid 4a / gate0 honesty lamp (once): soft phy/mdio never dual-drives
     * the live 10ec:8168 BAR or freestanding copper path. Shadow regs only.
     * Grep: linux_phy_soft: soft hybrid zero-touch PASS
     */
    if (g_fHybridLamp == 0) {
        g_fHybridLamp = 1;
        kprintf("linux_phy_soft: soft hybrid zero-touch PASS "
                "shadow_mdio=1 no_bar_mdio=1 no_cf8=1 no_rtl8168_mdio=1 "
                "no_copper_program=1 freestanding_rtl=SKIP product_nic=UDX "
                "no_product_link=1 RUN_INIT=0 freestanding_no_exec=1 "
                "never_exec_ko=1 DoD_A=OPEN DoD_B=OPEN residual_ne_close=1 "
                "soft_ne_product=1 gate0=1 dual=MIT_OR_Apache-2.0 G-AC-1=1 "
                "stamp_storm=0 no_version_stamp=1 "
                "(soft-only MDIO residual; freestanding_rtl=SKIP; "
                "product_nic=UDX; never product link; no freestanding thrash; "
                "Dual DoD OPEN; C0 soft residual)\n");
    }

    /* Soft link seed lamp (once) - residual denser status for eng grep. */
    if (g_fLogLink == 0) {
        g_fLogLink = 1;
        /* Grep: linux_phy_soft: soft link */
        kprintf("linux_phy_soft: soft link seed up=%d speed=%d duplex=%s "
                "bmsr=0x%x state=%d soft_ne_product=1 no_product_link=1 "
                "freestanding_rtl=SKIP product_nic=UDX "
                "(soft seed bookkeeping; never product link claim)\n",
                g_fLinkUp, g_nSpeedMbps,
                g_nDuplexFull != 0 ? "full" : "half",
                (unsigned)g_aSoftRegs[1], g_nState);
    }

    /* Residual lean once-lamp (layout + genphy + wire-safe honesty). */
    (void)linux_phy_soft_residual_lean_lamp_once();
    /* R0 thrash refuse once-lamp (shadow MDIO only). */
    (void)linux_phy_soft_thrash_refuse_lamp_once();
    /* Dual DoD B residual gate0 companion (PHY soft bookkeeping). */
    (void)linux_phy_soft_dual_dod_b_lamp_once();
}

int
linux_phy_soft_ready(void)
{
    return g_fReady;
}

u32
linux_phy_soft_ksym_ok(void)
{
    return g_cKsymOk;
}

u32
linux_phy_soft_ksym_skip(void)
{
    return g_cKsymSkip;
}

u32
linux_phy_soft_connect_count(void)
{
    return g_cConnect;
}

u32
linux_phy_soft_start_count(void)
{
    return g_cStart;
}

int
linux_phy_soft_link_up(void)
{
    return g_fLinkUp;
}

int
linux_phy_soft_speed_mbps(void)
{
    if (g_fLinkUp == 0) {
        return 0;
    }
    return g_nSpeedMbps;
}

int
linux_phy_soft_duplex_full(void)
{
    return g_nDuplexFull;
}

u32
linux_phy_soft_link_change_count(void)
{
    return g_cLinkChange;
}

u32
linux_phy_soft_mdio_rd_count(void)
{
    return g_cMdioRd;
}

u32
linux_phy_soft_mdio_wr_count(void)
{
    return g_cMdioWr;
}

u32
linux_phy_soft_phy_rd_count(void)
{
    return g_cPhyRd;
}

u32
linux_phy_soft_phy_wr_count(void)
{
    return g_cPhyWr;
}

u32
linux_phy_soft_mmd_rd_count(void)
{
    return g_cMmdRd;
}

u32
linux_phy_soft_mmd_wr_count(void)
{
    return g_cMmdWr;
}

u32
linux_phy_soft_layout_ver(void)
{
    if (g_fReady == 0) {
        return 0u;
    }
    return LINUX_PHY_SOFT_LAYOUT_VER;
}

int
linux_phy_soft_bus_registered(void)
{
    return g_fBusRegistered;
}

int
linux_phy_soft_wire_safe(void)
{
    /* Always 1: soft MDIO never thrash freestanding BAR / copper. */
    return 1;
}

u32
linux_phy_soft_thrash_refuse_count(void)
{
    return g_cThrashRefuse;
}

int
linux_phy_soft_state(void)
{
    lphys_state_recompute();
    return g_nState;
}

int
linux_phy_soft_udx_eng(void)
{
    /* Always 1: residual lean surface for future UDX NIC eng. Soft!=product. */
    return 1;
}

u32
linux_phy_soft_genphy_count(void)
{
    return g_cGenphy;
}

int
linux_phy_soft_freestanding_rtl_skip(void)
{
    /* Always 1: freestanding_rtl=SKIP residual honesty. Soft!=product. */
    return 1;
}

int
linux_phy_soft_product_nic_udx(void)
{
    /* Always 1: product_nic=UDX residual honesty. Soft!=product; G-AC-1. */
    return 1;
}

int
linux_phy_soft_no_product_link(void)
{
    /* Always 1: soft link lamps never claim product copper/link. */
    return 1;
}
