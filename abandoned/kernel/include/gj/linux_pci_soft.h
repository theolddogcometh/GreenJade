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
 * inventory (10ec:8168, 8086:a12f, ...) and optionally invoke a soft probe.
 * Complements F2 linux_ksym resolution: this TU owns the real soft bodies;
 * ksym takes their addresses, or linux_pci_soft_init() registers them via
 * linux_ksym_register.
 *
 * Soft != ABI-stable
 * -----------------
 * struct pci_dev / pci_driver / pci_device_id here are INCOMPLETE soft shapes.
 * Field order and size are NOT guaranteed to match any Linux kernel version.
 * Do not claim binary .ko layout compatibility. Soft modules / tests built
 * against this header are the intended consumers until a product ABI exists.
 *
 * Residual lean deepen (layout_ver ≥ 1 · DDI/UDX inventory eng):
 * Soft pci_dev front stamps layout_ver + u8UdxEng / u8DdiInv /
 * u8ProductOpen / u8BarInvOnly / u8Gate0Pin so future userspace UDX hosts
 * and DDI door inventory can lean on a stable soft bookkeeping shape
 * (VID:DID/BDF/BAR phys from devmgr_soft only). Soft inventory != product
 * MMIO/IRQ/DMA cap mint. Soft!=product · G-AC-1 · once-lamps only.
 * Grep: linux_pci_soft: soft residual lean PASS
 * Grep: linux_pci_soft: soft residual lean UDX eng
 * Grep: linux_pci_soft: soft residual lean DDI inventory
 *
 * Greppable markers (keep stable):
 *   linux_pci_soft: soft init PASS
 *   linux_pci_soft: soft register PASS
 *   linux_pci_soft: soft register SKIP
 *   linux_pci_soft: soft __pci_register PASS
 *   linux_pci_soft: soft probe 10ec:8168 PASS|FAIL|SKIP
 *   linux_pci_soft: soft ksym register PASS|SKIP
 *   linux_pci_soft: soft pci_dev incomplete field=...
 *   linux_pci_soft: soft hostish probe ENTER|PASS|FAIL|FAULT
 *   linux_pci_soft: soft hostish probe SKIP hybrid
 *   linux_pci_soft: soft hybrid zero-touch PASS|READY
 *   linux_pci_soft: soft residual lean PASS
 *   linux_pci_soft: soft residual lean UDX eng
 *   linux_pci_soft: soft residual lean DDI inventory
 *   linux_pci_soft: soft residual layout ...
 *   linux_pci_soft: soft cf8 write NOOP hybrid
 *   linux_pci_soft: soft cf8 BDF refuse hybrid
 *   linux_pci_soft: soft iomap refuse hybrid
 *   linux_pci_soft: soft BAR inventory only hybrid
 *
 * Probe-shaped host field plan (Soft!=ABI): docs/PCI_DEV_SOFT_LAYOUT.md
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

/*
 * Soft layout residual lean (eng only; Soft!=host ABI; not an image stamp).
 * Bump when soft pci_dev residual front field set grows for DDI/UDX inventory.
 * Grep: linux_pci_soft: soft residual lean PASS layout_ver=
 * ver1: DDI/UDX inventory residual stamps on soft pci_dev
 *       (udx_eng/ddi_inv/product_open/bar_inv_only/gate0_pin + layout_ver).
 */
#define LINUX_PCI_SOFT_LAYOUT_VER  1u

/* Soft magic marker ('GJpd' LE) - debugger / residual honesty. */
#define LINUX_PCI_SOFT_PCI_DEV_MAGIC  0x64704a47u /* 'GJpd' little-endian */

