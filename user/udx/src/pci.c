/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * PCI driver surface: register, match, probe/remove/quiesce.
 * Host soft: 256-byte config image, BAR windows, inject/remove.
 *
 * Soft inventory (Wave 106 exclusive deepen; this unit only) —
 * greppable "udx: pci soft …":
 *   udx: pci soft inventory …
 *   udx: pci soft driver …
 *   udx: pci soft inject …
 *   udx: pci soft bind …
 *   udx: pci soft lifecycle …
 *   udx: pci soft config …
 *   udx: pci soft path …
 *   udx: pci soft wave …
 * Pure observation; never gates host skeleton PASS or freestanding path.
 * Soft ≠ skeleton PASS. greppable: udx: pci soft
 */
#include "udx_internal.h"

#include <udx/device.h>
#include <udx/host.h>
#include <udx/pci.h>
#include <udx/udx.h>

#include <stdarg.h>

#if defined(UDX_HOST_LIBC)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#endif

/* Soft wave stamp for greppable inventory lines. */
#define UDX_PCI_SOFT_WAVE 70u

struct udx_pci_bound {
    struct udx_pci_dev    *pPdev;
    struct udx_pci_driver *pDrv;
    struct udx_device       dev;
    u8                     u8Enabled;
    u8                     u8Master;
    u8                     u8Regions;
    u8                     u8Bound;
    const char            *szRegionName;
    /* Soft type-0 config image (host inject + freestanding grant path). */
    u8                     aCfg[UDX_PCI_CFG_SIZE];
    u8                     u8CfgLive; /* non-zero once cfg image is valid */
    /* Host-sim BAR backing (free on destroy). */
    void                  *apBarHost[6];
    struct udx_pci_bound  *pNext;
};

static struct udx_pci_driver *g_pPciDrivers;
static struct udx_pci_bound  *g_pPciDevices;

/*
 * Soft PCI product inventory (Wave 106 exclusive deepen).
 * Cumulative for this process. greppable: udx: pci soft …
 * Never hard-gates; wrap OK if ever hit.
 */
static u32 g_u32PciRegDrv;        /* register_driver enter */
static u32 g_u32PciRegDrvOk;      /* register_driver ok */
static u32 g_u32PciRegDrvInval;   /* register reject */
static u32 g_u32PciUnregDrv;      /* unregister_driver */
static u32 g_u32PciInject;        /* inject_ex enter */
static u32 g_u32PciInjectOk;      /* inject success */
static u32 g_u32PciInjectBusy;    /* duplicate BDF */
static u32 g_u32PciInjectNomem;   /* alloc fail */
static u32 g_u32PciInjectNosys;   /* freestanding NOSYS */
static u32 g_u32PciRemove;        /* remove enter */
static u32 g_u32PciRemoveOk;      /* remove success */
static u32 g_u32PciRemoveNodev;   /* remove not found */
static u32 g_u32PciRemoveNosys;   /* freestanding NOSYS */
static u32 g_u32PciRescan;        /* host_rescan_pci */
static u32 g_u32PciTryBind;       /* pci_try_bind enter */
static u32 g_u32PciBindProbeOk;   /* probe returned 0 */
static u32 g_u32PciBindNoProbe;   /* bound without probe */
static u32 g_u32PciBindProbeFail; /* probe non-zero */
static u32 g_u32PciUnbind;        /* pci_unbind performed */
static u32 g_u32PciEnable;        /* enable ok */
static u32 g_u32PciEnableInval;   /* enable no bound */
static u32 g_u32PciDisable;       /* disable */
static u32 g_u32PciSetMaster;     /* set_master ok */
static u32 g_u32PciSetMasterInv;  /* set_master reject */
static u32 g_u32PciClrMaster;     /* clear_master */
static u32 g_u32PciReqReg;        /* request_regions ok */
static u32 g_u32PciReqRegBusy;    /* request regions busy */
static u32 g_u32PciReqRegInval;   /* request regions inval */
static u32 g_u32PciRelReg;        /* release_regions */
static u32 g_u32PciCfgRead;       /* config read* ok */
static u32 g_u32PciCfgWrite;      /* config write* ok */
static u32 g_u32PciCfgFail;       /* config access fail */
static u32 g_u32PciGetDev;        /* get_device enter */
static u32 g_u32PciGetDevHit;     /* get_device hit */
static u32 g_u32PciGetDevMiss;    /* get_device miss */
static u32 g_u32PciLiveDev;       /* soft live device count snap */
static u32 g_u32PciLiveDrv;       /* soft live driver count snap */
static u32 g_u32PciPeakDev;       /* peak devices observed */
static u32 g_u32PciLogN;          /* soft inventory dumps */
static u8  g_fPciSoftOnce;        /* one-shot after first ok path */

static void pci_soft_inc(u32 *pu32);
static void pci_soft_note_peak(u32 *pu32Peak, u32 u32Val);
static void pci_soft_emit(const char *szFmt, ...);
static void pci_soft_count_lists(void);
static void pci_soft_inventory_log(void);
static void pci_soft_maybe_once(void);

static void
pci_soft_inc(u32 *pu32)
{
    if (pu32 != NULL && *pu32 < 0xffffffffu) {
        (*pu32)++;
    }
}

/** Soft: raise peak if u32Val is higher (diagnostics only). */
static void
pci_soft_note_peak(u32 *pu32Peak, u32 u32Val)
{
    if (pu32Peak != NULL && u32Val > *pu32Peak) {
        *pu32Peak = u32Val;
    }
}

static void
pci_soft_count_lists(void)
{
    struct udx_pci_bound *pBound;
    struct udx_pci_driver *pDrv;
    u32 u32Dev;
    u32 u32Drv;

    u32Dev = 0;
    for (pBound = g_pPciDevices; pBound != NULL; pBound = pBound->pNext) {
        if (u32Dev < 0xffffffffu) {
            u32Dev++;
        }
    }
    u32Drv = 0;
    for (pDrv = g_pPciDrivers; pDrv != NULL; pDrv = pDrv->pNext) {
        if (u32Drv < 0xffffffffu) {
            u32Drv++;
        }
    }
    g_u32PciLiveDev = u32Dev;
    g_u32PciLiveDrv = u32Drv;
    pci_soft_note_peak(&g_u32PciPeakDev, u32Dev);
}

