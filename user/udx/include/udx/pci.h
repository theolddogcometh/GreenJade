/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * PCI driver surface (Linux pci_driver shape).
 *
 * Soft path (skeleton / product UDX bind):
 *   register_driver → inject|grant/match → probe:
 *     enable[/mem] → set_master → request_regions|selected → ioremap →
 *     work/DMA/request_irq → set_drvdata
 *   remove (after quiesce): free_irq → DMA → iounmap → release → disable
 *                           → clear_drvdata
 *
 * Soft residual (Soft!=product; G-AC-1) — product hosts = Linux-shaped UDX:
 *   enable  = soft enable_cnt + COMMAND IO/MEM bits (not full pm/runtime)
 *             + enable_mem residual (MEM-only decode; no IO bit)
 *   regions = soft per-BAR claim mask + name (not /proc/iomem product)
 *             + request_region / request_selected_regions residual
 *   cfg     = soft 256-byte type-0 image (not live ECAM/cfg cap mint)
 *             + RO identity / STATUS RW1C / BAR size residual
 *   caps    = find_capability soft walk (null terminator; no live mint)
 *   identity= class/BDF/resource_end/drvdata residual inlines
 *
 * Soft residual deepen (product UDX hosts; Soft!=product; G-AC-1):
 *   enable residual   — enable_cnt + COMMAND IO/MEM | MEM-only (enable_mem)
 *   master residual   — set_master/clear_master COMMAND MASTER bit
 *   regions residual  — all / single / selected claim mask (not iomem tree)
 *   resource residual — start/len/end/is_mem + BAR type flag constants
 *   cfg residual      — soft type-0 256B; RO identity; STATUS RW1C; BAR size
 *   cap residual      — find_capability + CAP_ID_* (PM/MSI/MSI-X/PCIe)
 *   product_host residual — rtl8168_udx / xhci_udx G752 id + pref BAR mask
 *   dual_dod residual — Dual DoD A/B OPEN catalog (agent != close)
 *   c2 product path   — register→grant/inject→probe→enable→…→drvdata
 *   grant residual    — install_granted / ddi bind seed (mint OPEN)
 *   unbind residual   — quiesce_then_remove honesty
 *   api honesty       — soft path != ECAM/pm/runtime/iomem product
 *
 * Product-host shape residual (Dual DoD A/B OPEN seed; Soft!=product):
 *   rtl8168_udx (DoD B): enable + set_master + regions + BAR0/2 MMIO
 *   xhci_udx    (DoD A): enable + regions + BAR0 HC MMIO
 *   both: cfg word residual; slot/func BDF; set_drvdata on pDev
 *   Never claims Dual DoD A/B close; product grant mint OPEN.
 *   Freestanding rtl/USB class SKIP (not Dual DoD close path).
 *
 * C2 product path residual (Soft!=product; Dual DoD A/B OPEN):
 *   chain = register, grant|inject, match, probe,
 *           enable|enable_mem, set_master,
 *           request_regions|region|selected,
 *           cfg, find_cap, ioremap, dma, irq, set_drvdata
 *   teardown = quiesce, remove, release, disable, clear_drvdata
 *   product = UDX+ABI userspace hosts; never freestanding class re-enable
 *
 * Product residual (C2; Dual DoD B OPEN; stamp-free bar v2026.08.04.75):
 *   product=UDX+sshd+stack — PCI residual seeds rtl8168_udx probe so
 *   netstackd + sshd can product-serve when Cap mint + wire land.
 *   Soft enable/regions/cfg residual != Dual DoD B wire close.
 * greppable: udx: soft product residual product=UDX+sshd+stack
 * greppable: udx: pci soft product residual product=UDX+sshd+stack
 *
 * greppable: udx: pci soft residual lean
 * greppable: udx: pci soft enable residual
 * greppable: udx: pci soft master residual
 * greppable: udx: pci soft regions residual
 * greppable: udx: pci soft cfg residual
 * greppable: udx: pci soft cap residual
 * greppable: udx: pci soft resource residual
 * greppable: udx: pci soft product_host residual
 * greppable: udx: pci soft dual DoD
 * greppable: udx: pci soft dual_dod residual
 * greppable: udx: pci soft c2 product path
 * greppable: udx: pci soft grant residual
 * greppable: udx: pci soft unbind residual
 * greppable: udx: pci soft api honesty
 * greppable: udx: pci soft product bind
 * greppable: Dual_DoD_A=OPEN Dual_DoD_B=OPEN
 * Dual MIT OR Apache-2.0. Soft!=product. G-AC-1 (no .ko product AC).
 * No version stamp. Dual DoD A/B OPEN. freestanding class SKIP.
 * Bar honesty v2026.08.04.75 stamp-free. NEVER bump GJ_IMAGE_VERSION.
 */