/**
 * Soft PCI device id row (Linux pci_device_id *shape* for RHEL 9 / 5.14+).
 * Size must be 40 on x86_64: real r8169.ko id tables use 40-byte rows
 * (driver_data 8 + override_only 4 + pad 4). A 32-byte soft row mis-walked
 * the table and never matched 10ec:8168 (G752 NETDEV SOFT 0 with INIT=0).
 * Sentinel / end: vendor==0 && device==0 && class_mask==0 (and typically
 * all-zero row). Soft!=ABI-stable across every distro; 40 matches host .ko.
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
 * pNext / owner / mod_name are GreenJade soft fields - not Linux ABI.
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
 * Soft!=product: BAR phys from inventory; config may use CF8/CFC when allowed.
 * Gate0 hybrid residual: 10ec:8168 CF8 R/W (enable/master/read|write_config/
 * status) and BAR scrub are NOOP while freestanding owns wire - soft bookkeep
 * only; reads from soft inventory (no CF8 ADDR thrash). Hybrid Soft!=product.
 * Gap E residual deepen: soft fill pins u8Regions=0 for gate0 8168 (BAR phys
 * inventory honesty only - no soft BAR claim / grab fight). Soft!=product.
 * Gap E dual-DoD deepen: VID+BDF fail-closed CF8/iomap refuse; hostish SKIP;
 * soft inventory BAR phys only (no CF8 size probe). Soft!=product.
 * Gap E dual-DoD residual deepen (prior): BDF refuse triple-source; gate1 BDF
 * while fs ready; hostish SKIP VID|BDF; select_bars hostish fail-closed;
 * VID-agnostic freestanding BDF pin; inventory-fn zero-touch; claim-pin;
 * pci_resource_* BAR inventory once lamp; pointer zero-touch choke;
 * hostish select_bars/resource fail-closed.
 * Gap E dual-DoD residual deepen (prior / OPEN): unified refuse-fire
 * choke (CF8 BDF / iomap / PASS|READY from every refuse path); fill claim-pin
 * via inventory-fn zero-touch; soft select_bars BAR inventory lamp + claim-pin
 * under freestanding pin.
 * Gap E dual-DoD residual deepen (prior / OPEN): soft inventory BAR phys
 * co-light refuse-fire (soft resource_*, select_bars, soft_cfg under freestanding
 * pin); soft_pin + cf8_noop co-fire PASS|READY+BDF+iomap; EMU bind claim-pin
 * via inventory-fn zero-touch.
 * Gap E dual-DoD residual deepen (prior / OPEN): refuse-fire co-lights BAR
 * inventory honesty from every CF8/iomap/BDF path (NULL-dev ok); inventory
 * full-walk BDF source (1b); force-EMU already-bound re-pin; hostish resource_*
 * SKIP; IRQ alloc/vector + reset_bus fail-closed under freestanding pin.
 * Gap E dual-DoD residual deepen (prior / OPEN): pure CF8 BDF refuse
 * re-pins soft-bound at freestanding BDF; hostish pool scrub under freestanding
 * pin (kill live slip fill); match already-bound re-pin; PM/wake/link
 * fail-closed under freestanding pin; disable/master/release soft_pin co-light;
 * init inventory freestanding scrub. Soft inventory BAR phys only; hostish
 * SKIP; once greppable PASS/READY/NOOP lamps.
 * Gap E dual-DoD residual deepen (prior / OPEN): hostish is_hostish
 * pool-membership fail-closed (scrubbed slots stay hostish-shaped); hostish
 * scrub zeros BAR phys in killed blobs (soft inventory BAR phys only); repin-all
 * soft freestanding; refuse-fire reentrancy + co-repin; register/force-EMU/
 * EMU-bind post scrub; status_clear + release_region soft_pin; hostish drvdata
 * fail-closed under freestanding pin.
 * Gap E dual-DoD residual deepen (prior / OPEN): denser hostish SKIP
 * honesty (scrub+repin every SKIP; residual non-live BAR bait kill; clear
 * enable/driver/drvdata REAL hooks); hw_touch denser soft_pin+scrub;
 * fill/try_real/match denser SKIP; enable/request hostish freestanding
 * co-light SKIP; never REAL probe 8168 under gate0; soft inventory BAR phys
 * only; once greppable PASS/READY/NOOP + iomap/BDF/hostish SKIP/BAR inventory.
 * Gap E dual-DoD residual deepen (prior / OPEN): denser hostish kill
 * (DMA/bus/devfn/VID identity bait zero); soft_pin hostish freestanding
 * kill+SKIP; request_region singular SKIP parity; try_real FAIL kill hooks;
 * soft_cfg/disable/master/status/release hostish SKIP co-light; init scrub
 * without inventory (VID gate0 kill). Fail-closed CF8/iomap; hostish SKIP
 * denser honesty; never REAL BAR under gate0; once greppable PASS/READY/NOOP
 * + iomap/BDF/hostish SKIP/BAR inventory.
 * Gap E dual-DoD residual deepen (prior / OPEN): soft_pin full claim
 * triad (u8Regions+u8Enabled+u8Master=0 under freestanding pin); denser
 * hostish kill class/subsys + bus[]/DMA-mask aux zero; enable/set_master/
 * clear_master hostish freestanding soft_pin+SKIP co-light; resource_*
 * IRQ/PM soft freestanding soft_pin denser; init scrub no-inv also
 * repin-all; force-EMU already-bound refuse-fire+scrub denser. Fail-closed
 * CF8/iomap; hostish SKIP denser honesty; never REAL BAR under gate0; once
 * greppable PASS/READY/NOOP + iomap/BDF/hostish SKIP/BAR inventory.
 * Gap E dual-DoD residual deepen (prior / OPEN): soft_pin extended
 * bookkeep (claim triad + u8IrqAllocated+msi_enabled=0); hostish kill
 * full-blob memset; hostish range-membership fail-closed; select_bars/
 * drvdata hostish freestanding soft_pin+SKIP; try_real SKIP scrub+repin;
 * init scrub inventory-ready-no-8168 still scrub+repin; match/force-EMU
 * already-bound repin-all denser. Fail-closed CF8/iomap; hostish SKIP
 * denser honesty; never REAL BAR under gate0; once greppable
 * PASS/READY/NOOP + iomap/BDF/hostish SKIP/BAR inventory.
 * Gap E dual-DoD residual deepen (prior / OPEN): soft_pin soft REAL-hook
 * clear (driver=NULL under freestanding pin); enable/set_master pin-first
 * (never assign claim under freestanding pin); soft_cfg hostish denser
 * soft_pin; write_config denser soft_pin+SKIP; init scrub gate0 refuse-fire
 * even without inv/8168 (policy lamp honesty); try_real FAIL denser
 * refuse-fire co-light.
 * Gap E dual-DoD residual deepen (prior / OPEN): fully define
 * lpcis_hostish_refuse_dense (prior incomplete mid-blob cast fixed via
 * blob_base + soft_pin + kill+unlive+aux + SKIP denser); wire hostish
 * freestanding CF8/iomap/resource/IRQ/PM/enable/master/write_config refuse.
 * Gap E dual-DoD residual deepen (prior / OPEN): hostish refuse dense
 * zero-touch residual - BDF/VID resolve via blob_base (mid-blob fail-closed);
 * sticky freestanding pin on hostish pool slots after scrub/kill/fill-refuse
 * so zeroed identity still CF8/iomap NOOP under freestanding sole wire;
 * refuse_dense + hw_touch honor pin.
 * Gap E dual-DoD residual deepen (prior / OPEN / R0 class): sticky
 * freestanding 8168 BDF once-known (pure CF8 refuse under inventory lag -
 * closes ADDR thrash -> EMPTY poll + R0); soft_cfg CMD synthetic forced 0
 * under freestanding pin (no soft-pretend MEM|IO|BM); hostish SKIP honesty
 * denser on force-EMU already-bound + EMU bind freestanding paths; note_bdf
 * from inventory/fn/fill/soft_pin/scrub. Fail-closed CF8/iomap; zero-touch
 * hostish refuse; never REAL BAR under gate0; once greppable PASS/READY/NOOP +
 * iomap/BDF/hostish SKIP/BAR inventory. Soft!=product; hybrid Soft!=product;
 * G-AC-1. Eng residual only - dual DoD OPEN.
 * Gap E dual-DoD residual deepen (prior / OPEN / R0 class H4 denser):
 * note_bdf never poisons sticky with 0:0.0 host-bridge class (scrubbed zero
 * identity must not fail-open real freestanding 8168 @ bus 3-class G752VT);
 * soft_cfg BAR 0x10..0x24 synthetic forced 0 under freestanding pin (parity
 * CMD; no config BAR bait); soft resource_star/select_bars return 0 under
 * freestanding pin (iomap bait parity hostish; soft arrays keep inventory
 * internal only); init scrub pre-sticky-pins entire hostish pool under gate0
 * when 8168 present. Fail-closed CF8/iomap; hostish SKIP denser honesty;
 * never REAL BAR under gate0; once greppable PASS/READY/NOOP + iomap/BDF/
 * hostish SKIP/BAR inventory. Soft!=product; hybrid Soft!=product; G-AC-1.
 * Residual lean (prior / UDX handoff eng / gate0 zero-touch):
 * freestanding SKIP (hostish probe SKIP hybrid, dead freestanding, or
 * inventory-only 8168) means soft must not thrash BAR - no CF8 size probe,
 * no BAR grab/scrub, no live resource_star leak, no select_bars second-owner
 * mask. Soft inventory BAR phys internal only; exposed resource_star +
 * select_bars + soft_cfg BAR fail-closed 0 under freestanding pin. Comments
 * use resource_star (never star-then-slash mid-comment footgun). Soft!=product
 * ASCII only (no unicode not-equals). G-AC-1.
 * Residual lean deepen (this pass / DDI/UDX inventory eng / layout_ver ≥ 1):
 * Soft pci_dev residual front stamps layout_ver + udx_eng / ddi_inv /
 * product_open / bar_inv_only / gate0_pin + hdr_type from devmgr inventory.
 * Seeds future DDI door SCAN/GET/OPEN + UDX host match (mint OPEN later).
 * Soft inventory BAR phys only; never product MMIO/IRQ/DMA cap mint.
 * Grep: linux_pci_soft: soft residual lean PASS
 * Grep: linux_pci_soft: soft residual lean UDX eng
 * Grep: linux_pci_soft: soft residual lean DDI inventory
 * Eng residual only - dual DoD OPEN.
 * No product MMIO map / IRQ install / DMA cap mint here.
 *
 * Soft!=Linux layout (why EMU bind skips .ko probe):
 *   - Host RHEL 5.14 pci_dev: vendor @ ~0x3c, resource[] after embedded
 *     struct device, drvdata at dev.driver_data - multi-KB object.
 *   - Soft: compact fields at front; parallel BAR arrays; no struct device.
 * Inlined r8169 accesses (pci_resource_*, pci_set_drvdata) need a future
 * probe-shaped blob - see docs/PCI_DEV_SOFT_LAYOUT.md. Do not pass this
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
    u8  u8HdrType; /* inventory hdr_type bits 6:0; soft bookkeep */
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
    /*
     * layout_ver ≥ 1 residual lean for DDI/UDX inventory eng (Soft!=product).
     * Stamps only - not Linux pci_dev ABI; not product cap mint. G-AC-1.
     */
    u32 u32Magic;       /* LINUX_PCI_SOFT_PCI_DEV_MAGIC */
    u32 u32LayoutVer;   /* LINUX_PCI_SOFT_LAYOUT_VER */
    u8  u8SoftOnly;     /* always 1 - Soft!=product */
    u8  u8UdxEng;       /* always 1 - future UDX host lean surface */
    u8  u8DdiInv;       /* always 1 - DDI inventory seed surface */
    u8  u8ProductOpen;  /* always 1 - Dual DoD A/B product OPEN */
    u8  u8BarInvOnly;   /* 1 = BAR phys inventory-only (no soft claim) */
    u8  u8Gate0Pin;     /* 1 when freestanding zero-touch pin applied */
    u8  u8WireSafe;     /* always 1 - no freestanding BAR thrash from soft */
    u8  u8PadRes;       /* reserved residual pad */
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
 * Prefer: linux_ksym_register("__pci_register_driver", __pci_register_driver) ...
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
 * Soft bodies with Linux C names - ksym / modules resolve these symbols.
 * Arguments are void * so F2 stubs and soft callers share one signature.
 * Soft!=ABI-stable struct layout under the pointer.
 */