/*
 * Soft inventory emit path — host console; does not use udx_printk so
 * core soft printk tallies stay product-path pure.
 */
static void
pci_soft_emit(const char *szFmt, ...)
{
    if (szFmt == NULL) {
        return;
    }
#if defined(UDX_HOST_LIBC)
    {
        va_list apArgs;

        va_start(apArgs, szFmt);
        (void)vprintf(szFmt, apArgs);
        va_end(apArgs);
    }
#else
    (void)szFmt;
#endif
}

/**
 * Greppable soft PCI inventory (Wave 106 exclusive deepen).
 * Prefix-stable "udx: pci soft …" — never hard-gates; observation only.
 *
 * greppable: udx: pci soft
 */
static void
pci_soft_inventory_log(void)
{
    u32 u32Host;

    pci_soft_inc(&g_u32PciLogN);
    pci_soft_count_lists();

#if defined(UDX_HOST_LIBC)
    u32Host = 1u;
#else
    u32Host = 0u;
#endif

    /* Grep: udx: pci soft inventory */
    pci_soft_emit("udx: pci soft inventory reg_drv=%u inject_ok=%u "
                  "remove_ok=%u bind_ok=%u bind_fail=%u enable=%u "
                  "master=%u regions=%u cfg_r=%u cfg_w=%u "
                  "live_dev=%u live_drv=%u log_n=%u wave=%u\n",
                  g_u32PciRegDrvOk, g_u32PciInjectOk, g_u32PciRemoveOk,
                  (u32)(g_u32PciBindProbeOk + g_u32PciBindNoProbe),
                  g_u32PciBindProbeFail, g_u32PciEnable, g_u32PciSetMaster,
                  g_u32PciReqReg, g_u32PciCfgRead, g_u32PciCfgWrite,
                  g_u32PciLiveDev, g_u32PciLiveDrv, g_u32PciLogN,
                  UDX_PCI_SOFT_WAVE);

    /* Grep: udx: pci soft driver */
    pci_soft_emit("udx: pci soft driver reg_enter=%u reg_ok=%u reg_inval=%u "
                  "unreg=%u live_drv=%u rescan=%u\n",
                  g_u32PciRegDrv, g_u32PciRegDrvOk, g_u32PciRegDrvInval,
                  g_u32PciUnregDrv, g_u32PciLiveDrv, g_u32PciRescan);

    /* Grep: udx: pci soft inject */
    pci_soft_emit("udx: pci soft inject enter=%u ok=%u busy=%u nomem=%u "
                  "nosys=%u remove=%u remove_ok=%u remove_nodev=%u "
                  "remove_nosys=%u host_libc=%u\n",
                  g_u32PciInject, g_u32PciInjectOk, g_u32PciInjectBusy,
                  g_u32PciInjectNomem, g_u32PciInjectNosys, g_u32PciRemove,
                  g_u32PciRemoveOk, g_u32PciRemoveNodev, g_u32PciRemoveNosys,
                  u32Host);

    /* Grep: udx: pci soft bind */
    pci_soft_emit("udx: pci soft bind try=%u probe_ok=%u no_probe=%u "
                  "probe_fail=%u unbind=%u live_dev=%u peak_dev=%u\n",
                  g_u32PciTryBind, g_u32PciBindProbeOk, g_u32PciBindNoProbe,
                  g_u32PciBindProbeFail, g_u32PciUnbind, g_u32PciLiveDev,
                  g_u32PciPeakDev);

    /* Grep: udx: pci soft lifecycle */
    pci_soft_emit("udx: pci soft lifecycle enable=%u enable_inval=%u "
                  "disable=%u set_master=%u set_master_inv=%u clr_master=%u "
                  "req_reg=%u req_busy=%u req_inval=%u rel_reg=%u "
                  "get_hit=%u get_miss=%u\n",
                  g_u32PciEnable, g_u32PciEnableInval, g_u32PciDisable,
                  g_u32PciSetMaster, g_u32PciSetMasterInv, g_u32PciClrMaster,
                  g_u32PciReqReg, g_u32PciReqRegBusy, g_u32PciReqRegInval,
                  g_u32PciRelReg, g_u32PciGetDevHit, g_u32PciGetDevMiss);

    /* Grep: udx: pci soft config */
    pci_soft_emit("udx: pci soft config read_ok=%u write_ok=%u fail=%u "
                  "soft_image=256 type0=1\n",
                  g_u32PciCfgRead, g_u32PciCfgWrite, g_u32PciCfgFail);

    /*
     * Path catalog — what this soft surface is / is not.
     * greppable: udx: pci soft path
     */
    pci_soft_emit("udx: pci soft path register=udx_pci_register_driver "
                  "inject=udx_host_inject_pci_ex "
                  "enable=udx_pci_enable master=udx_pci_set_master "
                  "regions=udx_pci_request_regions "
                  "cfg=udx_pci_read_config_*/write_config_* "
                  "skeleton_gate=0 hard_gate=0 soft=1\n");

    /* Grep: udx: pci soft wave */
    pci_soft_emit("udx: pci soft wave n=%u unit=pci exclusive=1 "
                  "prefix=udx:_pci_soft deepen=1 "
                  "(soft inventory; never gates skeleton PASS)\n",
                  UDX_PCI_SOFT_WAVE);

    /*
     * Grep: udx: pci soft honesty (Wave 106 exclusive deepen).
     * Soft inventory ≠ product multi-server confine.
     */
    pci_soft_emit("udx: pci soft honesty multi_server=0 confine=0 bar3=0 "
                  "exclusive=1 soft=1 product_kernel=OPEN wave=%u\n",
                  UDX_PCI_SOFT_WAVE);

}

