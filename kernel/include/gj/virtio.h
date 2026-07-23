/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Virtio transport discovery + modern PCI + split virtqueues (clean-room OASIS).
 * Pure C11 freestanding. Dual MIT OR Apache-2.0 only. No Linux virtio source,
 * no GPL paste — layout constants match public OASIS virtio 1.x (read as
 * specification numbers, not as a copy of any implementation).
 *
 * Scope of this header:
 *   - PCI modern-cap discovery (vendor 0x1AF4, modern device IDs)
 *   - Common-cfg feature negotiate + soft ladder
 *   - Split virtqueue setup (desc | avail | used) + kick/poll/reap
 *   - Ring / DMA export shapes for UDX / door MAP_RING consumers
 *
 * Not in this header (device-class modules):
 *   virtio_blk.h / virtio_net.h / virtio_gpu.h / virtio_input.h / virtio_scsi.h
 * AArch64 soft virtio-mmio (arch tree) is a separate scaffold, not PCI.
 *
 * Bring-up lifecycle (product drivers):
 *   1. virtio_init()          — zero inventory
 *   2. virtio_pci_scan()      — enumerate BDF + BARs; set u32Kind
 *   3. virtio_pci_setup()     — walk vendor caps → pCommon/pNotify/pIsr/pDevice
 *   4. virtio_negotiate[_soft]— FEATURES_OK; snapshot on pDev
 *   5. virtio_q_setup()       — per-queue desc/avail/used + enable verify
 *   6. virtio_driver_ok()     — OR DRIVER_OK; device may process avail
 *   7. virtio_q_add{,2,3} + kick + poll/reap  — I/O path
 *
 * Soft product depth (common-cfg / queues; greppable logs in virtio_pci.c):
 *   - modern common-cfg cap walk + soft reset (status clear spin)
 *   - feature read/write helpers + soft negotiate ladder
 *   - queue soft size clamp, disable-before-setup, enable verify
 *
 * Greppable product markers (prefix-stable; see virtio_pci.c logs):
 *   virtio: scan PASS
 *   virtio: modern common@
 *   virtio: features
 *   virtio: features soft
 *   virtio: q soft
 *   virtio: q%u size=
 *   virtio: driver_ok
 *
 * greppable: GJ_VIRTIO_PCI_ GJ_VIRTIO_S_ GJ_VIRTIO_F_ GJ_VIRTQ_
 */
#pragma once

#include <gj/error.h>
#include <gj/types.h>

/* ---- PCI identity (modern virtio; transitional IDs not used on product) - */

/** PCI vendor ID for all virtio devices (OASIS). */
#define GJ_VIRTIO_PCI_VENDOR 0x1AF4u
/** Modern network (virtio-net). */
#define GJ_VIRTIO_PCI_DEV_NET_MODERN   0x1041u
/** Modern block (virtio-blk). */
#define GJ_VIRTIO_PCI_DEV_BLK_MODERN   0x1042u
/** Console (inventory only on product path). */
#define GJ_VIRTIO_PCI_DEV_CONSOLE      0x1043u
/** Modern SCSI HBA (virtio-scsi). */
#define GJ_VIRTIO_PCI_DEV_SCSI_MODERN  0x1048u
/** Modern GPU (virtio-gpu). */
#define GJ_VIRTIO_PCI_DEV_GPU_MODERN   0x1050u
/** Input (keyboard / tablet / relative). */
#define GJ_VIRTIO_PCI_DEV_INPUT        0x1052u

/*
 * virtio_pci_cap.cfg_type (vendor capability structures).
 * setup walks the PCI cap list and maps each type into pCommon/pNotify/…
 */
#define GJ_VIRTIO_PCI_CAP_COMMON_CFG 1 /* device-wide common config */
#define GJ_VIRTIO_PCI_CAP_NOTIFY_CFG 2 /* queue notify MMIO + multiplier */
#define GJ_VIRTIO_PCI_CAP_ISR_CFG    3 /* ISR status (read to ack) */
#define GJ_VIRTIO_PCI_CAP_DEVICE_CFG 4 /* device-type config space */
#define GJ_VIRTIO_PCI_CAP_PCI_CFG    5 /* PCI cfg access window (unused soft) */

/*
 * Modern virtio_pci_common_cfg field offsets (OASIS; greppable GJ_VIRTIO_PCI_COMMON_*).
 * Drivers may use these with pDev->pCommon for soft feature/queue peeks.
 * Widths: most are u32; MSIX/NUMQ/STATUS/CFGGEN/Q_* are u16 or u8 as noted
 * in OASIS — GreenJade peeks via byte-addressed MMIO helpers in virtio_pci.c.
 */