/** Linux-shaped: __pci_register_driver(drv, owner, mod_name) -> soft register. */
int  __pci_register_driver(void *drv, void *owner, const char *mod_name);
int  pci_register_driver(void *drv);
void pci_unregister_driver(void *drv);

int  pci_enable_device(void *dev);
void pci_disable_device(void *dev);
void pci_set_master(void *dev);
void pci_clear_master(void *dev);
int  pci_request_regions(void *dev, const char *szName);
void pci_release_regions(void *dev);
/**
 * Residual beyond main: singular BAR claim - same zero-touch (no BAR grab).
 * Gap E deepen: gate0 8168 soft-success leaves u8Regions=0 (no claim fight).
 */
int  pci_request_region(void *dev, int nBar, const char *szName);
void pci_release_region(void *dev, int nBar);

/**
 * Soft config space. Non-hybrid: type-0 CF8/CFC (bus/devfn from soft pci_dev).
 * Gate0 hybrid 10ec:8168: soft inventory dword only (no CF8 ADDR thrash);
 * write_config soft-success NOOP. Soft!=product; hybrid Soft!=product.
 * Residual beyond main: word/dword R/W + BDF-level CF8 refuse.
 * Gap E deepen: BDF refuse dual-source (inventory + soft-bound 8168).
 * Grep: linux_pci_soft: soft read_config NOOP hybrid
 * Grep: linux_pci_soft: soft write_config NOOP hybrid
 * Grep: linux_pci_soft: soft cf8 BDF refuse hybrid
 */