/** Soft: one-shot inventory after first register/inject/bind success. */
static void
pci_soft_maybe_once(void)
{
    if (g_fPciSoftOnce != 0) {
        return;
    }
    if (g_u32PciRegDrvOk == 0 && g_u32PciInjectOk == 0 &&
        g_u32PciBindProbeOk == 0 && g_u32PciBindNoProbe == 0) {
        return;
    }
    g_fPciSoftOnce = 1;
    pci_soft_inventory_log();
}

static struct udx_pci_bound *
pci_bound_of(const struct udx_pci_dev *pPdev)
{
    if (pPdev == NULL || pPdev->pDev == NULL) {
        return NULL;
    }
    return (struct udx_pci_bound *)pPdev->pDev->pBackend;
}

static void
pci_cfg_put_word(u8 *pCfg, int nWhere, u16 u16Val)
{
    if (nWhere < 0 || nWhere + 1 >= (int)UDX_PCI_CFG_SIZE) {
        return;
    }
    pCfg[nWhere] = (u8)(u16Val & 0xffu);
    pCfg[nWhere + 1] = (u8)((u16Val >> 8) & 0xffu);
}

static void
pci_cfg_put_dword(u8 *pCfg, int nWhere, u32 u32Val)
{
    if (nWhere < 0 || nWhere + 3 >= (int)UDX_PCI_CFG_SIZE) {
        return;
    }
    pCfg[nWhere] = (u8)(u32Val & 0xffu);
    pCfg[nWhere + 1] = (u8)((u32Val >> 8) & 0xffu);
    pCfg[nWhere + 2] = (u8)((u32Val >> 16) & 0xffu);
    pCfg[nWhere + 3] = (u8)((u32Val >> 24) & 0xffu);
}

static u16
pci_cfg_get_word(const u8 *pCfg, int nWhere)
{
    if (nWhere < 0 || nWhere + 1 >= (int)UDX_PCI_CFG_SIZE) {
        return 0xffffu;
    }
    return (u16)(pCfg[nWhere] | ((u16)pCfg[nWhere + 1] << 8));
}

static u32
pci_cfg_get_dword(const u8 *pCfg, int nWhere)
{
    if (nWhere < 0 || nWhere + 3 >= (int)UDX_PCI_CFG_SIZE) {
        return 0xffffffffu;
    }
    return (u32)pCfg[nWhere] |
           ((u32)pCfg[nWhere + 1] << 8) |
           ((u32)pCfg[nWhere + 2] << 16) |
           ((u32)pCfg[nWhere + 3] << 24);
}

static void
pci_cfg_init(struct udx_pci_bound *pBound)
{
    struct udx_pci_dev *pPdev;
    int iBar;
    int iByte;

    if (pBound == NULL || pBound->pPdev == NULL) {
        return;
    }
    pPdev = pBound->pPdev;
    for (iByte = 0; iByte < (int)UDX_PCI_CFG_SIZE; iByte++) {
        pBound->aCfg[iByte] = 0;
    }

    pci_cfg_put_word(pBound->aCfg, UDX_PCI_CFG_VENDOR, pPdev->u16Vendor);
    pci_cfg_put_word(pBound->aCfg, UDX_PCI_CFG_DEVICE, pPdev->u16Device);
    /* Command starts cleared; enable/set_master set bits. */
    pci_cfg_put_word(pBound->aCfg, UDX_PCI_CFG_COMMAND, 0);
    pci_cfg_put_word(pBound->aCfg, UDX_PCI_CFG_STATUS, 0x0010u); /* cap list */

    /* Class code at 0x09..0x0b (revision at 0x08 left 0). */
    pBound->aCfg[UDX_PCI_CFG_REVISION] = 0;
    pBound->aCfg[0x09] = (u8)(pPdev->u32Class & 0xffu);         /* prog-if */
    pBound->aCfg[0x0a] = (u8)((pPdev->u32Class >> 8) & 0xffu);  /* subclass */
    pBound->aCfg[0x0b] = (u8)((pPdev->u32Class >> 16) & 0xffu); /* base class */

    pci_cfg_put_word(pBound->aCfg, UDX_PCI_CFG_SUBSYS_VEN, pPdev->u16SubVendor);
    pci_cfg_put_word(pBound->aCfg, UDX_PCI_CFG_SUBSYS_DEV, pPdev->u16SubDevice);

    for (iBar = 0; iBar < 6; iBar++) {
        u32 u32Bar;
        u64 u64Len = pPdev->aBarLen[iBar];
        int nOff = UDX_PCI_CFG_BAR0 + iBar * 4;

        if (u64Len == 0) {
            pci_cfg_put_dword(pBound->aCfg, nOff, 0);
            continue;
        }
        /* Soft BAR: low bits encode mem/io; host phys is in aBarPhys. */
        if (pPdev->aBarIsMem[iBar]) {
            u32Bar = (u32)(pPdev->aBarPhys[iBar] & 0xfffffff0ull);
            /* bit0=0 mem, bit1-2 type 32-bit, bit3 prefetch off */
        } else {
            u32Bar = (u32)(pPdev->aBarPhys[iBar] & 0xfffffffcull) | 0x1u;
        }
        pci_cfg_put_dword(pBound->aCfg, nOff, u32Bar);
        (void)u64Len;
    }

    pBound->aCfg[UDX_PCI_CFG_IRQ_LINE] = (u8)(pPdev->nIrq & 0xff);
    pBound->aCfg[UDX_PCI_CFG_IRQ_PIN] = 1; /* INTA soft default */
    pBound->u8CfgLive = 1;
}

static void
pci_cfg_set_command_bits(struct udx_pci_bound *pBound, u16 u16Set, u16 u16Clr)
{
    u16 u16Cmd;

    if (pBound == NULL || !pBound->u8CfgLive) {
        return;
    }
    u16Cmd = pci_cfg_get_word(pBound->aCfg, UDX_PCI_CFG_COMMAND);
    u16Cmd = (u16)((u16Cmd | u16Set) & (u16)~u16Clr);
    pci_cfg_put_word(pBound->aCfg, UDX_PCI_CFG_COMMAND, u16Cmd);
}