#define GJ_VIRTIO_PCI_COMMON_DFSELECT 0u  /* device_feature_select (u32) */
#define GJ_VIRTIO_PCI_COMMON_DF       4u  /* device_feature (u32) */
#define GJ_VIRTIO_PCI_COMMON_GFSELECT 8u  /* driver_feature_select (u32) */
#define GJ_VIRTIO_PCI_COMMON_GF       12u /* driver_feature (u32) */
#define GJ_VIRTIO_PCI_COMMON_MSIX     16u /* msix_config (u16) */
#define GJ_VIRTIO_PCI_COMMON_NUMQ     18u /* num_queues (u16) */
#define GJ_VIRTIO_PCI_COMMON_STATUS   20u /* device_status (u8) */
#define GJ_VIRTIO_PCI_COMMON_CFGGEN   21u /* config_generation (u8) */
#define GJ_VIRTIO_PCI_COMMON_Q_SELECT 22u /* queue_select (u16) */
#define GJ_VIRTIO_PCI_COMMON_Q_SIZE   24u /* queue_size (u16) */
#define GJ_VIRTIO_PCI_COMMON_Q_MSIX   26u /* queue_msix_vector (u16) */
#define GJ_VIRTIO_PCI_COMMON_Q_ENABLE 28u /* queue_enable (u16) */
#define GJ_VIRTIO_PCI_COMMON_Q_NOFF   30u /* queue_notify_off (u16) */
#define GJ_VIRTIO_PCI_COMMON_Q_DESC   32u /* queue_desc (u64 guest PA) */
#define GJ_VIRTIO_PCI_COMMON_Q_DRIVER 40u /* queue_driver / avail (u64) */
#define GJ_VIRTIO_PCI_COMMON_Q_DEVICE 48u /* queue_device / used (u64) */

/*
 * Device status bits (common-cfg device_status).
 * Product path: reset → ACK → DRIVER → FEATURES_OK → queues → DRIVER_OK.
 * FAILED is sticky failure; soft reset clears via status=0 spin.
 */
#define GJ_VIRTIO_S_ACKNOWLEDGE 1u
#define GJ_VIRTIO_S_DRIVER      2u
#define GJ_VIRTIO_S_DRIVER_OK   4u
#define GJ_VIRTIO_S_FEATURES_OK 8u
#define GJ_VIRTIO_S_FAILED      128u

/*
 * Transport / common feature bits (device-independent, OASIS virtio 1.x).
 * Bits 0–23 are device-class; 24–27 reserved; 28+ are transport.
 * Product negotiate always wants VERSION_1; optional mask for class bits.
 */
#define GJ_VIRTIO_F_RING_INDIRECT_DESC (1ull << 28) /* indirect desc tables */
#define GJ_VIRTIO_F_RING_EVENT_IDX     (1ull << 29) /* used_event / avail_event */
#define GJ_VIRTIO_F_VERSION_1          (1ull << 32) /* virtio 1.0+ layout */
#define GJ_VIRTIO_F_ACCESS_PLATFORM    (1ull << 33) /* IOMMU / platform DMA */
#define GJ_VIRTIO_F_RING_PACKED        (1ull << 34) /* packed VQ (not used) */
#define GJ_VIRTIO_F_IN_ORDER           (1ull << 35) /* in-order completion */
#define GJ_VIRTIO_F_ORDER_PLATFORM     (1ull << 36) /* platform memory order */
#define GJ_VIRTIO_F_SR_IOV             (1ull << 37) /* SR-IOV (inventory) */
#define GJ_VIRTIO_F_NOTIFICATION_DATA  (1ull << 38) /* notify data payload */

/** Soft reset spin bound (common-cfg status clear wait). */
#define GJ_VIRTIO_RESET_SPINS 1000000u

/*
 * gj_virtio_dev.u32Kind — inventory class after scan (not a PCI ID).
 * 0 unknown / unmatched modern ID; 1..6 match product device classes.
 */
#define GJ_VIRTIO_KIND_UNKNOWN 0u
#define GJ_VIRTIO_KIND_NET     1u
#define GJ_VIRTIO_KIND_BLK     2u
#define GJ_VIRTIO_KIND_GPU     3u
#define GJ_VIRTIO_KIND_INPUT   4u
#define GJ_VIRTIO_KIND_CONSOLE 5u
#define GJ_VIRTIO_KIND_SCSI    6u