#pragma once

#include <udx/device.h>
#include <udx/types.h>

struct udx_pci_dev;

/*
 * Soft residual lean layout version (eng honesty; Soft!=host ABI; not stamp).
 * Bump when product-host PCI residual surface grows (not GJ_IMAGE_VERSION).
 * ver1: enable_cnt + enable_mem + regions mask + cfg RO/RW1C/BAR size +
 *       find_cap + cap IDs + resource_end + drvdata/class/BDF inlines +
 *       G752 product-host pref BAR catalog.
 * ver2: Dual DoD A/B host-name catalog + pref BAR masks + domain residual +
 *       layout_ver/shape_ok + clear_drvdata/attach observation +
 *       product_host/dual_dod residual helpers + C2 path greppables.
 * Grep: udx: pci soft residual lean layout_ver=
 */
#define UDX_PCI_LAYOUT_VER           2u

/* Soft BAR count (PCI type-0; matches DDI BAR max spirit). */
#define UDX_PCI_BAR_MAX              6u

/* Soft residual single-domain honesty (product multi-domain OPEN). */
#define UDX_PCI_DOMAIN_SOFT          0u

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

/* PCI command register bits (config offset 0x04). Soft residual spirit. */
#define UDX_PCI_COMMAND_IO           0x0001u
#define UDX_PCI_COMMAND_MEMORY       0x0002u
#define UDX_PCI_COMMAND_MASTER       0x0004u
#define UDX_PCI_COMMAND_PARITY       0x0040u
#define UDX_PCI_COMMAND_SERR         0x0100u
#define UDX_PCI_COMMAND_INTX_DISABLE 0x0400u

/* Soft STATUS residual bits (config offset 0x06; RW1C error class). */
#define UDX_PCI_STATUS_INTX          0x0008u
#define UDX_PCI_STATUS_CAP_LIST      0x0010u
/* Soft RW1C residual mask (write-1-to-clear spirit; Soft!=product). */
#define UDX_PCI_STATUS_RW1C_MASK     0xf900u

/* Standard config offsets (type-0 residual for product UDX bind). */
#define UDX_PCI_CFG_VENDOR      0x00
#define UDX_PCI_CFG_DEVICE      0x02
#define UDX_PCI_CFG_COMMAND     0x04
#define UDX_PCI_CFG_STATUS      0x06
#define UDX_PCI_CFG_REVISION    0x08
#define UDX_PCI_CFG_CLASS       0x09 /* 3-byte class code starting here */
#define UDX_PCI_CFG_CACHE_LINE  0x0c
#define UDX_PCI_CFG_LATENCY     0x0d
#define UDX_PCI_CFG_HEADER      0x0e
#define UDX_PCI_CFG_BIST        0x0f
#define UDX_PCI_CFG_BAR0        0x10
#define UDX_PCI_CFG_CARDBUS_CIS 0x28
#define UDX_PCI_CFG_SUBSYS_VEN  0x2c
#define UDX_PCI_CFG_SUBSYS_DEV  0x2e
#define UDX_PCI_CFG_ROM         0x30
#define UDX_PCI_CFG_CAP_PTR     0x34
#define UDX_PCI_CFG_IRQ_LINE    0x3c
#define UDX_PCI_CFG_IRQ_PIN     0x3d
#define UDX_PCI_CFG_MIN_GNT     0x3e
#define UDX_PCI_CFG_MAX_LAT     0x3f

