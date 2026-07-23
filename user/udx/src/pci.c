/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * PCI driver surface: register, match, probe/remove/quiesce.
 * Host soft: 256-byte config image, BAR windows, inject/remove.
 */
#include "udx_internal.h"

#include <udx/device.h>
#include <udx/host.h>
#include <udx/pci.h>
#include <udx/udx.h>

#if defined(UDX_HOST_LIBC)
#include <stdlib.h>
#include <string.h>
#endif

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
            udx_printk("udx: pci %s bound (no probe)\n", pDrv->szName);
            return;
        }
        nSt = pDrv->pfnProbe(pBound->pPdev, pId);
        if (nSt == 0) {
            pBound->u8Bound = 1;
            udx_printk("udx: pci %s probe ok %04x:%04x\n",
                       pDrv->szName,
                       pBound->pPdev->u16Vendor,
                       pBound->pPdev->u16Device);
            return;
        }
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

    if (pDrv == NULL || pDrv->szName == NULL) {
        return UDX_ERR_INVAL;
    }
    pDrv->pNext = g_pPciDrivers;
    g_pPciDrivers = pDrv;
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
            return UDX_ERR_BUSY;
        }
    }

    pBound = (struct udx_pci_bound *)calloc(1, sizeof(*pBound));
    pPdev = (struct udx_pci_dev *)calloc(1, sizeof(*pPdev));
    if (pBound == NULL || pPdev == NULL) {
        free(pBound);
        free(pPdev);
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
            return UDX_ERR_NOMEM;
        }
        pPdev->aBarPhys[iBar] = g_u64NextPhys;
        g_u64NextPhys += u64Aligned;
        pBound->apBarHost[iBar] = calloc(1, (size_t)u64Len);
        if (pBound->apBarHost[iBar] == NULL) {
            pci_destroy_bound(pBound);
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
    return UDX_OK;
#endif
}

udx_status_t
udx_host_remove_pci(struct udx_pci_dev *pPdev)
{
#if !defined(UDX_HOST_LIBC)
    (void)pPdev;
    return UDX_ERR_NOSYS;
#else
    struct udx_pci_bound *pBound;

    if (pPdev == NULL) {
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
        return UDX_ERR_NODEV;
    }
    pci_unlink_bound(pBound);
    pci_destroy_bound(pBound);
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
        return UDX_ERR_INVAL;
    }
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
        return UDX_ERR_INVAL;
    }
    pBound->u8Master = 1;
    pci_cfg_set_command_bits(pBound, UDX_PCI_COMMAND_MASTER, 0);
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
        return UDX_ERR_INVAL;
    }
    if (pBound->u8Regions) {
        return UDX_ERR_BUSY;
    }
    pBound->u8Regions = 1;
    pBound->szRegionName = szName;
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

    for (pBound = g_pPciDevices; pBound != NULL; pBound = pBound->pNext) {
        if (pBound->pPdev &&
            pBound->pPdev->u8Bus == u8Bus &&
            pBound->pPdev->u8Devfn == u8Devfn) {
            return pBound->pPdev;
        }
    }
    return NULL;
}

udx_status_t
udx_pci_read_config_byte(struct udx_pci_dev *pPdev, int nWhere, u8 *pVal)
{
    struct udx_pci_bound *pBound;

    if (pPdev == NULL || pVal == NULL) {
        return UDX_ERR_INVAL;
    }
    if (nWhere < 0 || nWhere >= (int)UDX_PCI_CFG_SIZE) {
        return UDX_ERR_INVAL;
    }
    pBound = pci_bound_of(pPdev);
    if (pBound != NULL && pBound->u8CfgLive) {
        *pVal = pBound->aCfg[nWhere];
        return UDX_OK;
    }
    /* Fallback without bound cfg image. */
    if (nWhere == UDX_PCI_CFG_VENDOR) {
        *pVal = (u8)(pPdev->u16Vendor & 0xffu);
        return UDX_OK;
    }
    if (nWhere == UDX_PCI_CFG_VENDOR + 1) {
        *pVal = (u8)((pPdev->u16Vendor >> 8) & 0xffu);
        return UDX_OK;
    }
    if (nWhere == UDX_PCI_CFG_DEVICE) {
        *pVal = (u8)(pPdev->u16Device & 0xffu);
        return UDX_OK;
    }
    if (nWhere == UDX_PCI_CFG_DEVICE + 1) {
        *pVal = (u8)((pPdev->u16Device >> 8) & 0xffu);
        return UDX_OK;
    }
    *pVal = 0xffu;
    return UDX_ERR_NOSYS;
}