/**
 * One discovered virtio PCI function after virtio_pci_scan / setup.
 *
 * Identity: BDF + vendor/device + u32Kind.
 * BARs: raw config BAR values (memory PA base when !I/O); u32BarIsIo bit i
 * means BAR i is I/O space (modern product path uses memory BARs only).
 *
 * Modern MMIO: pCommon / pNotify / pIsr / pDevice are identity-mapped
 * capability bases (NULL/0 if cap missing). u32NotifyMult scales
 * queue_notify_off for the notify write address.
 *
 * Features: after successful negotiate*, u64FeaturesDev is what the device
 * offered; u64FeaturesDrv is the guest-accepted snapshot used by
 * virtio_features_has / class drivers.
 *
 * fModern: set after virtio_pci_setup finds common-cfg; legacy-only devices
 * leave soft paths no-op / soft-fail without writing unknown MMIO.
 */
struct gj_virtio_dev {
    u16  u16Vendor;
    u16  u16Device;
    u8   u8Bus;
    u8   u8Slot;
    u8   u8Func;
    u8   fModern; /* 1 after successful modern common-cfg setup */
    u64  u64Bar[6];
    u32  u32BarIsIo; /* bit i set if BAR i is I/O */
    u32  u32Kind;    /* GJ_VIRTIO_KIND_* */
    /* Modern capability MMIO bases (0 if missing) */
    volatile u8 *pCommon;
    volatile u8 *pNotify;
    volatile u8 *pIsr;
    volatile u8 *pDevice;
    u32  u32NotifyMult; /* notify_off * mult → notify MMIO offset */
    u32  u32NumQueues;  /* common-cfg num_queues snapshot */
    /* Feature snapshot after last successful negotiate (soft helpers) */
    u64  u64FeaturesDev;
    u64  u64FeaturesDrv;
};

/** Max PCI virtio functions retained in the scan inventory. */
#define GJ_VIRTIO_MAX_DEVS 16
/**
 * Max split-VQ size for static ring structs in this header.
 * Drivers may request smaller; q_setup soft-clamps to device max and this.
 */
#define GJ_VIRTQ_MAX_SIZE  256

/**
 * One split virtqueue descriptor (16 bytes, OASIS).
 * Chains: u16Flags NEXT + u16Next; device-write buffers set WRITE.
 */
struct gj_virtq_desc {
    u64 u64Addr;  /* guest physical address of buffer */
    u32 u32Len;   /* buffer length in bytes */
    u16 u16Flags; /* GJ_VIRTQ_DESC_F_* */
    u16 u16Next;  /* next desc index when NEXT set */
} __attribute__((packed));

#define GJ_VIRTQ_DESC_F_NEXT  1 /* chain continues at u16Next */
#define GJ_VIRTQ_DESC_F_WRITE 2 /* device writes this buffer */

/**
 * Available ring (driver → device). u16Idx is free-running (mod 2^16);
 * aRing[] holds descriptor heads. Flags: NO_INTERRUPT is device-side only
 * on used; avail flags left 0 on product soft path.
 */
struct gj_virtq_avail {
    u16 u16Flags;
    u16 u16Idx;
    u16 aRing[GJ_VIRTQ_MAX_SIZE];
} __attribute__((packed));

/** One used-ring completion: head id + total bytes written by device. */
struct gj_virtq_used_elem {
    u32 u32Id;
    u32 u32Len;
} __attribute__((packed));

/**
 * Used ring (device → driver). Driver tracks last seen idx in
 * gj_virtq.u16LastUsed for poll/reap without races against free-running idx.
 */
struct gj_virtq_used {
    u16 u16Flags;
    u16 u16Idx;
    struct gj_virtq_used_elem aRing[GJ_VIRTQ_MAX_SIZE];
} __attribute__((packed));

/**
 * Driver-side virtqueue state (split layout, non-packed).
 *
 * Free list: u16FreeHead / u16NumFree chain unused descriptors via u16Next.
 * Notify: u16NotifyOff from common-cfg; kick writes pNotify + off*mult.
 * PAs: programmed into common-cfg queue_desc/driver/device; must be
 * DMA-reachable (identity map on T0 QEMU).
 */
struct gj_virtq {
    u16                     u16Size;      /* enabled queue size (power of 2) */
    u16                     u16FreeHead;  /* free-list head desc index */
    u16                     u16NumFree;   /* free descriptors remaining */
    u16                     u16LastUsed;  /* last reaped used idx */
    u16                     u16QueueIdx;  /* queue_select index */
    u16                     u16NotifyOff; /* common-cfg queue_notify_off */
    struct gj_virtq_desc   *pDesc;
    struct gj_virtq_avail  *pAvail;
    struct gj_virtq_used   *pUsed;
    gj_paddr_t              paDesc;
    gj_paddr_t              paAvail;
    gj_paddr_t              paUsed;
    struct gj_virtio_dev   *pDev;
};