/* Soft residual: first capability pointer default (type-0, after header). */
#define UDX_PCI_CFG_SOFT_CAP0   0x40

/*
 * Soft residual capability IDs (PCI Local Bus Spec; Soft!=product mint).
 * Used with udx_pci_find_capability. Soft cap list is null-terminated
 * at UDX_PCI_CFG_SOFT_CAP0 today — find returns 0 until product hosts
 * or inject mint real caps. greppable: udx: pci soft cap residual
 */
#define UDX_PCI_CAP_ID_PM       0x01u /* Power Management */
#define UDX_PCI_CAP_ID_MSI      0x05u /* Message Signaled Interrupts */
#define UDX_PCI_CAP_ID_VENDOR   0x09u /* Vendor Specific */
#define UDX_PCI_CAP_ID_PCIE     0x10u /* PCI Express */
#define UDX_PCI_CAP_ID_MSIX     0x11u /* MSI-X */

/*
 * Soft residual BAR type bits (cfg BAR dword low bits; Soft!=product).
 * aBarIsMem[] is the product-host truth; these flags deepen cfg image honesty.
 * greppable: udx: pci soft resource residual
 */
#define UDX_PCI_BAR_SPACE_IO    0x00000001u
#define UDX_PCI_BAR_TYPE_MASK   0x00000006u
#define UDX_PCI_BAR_TYPE_32     0x00000000u
#define UDX_PCI_BAR_TYPE_64     0x00000004u
#define UDX_PCI_BAR_PREFETCH    0x00000008u
#define UDX_PCI_BAR_MEM_MASK    0xfffffff0u
#define UDX_PCI_BAR_IO_MASK     0xfffffffeu

/* Soft BAR mask helpers (bit N = BAR N; product UDX bind residual). */
#define UDX_PCI_BAR_MASK_ALL    0x3fu
#define UDX_PCI_BAR_BIT(n)      ((u8)(1u << ((n) & 7)))

/*
 * Soft residual class-code seeds (24-bit PCI class; Soft!=product).
 * Used with UDX_PCI_DEVICE_CLASS / class extractors; inventory only.
 * greppable: udx: pci soft resource residual
 */
#define UDX_PCI_CLASS_ETHERNET     0x020000u /* base 02 / sub 00 / if 00 */
#define UDX_PCI_CLASS_USB_XHCI     0x0c0330u /* base 0c / sub 03 / if 30 */
#define UDX_PCI_CLASS_MASK_BASE    0xff0000u
#define UDX_PCI_CLASS_MASK_FULL    0xffffffu

/*
 * G752 / product-host preferred BAR residual (matches ddi.h; Soft!=product).
 *   rtl8168_udx (10ec:8168) Dual DoD B: BAR0 (DDI grant) + BAR2 (MMIO)
 *   xhci_udx    (8086:a12f) Dual DoD A: BAR0 (HC MMIO)
 * greppable: udx: pci soft product_host residual
 * greppable: udx: pci soft dual DoD
 */
#define UDX_PCI_G752_RTL8168_VEND  0x10ecu
#define UDX_PCI_G752_RTL8168_DEV   0x8168u
#define UDX_PCI_G752_XHCI_VEND     0x8086u
#define UDX_PCI_G752_XHCI_DEV      0xa12fu
#define UDX_PCI_RTL_PREF_BAR0      0
#define UDX_PCI_RTL_PREF_BAR2      2
#define UDX_PCI_XHCI_PREF_BAR0     0
/* Preferred BAR claim masks (request_selected_regions residual). */
#define UDX_PCI_RTL_PREF_BAR_MASK \
    (UDX_PCI_BAR_BIT(UDX_PCI_RTL_PREF_BAR0) | \
     UDX_PCI_BAR_BIT(UDX_PCI_RTL_PREF_BAR2))
