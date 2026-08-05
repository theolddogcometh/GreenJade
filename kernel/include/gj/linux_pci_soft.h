/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Soft Linux PCI driver-model seed (clean-room).
 * Dual MIT OR Apache-2.0. No GPL / no Linux source.
 *
 * Purpose
 * -------
 * Seed surface so a soft-loaded module's pci_register_driver /
 * __pci_register_driver can soft-match G752 devices from the devmgr_soft
 * inventory (10ec:8168, 8086:a12f, …) and optionally invoke a soft probe.
 * Complements F2 linux_ksym resolution: this TU owns the real soft bodies;
 * ksym takes their addresses, or linux_pci_soft_init() registers them via
 * linux_ksym_register.
 *
 * Soft ≠ ABI-stable
 * -----------------
 * struct pci_dev / pci_driver / pci_device_id here are INCOMPLETE soft shapes.
 * Field order and size are NOT guaranteed to match any Linux kernel version.
 * Do not claim binary .ko layout compatibility. Soft modules / tests built
 * against this header are the intended consumers until a product ABI exists.
 *
 * Greppable markers (keep stable):
 *   linux_pci_soft: soft init PASS
 *   linux_pci_soft: soft register PASS
 *   linux_pci_soft: soft register SKIP
 *   linux_pci_soft: soft __pci_register PASS
 *   linux_pci_soft: soft probe 10ec:8168 PASS|FAIL|SKIP
 *   linux_pci_soft: soft ksym register PASS|SKIP
 *   linux_pci_soft: soft pci_dev incomplete field=…
 *   linux_pci_soft: soft hostish probe ENTER|PASS|FAIL|FAULT
 *
 * Probe-shaped host field plan (Soft≠ABI): docs/PCI_DEV_SOFT_LAYOUT.md
 * See docs/DDI_SOFT.md · docs/LAPTOP_LINUX_DRIVER_HOST.md · UDX_LINUX_PORTER.md.
 */
#pragma once

#include <gj/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Soft table bounds (inventory density for laptop host bring-up). */
#define LINUX_PCI_SOFT_DRV_MAX   16u
#define LINUX_PCI_SOFT_DEV_MAX   32u
#define LINUX_PCI_SOFT_BAR_MAX   6u

/** Soft default line IRQ used by pci_irq_vector / fill path (G752-ish). */
#define LINUX_PCI_SOFT_IRQ       11

/** Linux-shaped PCI_ANY_ID (~0) for soft id tables. */
#define PCI_ANY_ID  0xffffffffu

/* Soft IORESOURCE_* fragments for pci_select_bars (not full Linux resource). */
#define IORESOURCE_IO   0x00000100ul
#define IORESOURCE_MEM  0x00000200ul

/**
 * Soft PCI device id row (Linux pci_device_id *shape* for RHEL 9 / 5.14+).
 * Size must be 40 on x86_64: real r8169.ko id tables use 40-byte rows
 * (driver_data 8 + override_only 4 + pad 4). A 32-byte soft row mis-walked
 * the table and never matched 10ec:8168 (G752 NETDEV SOFT 0 with INIT=0).
 * Sentinel / end: vendor==0 && device==0 && class_mask==0 (and typically
 * all-zero row). Soft≠ABI-stable across every distro; 40 matches host .ko.
 */
struct pci_device_id {
    u32 vendor;
    u32 device;
    u32 subvendor;
    u32 subdevice;
    u32 class;       /* 24-bit class code when class_match used */
    u32 class_mask;
    unsigned long driver_data; /* kernel_ulong_t */
    u32 override_only;         /* Linux 5.7+; soft ignore */
    u32 u32Pad;                /* keep 40-byte stride for .ko tables */
};

struct pci_dev; /* forward */

/**
 * Soft PCI driver (Linux pci_driver *shape*, not layout-stable).
 * Early Linux fields: name, id_table, probe, remove.
 * pNext / owner / mod_name are GreenJade soft fields — not Linux ABI.
 *
 * Probe signature matches Linux / r8169:
 *   int (*probe)(struct pci_dev *, const struct pci_device_id *);
 */