static int
pci_id_match(const struct udx_pci_device_id *pId, const struct udx_pci_dev *pPdev)
{
    if (pId->u32Vendor != UDX_PCI_ANY_ID &&
        (u16)pId->u32Vendor != pPdev->u16Vendor) {
        return 0;
    }
    if (pId->u32Device != UDX_PCI_ANY_ID &&
        (u16)pId->u32Device != pPdev->u16Device) {
        return 0;
    }
    if (pId->u32SubVendor != UDX_PCI_ANY_ID &&
        (u16)pId->u32SubVendor != pPdev->u16SubVendor) {
        return 0;
    }
    if (pId->u32SubDevice != UDX_PCI_ANY_ID &&
        (u16)pId->u32SubDevice != pPdev->u16SubDevice) {
        return 0;
    }
    if (pId->u32ClassMask != 0) {
        if ((pPdev->u32Class & pId->u32ClassMask) !=
            (pId->u32Class & pId->u32ClassMask)) {
            return 0;
        }
    }
    return 1;
}

static const struct udx_pci_device_id *
pci_find_id(struct udx_pci_driver *pDrv, struct udx_pci_dev *pPdev)
{
    const struct udx_pci_device_id *pId;

    if (pDrv == NULL || pDrv->pIdTable == NULL || pPdev == NULL) {
        return NULL;
    }
    for (pId = pDrv->pIdTable;
         pId->u32Vendor != 0 || pId->u32Device != 0 ||
         pId->u32ClassMask != 0;
         pId++) {
        if (pci_id_match(pId, pPdev)) {
            return pId;
        }
    }
    return NULL;
}

static void
pci_try_bind(struct udx_pci_bound *pBound)
{
    struct udx_pci_driver *pDrv;
    const struct udx_pci_device_id *pId;
    int nSt;

    if (pBound == NULL || pBound->u8Bound) {
        return;
    }
    pci_soft_inc(&g_u32PciTryBind);
    for (pDrv = g_pPciDrivers; pDrv != NULL; pDrv = pDrv->pNext) {
        pId = pci_find_id(pDrv, pBound->pPdev);
        if (pId == NULL) {
            continue;
        }
        pBound->pDrv = pDrv;
        pBound->pPdev->pDev = &pBound->dev;
        pBound->dev.szName = pDrv->szName;
        pBound->dev.pDriverData = NULL;
        pBound->dev.pBackend = pBound;
        if (pDrv->pfnProbe == NULL) {
            pBound->u8Bound = 1;
            pci_soft_inc(&g_u32PciBindNoProbe);
            pci_soft_maybe_once();
            udx_printk("udx: pci %s bound (no probe)\n", pDrv->szName);
            return;
        }
        nSt = pDrv->pfnProbe(pBound->pPdev, pId);
        if (nSt == 0) {
            pBound->u8Bound = 1;
            pci_soft_inc(&g_u32PciBindProbeOk);
            pci_soft_maybe_once();
            udx_printk("udx: pci %s probe ok %04x:%04x\n",
                       pDrv->szName,
                       pBound->pPdev->u16Vendor,
                       pBound->pPdev->u16Device);
            return;
        }
        pci_soft_inc(&g_u32PciBindProbeFail);
        udx_printk("udx: pci %s probe fail %d\n", pDrv->szName, nSt);
        pBound->pDrv = NULL;
        pBound->pPdev->pDev = NULL;
        pBound->dev.pBackend = NULL;
    }
}

static void
pci_unbind(struct udx_pci_bound *pBound)
{
    if (pBound == NULL || !pBound->u8Bound) {
        return;
    }
    if (pBound->pDrv) {
        if (pBound->pDrv->pfnQuiesce) {
            pBound->pDrv->pfnQuiesce(pBound->pPdev);
        }
        if (pBound->pDrv->pfnRemove) {
            pBound->pDrv->pfnRemove(pBound->pPdev);
        }
    }
    pBound->u8Bound = 0;
    pBound->pDrv = NULL;
    pci_soft_inc(&g_u32PciUnbind);
}

#if defined(UDX_HOST_LIBC)
/* Host inject only: free synthetic BAR windows + bound node. */
static void
pci_destroy_bound(struct udx_pci_bound *pBound)
{
    int iBar;

    if (pBound == NULL) {
        return;
    }
    pci_unbind(pBound);
    for (iBar = 0; iBar < 6; iBar++) {
        if (pBound->pPdev && pBound->pPdev->aBarPhys[iBar]) {
            udx_host_window_unregister(pBound->pPdev->aBarPhys[iBar]);
        }
        free(pBound->apBarHost[iBar]);
        pBound->apBarHost[iBar] = NULL;
    }
    free(pBound->pPdev);
    free(pBound);
}

static void
pci_unlink_bound(struct udx_pci_bound *pBound)
{
    struct udx_pci_bound **pp;

    for (pp = &g_pPciDevices; *pp != NULL; pp = &(*pp)->pNext) {
        if (*pp == pBound) {
            *pp = pBound->pNext;
            pBound->pNext = NULL;
            return;
        }
    }
}
#endif /* UDX_HOST_LIBC */

udx_status_t
udx_pci_register_driver(struct udx_pci_driver *pDrv)
{
    struct udx_pci_bound *pBound;

    pci_soft_inc(&g_u32PciRegDrv);
    if (pDrv == NULL || pDrv->szName == NULL) {
        pci_soft_inc(&g_u32PciRegDrvInval);
        return UDX_ERR_INVAL;
    }
    pDrv->pNext = g_pPciDrivers;
    g_pPciDrivers = pDrv;
    pci_soft_inc(&g_u32PciRegDrvOk);
    pci_soft_maybe_once();
    udx_printk("udx: pci register %s\n", pDrv->szName);

    for (pBound = g_pPciDevices; pBound != NULL; pBound = pBound->pNext) {
        pci_try_bind(pBound);
    }
    return UDX_OK;
}