/**
 * Userspace/UDX ring export (net, blk, …).
 * MAP_RING maps three pages at vaBase: desc | avail | used; byte offsets
 * in u32Off* are relative to that vaBase. Snapshot of free list is at
 * export time only — userspace must not assume live free counts without
 * its own protocol. u32Ready non-zero when rings are live for MAP.
 */
struct gj_virtq_export {
    u16 u16Which;      /* class-local queue selector (e.g. net 0=RX 1=TX) */
    u16 u16Size;
    u16 u16QueueIdx;   /* transport queue index */
    u16 u16NotifyOff;
    u64 u64PaDesc;
    u64 u64PaAvail;
    u64 u64PaUsed;
    u32 u32NotifyMult;
    u32 u32Ready;
    /** Byte offsets from MAP_RING vaBase: desc | avail | used */
    u32 u32OffDesc;
    u32 u32OffAvail;
    u32 u32OffUsed;
    u16 u16FreeHead; /* snapshot at export time */
    u16 u16NumFree;
};

/**
 * Bounce / DMA pool export for userspace payload buffers.
 * aPa[i] is the physical address for desc.addr; VA side is contiguous
 * from u64VaBase after map_dma_user. Slot size is class-defined.
 */
#define GJ_VIRTQ_DMA_MAX_SLOTS 8u
struct gj_virtq_dma_export {
    u32 u32NSlots;
    u32 u32SlotSz;
    u64 u64VaBase;
    u64 aPa[GJ_VIRTQ_DMA_MAX_SLOTS];
    u32 u32Ready;
};

/* ---- Inventory / PCI setup ---------------------------------------------- */

/** Zero scan inventory (call once early in boot before class probes). */
void virtio_init(void);

/**
 * Enumerate PCI for vendor 0x1AF4 modern IDs; fill g inventory.
 * Logs greppable "virtio: scan PASS" when c>0. Returns device count.
 */
u32  virtio_pci_scan(void);

/** Current inventory count (after last scan). */
u32  virtio_dev_count(void);

/**
 * Pointer to inventory slot u32Index, or NULL if out of range.
 * Valid for the lifetime of the kernel (static table).
 */
struct gj_virtio_dev *virtio_dev_get(u32 u32Index);

/**
 * Parse modern vendor caps, enable BME/MEM on the function, map cap MMIO,
 * leave device in reset (status=0 path). Soft-fails if legacy-only (no
 * common-cfg): returns error without programming unknown layout.
 */
gj_status_t virtio_pci_setup(struct gj_virtio_dev *pDev);

/**
 * Soft device reset via common-cfg status=0; spin until clear or
 * GJ_VIRTIO_RESET_SPINS. Clears feature snapshot on success.
 * No-op / soft log on missing pCommon.
 */
void virtio_reset(struct gj_virtio_dev *pDev);

/* ---- Feature negotiate -------------------------------------------------- */

/**
 * Feature negotiate VERSION_1 (+ optional class mask in u64WantFeatures).
 * Soft-resets first; writes driver features; requires FEATURES_OK stick.
 * On success stores snapshot on pDev (dev + drv). Logs "virtio: features".
 */
gj_status_t virtio_negotiate(struct gj_virtio_dev *pDev, u64 u64WantFeatures);

/**
 * Soft feature ladder: try each want mask in order until FEATURES_OK.
 * Always intersects with offered device features. On success stores
 * snapshot on pDev and optionally *pOutDrv. Logs greppable
 * "virtio: features soft". Use when a single want set is rejected by host.
 */
gj_status_t virtio_negotiate_soft(struct gj_virtio_dev *pDev,
                                  const u64 *pWants, u32 cWants,
                                  u64 *pOutDrv);

/** Read device-offered features (common-cfg device_feature select 0/1). */
u64 virtio_features_device(struct gj_virtio_dev *pDev);
/** Read guest/driver features currently programmed (common-cfg guest_feature). */
u64 virtio_features_driver(struct gj_virtio_dev *pDev);
/** Cached negotiated guest features (0 until negotiate succeeds). */
u64 virtio_features_negotiated(struct gj_virtio_dev *pDev);
/** Non-zero if negotiated snapshot includes u64Bit (single bit or mask). */
int virtio_features_has(struct gj_virtio_dev *pDev, u64 u64Bit);

/* ---- Virtqueue setup + I/O ---------------------------------------------- */

