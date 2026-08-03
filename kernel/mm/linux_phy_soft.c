/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Soft Linux PHY / MDIO seed.
 * Pure C11 freestanding. Dual MIT OR Apache-2.0. No GPL / no Linux .ko source.
 *
 * Soft only:
 *   - linux_phy_soft_init: ready lamp + linux_ksym_register of soft bodies
 *   - phy_* / mdiobus_* no-op / success paths so r8169 init soft-completes
 *   - non-NULL soft static objects for alloc / connect / get_phy
 *
 * Soft ≠ ABI-stable: struct layouts under void* are incomplete soft shapes.
 * Soft ≠ product: no real MDIO bus, no copper link, no .ko PHY driver load.
 *
 * Greppable markers (keep stable):
 *   linux_phy_soft: soft init PASS n=
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

/* ---- Soft state --------------------------------------------------------- */

static int g_fReady;
static u32 g_cInitCalls;
static u32 g_cKsymOk;
static u32 g_cKsymSkip;

/* Soft static objects returned by pointer APIs (never free; zeroed once). */
static u8 g_aSoftPhyDev[LINUX_PHY_SOFT_PHYDEV_CB];
static u8 g_aSoftMiiBus[LINUX_PHY_SOFT_MIIBUS_CB];
static u8 g_fSoftObjsInit;

/* Call counters (soft diagnostics; not product metrics). */
static u32 g_cConnect;
static u32 g_cDisconnect;
static u32 g_cStart;
static u32 g_cStop;
static u32 g_cMdioRd;
static u32 g_cMdioWr;
static u32 g_cAlloc;
static u32 g_cRegister;

/* ---- Helpers ------------------------------------------------------------ */

static void
lphys_ksym_one(const char *szName, void *pFn)
{
    int nSt;

    if (szName == NULL || pFn == NULL) {
        if (g_cKsymSkip < 0xffffffffu) {
            g_cKsymSkip++;
        }
        return;
    }
    if (linux_ksym_register == NULL) {
        if (g_cKsymSkip < 0xffffffffu) {
            g_cKsymSkip++;
        }
        return;
    }
    nSt = linux_ksym_register(szName, pFn);
    if (nSt == 0) {
        if (g_cKsymOk < 0xffffffffu) {
            g_cKsymOk++;
        }
    } else {
        if (g_cKsymSkip < 0xffffffffu) {
            g_cKsymSkip++;
        }
    }
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
    g_fSoftObjsInit = 1u;
}

/* ---- phy connect / lifecycle -------------------------------------------- */

int
phy_connect_direct(void *pNetdev, void *pPhy, void *pfnHandler, int nIface)
{
    (void)pNetdev;
    (void)pfnHandler;
    (void)nIface;

    if (g_cConnect < 0xffffffffu) {
        g_cConnect++;
    }
    lphys_soft_objs_once();
    /*
     * Linux-shaped: int 0 = success (caller already holds pPhy from
     * mdiobus_get_phy / scan). Soft: ensure soft phy blob exists; if caller
     * passed NULL, leave g_aSoftPhyDev as the soft stand-in for later
     * phy_start/stop paths that may only have been given a soft get_phy.
     * Soft≠product link state.
     */
    (void)pPhy;
    return 0;
}

void
phy_disconnect(void *pPhy)
{
    (void)pPhy;
    if (g_cDisconnect < 0xffffffffu) {
        g_cDisconnect++;
    }
}

void
phy_start(void *pPhy)
{
    (void)pPhy;
    if (g_cStart < 0xffffffffu) {
        g_cStart++;
    }
}

void
phy_stop(void *pPhy)
{
    (void)pPhy;
    if (g_cStop < 0xffffffffu) {
        g_cStop++;
    }
}

int
phy_init_hw(void *pPhy)
{
    (void)pPhy;
    return 0;
}

int
phy_resume(void *pPhy)
{
    (void)pPhy;
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
    return 0;
}

/* ---- MDIO bus ----------------------------------------------------------- */

int
mdiobus_read(void *pBus, int nAddr, u32 u32Regnum)
{
    (void)pBus;
    (void)nAddr;
    (void)u32Regnum;
    if (g_cMdioRd < 0xffffffffu) {
        g_cMdioRd++;
    }
    /* Soft: zero data (success-shaped; not a real PHY register dump). */
    return 0;
}