int  pci_read_config_byte(void *dev, int nWhere, u8 *pVal);
int  pci_read_config_word(void *dev, int nWhere, u16 *pVal);
int  pci_read_config_dword(void *dev, int nWhere, u32 *pVal);
int  pci_write_config_byte(void *dev, int nWhere, u8 u8Val);
int  pci_write_config_word(void *dev, int nWhere, u16 u16Val);
int  pci_write_config_dword(void *dev, int nWhere, u32 u32Val);

/** Soft IRQ vector bookkeeping (alloc -> 1; vector 0 -> irq 11). */
int  pci_alloc_irq_vectors(void *dev, unsigned int uMin, unsigned int uMax,
                           unsigned int uFlags);
int  pci_irq_vector(void *dev, unsigned int uNr);

/** Soft BAR select bitmask from inventory flags (IORESOURCE_MEM/IO). */
int  pci_select_bars(void *dev, unsigned long ulFlags);

/** Soft PM / link / bus helpers - return 0 (no-op soft). */
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
 * kernel #PF and greps FAULT once then still halts. Soft!=product; no recovery.
 * Defined in linux_pci_soft.c (BSS zero). Parent probe path may raise/clear.
 */
extern volatile u32 g_u32SoftHostishProbeInflight;

int  linux_pci_soft_last_probe_mode(void);
int  linux_pci_soft_last_probe_st(void);
void linux_pci_soft_note_probe(u16 u16Vend, u16 u16Dev, int nMode, int nSt);