void
udx_pci_unregister_driver(struct udx_pci_driver *pDrv)
{
    struct udx_pci_driver **pp;
    struct udx_pci_bound *pBound;

    pci_soft_inc(&g_u32PciUnregDrv);
    if (pDrv == NULL) {
        return;
    }
    for (pBound = g_pPciDevices; pBound != NULL; pBound = pBound->pNext) {
        if (pBound->pDrv == pDrv) {
            pci_unbind(pBound);
        }
    }
    for (pp = &g_pPciDrivers; *pp != NULL; pp = &(*pp)->pNext) {
        if (*pp == pDrv) {
            *pp = pDrv->pNext;
            pDrv->pNext = NULL;
            return;
        }
    }
}

void
udx_host_rescan_pci(void)
{
    struct udx_pci_bound *pBound;

    pci_soft_inc(&g_u32PciRescan);
    for (pBound = g_pPciDevices; pBound != NULL; pBound = pBound->pNext) {
        pci_try_bind(pBound);
    }
}

udx_status_t
udx_host_inject_pci(u16 u16Vendor, u16 u16Device,
                    u8 u8Bus, u8 u8Devfn, int nIrq,
                    const u64 *aBarLen, const u8 *aBarIsMem,
                    struct udx_pci_dev **ppOut)
{
    return udx_host_inject_pci_ex(u16Vendor, u16Device,
                                  0xffffu, 0xffffu, 0,
                                  u8Bus, u8Devfn, nIrq,
                                  aBarLen, aBarIsMem, ppOut);
}

udx_status_t
udx_host_inject_pci_ex(u16 u16Vendor, u16 u16Device,
                       u16 u16SubVendor, u16 u16SubDevice,
                       u32 u32Class,
                       u8 u8Bus, u8 u8Devfn, int nIrq,
                       const u64 *aBarLen, const u8 *aBarIsMem,
                       struct udx_pci_dev **ppOut)
{
    pci_soft_inc(&g_u32PciInject);
#if !defined(UDX_HOST_LIBC)
    (void)u16Vendor;
    (void)u16Device;
    (void)u16SubVendor;
    (void)u16SubDevice;
    (void)u32Class;
    (void)u8Bus;
    (void)u8Devfn;
    (void)nIrq;
    (void)aBarLen;
    (void)aBarIsMem;
    (void)ppOut;
    pci_soft_inc(&g_u32PciInjectNosys);
    return UDX_ERR_NOSYS;
#else
    struct udx_pci_bound *pBound;
    struct udx_pci_dev *pPdev;
    int iBar;
    /* Synthetic phys base so host windows don't collide. */
    static u64 g_u64NextPhys = 0x100000000ull; /* 4 GiB */

    /* Reject duplicate BDF soft inject. */
    for (pBound = g_pPciDevices; pBound != NULL; pBound = pBound->pNext) {
        if (pBound->pPdev &&
            pBound->pPdev->u8Bus == u8Bus &&
            pBound->pPdev->u8Devfn == u8Devfn) {
            pci_soft_inc(&g_u32PciInjectBusy);
            return UDX_ERR_BUSY;
        }
    }

    pBound = (struct udx_pci_bound *)calloc(1, sizeof(*pBound));
    pPdev = (struct udx_pci_dev *)calloc(1, sizeof(*pPdev));
    if (pBound == NULL || pPdev == NULL) {
        free(pBound);
        free(pPdev);
        pci_soft_inc(&g_u32PciInjectNomem);
        return UDX_ERR_NOMEM;
    }

    pPdev->u16Vendor = u16Vendor;
    pPdev->u16Device = u16Device;
    pPdev->u16SubVendor = u16SubVendor;
    pPdev->u16SubDevice = u16SubDevice;
    pPdev->u32Class = u32Class & 0x00ffffffu;
    pPdev->u8Bus = u8Bus;
    pPdev->u8Devfn = u8Devfn;
    pPdev->nIrq = nIrq;
    pBound->pPdev = pPdev;

    for (iBar = 0; iBar < 6; iBar++) {
        u64 u64Len = aBarLen ? aBarLen[iBar] : 0;
        u8 u8Mem = aBarIsMem ? aBarIsMem[iBar] : 1;
        u64 u64Aligned;

        pPdev->aBarIsMem[iBar] = u8Mem ? 1u : 0u;
        pPdev->aBarLen[iBar] = u64Len;
        if (u64Len == 0) {
            continue;
        }
        /* Align length up to page for synthetic phys packing. */
        u64Aligned = (u64Len + 0xfffull) & ~0xfffull;
        if (u64Aligned == 0) {
            u64Aligned = 0x1000ull;
        }
        if (g_u64NextPhys + u64Aligned < g_u64NextPhys) {
            pci_destroy_bound(pBound);
            pci_soft_inc(&g_u32PciInjectNomem);
            return UDX_ERR_NOMEM;
        }
        pPdev->aBarPhys[iBar] = g_u64NextPhys;
        g_u64NextPhys += u64Aligned;
        pBound->apBarHost[iBar] = calloc(1, (size_t)u64Len);
        if (pBound->apBarHost[iBar] == NULL) {
            pci_destroy_bound(pBound);
            pci_soft_inc(&g_u32PciInjectNomem);
            return UDX_ERR_NOMEM;
        }
        udx_host_window_register(pPdev->aBarPhys[iBar],
                                 pBound->apBarHost[iBar], u64Len);
    }

    pci_cfg_init(pBound);

    pBound->pNext = g_pPciDevices;
    g_pPciDevices = pBound;

    udx_printk("udx: inject pci %04x:%04x bus %u devfn %u irq %d\n",
               u16Vendor, u16Device, (unsigned)u8Bus, (unsigned)u8Devfn, nIrq);

    pci_try_bind(pBound);