int
mdiobus_write(void *pBus, int nAddr, u32 u32Regnum, u16 u16Val)
{
    (void)pBus;
    (void)nAddr;
    (void)u32Regnum;
    (void)u16Val;
    if (g_cMdioWr < 0xffffffffu) {
        g_cMdioWr++;
    }
    return 0;
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
    (void)nAddr;
    lphys_soft_objs_once();
    return (void *)g_aSoftPhyDev;
}

void *
devm_mdiobus_alloc_size(void *pDev, int nSizeofPriv)
{
    (void)pDev;
    (void)nSizeofPriv;
    if (g_cAlloc < 0xffffffffu) {
        g_cAlloc++;
    }
    lphys_soft_objs_once();
    return (void *)g_aSoftMiiBus;
}

int
__devm_mdiobus_register(void *pDev, void *pBus, void *pOwner)
{
    (void)pDev;
    (void)pBus;
    (void)pOwner;
    if (g_cRegister < 0xffffffffu) {
        g_cRegister++;
    }
    return 0;
}

/* ---- phy register / page helpers ---------------------------------------- */

int
phy_read_paged(void *pPhy, int nPage, u32 u32Regnum)
{
    (void)pPhy;
    (void)nPage;
    (void)u32Regnum;
    return 0;
}

int
phy_modify(void *pPhy, u32 u32Regnum, u16 u16Mask, u16 u16Set)
{
    (void)pPhy;
    (void)u32Regnum;
    (void)u16Mask;
    (void)u16Set;
    return 0;
}

int
phy_modify_paged(void *pPhy, int nPage, u32 u32Regnum, u16 u16Mask, u16 u16Set)
{
    (void)pPhy;
    (void)nPage;
    (void)u32Regnum;
    (void)u16Mask;
    (void)u16Set;
    return 0;
}

int
__phy_modify(void *pPhy, u32 u32Regnum, u16 u16Mask, u16 u16Set)
{
    return phy_modify(pPhy, u32Regnum, u16Mask, u16Set);
}

int
phy_select_page(void *pPhy, int nPage)
{
    (void)pPhy;
    /* Soft: return "old page" 0. */
    (void)nPage;
    return 0;
}

int
phy_restore_page(void *pPhy, int nOldPage, int nRet)
{
    (void)pPhy;
    (void)nOldPage;
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
    return 0;
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
    g_cMdioRd = 0u;
    g_cMdioWr = 0u;
    g_cAlloc = 0u;
    g_cRegister = 0u;
    lphys_soft_objs_once();
    g_fReady = 1;

    /*
     * Export soft bodies for F2 ksym / module resolve.
     * Replaces zero-return generic stubs for pointer-critical names.
     * Weak linux_ksym_register: count as skip when F2 not linked yet.
     */
    lphys_ksym_one("phy_connect_direct", (void *)phy_connect_direct);
    lphys_ksym_one("phy_disconnect", (void *)phy_disconnect);
    lphys_ksym_one("phy_start", (void *)phy_start);
    lphys_ksym_one("phy_stop", (void *)phy_stop);
    lphys_ksym_one("phy_init_hw", (void *)phy_init_hw);
    lphys_ksym_one("phy_resume", (void *)phy_resume);
    lphys_ksym_one("phy_attached_info", (void *)phy_attached_info);
    lphys_ksym_one("phy_print_status", (void *)phy_print_status);
    lphys_ksym_one("phy_set_max_speed", (void *)phy_set_max_speed);
    lphys_ksym_one("genphy_soft_reset", (void *)genphy_soft_reset);
    lphys_ksym_one("mdiobus_read", (void *)mdiobus_read);
    lphys_ksym_one("mdiobus_write", (void *)mdiobus_write);
    lphys_ksym_one("__mdiobus_write", (void *)__mdiobus_write);
    lphys_ksym_one("mdiobus_get_phy", (void *)mdiobus_get_phy);
    lphys_ksym_one("devm_mdiobus_alloc_size", (void *)devm_mdiobus_alloc_size);
    lphys_ksym_one("__devm_mdiobus_register", (void *)__devm_mdiobus_register);
    lphys_ksym_one("phy_read_paged", (void *)phy_read_paged);
    lphys_ksym_one("phy_modify", (void *)phy_modify);
    lphys_ksym_one("phy_modify_paged", (void *)phy_modify_paged);
    lphys_ksym_one("__phy_modify", (void *)__phy_modify);
    lphys_ksym_one("phy_mac_interrupt", (void *)phy_mac_interrupt);
    lphys_ksym_one("phy_get_pause", (void *)phy_get_pause);
    lphys_ksym_one("phy_set_asym_pause", (void *)phy_set_asym_pause);
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
            "product=OPEN\n",
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