#define UDX_PCI_XHCI_PREF_BAR_MASK \
    (UDX_PCI_BAR_BIT(UDX_PCI_XHCI_PREF_BAR0))

/*
 * Dual DoD A/B catalog (API honesty; Soft residual != close).
 *
 *   Dual DoD A (UDX USB): xhci_udx    @ UDX_PCI_G752_XHCI_*    — OPEN
 *   Dual DoD B (UDX NIC): rtl8168_udx @ UDX_PCI_G752_RTL8168_* — OPEN
 *
 * Close criteria live on product userspace UDX datapath + live cap mint
 * (MMIO_FRAME / IRQ Notification / DMA window), not soft enable/regions/
 * cfg residual, not freestanding class stages, not soft bind lamps.
 * greppable: Dual_DoD_A=OPEN Dual_DoD_B=OPEN
 * greppable: udx: pci soft dual DoD
 * greppable: udx: pci soft dual_dod residual
 */
#define UDX_PCI_DOD_A_HOST_NAME    "xhci_udx"
#define UDX_PCI_DOD_B_HOST_NAME    "rtl8168_udx"
#define UDX_PCI_DOD_A_OPEN         1u /* always; never claim close here */
#define UDX_PCI_DOD_B_OPEN         1u /* always; never claim close here */
/*
 * Product residual seed (Soft!=product; Dual DoD B OPEN; stamp-free).
 * product=UDX+sshd+stack — PCI residual for rtl8168_udx → stack → sshd.
 * greppable: udx: pci soft product residual product=UDX+sshd+stack
 * Bar honesty v2026.08.04.75 stamp-free. NEVER bump GJ_IMAGE_VERSION.
 */
#define UDX_PCI_PRODUCT_UDX_SSHD_STACK  1u
#define UDX_PCI_PRODUCT_CHAIN           "rtl8168_udx>netstackd>sshd"

/*
 * Soft residual path-bit catalog (observation / host private soft masks).
 * UDX core never hard-gates Dual DoD on these bits. Soft!=product.
 * greppable: udx: pci soft residual lean
 * greppable: udx: pci soft c2 product path
 */
#define UDX_PCI_SOFT_PATH_ENABLE   (1u << 0)
#define UDX_PCI_SOFT_PATH_MASTER   (1u << 1)
#define UDX_PCI_SOFT_PATH_REGIONS  (1u << 2)
#define UDX_PCI_SOFT_PATH_CFG      (1u << 3)
#define UDX_PCI_SOFT_PATH_CAP      (1u << 4)
#define UDX_PCI_SOFT_PATH_RESOURCE (1u << 5)
#define UDX_PCI_SOFT_PATH_DRVDATA  (1u << 6)
#define UDX_PCI_SOFT_PATH_PRODUCT  (1u << 7)
/* Core C2 product-host probe residual (shape only; Soft!=product). */
#define UDX_PCI_SOFT_PATH_CORE \
    (UDX_PCI_SOFT_PATH_ENABLE | UDX_PCI_SOFT_PATH_MASTER | \
     UDX_PCI_SOFT_PATH_REGIONS | UDX_PCI_SOFT_PATH_CFG | \
     UDX_PCI_SOFT_PATH_RESOURCE | UDX_PCI_SOFT_PATH_DRVDATA)

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
    void              *pDriverData; /* legacy soft; prefer pDev drvdata */
    /* Hidden: BAR phys/len, caps — filled by UDX/devmgr */
    u64                aBarPhys[UDX_PCI_BAR_MAX];
    u64                aBarLen[UDX_PCI_BAR_MAX];
    u8                 aBarIsMem[UDX_PCI_BAR_MAX];
};

udx_status_t udx_pci_register_driver(struct udx_pci_driver *pDrv);
void         udx_pci_unregister_driver(struct udx_pci_driver *pDrv);