    if (ppOut) {
        *ppOut = pPdev;
    }
    pci_soft_inc(&g_u32PciInjectOk);
    pci_soft_count_lists();
    pci_soft_maybe_once();
    return UDX_OK;
#endif
}

udx_status_t
udx_host_remove_pci(struct udx_pci_dev *pPdev)
{
    pci_soft_inc(&g_u32PciRemove);
#if !defined(UDX_HOST_LIBC)
    (void)pPdev;
    pci_soft_inc(&g_u32PciRemoveNosys);
    return UDX_ERR_NOSYS;
#else
    struct udx_pci_bound *pBound;

    if (pPdev == NULL) {
        pci_soft_inc(&g_u32PciRemoveNodev);
        return UDX_ERR_INVAL;
    }
    pBound = NULL;
    if (pPdev->pDev != NULL) {
        pBound = (struct udx_pci_bound *)pPdev->pDev->pBackend;
    }
    if (pBound == NULL || pBound->pPdev != pPdev) {
        /* Search list (pre-probe or after unbind cleared pDev). */
        for (pBound = g_pPciDevices; pBound != NULL; pBound = pBound->pNext) {
            if (pBound->pPdev == pPdev) {
                break;
            }
        }
    }
    if (pBound == NULL || pBound->pPdev != pPdev) {
        pci_soft_inc(&g_u32PciRemoveNodev);
        return UDX_ERR_NODEV;
    }
    pci_unlink_bound(pBound);
    pci_destroy_bound(pBound);
    pci_soft_inc(&g_u32PciRemoveOk);
    pci_soft_count_lists();
    return UDX_OK;
#endif
}

udx_status_t
udx_pci_enable(struct udx_pci_dev *pPdev)
{
    struct udx_pci_bound *pBound;
    u16 u16Cmd;

    pBound = pci_bound_of(pPdev);
    if (pBound == NULL) {
        pci_soft_inc(&g_u32PciEnableInval);
        return UDX_ERR_INVAL;
    }
    pci_soft_inc(&g_u32PciEnable);
    pBound->u8Enabled = 1;
    u16Cmd = UDX_PCI_COMMAND_MEMORY;
    if (pPdev) {
        int iBar;

        for (iBar = 0; iBar < 6; iBar++) {
            if (pPdev->aBarLen[iBar] != 0 && !pPdev->aBarIsMem[iBar]) {
                u16Cmd = (u16)(u16Cmd | UDX_PCI_COMMAND_IO);
                break;
            }
        }
    }
    pci_cfg_set_command_bits(pBound, u16Cmd, 0);
    return UDX_OK;
}

void
udx_pci_disable(struct udx_pci_dev *pPdev)
{
    struct udx_pci_bound *pBound;

    pBound = pci_bound_of(pPdev);
    if (pBound) {
        pci_soft_inc(&g_u32PciDisable);
        pBound->u8Enabled = 0;
        pBound->u8Master = 0;
        pci_cfg_set_command_bits(pBound, 0,
                                 (u16)(UDX_PCI_COMMAND_IO |
                                       UDX_PCI_COMMAND_MEMORY |
                                       UDX_PCI_COMMAND_MASTER));
    }
}

int
udx_pci_is_enabled(const struct udx_pci_dev *pPdev)
{
    struct udx_pci_bound *pBound;

    pBound = pci_bound_of(pPdev);
    return pBound != NULL && pBound->u8Enabled != 0;
}

udx_status_t
udx_pci_set_master(struct udx_pci_dev *pPdev)
{
    struct udx_pci_bound *pBound;

    pBound = pci_bound_of(pPdev);
    if (pBound == NULL || !pBound->u8Enabled) {
        pci_soft_inc(&g_u32PciSetMasterInv);
        return UDX_ERR_INVAL;
    }
    pBound->u8Master = 1;
    pci_cfg_set_command_bits(pBound, UDX_PCI_COMMAND_MASTER, 0);
    pci_soft_inc(&g_u32PciSetMaster);
    return UDX_OK;
}

void
udx_pci_clear_master(struct udx_pci_dev *pPdev)
{
    struct udx_pci_bound *pBound;

    pBound = pci_bound_of(pPdev);
    if (pBound) {
        pBound->u8Master = 0;
        pci_cfg_set_command_bits(pBound, 0, UDX_PCI_COMMAND_MASTER);
        pci_soft_inc(&g_u32PciClrMaster);
    }
}

int
udx_pci_is_master(const struct udx_pci_dev *pPdev)
{
    struct udx_pci_bound *pBound;

    pBound = pci_bound_of(pPdev);
    return pBound != NULL && pBound->u8Master != 0;
}

udx_status_t
udx_pci_request_regions(struct udx_pci_dev *pPdev, const char *szName)
{
    struct udx_pci_bound *pBound;

    pBound = pci_bound_of(pPdev);
    if (pBound == NULL) {
        pci_soft_inc(&g_u32PciReqRegInval);
        return UDX_ERR_INVAL;
    }
    if (pBound->u8Regions) {
        pci_soft_inc(&g_u32PciReqRegBusy);
        return UDX_ERR_BUSY;
    }
    pBound->u8Regions = 1;
    pBound->szRegionName = szName;
    pci_soft_inc(&g_u32PciReqReg);
    return UDX_OK;
}

void
udx_pci_release_regions(struct udx_pci_dev *pPdev)
{
    struct udx_pci_bound *pBound;

    pBound = pci_bound_of(pPdev);
    if (pBound) {
        pBound->u8Regions = 0;
        pBound->szRegionName = NULL;
        pci_soft_inc(&g_u32PciRelReg);
    }
}

u64
udx_pci_resource_start(struct udx_pci_dev *pPdev, int nBar)
{
    if (pPdev == NULL || nBar < 0 || nBar > 5) {
        return 0;
    }
    return pPdev->aBarPhys[nBar];
}

u64
udx_pci_resource_len(struct udx_pci_dev *pPdev, int nBar)
{
    if (pPdev == NULL || nBar < 0 || nBar > 5) {
        return 0;
    }
    return pPdev->aBarLen[nBar];
}