struct pci_driver {
    const char                   *name;
    const struct pci_device_id   *id_table;
    int  (*probe)(struct pci_dev *dev, const struct pci_device_id *id);
    void (*remove)(struct pci_dev *dev);
    void                         *owner;    /* soft: THIS_MODULE soft cookie */
    const char                   *mod_name; /* soft: KBUILD_MODNAME */
    struct pci_driver            *pNext;    /* soft list link */
};

/**
 * Soft PCI device snapshot filled from devmgr inventory on match.
 * Soft≠product: BAR phys from inventory; config may use real CF8/CFC.
 * No product MMIO map / IRQ install / DMA cap mint here.
 *
 * Soft≠Linux layout (why EMU bind skips .ko probe):
 *   - Host RHEL 5.14 pci_dev: vendor @ ~0x3c, resource[] after embedded
 *     struct device, drvdata at dev.driver_data — multi-KB object.
 *   - Soft: compact fields at front; parallel BAR arrays; no struct device.
 * Inlined r8169 accesses (pci_resource_*, pci_set_drvdata) need a future
 * probe-shaped blob — see docs/PCI_DEV_SOFT_LAYOUT.md. Do not pass this
 * soft object to real .ko probe.
 */
struct pci_dev {
    u16 vendor;
    u16 device;
    u16 subsystem_vendor;
    u16 subsystem_device;
    u32 class; /* class<<16 | subclass<<8 | prog_if */
    u8  bus;
    u8  devfn; /* slot<<3 | func */
    u8  u8Enabled;
    u8  u8Master;
    u8  u8Regions;
    u8  u8Bound;
    u8  u8IrqAllocated;
    u8  u8Pad;
    int irq; /* soft line; LINUX_PCI_SOFT_IRQ after fill */
    int msi_enabled; /* soft bookkeeping only */
    void *driver_data;
    struct pci_driver *driver;
    u64 resource_start[LINUX_PCI_SOFT_BAR_MAX];
    u64 resource_len[LINUX_PCI_SOFT_BAR_MAX];
    u8  resource_is_mem[LINUX_PCI_SOFT_BAR_MAX];
    const struct pci_device_id *pMatchedId;
    /*
     * Soft-only notes for future probe-shaped fill (not Linux ABI).
     * Zero = not tracked; non-zero bit flags optional diagnostics.
     */
    u32 u32IncompleteBits;
};

/* Convenience id-table row helpers (soft modules / tests). */
#define PCI_DEVICE(vend, dev) \
    { .vendor = (u32)(vend), .device = (u32)(dev), \
      .subvendor = PCI_ANY_ID, .subdevice = PCI_ANY_ID, \
      .class = 0u, .class_mask = 0u, .driver_data = 0ul }

#define PCI_DEVICE_CLASS(cls, mask) \
    { .vendor = PCI_ANY_ID, .device = PCI_ANY_ID, \
      .subvendor = PCI_ANY_ID, .subdevice = PCI_ANY_ID, \
      .class = (u32)(cls), .class_mask = (u32)(mask), .driver_data = 0ul }

/**
 * Soft init: idempotent ready lamp + optional ksym export of soft bodies.
 * Prefer: linux_ksym_register("__pci_register_driver", __pci_register_driver) …
 * when F2 linux_ksym is linked (weak-call safe if absent).
 * Grep: linux_pci_soft: soft init PASS
 */
void linux_pci_soft_init(void);

/** Non-zero after successful soft init. */
int  linux_pci_soft_ready(void);

/**
 * Soft register path (primary soft name).
 * Casts pDrv to soft struct pci_driver *, scans devmgr_soft inventory for
 * vendor/device (and class) matches, calls probe soft if non-NULL.
 * Logs: linux_pci_soft: soft register PASS|SKIP
 * Returns 0 on accepted registration, negative on bad args / not ready.
 */
int  linux_pci_soft_register_driver(void *pDrv);

/** Soft unregister: unbind matched soft devices, unlink driver. */
void linux_pci_soft_unregister_driver(void *pDrv);

/*
 * Soft bodies with Linux C names — ksym / modules resolve these symbols.
 * Arguments are void * so F2 stubs and soft callers share one signature.
 * Soft≠ABI-stable struct layout under the pointer.
 */

/** Linux-shaped: __pci_register_driver(drv, owner, mod_name) → soft register. */
int  __pci_register_driver(void *drv, void *owner, const char *mod_name);
int  pci_register_driver(void *drv);
void pci_unregister_driver(void *drv);