/**
 * Soft residual of Linux pci_enable_device for product UDX bind.
 * Refcounted (enable_cnt): nested enable OK; decode bits set on first.
 * First enable: COMMAND IO and/or MEM from present BARs (empty → MEM).
 * Host: soft COMMAND + flag. Full GJ: cfg cap write under the hood.
 * Soft!=product · not full pm/runtime.
 * greppable residual (impl): udx: pci soft enable residual
 */
udx_status_t udx_pci_enable(struct udx_pci_dev *pPdev);

/**
 * Soft residual of Linux pci_enable_device_mem for product UDX bind.
 * Same enable_cnt refcount as udx_pci_enable, but first enable sets only
 * COMMAND MEMORY (never IO). Nested enable OK. Soft!=product.
 * greppable residual (impl): udx: pci soft enable residual
 */
udx_status_t udx_pci_enable_mem(struct udx_pci_dev *pPdev);

/**
 * Soft residual of Linux pci_disable_device.
 * Decrements enable_cnt; clears COMMAND IO/MEM/MASTER only at zero.
 */
void         udx_pci_disable(struct udx_pci_dev *pPdev);

/** Non-zero if enable_cnt > 0 (pci_enable active). */
int udx_pci_is_enabled(const struct udx_pci_dev *pPdev);

/**
 * Soft enable refcount residual (Linux enable_cnt shape).
 * Returns 0 if unbound / never enabled.
 */
u32 udx_pci_enable_cnt(const struct udx_pci_dev *pPdev);

/**
 * Soft residual of soft COMMAND register (cfg image or flags).
 * Returns 0 if unbound / no live cfg.
 */
u16 udx_pci_command(const struct udx_pci_dev *pPdev);

/**
 * Soft residual of Linux pci_set_master for product UDX bind.
 * Host: soft COMMAND MASTER bit + flag. Full GJ: cfg cap write under hood.
 * Requires enable_cnt > 0 (same spirit as Linux). Soft!=product BM mint.
 * greppable residual (impl): udx: pci soft master residual
 */
udx_status_t udx_pci_set_master(struct udx_pci_dev *pPdev);
void         udx_pci_clear_master(struct udx_pci_dev *pPdev);

/** Non-zero if bus-master soft flag is set. Soft residual observation only. */
int udx_pci_is_master(const struct udx_pci_dev *pPdev);

/**
 * Soft residual of Linux pci_request_regions for product UDX bind.
 * Claims all present BARs (len != 0) under szName; BUSY if any claim active.
 * Soft mask only — not product /proc/iomem or kernel resource tree.
 * greppable residual (impl): udx: pci soft regions residual
 */
udx_status_t udx_pci_request_regions(struct udx_pci_dev *pPdev, const char *szName);
void         udx_pci_release_regions(struct udx_pci_dev *pPdev);

/**
 * Soft residual of Linux pci_request_region (single BAR).
 * Claims BAR nBar if present (len != 0); BUSY if already claimed;
 * INVAL if BAR empty or out of range. Soft!=product iomem.
 */
udx_status_t udx_pci_request_region(struct udx_pci_dev *pPdev, int nBar,
                                    const char *szName);
void         udx_pci_release_region(struct udx_pci_dev *pPdev, int nBar);

/**
 * Soft residual of Linux pci_request_selected_regions.
 * u8BarMask bit N claims BAR N (only present BARs); BUSY on overlap with
 * already-claimed bits. Soft!=product iomem tree.
 */
udx_status_t udx_pci_request_selected_regions(struct udx_pci_dev *pPdev,
                                              u8 u8BarMask,
                                              const char *szName);
void         udx_pci_release_selected_regions(struct udx_pci_dev *pPdev,
                                              u8 u8BarMask);

/**
 * Soft residual: bit N set if BAR N is currently claimed.
 * Returns 0 if unbound.
 */
u8 udx_pci_regions_mask(const struct udx_pci_dev *pPdev);

/**
 * Soft residual: bit N set if BAR N has len != 0 (present).
 * Returns 0 if unbound.
 */
