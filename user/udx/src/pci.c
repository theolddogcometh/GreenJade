/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * PCI driver surface: register, match, probe/remove/quiesce.
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
    /* Host-sim BAR backing (free on destroy). */
    void                  *apBarHost[6];
    struct udx_pci_bound  *pNext;
};

static struct udx_pci_driver *g_pPciDrivers;
static struct udx_pci_bound  *g_pPciDevices;

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
    /* Subsystem / class match can be extended later. */
    (void)pId->u32SubVendor;
    (void)pId->u32SubDevice;
    (void)pId->u32Class;
    (void)pId->u32ClassMask;
    return 1;
}

static const struct udx_pci_device_id *
pci_find_id(struct udx_pci_driver *pDrv, struct udx_pci_dev *pPdev)
{
    const struct udx_pci_device_id *pId;

    if (pDrv == NULL || pDrv->pIdTable == NULL || pPdev == NULL) {
        return NULL;
    }
    for (pId = pDrv->pIdTable; pId->u32Vendor != 0 || pId->u32Device != 0; pId++) {
        if (pci_id_match(pId, pPdev)) {
            return pId;
        }
    }
    /* Terminator-only table: also allow explicit ANY matches before {0,}. */
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
#if !defined(UDX_HOST_LIBC)
    (void)u16Vendor;
    (void)u16Device;
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

    pBound = (struct udx_pci_bound *)calloc(1, sizeof(*pBound));
    pPdev = (struct udx_pci_dev *)calloc(1, sizeof(*pPdev));
    if (pBound == NULL || pPdev == NULL) {
        free(pBound);
        free(pPdev);
        return UDX_ERR_NOMEM;
    }

    pPdev->u16Vendor = u16Vendor;
    pPdev->u16Device = u16Device;
    pPdev->u8Bus = u8Bus;
    pPdev->u8Devfn = u8Devfn;
    pPdev->nIrq = nIrq;
    pBound->pPdev = pPdev;

    for (iBar = 0; iBar < 6; iBar++) {
        u64 u64Len = aBarLen ? aBarLen[iBar] : 0;
        u8 u8Mem = aBarIsMem ? aBarIsMem[iBar] : 1;

        pPdev->aBarIsMem[iBar] = u8Mem;
        pPdev->aBarLen[iBar] = u64Len;
        if (u64Len == 0) {
            continue;
        }
        pPdev->aBarPhys[iBar] = g_u64NextPhys;
        g_u64NextPhys += (u64Len + 0xfffull) & ~0xfffull;
        pBound->apBarHost[iBar] = calloc(1, (size_t)u64Len);
        if (pBound->apBarHost[iBar] == NULL) {
            pci_destroy_bound(pBound);
            return UDX_ERR_NOMEM;
        }
        udx_host_window_register(pPdev->aBarPhys[iBar],
                                 pBound->apBarHost[iBar], u64Len);
    }

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
udx_pci_enable(struct udx_pci_dev *pPdev)
{
    struct udx_pci_bound *pBound;

    if (pPdev == NULL || pPdev->pDev == NULL) {
        return UDX_ERR_INVAL;
    }
    pBound = (struct udx_pci_bound *)pPdev->pDev->pBackend;
    if (pBound == NULL) {
        return UDX_ERR_INVAL;
    }
    pBound->u8Enabled = 1;
    /* Full: bus master enable via cfg cap */
    return UDX_OK;
}

void
udx_pci_disable(struct udx_pci_dev *pPdev)
{
    struct udx_pci_bound *pBound;

    if (pPdev == NULL || pPdev->pDev == NULL) {
        return;
    }
    pBound = (struct udx_pci_bound *)pPdev->pDev->pBackend;
    if (pBound) {
        pBound->u8Enabled = 0;
        pBound->u8Master = 0;
    }
}

udx_status_t
udx_pci_set_master(struct udx_pci_dev *pPdev)
{
    struct udx_pci_bound *pBound;

    if (pPdev == NULL || pPdev->pDev == NULL) {
        return UDX_ERR_INVAL;
    }
    pBound = (struct udx_pci_bound *)pPdev->pDev->pBackend;
    if (pBound == NULL || !pBound->u8Enabled) {
        return UDX_ERR_INVAL;
    }
    pBound->u8Master = 1;
    return UDX_OK;
}

void
udx_pci_clear_master(struct udx_pci_dev *pPdev)
{
    struct udx_pci_bound *pBound;

    if (pPdev == NULL || pPdev->pDev == NULL) {
        return;
    }
    pBound = (struct udx_pci_bound *)pPdev->pDev->pBackend;
    if (pBound) {
        pBound->u8Master = 0;
    }
}

udx_status_t
udx_pci_request_regions(struct udx_pci_dev *pPdev, const char *szName)
{
    struct udx_pci_bound *pBound;

    if (pPdev == NULL || pPdev->pDev == NULL) {
        return UDX_ERR_INVAL;
    }
    pBound = (struct udx_pci_bound *)pPdev->pDev->pBackend;
    if (pBound == NULL) {
        return UDX_ERR_INVAL;
    }
    if (pBound->u8Regions) {
        return UDX_ERR_BUSY;
    }
    pBound->u8Regions = 1;
    (void)szName;
    return UDX_OK;
}

void
udx_pci_release_regions(struct udx_pci_dev *pPdev)
{
    struct udx_pci_bound *pBound;

    if (pPdev == NULL || pPdev->pDev == NULL) {
        return;
    }
    pBound = (struct udx_pci_bound *)pPdev->pDev->pBackend;
    if (pBound) {
        pBound->u8Regions = 0;
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

udx_status_t
udx_pci_read_config_word(struct udx_pci_dev *pPdev, int nWhere, u16 *pVal)
{
    if (pPdev == NULL || pVal == NULL) {
        return UDX_ERR_INVAL;
    }
    /* Host stub: vendor/device at 0/2 */
    if (nWhere == 0) {
        *pVal = pPdev->u16Vendor;
        return UDX_OK;
    }
    if (nWhere == 2) {
        *pVal = pPdev->u16Device;
        return UDX_OK;
    }
    *pVal = 0xffff;
    return UDX_ERR_NOSYS;
}

udx_status_t
udx_pci_write_config_word(struct udx_pci_dev *pPdev, int nWhere, u16 u16Val)
{
    if (pPdev == NULL) {
        return UDX_ERR_INVAL;
    }
    (void)nWhere;
    (void)u16Val;
    return UDX_ERR_NOSYS;
}