int  pci_enable_device(void *dev);
void pci_disable_device(void *dev);
void pci_set_master(void *dev);
void pci_clear_master(void *dev);
int  pci_request_regions(void *dev, const char *szName);
void pci_release_regions(void *dev);

/** Soft config space via real type-0 CF8/CFC (bus/devfn from soft pci_dev). */
int  pci_read_config_byte(void *dev, int nWhere, u8 *pVal);
int  pci_read_config_word(void *dev, int nWhere, u16 *pVal);
int  pci_write_config_byte(void *dev, int nWhere, u8 u8Val);

/** Soft IRQ vector bookkeeping (alloc → 1; vector 0 → irq 11). */
int  pci_alloc_irq_vectors(void *dev, unsigned int uMin, unsigned int uMax,
                           unsigned int uFlags);
int  pci_irq_vector(void *dev, unsigned int uNr);

/** Soft BAR select bitmask from inventory flags (IORESOURCE_MEM/IO). */
int  pci_select_bars(void *dev, unsigned long ulFlags);

/** Soft PM / link / bus helpers — return 0 (no-op soft). */
int  pci_set_power_state(void *dev, int nState);
int  pci_wake_from_d3(void *dev, int nEnable);
int  pci_dev_run_wake(void *dev);
int  pci_disable_link_state(void *dev, int nState);
u16  pci_status_get_and_clear_errors(void *dev);
int  pci_reset_bus(void *dev);

/** Soft BAR helpers (Linux-shaped names; soft inventory only). */
u64  pci_resource_start(void *dev, int nBar);
u64  pci_resource_len(void *dev, int nBar);

/** Soft drvdata (Linux pci_set/get_drvdata shape). */
void  pci_set_drvdata(void *dev, void *pData);
void *pci_get_drvdata(void *dev);

/** Soft diagnostics. */
u32  linux_pci_soft_driver_count(void);
u32  linux_pci_soft_bound_count(void);
u32  linux_pci_soft_register_calls(void);
u32  linux_pci_soft_match_count(void);

/* Last probe mode for STATUS (soft only; see docs/STEAM_BAR3_STATUS.md for bar3). */
#define LINUX_PCI_SOFT_PROBE_MODE_NONE  (-1)
#define LINUX_PCI_SOFT_PROBE_MODE_SOFT  (0)
#define LINUX_PCI_SOFT_PROBE_MODE_REAL  (1)

/*
 * Set to 1 only around freestanding .ko probe(hostish); trap.c observes on
 * kernel #PF and greps FAULT once then still halts. Soft≠product; no recovery.
 * Defined in linux_pci_soft.c (BSS zero). Parent probe path may raise/clear.
 */
extern volatile u32 g_u32SoftHostishProbeInflight;

int  linux_pci_soft_last_probe_mode(void);
int  linux_pci_soft_last_probe_st(void);
void linux_pci_soft_note_probe(u16 u16Vend, u16 u16Dev, int nMode, int nSt);

/**
 * Force soft EMU bind for inventory VID:DID without requiring .ko probe.
 * Used after module init if netdev soft is still 0 (layout / id_table miss).
 * Returns number of new binds. Soft≠product; greppable:
 *   linux_pci_soft: soft force emu 10ec:8168 PASS|SKIP
 */
u32  linux_pci_soft_force_emu_bind(u16 u16Vend, u16 u16Dev);

/**
 * Gate0 hybrid safety: non-zero if soft ksym may issue real CF8 writes or
 * pci_iomap for this soft/hostish pci_dev.
 * Gate0 (handoff==0): always 0 for 10ec:8168 (freestanding sole BAR).
 * Gate1: 0 while rtl8168_ready(); 1 after freestanding quiesce.
 * Grep: linux_pci_soft: soft cf8 write NOOP hybrid
 */
int  linux_pci_soft_hw_touch_ok(void *dev);

/**
 * Once serial lamp for hybrid zero-touch policy (gate0 REAL skip + CF8/iomap).
 * Grep: linux_pci_soft: soft hybrid zero-touch
 */
void linux_pci_soft_zero_touch_lamp_once(void);

#ifdef __cplusplus
}
#endif