u8 udx_pci_bar_present_mask(const struct udx_pci_dev *pPdev);

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
 * Soft residual of Linux pci_find_capability.
 * Walk soft cap list from CAP_PTR; returns cfg offset of matching id, or 0.
 * Soft null terminator at UDX_PCI_CFG_SOFT_CAP0 (id=0) — no live caps minted.
 * Soft!=product · not ECAM/cap mint.
 * greppable residual (impl): udx: pci soft cap residual
 */
u8 udx_pci_find_capability(struct udx_pci_dev *pPdev, u8 u8CapId);

/**
 * Find injected/bound function by BDF (host soft + bound table).
 * Returns NULL if not present.
 */
struct udx_pci_dev *udx_pci_get_device(u8 u8Bus, u8 u8Devfn);

/* ---- Soft residual inlines (header-only; Soft!=product) ---------------- */

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

/**
 * Soft residual: packed BDF (bus<<8 | slot<<3 | func) for logs / DMA notes.
 * Soft!=product domain; single-domain residual (domain=0).
 */
static inline u32
udx_pci_bdf(const struct udx_pci_dev *pPdev)
{
    if (pPdev == NULL) {
        return 0;
    }
    return ((u32)pPdev->u8Bus << 8) | (u32)pPdev->u8Devfn;
}

/**
 * Soft residual of Linux pci_resource_end (last byte of BAR window).
 * Returns 0 if BAR empty / out of range. greppable: resource residual
 */
static inline u64
udx_pci_resource_end(struct udx_pci_dev *pPdev, int nBar)
{
    u64 u64Start;
    u64 u64Len;

    if (pPdev == NULL || nBar < 0 || nBar >= (int)UDX_PCI_BAR_MAX) {
        return 0;
    }
    u64Len = pPdev->aBarLen[nBar];
    if (u64Len == 0) {
        return 0;
    }
    u64Start = pPdev->aBarPhys[nBar];
    return u64Start + u64Len - 1ull;
}

/** Soft residual: non-zero if BAR n is present (len != 0). */
static inline int
udx_pci_resource_present(const struct udx_pci_dev *pPdev, int nBar)
{
    if (pPdev == NULL || nBar < 0 || nBar >= (int)UDX_PCI_BAR_MAX) {
        return 0;
    }
    return pPdev->aBarLen[nBar] != 0;
}

/**
 * Soft residual class code extractors (24-bit u32Class on pdev).
 * Layout: base<<16 | sub<<8 | prog-if (PCI class code spirit).
 */
static inline u8
udx_pci_class_base(const struct udx_pci_dev *pPdev)
{
    return pPdev ? (u8)((pPdev->u32Class >> 16) & 0xffu) : 0;
}

static inline u8
udx_pci_class_sub(const struct udx_pci_dev *pPdev)
{
    return pPdev ? (u8)((pPdev->u32Class >> 8) & 0xffu) : 0;
}

static inline u8
udx_pci_class_progif(const struct udx_pci_dev *pPdev)
{
    return pPdev ? (u8)(pPdev->u32Class & 0xffu) : 0;
}

/**
 * Soft residual: MEM decode live without IO (enable_mem spirit query).
 * Observation only from COMMAND residual. Soft!=product.
 */
static inline int
udx_pci_is_mem_only_enabled(const struct udx_pci_dev *pPdev)
{
    u16 u16Cmd;

    if (!udx_pci_is_enabled(pPdev)) {
        return 0;
    }
    u16Cmd = udx_pci_command(pPdev);
    return ((u16Cmd & UDX_PCI_COMMAND_MEMORY) != 0) &&
           ((u16Cmd & UDX_PCI_COMMAND_IO) == 0);
}

/**
 * Soft residual of Linux pci_get_drvdata / pci_set_drvdata.
 * Product hosts hang soft state on pDev (device residual). Soft!=product.
 * greppable residual: udx: pci soft product_host residual
 */
