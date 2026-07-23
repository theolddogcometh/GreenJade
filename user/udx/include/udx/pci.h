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

#define UDX_PCI_DEVICE_SUB(vend, dev, subvend, subdev) \
    { .u32Vendor = (vend), .u32Device = (dev), \
      .u32SubVendor = (subvend), .u32SubDevice = (subdev), \
      .u32Class = 0, .u32ClassMask = 0, .u64DriverData = 0 }

#define UDX_PCI_DEVICE_CLASS(class, class_mask) \
    { .u32Vendor = UDX_PCI_ANY_ID, .u32Device = UDX_PCI_ANY_ID, \
      .u32SubVendor = UDX_PCI_ANY_ID, .u32SubDevice = UDX_PCI_ANY_ID, \
      .u32Class = (class), .u32ClassMask = (class_mask), \
      .u64DriverData = 0 }

/* Soft config space size (type-0 header + capability area host soft). */
#define UDX_PCI_CFG_SIZE 256u

/* PCI command register bits (config offset 0x04). */
#define UDX_PCI_COMMAND_IO     0x0001u
#define UDX_PCI_COMMAND_MEMORY 0x0002u
#define UDX_PCI_COMMAND_MASTER 0x0004u

/* Standard config offsets. */
#define UDX_PCI_CFG_VENDOR     0x00
#define UDX_PCI_CFG_DEVICE     0x02
#define UDX_PCI_CFG_COMMAND    0x04
#define UDX_PCI_CFG_STATUS     0x06
#define UDX_PCI_CFG_REVISION   0x08
#define UDX_PCI_CFG_CLASS      0x09 /* 3-byte class code starting here */
#define UDX_PCI_CFG_BAR0       0x10
#define UDX_PCI_CFG_SUBSYS_VEN 0x2c
#define UDX_PCI_CFG_SUBSYS_DEV 0x2e
#define UDX_PCI_CFG_IRQ_LINE   0x3c
#define UDX_PCI_CFG_IRQ_PIN    0x3d

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
    u16                u16SubVendor;
    u16                u16SubDevice;
    u32                u32Class; /* 24-bit class code */
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

/** Non-zero if pci_enable has been called and not disabled. */
int udx_pci_is_enabled(const struct udx_pci_dev *pPdev);

/**
 * Bus-master enable bookkeeping (Linux pci_set_master).
 * Host: soft command bit + flag. Full GJ: cfg cap write under the hood.
 */
udx_status_t udx_pci_set_master(struct udx_pci_dev *pPdev);
void         udx_pci_clear_master(struct udx_pci_dev *pPdev);

/** Non-zero if bus-master soft flag is set. */
int udx_pci_is_master(const struct udx_pci_dev *pPdev);

udx_status_t udx_pci_request_regions(struct udx_pci_dev *pPdev, const char *szName);
void         udx_pci_release_regions(struct udx_pci_dev *pPdev);

u64          udx_pci_resource_start(struct udx_pci_dev *pPdev, int nBar);
u64          udx_pci_resource_len(struct udx_pci_dev *pPdev, int nBar);

/** Non-zero if BAR n is a memory BAR (not I/O port). */
int          udx_pci_resource_is_mem(const struct udx_pci_dev *pPdev, int nBar);

/** Driver / region name for logs (never NULL). */
const char  *udx_pci_name(const struct udx_pci_dev *pPdev);

/* Config space (soft 256-byte image on host inject; cap path on full GJ). */
udx_status_t udx_pci_read_config_byte(struct udx_pci_dev *pPdev, int nWhere,
                                      u8 *pVal);
udx_status_t udx_pci_write_config_byte(struct udx_pci_dev *pPdev, int nWhere,
                                       u8 u8Val);
udx_status_t udx_pci_read_config_word(struct udx_pci_dev *pPdev, int nWhere,
                                      u16 *pVal);
udx_status_t udx_pci_write_config_word(struct udx_pci_dev *pPdev, int nWhere,
                                       u16 u16Val);
udx_status_t udx_pci_read_config_dword(struct udx_pci_dev *pPdev, int nWhere,
                                       u32 *pVal);
udx_status_t udx_pci_write_config_dword(struct udx_pci_dev *pPdev, int nWhere,
                                        u32 u32Val);

/**
 * Find injected/bound function by BDF (host soft + bound table).
 * Returns NULL if not present.
 */
struct udx_pci_dev *udx_pci_get_device(u8 u8Bus, u8 u8Devfn);

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

static inline u8
udx_pci_devfn(u8 u8Slot, u8 u8Func)
{
    return (u8)(((u8Slot & 0x1fu) << 3) | (u8Func & 0x7u));
}