/**
 * Force soft EMU bind for inventory VID:DID without requiring .ko probe.
 * Used after module init if netdev soft is still 0 (layout / id_table miss).
 * Returns number of new binds. Soft!=product; greppable:
 *   linux_pci_soft: soft force emu 10ec:8168 PASS|SKIP
 */
u32  linux_pci_soft_force_emu_bind(u16 u16Vend, u16 u16Dev);

/**
 * Gate0 hybrid safety: non-zero if soft ksym may issue real CF8 R/W or
 * pci_iomap for this soft/hostish pci_dev.
 * Gate0 (handoff==0): always 0 for 10ec:8168 (freestanding sole BAR).
 * Gate1: 0 while rtl8168_ready(); 1 after freestanding quiesce.
 * Never REAL probe 10ec:8168 when handoff==0 (try_real + match skip).
 *
 * Grep: linux_pci_soft: soft hybrid zero-touch PASS|READY
 * Grep: linux_pci_soft: soft hybrid ... NOOP ... Soft!=product
 * Grep: linux_pci_soft: soft cf8 write NOOP hybrid
 * Grep: linux_pci_soft: soft enable NOOP hybrid
 * Grep: linux_pci_soft: soft disable NOOP hybrid
 * Grep: linux_pci_soft: soft clear_master NOOP hybrid
 * Grep: linux_pci_soft: soft set_master NOOP hybrid
 * Grep: linux_pci_soft: soft write_config NOOP hybrid
 * Grep: linux_pci_soft: soft read_config NOOP hybrid
 * Grep: linux_pci_soft: soft status_clear NOOP hybrid
 * Grep: linux_pci_soft: soft request_regions NOOP hybrid
 * Grep: linux_pci_soft: soft release_regions NOOP hybrid
 * Grep: linux_dma_soft: soft pci_iomap NOOP hybrid
 * Grep: linux_pci_soft: soft hostish probe SKIP hybrid
 *
 * Residual (gate0 Soft!=product; Dual DoD hybrid SOFT zero-touch; Gap E):
 *   enable/disable/set_master/clear_master/write_config/status_clear
 *   -> soft bookkeep only for 10ec:8168 (no CF8 CMD rewrite / thrash).
 *   read_config -> soft inventory dword only (no CF8 ADDR thrash).
 *   request_regions / request_region -> soft success, no BAR grab (u8Regions=0).
 *   disable / release_regions -> never BAR0..5 config scrub (no 0x10..0x24).
 *   hostish fill + REAL probe -> SKIP hybrid (no second live BAR owner).
 *   Soft EMU bind stable (match + force_emu); REAL only when handoff gated.
 *   Hostish IRQ helpers fail-closed (no soft-front scribble / #PF risk).
 *   Residual beyond main: BDF-level CF8 refuse + word/dword config NOOP.
 *   Residual deepen (prior):
 *     - BDF CF8 refuse dual-source (inventory + soft-bound 8168)
 *     - enable / request_region hard-pin u8Regions=0 on gate0 8168
 *     - soft fill BAR phys inventory only (claim flags cleared)
 *     - once PASS/READY/NOOP lamps (no storms)
 *     - hw_touch_ok fail-closed by VID:DID *and* freestanding 8168 BDF
 *     - exclusive iomap refuse once lamp (pci_iomap consumers honor return 0)
 *     - soft_cfg 64-bit BAR upper from inventory phys (no CF8 size probe)
 *     - unified hostish probe SKIP once helper (fill/try_real/match)
 *   Residual deepen (prior dual-DoD OPEN):
 *     - BDF CF8 refuse triple-source (inventory + soft-bound + hostish-pool)
 *     - gate1 BDF refuse while rtl8168_ready() (not only gate0)
 *     - hostish fill / try_real / match SKIP by VID *or* freestanding BDF
 *     - soft fill claim-pin by BDF even if VID mislabeled
 *     - pci_select_bars hostish fail-closed (no soft-front walk)
 *     - once greppable PASS/READY/NOOP + iomap/BDF/hostish SKIP lamps
 *   Residual deepen (prior dual-DoD OPEN):
 *     - VID-agnostic freestanding BDF pin (soft-bound + hostish slip)
 *     - unified inventory-fn zero-touch refuse (fill/try_real/match)
 *     - claim-pin defense-in-depth VID|BDF on enable/request_region
 *     - pci_resource_* BAR inventory-only once lamp (no CF8 size probe)
 *     - pointer zero-touch choke (lpcis_dev_fs8168_zero_touch) for CF8/iomap
 *     - hostish select_bars fail-closed 0 for freestanding 8168 BDF
 *     - hostish pci_resource_* return 0 under freestanding pin
 *   Residual deepen (prior dual-DoD OPEN):
 *     - unified refuse-fire (lpcis_zero_touch_refuse_fire_once): CF8 BDF +
 *       iomap + PASS|READY lamps from cfg, hw_touch, hostish SKIP, resource
 *     - fill claim-pin via inventory-fn zero-touch (VID|BDF gate0/gate1)
 *     - soft select_bars under freestanding pin: BAR inventory lamp + claim-pin
 *     - soft inventory BAR phys only; hostish SKIP co-lights refuse-fire
 *   Residual deepen (prior dual-DoD OPEN):
 *     - soft inventory BAR co-light refuse-fire (soft resource_*, select_bars,
 *       soft_cfg under freestanding pin) - greps PASS|READY from soft-only
 *     - soft_pin + cf8_noop co-fire PASS|READY+BDF+iomap once
 *     - EMU bind claim-pin via inventory-fn zero-touch (VID|BDF gate0/gate1)
 *     - fail-closed CF8/iomap; hostish SKIP; once greppable PASS/READY/NOOP
 *   Residual deepen (prior / dual-DoD OPEN):
 *     - refuse-fire co-lights BAR inventory honesty from every CF8/iomap/BDF
 *       path (NULL-dev bar lamp; pure cfg_* greps BAR inventory only)
 *     - inventory full-walk BDF source (1b) when find_by_id misses
 *     - force-EMU already-bound re-pin (u8Regions=0 + refuse-fire)
 *     - hostish resource_* co-lights SKIP (parity with select_bars)
 *     - IRQ alloc/vector + reset_bus fail-closed under freestanding pin
 *   Residual deepen (prior / dual-DoD OPEN):
 *     - pure CF8 BDF refuse re-pins soft-bound at freestanding BDF
 *     - hostish pool scrub under freestanding pin (kill live slip fill)
 *     - match already-bound re-pin + refuse-fire + hostish scrub
 *     - PM/wake/link fail-closed under freestanding pin
 *     - disable/master/release soft_pin co-light under freestanding pin
 *     - init inventory freestanding scrub (refuse + hostish kill + re-pin)
 *     - fail-closed CF8/iomap; soft inventory BAR phys only; hostish SKIP;
 *       once greppable PASS/READY/NOOP lamps
 *   Residual deepen (prior / dual-DoD OPEN):
 *     - hostish is_hostish pool-membership fail-closed (scrubbed still hostish)
 *     - hostish scrub zeros BAR phys in killed blobs (no iomap bait)
 *     - repin-all soft freestanding slots (sibling claim lag)
 *     - refuse-fire reentrancy guard + one-shot co-repin-all
 *     - register/force-EMU/EMU-bind post-scan scrub (BAR zero + repin-all)
 *     - status_clear + release_region soft_pin under freestanding pin
 *     - hostish drvdata get/set fail-closed under freestanding pin
 *   Residual deepen (prior / dual-DoD OPEN):
 *     - denser hostish SKIP honesty (scrub+repin every SKIP; no REAL BAR)
 *     - residual non-live hostish BAR bait kill under freestanding pin
 *     - clear enable_cnt/driver/drvdata REAL hooks on hostish scrub kill
 *     - hw_touch refuse denser soft_pin + scrub + hostish SKIP co-light
 *     - fill/try_real/match denser SKIP residual (never REAL probe gate0)
 *     - enable/request hostish freestanding co-light SKIP residual denser
 *     - fail-closed CF8/iomap; soft inventory BAR phys only; hostish SKIP;
 *       once greppable PASS/READY/NOOP + iomap/BDF/BAR inventory lamps
 *   Residual deepen (prior / dual-DoD OPEN):
 *     - denser hostish kill (DMA/bus/devfn/VID identity bait zero)
 *     - soft_pin hostish freestanding kill+SKIP (no early-return 0)
 *     - request_region singular hostish SKIP parity with plural
 *     - try_real FAIL kill hooks (no residual BAR bait leave-behind)
 *     - soft_cfg/disable/master/status/release hostish SKIP co-light
 *     - init scrub without inventory (VID gate0 kill lag close)
 *     - fail-closed CF8/iomap; soft inventory BAR phys only; hostish SKIP;
 *       once greppable PASS/READY/NOOP + iomap/BDF/BAR inventory lamps
 *   Residual deepen (prior / dual-DoD OPEN):
 *     - soft_pin full claim triad (u8Regions+u8Enabled+u8Master=0)
 *     - denser hostish kill class/subsys + bus[]/DMA-mask aux zero
 *     - enable/set_master/clear_master hostish freestanding soft_pin+SKIP
 *     - resource_star, IRQ/PM soft freestanding soft_pin denser
 *     - init scrub no-inv also repin-all soft freestanding
 *     - force-EMU already-bound refuse-fire+scrub denser
 *     - fail-closed CF8/iomap; soft inventory BAR phys only; hostish SKIP;
 *       never REAL BAR under gate0; once greppable PASS/READY/NOOP lamps
 *   Residual deepen (prior / dual-DoD OPEN):
 *     - soft_pin extended bookkeep (claim triad + u8IrqAllocated+msi=0)
 *     - hostish kill full-blob memset (all residual offset bait)
 *     - hostish range-membership fail-closed (mid-blob still hostish)
 *     - select_bars/drvdata hostish freestanding soft_pin+SKIP
 *     - try_real SKIP scrub+repin denser; never REAL BAR under gate0
 *     - init scrub inventory-ready-no-8168 still scrub+repin
 *     - match/force-EMU already-bound repin-all denser
 *     - fail-closed CF8/iomap; soft inventory BAR phys only; hostish SKIP;
 *       once greppable PASS/READY/NOOP + iomap/BDF/BAR inventory lamps
 *   Residual deepen (prior / dual-DoD OPEN):
 *     - soft_pin soft REAL-hook clear (driver=NULL under freestanding pin)
 *     - enable/set_master pin-first (never assign claim under freestanding pin)
 *     - soft_cfg hostish denser soft_pin; write_config denser soft_pin+SKIP
 *     - init scrub gate0 refuse-fire even without inv/8168 (policy lamp)
 *     - try_real FAIL denser refuse-fire co-light; never REAL BAR under gate0
 *     - fail-closed CF8/iomap; soft inventory BAR phys only; hostish SKIP;
 *       once greppable PASS/READY/NOOP + iomap/BDF/BAR inventory lamps
 *   Residual deepen (prior / dual-DoD OPEN):
 *     - BDF/VID via blob_base (mid-blob fail-closed; never (u8*)dev as base)
 *     - sticky hostish freestanding pin (scrub/kill/fill-refuse survives memset)
 *     - zero_touch honors sticky pin (scrubbed zero identity still CF8/iomap NOOP)
 *     - refuse_dense + hw_touch denser sticky pin; never REAL BAR under gate0
 *     - fail-closed CF8/iomap; hostish SKIP denser honesty; once lamps only
 *   Residual deepen (prior / dual-DoD OPEN / R0 class):
 *     - sticky freestanding 8168 BDF once-known (source 0 pure CF8 refuse;
 *       inventory lag / pre-soft-bind still fail-closed - no ADDR thrash -> R0)
 *     - soft_cfg CMD synthetic forced 0 under freestanding pin
 *     - hostish SKIP honesty denser force-EMU already-bound + EMU bind
 *     - note_bdf from inventory/fn/fill/soft_pin/scrub
 *     - fail-closed CF8/iomap; never REAL BAR under gate0; once lamps only
 *   Residual deepen (prior / dual-DoD OPEN / R0 class H4 denser):
 *     - note_bdf never poisons sticky with 0:0.0 host-bridge class
 *     - soft_cfg BAR 0x10..0x24 synthetic forced 0 under freestanding pin
 *     - soft resource_star/select_bars fail-closed 0 under freestanding pin
 *     - init scrub gate0 hostish pool pre-sticky pin when 8168 present
 *     - fail-closed CF8/iomap; never REAL BAR under gate0; once lamps only
 *   Residual lean (this pass / UDX handoff eng / gate0 zero-touch):
 *     - freestanding SKIP / dead freestanding => soft must not thrash BAR
 *     - resource_star + select_bars + soft_cfg BAR fail-closed under pin
 *     - comments use resource_star (never star-slash mid-comment footgun)
 *     - Soft!=product ASCII only; G-AC-1; once residual lean PASS lamp
 *   Once-lamps only - no kprintf storms. Hybrid Soft!=product / G-AC-1.
 * Grep pattern: linux_pci_soft: soft hybrid zero-touch
 * Grep pattern: linux_pci_soft: soft hybrid zero-touch PASS|READY
 * Grep pattern: linux_pci_soft: soft hybrid ... NOOP ... Soft!=product
 * Grep pattern: linux_pci_soft: soft residual lean PASS
 * Grep pattern: linux_pci_soft: soft cf8 BDF refuse hybrid
 * Grep pattern: linux_pci_soft: soft iomap refuse hybrid
 * Grep pattern: linux_pci_soft: soft BAR inventory only hybrid
 * Grep pattern: linux_pci_soft: soft hostish probe SKIP hybrid
 */