/**
 * Allocate and enable queue index u16QIdx (soft size clamp + enable verify).
 * Disables queue first if previously enabled. Fails if idx ≥ num_queues,
 * device max size 0, or page alloc fails. Logs "virtio: q soft".
 */
gj_status_t virtio_q_setup(struct gj_virtio_dev *pDev, struct gj_virtq *pQ,
                           u16 u16QIdx, u16 u16Size);

/**
 * Soft queue size query: select queue, return device queue_size max (0 if N/A).
 * Does not allocate or enable. Safe before setup for sizing decisions.
 */
u16 virtio_q_max_size(struct gj_virtio_dev *pDev, u16 u16QIdx);

/**
 * Soft-disable queue (queue_enable=0). Safe before re-setup; does not free
 * guest ring pages (caller owns lifetime of pQ buffers).
 */
void virtio_q_disable(struct gj_virtio_dev *pDev, u16 u16QIdx);

/**
 * Allocate one free descriptor (does not push avail). Returns head ≥0 or -1
 * if free list empty. Caller fills desc fields then push_head or class path.
 */
int virtio_q_alloc_desc(struct gj_virtq *pQ);

/**
 * Push descriptor head onto avail ring (userspace may have filled desc).
 * Updates avail.idx with appropriate memory barriers for device visibility.
 * Returns 0 on success, -1 if pQ invalid.
 */
int virtio_q_push_head(struct gj_virtq *pQ, u16 u16Head);

/**
 * Push one contiguous guest buffer; fWrite=1 for device-write.
 * Returns desc head ≥0 or -1 if no free desc.
 */
int virtio_q_add(struct gj_virtq *pQ, gj_paddr_t pa, u32 u32Len, int fWrite);

/**
 * Two-descriptor chain (req then resp). Typical virtio-gpu control:
 * desc0 device-read, desc1 device-write. Returns head of chain or -1.
 */
int virtio_q_add2(struct gj_virtq *pQ, gj_paddr_t pa0, u32 u32Len0, int fWrite0,
                  gj_paddr_t pa1, u32 u32Len1, int fWrite1);

/**
 * Three-descriptor chain (hdr + data + status). Typical virtio-blk:
 * desc0 device-read, desc1 R or W, desc2 device-write status.
 * Returns head of chain or -1.
 */
int virtio_q_add3(struct gj_virtq *pQ, gj_paddr_t pa0, u32 u32Len0, int fWrite0,
                  gj_paddr_t pa1, u32 u32Len1, int fWrite1,
                  gj_paddr_t pa2, u32 u32Len2, int fWrite2);

/**
 * Notify device of available buffers (write notify MMIO for this queue).
 * No-op if pNotify missing (should not happen after modern setup).
 */
void virtio_q_kick(struct gj_virtq *pQ);

/**
 * Poll used ring until at least one completion or u32Spins exhausted.
 * On success reaps one completion, frees desc chain, returns used length
 * (bytes device reported) or -1 on timeout. Optional *pOutId = head desc id
 * via virtio_q_poll_id.
 */
i32 virtio_q_poll(struct gj_virtq *pQ, u32 u32Spins);
i32 virtio_q_poll_id(struct gj_virtq *pQ, u32 u32Spins, u32 *pOutId);

/** Free descriptors available for AVAIL push (0 if pQ NULL). */
u16 virtio_q_num_free(struct gj_virtq *pQ);

/**
 * Reap up to u32Max used completions without spinning wait.
 * Returns number reaped (0 if none ready). Frees descriptor chains.
 */
u32 virtio_q_reap(struct gj_virtq *pQ, u32 u32Max);

/* ---- Status / ISR / config generation ----------------------------------- */

/** Write full device_status byte (common-cfg). Prefer driver_ok for product. */
void virtio_set_status(struct gj_virtio_dev *pDev, u8 u8Status);
/** Read device_status (0 if no common-cfg). */
u8   virtio_get_status(struct gj_virtio_dev *pDev);

/**
 * Soft DRIVER_OK: OR ACK|DRIVER|FEATURES_OK|DRIVER_OK onto status.
 * Greppable "virtio: driver_ok". Call after queues are enabled.
 */
void virtio_driver_ok(struct gj_virtio_dev *pDev);

/**
 * Read ISR status (ack by read per OASIS). Returns 0 if no ISR cap.
 * Bit0 queue interrupt; bit1 config change — class drivers may soft-ignore.
 */
u8 virtio_isr_read(struct gj_virtio_dev *pDev);

/**
 * Read common-cfg config_generation (soft config-change detect).
 * Device config consumers should re-read config if generation changes
 * across a multi-field read (OASIS rule).
 */
u8 virtio_config_generation(struct gj_virtio_dev *pDev);
