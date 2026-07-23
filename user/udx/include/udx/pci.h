/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * PCI driver surface (Linux pci_driver shape).
 *
 * Soft path (skeleton):
 *   register_driver → inject/match → probe:
 *     enable → set_master → request_regions → ioremap →
 *     work/DMA/request_irq → set_drvdata
 *   remove (after quiesce): free_irq → DMA → iounmap → release → disable
 */
#pragma once

#include <udx/types.h>

struct udx_pci_dev;
struct udx_device;

struct udx_pci_device_id {
    u32 u32Vendor;
    u32 u32Device;
    u32 u32SubVendor;
    u32 u32SubDevice;
    u32 u32Class;
    u32 u32ClassMask;
    u64 u64DriverData;
};

#define UDX_PCI_ANY_ID 0xffffffffu

#define UDX_PCI_DEVICE(vend, dev) \
    { .u32Vendor = (vend), .u32Device = (dev), \
      .u32SubVendor = UDX_PCI_ANY_ID, .u32SubDevice = UDX_PCI_ANY_ID, \
      .u32Class = 0, .u32ClassMask = 0, .u64DriverData = 0 }

struct udx_pci_driver {
    const char                      *szName;
    const struct udx_pci_device_id  *pIdTable;
    int (*pfnProbe)(struct udx_pci_dev *pPdev, const struct udx_pci_device_id *pId);
    void (*pfnRemove)(struct udx_pci_dev *pPdev);
    void (*pfnQuiesce)(struct udx_pci_dev *pPdev); /* DDI-like; optional */
    struct udx_pci_driver           *pNext;
};

struct udx_pci_dev {
    struct udx_device *pDev;
    u16                u16Vendor;
    u16                u16Device;
    u8                 u8Bus;
    u8                 u8Devfn;
    int                nIrq;
    void              *pDriverData;
    /* Hidden: BAR phys/len, caps — filled by UDX/devmgr */
    u64                aBarPhys[6];
    u64                aBarLen[6];
    u8                 aBarIsMem[6];
};

udx_status_t udx_pci_register_driver(struct udx_pci_driver *pDrv);
void         udx_pci_unregister_driver(struct udx_pci_driver *pDrv);

udx_status_t udx_pci_enable(struct udx_pci_dev *pPdev);
void         udx_pci_disable(struct udx_pci_dev *pPdev);

/**
 * Bus-master enable bookkeeping (Linux pci_set_master).
 * Host: flag only. Full GJ: cfg cap write under the hood.
 */
udx_status_t udx_pci_set_master(struct udx_pci_dev *pPdev);
void         udx_pci_clear_master(struct udx_pci_dev *pPdev);

udx_status_t udx_pci_request_regions(struct udx_pci_dev *pPdev, const char *szName);
void         udx_pci_release_regions(struct udx_pci_dev *pPdev);

u64          udx_pci_resource_start(struct udx_pci_dev *pPdev, int nBar);
u64          udx_pci_resource_len(struct udx_pci_dev *pPdev, int nBar);

/* Config space (via cap to PCI cfg in full impl). */
udx_status_t udx_pci_read_config_word(struct udx_pci_dev *pPdev, int nWhere,
                                      u16 *pVal);
udx_status_t udx_pci_write_config_word(struct udx_pci_dev *pPdev, int nWhere,
                                       u16 u16Val);

/** Devfn helpers (slot 5-bit, func 3-bit). */
static inline u8
udx_pci_slot(const struct udx_pci_dev *pPdev)
{
    return pPdev ? (u8)((pPdev->u8Devfn >> 3) & 0x1fu) : 0;
}

static inline u8
udx_pci_func(const struct udx_pci_dev *pPdev)
{
    return pPdev ? (u8)(pPdev->u8Devfn & 0x7u) : 0;
}