int  linux_pci_soft_hw_touch_ok(void *dev);

/**
 * Once serial lamp for hybrid zero-touch policy (gate0 REAL skip + CF8/iomap).
 * PASS (gate0): CF8 R/W + iomap + BAR NOOP + BDF triple-source refuse +
 * VID-agnostic freestanding BDF pin + pointer zero-touch choke +
 * unified refuse-fire from every refuse path +
 * VID+BDF hw_touch fail-closed + word/dword config NOOP +
 * request_region u8Regions=0 + claim-pin VID|BDF + hostish SKIP VID|BDF +
 * hostish select_bars/resource fail-closed + fill inventory-fn claim-pin +
 * soft select_bars BAR inventory pin + soft inventory BAR co-light +
 * soft_pin/cf8_noop co-fire + EMU bind inventory-fn pin +
 * BAR honesty from every refuse + inventory full-walk 1b +
 * force-EMU already-bound re-pin + hostish resource SKIP +
 * IRQ/reset fail-closed + CF8 BDF re-pin + hostish pool scrub +
 * match already-bound re-pin + PM/wake/link fail-closed +
 * disable/master soft_pin + init freestanding scrub +
 * hostish pool-membership fail-closed + scrub zeros BAR phys +
 * repin-all soft freestanding + register/force-EMU post scrub +
 * status_clear soft_pin + hostish drvdata refuse +
 * denser hostish SKIP residual (scrub+repin every SKIP) +
 * residual non-live BAR bait kill + clear enable/driver/drvdata hooks +
 * hw_touch denser soft_pin+scrub + fill/try_real denser SKIP +
 * enable/request hostish freestanding SKIP + never REAL probe gate0 +
 * denser kill DMA/bus/devfn/VID/class/subsys identity bait +
 * bus[]/DMA-mask aux zero + soft_pin full claim triad +
 * soft_pin extended bookkeep IRQ/msi + soft driver REAL-hook clear +
 * hostish full-blob memset + hostish range-membership fail-closed +
 * soft_pin hostish freestanding kill+SKIP +
 * enable/set_master pin-first (no claim under freestanding pin) +
 * request_region singular SKIP parity + try_real FAIL kill hooks +
 * try_real FAIL refuse-fire co-light + try_real SKIP scrub+repin +
 * soft_cfg/write denser soft_pin+SKIP +
 * soft_cfg/disable/master/status/release hostish SKIP +
 * enable/master/clear_master hostish soft_pin +
 * select_bars/drvdata hostish soft_pin +
 * resource/IRQ/PM soft_pin denser +
 * init scrub without inventory / no-8168 VID kill+repin +
 * init gate0 refuse-fire without inv/8168 (policy lamp honesty) +
 * match/force-EMU already-bound repin-all denser +
 * force-EMU already-bound refuse-fire+scrub denser +
 * BDF/VID via blob_base mid-blob safe + sticky hostish freestanding pin +
 * zero_touch pin honor (scrubbed zero identity still CF8/iomap NOOP) +
 * refuse_dense/hw_touch sticky pin residual denser +
 * sticky freestanding 8168 BDF once-known (R0 class pure CF8 refuse) +
 * soft_cfg CMD synthetic forced 0 under freestanding pin +
 * force-EMU/EMU hostish SKIP honesty denser +
 * note_bdf never 0:0.0 poison (R0 H4 denser) +
 * soft_cfg BAR synthetic forced 0 under freestanding pin +
 * soft resource_star/select_bars fail-closed 0 under freestanding pin +
 * init scrub gate0 hostish pool pre-sticky pin +
 * residual lean freestanding SKIP/dead freestanding no BAR thrash +
 * residual lean PASS once lamp (UDX handoff eng) +
 * residual lean DDI/UDX inventory eng stamps (layout_ver ≥ 1);
 * hybrid Soft!=product; G-AC-1.
 * READY (gate1): CF8/iomap NOOP while freestanding ready (BDF refuse too).
 * Grep: linux_pci_soft: soft hybrid zero-touch PASS|READY
 * Grep: linux_pci_soft: soft hybrid zero-touch
 * Grep: linux_pci_soft: soft residual lean PASS
 */