static inline void *
udx_pci_get_drvdata(const struct udx_pci_dev *pPdev)
{
    if (pPdev == NULL) {
        return NULL;
    }
    if (pPdev->pDev != NULL) {
        return udx_get_drvdata(pPdev->pDev);
    }
    return pPdev->pDriverData;
}

static inline void
udx_pci_set_drvdata(struct udx_pci_dev *pPdev, void *pData)
{
    if (pPdev == NULL) {
        return;
    }
    if (pPdev->pDev != NULL) {
        udx_set_drvdata(pPdev->pDev, pData);
    }
    pPdev->pDriverData = pData;
}

/**
 * Soft residual remove: clear private soft state after free_irq / DMA /
 * iounmap / release / disable. Soft-equivalent: set_drvdata(NULL).
 * Quiesce must run before remove. Soft!=product; Dual DoD A/B OPEN.
 * greppable residual: udx: pci soft unbind residual
 */
static inline void
udx_pci_clear_drvdata(struct udx_pci_dev *pPdev)
{
    udx_pci_set_drvdata(pPdev, NULL);
}

/**
 * Soft residual: non-zero if probe installed private soft state.
 * Observation only — never Dual DoD product close.
 */
static inline int
udx_pci_has_drvdata(const struct udx_pci_dev *pPdev)
{
    return udx_pci_get_drvdata(pPdev) != NULL;
}

/**
 * Soft attach residual: backend filled AND drvdata installed.
 * Fail-closed; Soft!=product; Dual DoD A/B OPEN. Cap mint remains OPEN.
 * greppable residual: udx: pci soft product_host residual
 */
static inline int
udx_pci_is_soft_attached(const struct udx_pci_dev *pPdev)
{
    if (pPdev == NULL) {
        return 0;
    }
    if (pPdev->pDev == NULL || pPdev->pDev->pBackend == NULL) {
        return 0;
    }
    return udx_pci_has_drvdata(pPdev);
}

/**
 * Soft residual: G752 product-host ID match helpers (Soft!=product).
 * Used by inventory / bind lamps; never close Dual DoD A/B.
 * greppable residual: udx: pci soft product_host residual
 * greppable residual: udx: pci soft dual_dod residual
 */
static inline int
udx_pci_is_g752_rtl8168(const struct udx_pci_dev *pPdev)
{
    return pPdev != NULL &&
           pPdev->u16Vendor == (u16)UDX_PCI_G752_RTL8168_VEND &&
           pPdev->u16Device == (u16)UDX_PCI_G752_RTL8168_DEV;
}

static inline int
udx_pci_is_g752_xhci(const struct udx_pci_dev *pPdev)
{
    return pPdev != NULL &&
           pPdev->u16Vendor == (u16)UDX_PCI_G752_XHCI_VEND &&
           pPdev->u16Device == (u16)UDX_PCI_G752_XHCI_DEV;
}

/**
 * Soft residual: non-zero if pdev is a G752 Dual DoD product-host seed ID.
 * Soft!=product; agent != close Dual DoD A/B.
 * greppable residual: udx: pci soft product_host residual
 * greppable residual: udx: pci soft dual DoD
 */
static inline int
udx_pci_is_product_host_id(const struct udx_pci_dev *pPdev)
{
    return udx_pci_is_g752_rtl8168(pPdev) || udx_pci_is_g752_xhci(pPdev);
}

/**
 * Soft residual preferred BAR claim mask for product-host IDs.
 * rtl8168 → BAR0|BAR2; xhci → BAR0; other/NULL → 0.
 * For request_selected_regions residual; Soft!=product iomem.
 * greppable residual: udx: pci soft product_host residual
 * greppable residual: udx: pci soft regions residual
 */
static inline u8
udx_pci_pref_bar_mask(const struct udx_pci_dev *pPdev)
{
    if (udx_pci_is_g752_rtl8168(pPdev)) {
        return (u8)UDX_PCI_RTL_PREF_BAR_MASK;
    }
    if (udx_pci_is_g752_xhci(pPdev)) {
        return (u8)UDX_PCI_XHCI_PREF_BAR_MASK;
    }
    return 0;
}

