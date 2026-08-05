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
 *
 * Soft ≠ ABI-stable: struct layouts under void* are incomplete soft shapes.
 * Soft ≠ product: no real MDIO bus, no copper link, no .ko PHY driver load.
 *
 * Greppable markers (keep stable):
 *   linux_phy_soft: soft init PASS n=
 *   linux_phy_soft: soft connect …
 *   linux_phy_soft: soft start …
 *   linux_phy_soft: soft stop …
 *   linux_phy_soft: soft reset …
 *   linux_phy_soft: soft mdiobus …
 *   linux_phy_soft: soft phy_read …
 *   linux_phy_soft: soft phy_write …
 */
#include <gj/klog.h>
#include <gj/linux_phy_soft.h>
#include <gj/string.h>
#include <gj/types.h>

/*
 * F2 linux_ksym may be linked later. Weak unresolved → NULL; init skips export.
 * Coordinator may also point ksym at these symbols by C name without this call.
 */
int linux_ksym_register(const char *szName, void *pFn) __attribute__((weak));

/* Soft clause-22-ish seeds (not a real PHY; Soft≠product link). */
#define LPHYS_BMCR_ANENABLE   0x1000u
#define LPHYS_BMSR_DEFAULT    0x7949u /* abilities + soft aneg-capable */
#define LPHYS_BMSR_LSTATUS    0x0004u
#define LPHYS_BMSR_ANEGCOMPLETE 0x0020u
#define LPHYS_PHYID1_SOFT     0x001cu /* Realtek-ish OUI nibble soft only */
#define LPHYS_PHYID2_SOFT     0xc800u

/* ---- Soft state --------------------------------------------------------- */

static int g_fReady;
static u32 g_cInitCalls;
static u32 g_cKsymOk;
static u32 g_cKsymSkip;

/* Soft static objects returned by pointer APIs (never free; zeroed once). */
static u8 g_aSoftPhyDev[LINUX_PHY_SOFT_PHYDEV_CB];
static u8 g_aSoftMiiBus[LINUX_PHY_SOFT_MIIBUS_CB];
static u8 g_fSoftObjsInit;

/* Soft lifecycle bookkeeping (post-probe r8169 soft path). */
static int g_fConnected;
static int g_fStarted;
static int g_fBusRegistered;
static int g_nIface;
static int g_nPage;
static int g_nPhyAddr;
static void *g_pNetdev;
static void *g_pfnHandler;

/* Soft MDIO clause-22 register shadow (addr-agnostic single soft PHY). */
static u16 g_aSoftRegs[LINUX_PHY_SOFT_REG_MAX];
static u16 g_u16LastWrite;
static u32 g_u32LastReg;

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

/* Once lamps for greppable soft breadcrumbs (hot path stays quiet after). */
static int g_fLogConnect;
static int g_fLogStart;
static int g_fLogStop;
static int g_fLogReset;
static int g_fLogMdio;
static int g_fLogPhyRd;
static int g_fLogPhyWr;
static int g_fLogBusReg;

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

static void
lphys_regs_seed(void)
{
    u32 i;

    for (i = 0u; i < LINUX_PHY_SOFT_REG_MAX; i++) {
        g_aSoftRegs[i] = 0u;
    }
    /*
     * Soft-plausible clause-22 defaults so post-probe status reads are not
     * all-zero death (Soft≠product copper; values are honesty-seed only).
     * BMCR: ANE; BMSR: abilities + LSTATUS + ANEGCOMPLETE (soft link-up).
     */
    g_aSoftRegs[0] = (u16)LPHYS_BMCR_ANENABLE;
    g_aSoftRegs[1] = (u16)(LPHYS_BMSR_DEFAULT | LPHYS_BMSR_LSTATUS |
                           LPHYS_BMSR_ANEGCOMPLETE);
    g_aSoftRegs[2] = (u16)LPHYS_PHYID1_SOFT;
    g_aSoftRegs[3] = (u16)LPHYS_PHYID2_SOFT;
    g_nPage = 0;
    g_u16LastWrite = 0u;
    g_u32LastReg = 0u;
}