udx_status_t
udx_pci_write_config_byte(struct udx_pci_dev *pPdev, int nWhere, u8 u8Val)
{
    struct udx_pci_bound *pBound;

    if (pPdev == NULL) {
        return UDX_ERR_INVAL;
    }
    if (nWhere < 0 || nWhere >= (int)UDX_PCI_CFG_SIZE) {
        return UDX_ERR_INVAL;
    }
    /* Read-only identity fields. */
    if (nWhere == UDX_PCI_CFG_VENDOR || nWhere == UDX_PCI_CFG_VENDOR + 1 ||
        nWhere == UDX_PCI_CFG_DEVICE || nWhere == UDX_PCI_CFG_DEVICE + 1) {
        return UDX_ERR_INVAL;
    }
    pBound = pci_bound_of(pPdev);
    if (pBound == NULL || !pBound->u8CfgLive) {
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
    return UDX_OK;
}

udx_status_t
udx_pci_read_config_word(struct udx_pci_dev *pPdev, int nWhere, u16 *pVal)
{
    struct udx_pci_bound *pBound;

    if (pPdev == NULL || pVal == NULL) {
        return UDX_ERR_INVAL;
    }
    if (nWhere < 0 || (nWhere & 1) != 0 ||
        nWhere + 1 >= (int)UDX_PCI_CFG_SIZE) {
        return UDX_ERR_INVAL;
    }
    pBound = pci_bound_of(pPdev);
    if (pBound != NULL && pBound->u8CfgLive) {
        *pVal = pci_cfg_get_word(pBound->aCfg, nWhere);
        return UDX_OK;
    }
    if (nWhere == UDX_PCI_CFG_VENDOR) {
        *pVal = pPdev->u16Vendor;
        return UDX_OK;
    }
    if (nWhere == UDX_PCI_CFG_DEVICE) {
        *pVal = pPdev->u16Device;
        return UDX_OK;
    }
    *pVal = 0xffffu;
    return UDX_ERR_NOSYS;
}

udx_status_t
udx_pci_write_config_word(struct udx_pci_dev *pPdev, int nWhere, u16 u16Val)
{
    struct udx_pci_bound *pBound;

    if (pPdev == NULL) {
        return UDX_ERR_INVAL;
    }
    if (nWhere < 0 || (nWhere & 1) != 0 ||
        nWhere + 1 >= (int)UDX_PCI_CFG_SIZE) {
        return UDX_ERR_INVAL;
    }
    if (nWhere == UDX_PCI_CFG_VENDOR || nWhere == UDX_PCI_CFG_DEVICE) {
        return UDX_ERR_INVAL;
    }
    pBound = pci_bound_of(pPdev);
    if (pBound == NULL || !pBound->u8CfgLive) {
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
    return UDX_OK;
}

udx_status_t
udx_pci_read_config_dword(struct udx_pci_dev *pPdev, int nWhere, u32 *pVal)
{
    struct udx_pci_bound *pBound;

    if (pPdev == NULL || pVal == NULL) {
        return UDX_ERR_INVAL;
    }
    if (nWhere < 0 || (nWhere & 3) != 0 ||
        nWhere + 3 >= (int)UDX_PCI_CFG_SIZE) {
        return UDX_ERR_INVAL;
    }
    pBound = pci_bound_of(pPdev);
    if (pBound != NULL && pBound->u8CfgLive) {
        *pVal = pci_cfg_get_dword(pBound->aCfg, nWhere);
        return UDX_OK;
    }
    if (nWhere == 0) {
        *pVal = (u32)pPdev->u16Vendor | ((u32)pPdev->u16Device << 16);
        return UDX_OK;
    }
    *pVal = 0xffffffffu;
    return UDX_ERR_NOSYS;
}

udx_status_t
udx_pci_write_config_dword(struct udx_pci_dev *pPdev, int nWhere, u32 u32Val)
{
    struct udx_pci_bound *pBound;

    if (pPdev == NULL) {
        return UDX_ERR_INVAL;
    }
    if (nWhere < 0 || (nWhere & 3) != 0 ||
        nWhere + 3 >= (int)UDX_PCI_CFG_SIZE) {
        return UDX_ERR_INVAL;
    }
    if (nWhere == 0) {
        return UDX_ERR_INVAL; /* vendor/device RO */
    }
    pBound = pci_bound_of(pPdev);
    if (pBound == NULL || !pBound->u8CfgLive) {
        return UDX_ERR_NOSYS;
    }
    pci_cfg_put_dword(pBound->aCfg, nWhere, u32Val);
    if (nWhere == UDX_PCI_CFG_COMMAND) {
        u16 u16Cmd = (u16)(u32Val & 0xffffu);

        pBound->u8Master = (u16Cmd & UDX_PCI_COMMAND_MASTER) ? 1u : 0u;
        pBound->u8Enabled =
            (u16Cmd & (UDX_PCI_COMMAND_IO | UDX_PCI_COMMAND_MEMORY)) ? 1u : 0u;
    }
    return UDX_OK;
}