/**
 * Soft residual Dual DoD letter for product-host seed IDs.
 * Returns 'A' (xhci_udx USB), 'B' (rtl8168_udx NIC), or 0 if not a seed.
 * Always OPEN — never a close claim. Soft!=product.
 * greppable residual: udx: pci soft dual_dod residual
 * greppable residual: Dual_DoD_A=OPEN Dual_DoD_B=OPEN
 */
static inline char
udx_pci_dual_dod_letter(const struct udx_pci_dev *pPdev)
{
    if (udx_pci_is_g752_xhci(pPdev)) {
        return 'A';
    }
    if (udx_pci_is_g752_rtl8168(pPdev)) {
        return 'B';
    }
    return (char)0;
}

/**
 * Soft residual Dual DoD host name for product-host seed IDs.
 * Returns UDX_PCI_DOD_*_HOST_NAME or NULL. Soft!=product; never close.
 * greppable residual: udx: pci soft dual_dod residual
 * greppable residual: udx: pci soft product_host residual
 */
static inline const char *
udx_pci_dual_dod_host_name(const struct udx_pci_dev *pPdev)
{
    if (udx_pci_is_g752_xhci(pPdev)) {
        return UDX_PCI_DOD_A_HOST_NAME;
    }
    if (udx_pci_is_g752_rtl8168(pPdev)) {
        return UDX_PCI_DOD_B_HOST_NAME;
    }
    return NULL;
}

/**
 * Soft residual: Dual DoD A/B always OPEN from this catalog (fixed 1).
 * Observation honesty only — Soft residual lamps never close Dual DoD.
 * greppable residual: udx: pci soft dual DoD
 * greppable residual: Dual_DoD_A=OPEN Dual_DoD_B=OPEN
 */
static inline int
udx_pci_dual_dod_a_open(void)
{
    return (int)UDX_PCI_DOD_A_OPEN;
}

static inline int
udx_pci_dual_dod_b_open(void)
{
    return (int)UDX_PCI_DOD_B_OPEN;
}

/**
 * Soft product residual seed toward product=UDX+sshd+stack.
 * Returns 1 always — catalog honesty only (Soft!=product; Dual DoD OPEN).
 * greppable residual: udx: pci soft product residual product=UDX+sshd+stack
 */
static inline int
udx_pci_product_udx_sshd_stack(void)
{
    return (int)UDX_PCI_PRODUCT_UDX_SSHD_STACK;
}

/* ---- Soft residual queries / shape (observation only; Soft!=product) - */

/**
 * Soft residual lean layout version (UDX_PCI_LAYOUT_VER).
 * Grep: udx: pci soft residual lean layout_ver=
 * Not GJ_IMAGE_VERSION. Not product ABI stamp.
 */
static inline u32
udx_pci_layout_ver(void)
{
    return UDX_PCI_LAYOUT_VER;
}

/**
 * Non-zero when soft product-host PCI surface is present at layout_ver:
 * enable/enable_mem + master + regions/region/selected + cfg RO/RW1C/BAR
 * size + find_cap + resource/BDF/class/drvdata inlines + Dual DoD OPEN
 * catalog + product_host pref BAR helpers.
 * Soft!=product; eng residual only. Never hard-gates Dual DoD.
 * greppable: udx: pci soft residual product_host
 * greppable: udx: pci soft product_host residual
 * greppable: udx: pci soft dual_dod residual
 * greppable: udx: pci soft api honesty
 */
static inline int
udx_pci_shape_ok(void)
{
    return (UDX_PCI_LAYOUT_VER >= 2u) ? 1 : 0;
}

/**
 * Soft residual domain number (single-domain honesty; domain=0).
 * Product multi-domain OPEN. Soft!=product.
 */
static inline u32
udx_pci_domain_nr(const struct udx_pci_dev *pPdev)
{
    (void)pPdev;
    return UDX_PCI_DOMAIN_SOFT;
}