void linux_pci_soft_zero_touch_lamp_once(void);

/**
 * Once-lamp soft residual lean honesty (layout / DDI inventory / UDX eng).
 * Soft!=product; hard-capped (no stamp storms). Returns 1 if lamp fired.
 * Grep: linux_pci_soft: soft residual lean PASS
 * Grep: linux_pci_soft: soft residual lean UDX eng
 * Grep: linux_pci_soft: soft residual lean DDI inventory
 * Grep: linux_pci_soft: soft residual layout
 */
int  linux_pci_soft_residual_lean_lamp_once(void);

/** Soft layout residual lean stamp (LINUX_PCI_SOFT_LAYOUT_VER) or 0. */
u32  linux_pci_soft_layout_ver(void);

/** Always 1: residual lean surface for future UDX host eng (Soft!=product). */
int  linux_pci_soft_udx_eng(void);

/** Always 1: residual lean surface for DDI inventory seed (Soft!=product). */
int  linux_pci_soft_ddi_inv(void);

/** Always 1: soft never thrash freestanding BAR (Soft!=product). */
int  linux_pci_soft_wire_safe(void);

/**
 * Non-zero when soft pci_dev residual shape is ready for DDI/UDX inventory eng:
 * layout_ver stamps + BAR inventory arrays + register/match surface.
 * Soft!=product; eng residual only (not Dual DoD close).
 */
int  linux_pci_soft_udx_shape_ok(void);

#ifdef __cplusplus
}
#endif