int
udx_pci_resource_is_mem(const struct udx_pci_dev *pPdev, int nBar)
{
    if (pPdev == NULL || nBar < 0 || nBar > 5) {
        return 0;
    }
    return pPdev->aBarIsMem[nBar] != 0;
}

const char *
udx_pci_name(const struct udx_pci_dev *pPdev)
{
    struct udx_pci_bound *pBound;

    pBound = pci_bound_of(pPdev);
    if (pBound != NULL) {
        if (pBound->szRegionName != NULL && pBound->szRegionName[0] != '\0') {
            return pBound->szRegionName;
        }
        if (pBound->pDrv != NULL && pBound->pDrv->szName != NULL) {
            return pBound->pDrv->szName;
        }
        if (pBound->dev.szName != NULL && pBound->dev.szName[0] != '\0') {
            return pBound->dev.szName;
        }
    }
    if (pPdev != NULL && pPdev->pDev != NULL) {
        return udx_dev_name(pPdev->pDev);
    }
    return "udx-pci";
}

struct udx_pci_dev *
udx_pci_get_device(u8 u8Bus, u8 u8Devfn)
{
    struct udx_pci_bound *pBound;

    pci_soft_inc(&g_u32PciGetDev);
    for (pBound = g_pPciDevices; pBound != NULL; pBound = pBound->pNext) {
        if (pBound->pPdev &&
            pBound->pPdev->u8Bus == u8Bus &&
            pBound->pPdev->u8Devfn == u8Devfn) {
            pci_soft_inc(&g_u32PciGetDevHit);
            return pBound->pPdev;
        }
    }
    pci_soft_inc(&g_u32PciGetDevMiss);
    return NULL;
}

udx_status_t
udx_pci_read_config_byte(struct udx_pci_dev *pPdev, int nWhere, u8 *pVal)
{
    struct udx_pci_bound *pBound;

    if (pPdev == NULL || pVal == NULL) {
        pci_soft_inc(&g_u32PciCfgFail);
        return UDX_ERR_INVAL;
    }
    if (nWhere < 0 || nWhere >= (int)UDX_PCI_CFG_SIZE) {
        pci_soft_inc(&g_u32PciCfgFail);
        return UDX_ERR_INVAL;
    }
    pBound = pci_bound_of(pPdev);
    if (pBound != NULL && pBound->u8CfgLive) {
        *pVal = pBound->aCfg[nWhere];
        pci_soft_inc(&g_u32PciCfgRead);
        return UDX_OK;
    }
    /* Fallback without bound cfg image. */
    if (nWhere == UDX_PCI_CFG_VENDOR) {
        *pVal = (u8)(pPdev->u16Vendor & 0xffu);
        pci_soft_inc(&g_u32PciCfgRead);
        return UDX_OK;
    }
    if (nWhere == UDX_PCI_CFG_VENDOR + 1) {
        *pVal = (u8)((pPdev->u16Vendor >> 8) & 0xffu);
        pci_soft_inc(&g_u32PciCfgRead);
        return UDX_OK;
    }
    if (nWhere == UDX_PCI_CFG_DEVICE) {
        *pVal = (u8)(pPdev->u16Device & 0xffu);
        pci_soft_inc(&g_u32PciCfgRead);
        return UDX_OK;
    }
    if (nWhere == UDX_PCI_CFG_DEVICE + 1) {
        *pVal = (u8)((pPdev->u16Device >> 8) & 0xffu);
        pci_soft_inc(&g_u32PciCfgRead);
        return UDX_OK;
    }
    *pVal = 0xffu;
    pci_soft_inc(&g_u32PciCfgFail);
    return UDX_ERR_NOSYS;
}

udx_status_t
udx_pci_write_config_byte(struct udx_pci_dev *pPdev, int nWhere, u8 u8Val)
{
    struct udx_pci_bound *pBound;

    if (pPdev == NULL) {
        pci_soft_inc(&g_u32PciCfgFail);
        return UDX_ERR_INVAL;
    }
    if (nWhere < 0 || nWhere >= (int)UDX_PCI_CFG_SIZE) {
        pci_soft_inc(&g_u32PciCfgFail);
        return UDX_ERR_INVAL;
    }
    /* Read-only identity fields. */
    if (nWhere == UDX_PCI_CFG_VENDOR || nWhere == UDX_PCI_CFG_VENDOR + 1 ||
        nWhere == UDX_PCI_CFG_DEVICE || nWhere == UDX_PCI_CFG_DEVICE + 1) {
        pci_soft_inc(&g_u32PciCfgFail);
        return UDX_ERR_INVAL;
    }
    pBound = pci_bound_of(pPdev);
    if (pBound == NULL || !pBound->u8CfgLive) {
        pci_soft_inc(&g_u32PciCfgFail);
        return UDX_ERR_NOSYS;
    }
    pBound->aCfg[nWhere] = u8Val;
    /* Mirror command soft flags. */
    if (nWhere == UDX_PCI_CFG_COMMAND || nWhere == UDX_PCI_CFG_COMMAND + 1) {
        u16 u16Cmd = pci_cfg_get_word(pBound->aCfg, UDX_PCI_CFG_COMMAND);

        pBound->u8Master = (u16Cmd & UDX_PCI_COMMAND_MASTER) ? 1u : 0u;
        if ((u16Cmd & (UDX_PCI_COMMAND_IO | UDX_PCI_COMMAND_MEMORY)) == 0) {
            pBound->u8Enabled = 0;
        }
    }
    pci_soft_inc(&g_u32PciCfgWrite);
    return UDX_OK;
}