static void
lphys_soft_objs_once(void)
{
    if (g_fSoftObjsInit != 0) {
        return;
    }
    memset(g_aSoftPhyDev, 0, sizeof(g_aSoftPhyDev));
    memset(g_aSoftMiiBus, 0, sizeof(g_aSoftMiiBus));
    /*
     * Soft marker bytes so a debugger can recognize the blobs; not Linux ABI.
     * "GJph" / "GJmb" at offset 0.
     */
    g_aSoftPhyDev[0] = (u8)'G';
    g_aSoftPhyDev[1] = (u8)'J';
    g_aSoftPhyDev[2] = (u8)'p';
    g_aSoftPhyDev[3] = (u8)'h';
    g_aSoftMiiBus[0] = (u8)'G';
    g_aSoftMiiBus[1] = (u8)'J';
    g_aSoftMiiBus[2] = (u8)'m';
    g_aSoftMiiBus[3] = (u8)'b';
    lphys_regs_seed();
    g_fSoftObjsInit = 1u;
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

    /*
     * Soft BMCR reset bit (0x8000): clear after "reset" and reseed status.
     * Soft≠product; no real silicon delay.
     */
    if (u32Idx == 0u && (u16Val & 0x8000u) != 0u) {
        g_aSoftRegs[0] = (u16)(u16Val & (u16)~0x8000u);
        if (g_aSoftRegs[0] == 0u) {
            g_aSoftRegs[0] = (u16)LPHYS_BMCR_ANENABLE;
        }
        g_aSoftRegs[1] = (u16)(LPHYS_BMSR_DEFAULT | LPHYS_BMSR_LSTATUS |
                               LPHYS_BMSR_ANEGCOMPLETE);
    }
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
    lphys_inc(&g_cConnect);
    lphys_soft_objs_once();

    g_pNetdev = pNetdev;
    g_pfnHandler = pfnHandler;
    g_nIface = nIface;
    g_fConnected = 1;
    /* Soft: started-off until phy_start; Soft≠product link state machine. */
    g_fStarted = 0;

    if (g_fLogConnect == 0) {
        g_fLogConnect = 1;
        /* Grep: linux_phy_soft: soft connect */
        kprintf("linux_phy_soft: soft connect bus=%s netdev=%p iface=%d "
                "phy=%p handler=%p calls=%u soft_ne_product=1\n",
                szBusId != NULL ? szBusId : "?",
                g_pNetdev, g_nIface, (void *)g_aSoftPhyDev, g_pfnHandler,
                (unsigned)g_cConnect);
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

    if (g_fLogConnect == 0) {
        g_fLogConnect = 1;
        /* Grep: linux_phy_soft: soft connect */
        kprintf("linux_phy_soft: soft connect direct=1 netdev=%p phy=%p "
                "iface=%d handler=%p calls=%u soft_ne_product=1\n",
                g_pNetdev, pPhy != NULL ? pPhy : (void *)g_aSoftPhyDev,
                g_nIface, g_pfnHandler, (unsigned)g_cConnect);
    }
    /*
     * Linux-shaped: int 0 = success (caller already holds pPhy from
     * mdiobus_get_phy / scan). Soft≠product link state.
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
}

void
phy_start(void *pPhy)
{
    (void)lphys_phy_of(pPhy);
    lphys_inc(&g_cStart);
    g_fStarted = 1;
    /* Soft: ensure BMSR looks link-up after start (post-probe open path). */
    g_aSoftRegs[1] = (u16)(g_aSoftRegs[1] | LPHYS_BMSR_LSTATUS |
                           LPHYS_BMSR_ANEGCOMPLETE);

    if (g_fLogStart == 0) {
        g_fLogStart = 1;
        /* Grep: linux_phy_soft: soft start */
        kprintf("linux_phy_soft: soft start phy=%p connected=%d started=%d "
                "bmsr=0x%x calls=%u soft_ne_product=1\n",
                pPhy != NULL ? pPhy : (void *)g_aSoftPhyDev,
                g_fConnected, g_fStarted, (unsigned)g_aSoftRegs[1],
                (unsigned)g_cStart);
    }
}

void
phy_stop(void *pPhy)
{
    (void)pPhy;
    lphys_inc(&g_cStop);
    g_fStarted = 0;

    if (g_fLogStop == 0) {
        g_fLogStop = 1;
        /* Grep: linux_phy_soft: soft stop */
        kprintf("linux_phy_soft: soft stop phy=%p started=%d disc=%u "
                "calls=%u soft_ne_product=1\n",
                pPhy != NULL ? pPhy : (void *)g_aSoftPhyDev, g_fStarted,
                (unsigned)g_cDisconnect, (unsigned)g_cStop);
    }
}

int
phy_start_aneg(void *pPhy)
{
    (void)pPhy;
    lphys_inc(&g_cStartAneg);
    /* Soft: pretend ANEGCOMPLETE + LSTATUS immediately. Soft≠product. */
    g_aSoftRegs[0] = (u16)(g_aSoftRegs[0] | LPHYS_BMCR_ANENABLE);
    g_aSoftRegs[1] = (u16)(g_aSoftRegs[1] | LPHYS_BMSR_LSTATUS |
                           LPHYS_BMSR_ANEGCOMPLETE);
    if (g_cStartAneg == 1u && g_fLogStart == 0) {
        /* Grep: linux_phy_soft: soft start */
        kprintf("linux_phy_soft: soft start aneg=1 bmsr=0x%x calls=%u "
                "soft_ne_product=1\n",
                (unsigned)g_aSoftRegs[1], (unsigned)g_cStartAneg);
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
    g_fStarted = 1;
    return 0;
}

int
phy_suspend(void *pPhy)
{
    (void)pPhy;
    g_fStarted = 0;
    return 0;
}

void
phy_attached_info(void *pPhy)
{
    (void)pPhy;
}

void
phy_print_status(void *pPhy)
{
    (void)pPhy;
}

int
phy_set_max_speed(void *pPhy, int nSpeed)
{
    (void)pPhy;
    (void)nSpeed;
    return 0;
}

int
genphy_soft_reset(void *pPhy)
{
    (void)pPhy;
    lphys_inc(&g_cReset);
    lphys_soft_objs_once();
    /* Soft reset: reseed BMCR/BMSR; Soft≠product delay. */
    g_aSoftRegs[0] = (u16)LPHYS_BMCR_ANENABLE;
    g_aSoftRegs[1] = (u16)(LPHYS_BMSR_DEFAULT | LPHYS_BMSR_LSTATUS |
                           LPHYS_BMSR_ANEGCOMPLETE);

    if (g_fLogReset == 0) {
        g_fLogReset = 1;
        /* Grep: linux_phy_soft: soft reset */
        kprintf("linux_phy_soft: soft reset genphy=1 phy=%p calls=%u "
                "soft_ne_product=1\n",
                pPhy != NULL ? pPhy : (void *)g_aSoftPhyDev,
                (unsigned)g_cReset);
    }
    return 0;
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
    u16Val = lphys_reg_get(u32Regnum);

    if (g_fLogMdio == 0) {
        g_fLogMdio = 1;
        /* Grep: linux_phy_soft: soft mdiobus */
        kprintf("linux_phy_soft: soft mdiobus read addr=%d reg=0x%x "
                "val=0x%x bus_reg=%d last_wr=0x%x calls=%u soft_ne_product=1\n",
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
    lphys_reg_set(u32Regnum, u16Val);

    if (g_fLogMdio == 0) {
        g_fLogMdio = 1;
        /* Grep: linux_phy_soft: soft mdiobus */
        kprintf("linux_phy_soft: soft mdiobus write addr=%d reg=0x%x "
                "val=0x%x last_reg=0x%x calls=%u soft_ne_product=1\n",
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
    return (void *)g_aSoftPhyDev;
}

void *
mdiobus_alloc_size(int nSizeofPriv)
{
    (void)nSizeofPriv;
    lphys_inc(&g_cAlloc);
    lphys_soft_objs_once();
    if (g_fLogBusReg == 0 && g_cAlloc == 1u) {
        /* First alloc breadcrumb; register log may replace later. */
        /* Grep: linux_phy_soft: soft mdiobus */
        kprintf("linux_phy_soft: soft mdiobus alloc bus=%p priv=%d "
                "calls=%u soft_ne_product=1\n",
                (void *)g_aSoftMiiBus, nSizeofPriv, (unsigned)g_cAlloc);
    }
    return (void *)g_aSoftMiiBus;
}

void *
devm_mdiobus_alloc_size(void *pDev, int nSizeofPriv)
{
    (void)pDev;
    return mdiobus_alloc_size(nSizeofPriv);
}

void
mdiobus_free(void *pBus)
{
    (void)pBus;
    /* Soft static bus never freed; Soft≠product allocator. */
    g_fBusRegistered = 0;
}

int
mdiobus_register(void *pBus)
{
    return __mdiobus_register(pBus, NULL);
}

int
__mdiobus_register(void *pBus, void *pOwner)
{
    (void)pBus;
    (void)pOwner;
    lphys_inc(&g_cRegister);
    lphys_soft_objs_once();
    g_fBusRegistered = 1;

    if (g_fLogBusReg == 0) {
        g_fLogBusReg = 1;
        /* Grep: linux_phy_soft: soft mdiobus */
        kprintf("linux_phy_soft: soft mdiobus register bus=%p calls=%u "
                "soft_ne_product=1\n",
                pBus != NULL ? pBus : (void *)g_aSoftMiiBus,
                (unsigned)g_cRegister);
    }
    return 0;
}

int
__devm_mdiobus_register(void *pDev, void *pBus, void *pOwner)
{
    (void)pDev;
    return __mdiobus_register(pBus, pOwner);
}

void
mdiobus_unregister(void *pBus)
{
    (void)pBus;
    g_fBusRegistered = 0;
}

/* ---- phy register / page helpers ---------------------------------------- */

int
phy_read(void *pPhy, u32 u32Regnum)
{
    u16 u16Val;

    (void)pPhy;
    lphys_inc(&g_cPhyRd);
    lphys_soft_objs_once();
    u16Val = lphys_reg_get(u32Regnum);

    if (g_fLogPhyRd == 0) {
        g_fLogPhyRd = 1;
        /* Grep: linux_phy_soft: soft phy_read */
        kprintf("linux_phy_soft: soft phy_read reg=0x%x val=0x%x page=%d "
                "calls=%u soft_ne_product=1\n",
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
    lphys_reg_set(u32Regnum, u16Val);

    if (g_fLogPhyWr == 0) {
        g_fLogPhyWr = 1;
        /* Grep: linux_phy_soft: soft phy_write */
        kprintf("linux_phy_soft: soft phy_write reg=0x%x val=0x%x page=%d "
                "calls=%u soft_ne_product=1\n",
                (unsigned)u32Regnum, (unsigned)u16Val, g_nPage,
                (unsigned)g_cPhyWr);
    }
    return 0;
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
    (void)nDevad;
    /* Soft: fold MMD into clause-22 shadow (Soft≠product MMD space). */
    return phy_read(pPhy, u32Regnum);
}

int
phy_write_mmd(void *pPhy, int nDevad, u32 u32Regnum, u16 u16Val)
{
    (void)nDevad;
    return phy_write(pPhy, u32Regnum, u16Val);
}

int
phy_modify(void *pPhy, u32 u32Regnum, u16 u16Mask, u16 u16Set)
{
    u16 u16Old;
    u16 u16New;

    lphys_soft_objs_once();
    u16Old = lphys_reg_get(u32Regnum);
    u16New = (u16)((u16Old & (u16)~u16Mask) | (u16Set & u16Mask));
    lphys_reg_set(u32Regnum, u16New);
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
__phy_modify(void *pPhy, u32 u32Regnum, u16 u16Mask, u16 u16Set)
{
    return phy_modify(pPhy, u32Regnum, u16Mask, u16Set);
}

int
phy_select_page(void *pPhy, int nPage)
{
    int nOld;

    (void)pPhy;
    nOld = g_nPage;
    g_nPage = nPage;
    /* Soft: return previous page (Linux-shaped). */
    return nOld;
}

int
phy_restore_page(void *pPhy, int nOldPage, int nRet)
{
    (void)pPhy;
    g_nPage = nOldPage;
    return nRet;
}

/* ---- pause / eee / speed / ioctl ---------------------------------------- */

void
phy_mac_interrupt(void *pPhy)
{
    (void)pPhy;
}

void
phy_get_pause(void *pPhy, u8 *pu8Tx, u8 *pu8Rx)
{
    (void)pPhy;
    if (pu8Tx != NULL) {
        *pu8Tx = 0u;
    }
    if (pu8Rx != NULL) {
        *pu8Rx = 0u;
    }
}

void
phy_set_asym_pause(void *pPhy, int nRx, int nTx)
{
    (void)pPhy;
    (void)nRx;
    (void)nTx;
}

void
phy_support_asym_pause(void *pPhy)
{
    (void)pPhy;
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
    return 0;
}

int
phy_speed_up(void *pPhy)
{
    (void)pPhy;
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
    (void)pCmd;
    return 0;
}

int
phy_ethtool_set_link_ksettings(void *pNetdev, const void *pCmd)
{
    (void)pNetdev;
    (void)pCmd;
    return 0;
}

/* ---- Public init / diagnostics ------------------------------------------ */

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
    g_fConnected = 0;
    g_fStarted = 0;
    g_fBusRegistered = 0;
    g_nIface = 0;
    g_nPhyAddr = 0;
    g_pNetdev = NULL;
    g_pfnHandler = NULL;
    g_fLogConnect = 0;
    g_fLogStart = 0;
    g_fLogStop = 0;
    g_fLogReset = 0;
    g_fLogMdio = 0;
    g_fLogPhyRd = 0;
    g_fLogPhyWr = 0;
    g_fLogBusReg = 0;
    g_fSoftObjsInit = 0u;
    lphys_soft_objs_once();
    g_fReady = 1;

    /*
     * Export soft bodies for F2 ksym / module resolve.
     * Replaces zero-return generic stubs for pointer-critical names.
     * Weak linux_ksym_register: count as skip when F2 not linked yet.
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
    lphys_ksym_one("genphy_soft_reset", (void *)genphy_soft_reset);
    lphys_ksym_one("mdiobus_read", (void *)mdiobus_read);
    lphys_ksym_one("mdiobus_write", (void *)mdiobus_write);
    lphys_ksym_one("__mdiobus_read", (void *)__mdiobus_read);
    lphys_ksym_one("__mdiobus_write", (void *)__mdiobus_write);
    lphys_ksym_one("mdiobus_get_phy", (void *)mdiobus_get_phy);
    lphys_ksym_one("mdiobus_alloc_size", (void *)mdiobus_alloc_size);
    lphys_ksym_one("devm_mdiobus_alloc_size", (void *)devm_mdiobus_alloc_size);
    lphys_ksym_one("mdiobus_free", (void *)mdiobus_free);
    lphys_ksym_one("mdiobus_register", (void *)mdiobus_register);
    lphys_ksym_one("__mdiobus_register", (void *)__mdiobus_register);
    lphys_ksym_one("__devm_mdiobus_register", (void *)__devm_mdiobus_register);
    lphys_ksym_one("mdiobus_unregister", (void *)mdiobus_unregister);
    lphys_ksym_one("phy_read", (void *)phy_read);
    lphys_ksym_one("phy_write", (void *)phy_write);
    lphys_ksym_one("phy_read_paged", (void *)phy_read_paged);
    lphys_ksym_one("phy_write_paged", (void *)phy_write_paged);
    lphys_ksym_one("phy_read_mmd", (void *)phy_read_mmd);
    lphys_ksym_one("phy_write_mmd", (void *)phy_write_mmd);
    lphys_ksym_one("phy_modify", (void *)phy_modify);
    lphys_ksym_one("phy_modify_paged", (void *)phy_modify_paged);
    lphys_ksym_one("__phy_modify", (void *)__phy_modify);
    lphys_ksym_one("phy_mac_interrupt", (void *)phy_mac_interrupt);
    lphys_ksym_one("phy_get_pause", (void *)phy_get_pause);
    lphys_ksym_one("phy_set_asym_pause", (void *)phy_set_asym_pause);
    lphys_ksym_one("phy_support_asym_pause", (void *)phy_support_asym_pause);
    lphys_ksym_one("phy_advertise_eee_all", (void *)phy_advertise_eee_all);
    lphys_ksym_one("phy_speed_down", (void *)phy_speed_down);
    lphys_ksym_one("phy_speed_up", (void *)phy_speed_up);
    lphys_ksym_one("phy_do_ioctl_running", (void *)phy_do_ioctl_running);
    lphys_ksym_one("phy_select_page", (void *)phy_select_page);
    lphys_ksym_one("phy_restore_page", (void *)phy_restore_page);
    lphys_ksym_one("phy_ethtool_nway_reset", (void *)phy_ethtool_nway_reset);
    lphys_ksym_one("phy_ethtool_get_eee", (void *)phy_ethtool_get_eee);
    lphys_ksym_one("phy_ethtool_set_eee", (void *)phy_ethtool_set_eee);
    lphys_ksym_one("phy_ethtool_get_link_ksettings",
                   (void *)phy_ethtool_get_link_ksettings);
    lphys_ksym_one("phy_ethtool_set_link_ksettings",
                   (void *)phy_ethtool_set_link_ksettings);

    /* Grep: linux_phy_soft: soft init PASS n= */
    kprintf("linux_phy_soft: soft init PASS n=%u skip=%u soft_ne_abi=1 "
            "product=OPEN soft_ne_product=1 (post-probe phy/mdio deepen)\n",
            (unsigned)g_cKsymOk, (unsigned)g_cKsymSkip);
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