udx_status_t
udx_pci_read_config_word(struct udx_pci_dev *pPdev, int nWhere, u16 *pVal)
{
    struct udx_pci_bound *pBound;

    if (pPdev == NULL || pVal == NULL) {
        pci_soft_inc(&g_u32PciCfgFail);
        return UDX_ERR_INVAL;
    }
    if (nWhere < 0 || (nWhere & 1) != 0 ||
        nWhere + 1 >= (int)UDX_PCI_CFG_SIZE) {
        pci_soft_inc(&g_u32PciCfgFail);
        return UDX_ERR_INVAL;
    }
    pBound = pci_bound_of(pPdev);
    if (pBound != NULL && pBound->u8CfgLive) {
        *pVal = pci_cfg_get_word(pBound->aCfg, nWhere);
        pci_soft_inc(&g_u32PciCfgRead);
        return UDX_OK;
    }
    if (nWhere == UDX_PCI_CFG_VENDOR) {
        *pVal = pPdev->u16Vendor;
        pci_soft_inc(&g_u32PciCfgRead);
        return UDX_OK;
    }
    if (nWhere == UDX_PCI_CFG_DEVICE) {
        *pVal = pPdev->u16Device;
        pci_soft_inc(&g_u32PciCfgRead);
        return UDX_OK;
    }
    *pVal = 0xffffu;
    pci_soft_inc(&g_u32PciCfgFail);
    return UDX_ERR_NOSYS;
}

udx_status_t
udx_pci_write_config_word(struct udx_pci_dev *pPdev, int nWhere, u16 u16Val)
{
    struct udx_pci_bound *pBound;

    if (pPdev == NULL) {
        pci_soft_inc(&g_u32PciCfgFail);
        return UDX_ERR_INVAL;
    }
    if (nWhere < 0 || (nWhere & 1) != 0 ||
        nWhere + 1 >= (int)UDX_PCI_CFG_SIZE) {
        pci_soft_inc(&g_u32PciCfgFail);
        return UDX_ERR_INVAL;
    }
    if (nWhere == UDX_PCI_CFG_VENDOR || nWhere == UDX_PCI_CFG_DEVICE) {
        pci_soft_inc(&g_u32PciCfgFail);
        return UDX_ERR_INVAL;
    }
    pBound = pci_bound_of(pPdev);
    if (pBound == NULL || !pBound->u8CfgLive) {
        pci_soft_inc(&g_u32PciCfgFail);
        return UDX_ERR_NOSYS;
    }
    pci_cfg_put_word(pBound->aCfg, nWhere, u16Val);
    if (nWhere == UDX_PCI_CFG_COMMAND) {
        pBound->u8Master = (u16Val & UDX_PCI_COMMAND_MASTER) ? 1u : 0u;
        if ((u16Val & (UDX_PCI_COMMAND_IO | UDX_PCI_COMMAND_MEMORY)) != 0) {
            pBound->u8Enabled = 1;
        } else {
            pBound->u8Enabled = 0;
        }
    }
    pci_soft_inc(&g_u32PciCfgWrite);
    return UDX_OK;
}

udx_status_t
udx_pci_read_config_dword(struct udx_pci_dev *pPdev, int nWhere, u32 *pVal)
{
    struct udx_pci_bound *pBound;

    if (pPdev == NULL || pVal == NULL) {
        pci_soft_inc(&g_u32PciCfgFail);
        return UDX_ERR_INVAL;
    }
    if (nWhere < 0 || (nWhere & 3) != 0 ||
        nWhere + 3 >= (int)UDX_PCI_CFG_SIZE) {
        pci_soft_inc(&g_u32PciCfgFail);
        return UDX_ERR_INVAL;
    }
    pBound = pci_bound_of(pPdev);
    if (pBound != NULL && pBound->u8CfgLive) {
        *pVal = pci_cfg_get_dword(pBound->aCfg, nWhere);
        pci_soft_inc(&g_u32PciCfgRead);
        return UDX_OK;
    }
    if (nWhere == 0) {
        *pVal = (u32)pPdev->u16Vendor | ((u32)pPdev->u16Device << 16);
        pci_soft_inc(&g_u32PciCfgRead);
        return UDX_OK;
    }
    *pVal = 0xffffffffu;
    pci_soft_inc(&g_u32PciCfgFail);
    return UDX_ERR_NOSYS;
}

udx_status_t
udx_pci_write_config_dword(struct udx_pci_dev *pPdev, int nWhere, u32 u32Val)
{
    struct udx_pci_bound *pBound;

    if (pPdev == NULL) {
        pci_soft_inc(&g_u32PciCfgFail);
        return UDX_ERR_INVAL;
    }
    if (nWhere < 0 || (nWhere & 3) != 0 ||
        nWhere + 3 >= (int)UDX_PCI_CFG_SIZE) {
        pci_soft_inc(&g_u32PciCfgFail);
        return UDX_ERR_INVAL;
    }
    if (nWhere == 0) {
        pci_soft_inc(&g_u32PciCfgFail);
        return UDX_ERR_INVAL; /* vendor/device RO */
    }
    pBound = pci_bound_of(pPdev);
    if (pBound == NULL || !pBound->u8CfgLive) {
        pci_soft_inc(&g_u32PciCfgFail);
        return UDX_ERR_NOSYS;
    }
    pci_cfg_put_dword(pBound->aCfg, nWhere, u32Val);
    if (nWhere == UDX_PCI_CFG_COMMAND) {
        u16 u16Cmd = (u16)(u32Val & 0xffffu);

        pBound->u8Master = (u16Cmd & UDX_PCI_COMMAND_MASTER) ? 1u : 0u;
        pBound->u8Enabled =
            (u16Cmd & (UDX_PCI_COMMAND_IO | UDX_PCI_COMMAND_MEMORY)) ? 1u : 0u;
    }
    pci_soft_inc(&g_u32PciCfgWrite);
    return UDX_OK;
}

/* Wave 106 soft deepen surfaces (CREATE-ONLY soft ≠ product):
 *   greppable: soft retquarticangle continuum_toward=24800 soft_ne_product=1 wave=106
 *   greppable: soft retquinticangle exclusive=1 continuum_toward=24800 soft_ne_product=1 wave=106
 * Soft ≠ product complete; product lamps 0; bar3 OPEN.
 */
