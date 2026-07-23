/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Virtio transport discovery + modern PCI + virtqueues (clean-room OASIS).
 */
#pragma once

#include <gj/error.h>
#include <gj/types.h>

#define GJ_VIRTIO_PCI_VENDOR 0x1AF4u
#define GJ_VIRTIO_PCI_DEV_NET_MODERN   0x1041u
#define GJ_VIRTIO_PCI_DEV_BLK_MODERN   0x1042u
#define GJ_VIRTIO_PCI_DEV_CONSOLE      0x1043u
#define GJ_VIRTIO_PCI_DEV_SCSI_MODERN  0x1048u
#define GJ_VIRTIO_PCI_DEV_GPU_MODERN   0x1050u
#define GJ_VIRTIO_PCI_DEV_INPUT        0x1052u

/* virtio_pci_cap.cfg_type */
#define GJ_VIRTIO_PCI_CAP_COMMON_CFG 1
#define GJ_VIRTIO_PCI_CAP_NOTIFY_CFG 2
#define GJ_VIRTIO_PCI_CAP_ISR_CFG    3
#define GJ_VIRTIO_PCI_CAP_DEVICE_CFG 4
#define GJ_VIRTIO_PCI_CAP_PCI_CFG    5

/* device status bits */
#define GJ_VIRTIO_S_ACKNOWLEDGE 1u
#define GJ_VIRTIO_S_DRIVER      2u
#define GJ_VIRTIO_S_DRIVER_OK   4u
#define GJ_VIRTIO_S_FEATURES_OK 8u
#define GJ_VIRTIO_S_FAILED      128u

#define GJ_VIRTIO_F_VERSION_1   (1ull << 32)

struct gj_virtio_dev {
    u16  u16Vendor;
    u16  u16Device;
    u8   u8Bus;
    u8   u8Slot;
    u8   u8Func;
    u8   u8Pad;
    u64  u64Bar[6];
    u32  u32BarIsIo; /* bit i set if BAR i is I/O */
    u32  u32Kind;    /* 0 unknown, 1 net, 2 blk, 3 gpu, 4 input, 5 console */
    /* Modern capability MMIO bases (0 if missing) */
    volatile u8 *pCommon;
    volatile u8 *pNotify;
    volatile u8 *pIsr;
    volatile u8 *pDevice;
    u32  u32NotifyMult;
    u32  u32NumQueues;
};

#define GJ_VIRTIO_MAX_DEVS 16
#define GJ_VIRTQ_MAX_SIZE  256

struct gj_virtq_desc {
    u64 u64Addr;
    u32 u32Len;
    u16 u16Flags;
    u16 u16Next;
} __attribute__((packed));

#define GJ_VIRTQ_DESC_F_NEXT  1
#define GJ_VIRTQ_DESC_F_WRITE 2

struct gj_virtq_avail {
    u16 u16Flags;
    u16 u16Idx;
    u16 aRing[GJ_VIRTQ_MAX_SIZE];
} __attribute__((packed));

struct gj_virtq_used_elem {
    u32 u32Id;
    u32 u32Len;
} __attribute__((packed));

struct gj_virtq_used {
    u16 u16Flags;
    u16 u16Idx;
    struct gj_virtq_used_elem aRing[GJ_VIRTQ_MAX_SIZE];
} __attribute__((packed));

struct gj_virtq {
    u16                     u16Size;
    u16                     u16FreeHead;
    u16                     u16NumFree;
    u16                     u16LastUsed;
    u16                     u16QueueIdx;
    u16                     u16NotifyOff;
    struct gj_virtq_desc   *pDesc;
    struct gj_virtq_avail  *pAvail;
    struct gj_virtq_used   *pUsed;
    gj_paddr_t              paDesc;
    gj_paddr_t              paAvail;
    gj_paddr_t              paUsed;
    struct gj_virtio_dev   *pDev;
};

/** Userspace/UDX ring export (net, blk, …). */
struct gj_virtq_export {
    u16 u16Which;
    u16 u16Size;
    u16 u16QueueIdx;
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

/** Bounce / DMA pool export for userspace payload buffers. */
#define GJ_VIRTQ_DMA_MAX_SLOTS 8u
struct gj_virtq_dma_export {
    u32 u32NSlots;
    u32 u32SlotSz;
    u64 u64VaBase;
    u64 aPa[GJ_VIRTQ_DMA_MAX_SLOTS];
    u32 u32Ready;
};

void virtio_init(void);
u32  virtio_pci_scan(void);
u32  virtio_dev_count(void);
struct gj_virtio_dev *virtio_dev_get(u32 u32Index);

/** Parse modern caps, enable BME/MEM, leave device in reset. */
gj_status_t virtio_pci_setup(struct gj_virtio_dev *pDev);

/** Feature negotiate VERSION_1 (+ optional mask). */
gj_status_t virtio_negotiate(struct gj_virtio_dev *pDev, u64 u64WantFeatures);

/** Allocate and enable queue index u16Q. */
gj_status_t virtio_q_setup(struct gj_virtio_dev *pDev, struct gj_virtq *pQ,
                           u16 u16QIdx, u16 u16Size);

/** Allocate one free descriptor (does not push avail). Returns head or -1. */
int virtio_q_alloc_desc(struct gj_virtq *pQ);
/** Push descriptor head onto avail ring (userspace may have filled desc fields). */
int virtio_q_push_head(struct gj_virtq *pQ, u16 u16Head);

/** Push one contiguous guest buffer; fWrite=1 for device-write. Returns desc head or -1. */
int virtio_q_add(struct gj_virtq *pQ, gj_paddr_t pa, u32 u32Len, int fWrite);

/**
 * Two-descriptor chain (req then resp). Typical virtio-gpu control:
 * desc0 device-read, desc1 device-write.
 */
int virtio_q_add2(struct gj_virtq *pQ, gj_paddr_t pa0, u32 u32Len0, int fWrite0,
                  gj_paddr_t pa1, u32 u32Len1, int fWrite1);

/**
 * Three-descriptor chain (hdr + data + status). Typical virtio-blk:
 * desc0 device-read, desc1 R or W, desc2 device-write status.
 */
int virtio_q_add3(struct gj_virtq *pQ, gj_paddr_t pa0, u32 u32Len0, int fWrite0,
                  gj_paddr_t pa1, u32 u32Len1, int fWrite1,
                  gj_paddr_t pa2, u32 u32Len2, int fWrite2);

/** Notify device of available buffers. */
void virtio_q_kick(struct gj_virtq *pQ);

/**
 * Poll used ring until at least one completion or u32Spins exhausted.
 * Returns bytes used or -1 on timeout. Optional *pOutId = head desc id.
 */
i32 virtio_q_poll(struct gj_virtq *pQ, u32 u32Spins);
i32 virtio_q_poll_id(struct gj_virtq *pQ, u32 u32Spins, u32 *pOutId);

/** Free descriptors available for AVAIL push. */
u16 virtio_q_num_free(struct gj_virtq *pQ);
/** Reap up to max used completions without spinning wait. Returns reaped. */
u32 virtio_q_reap(struct gj_virtq *pQ, u32 u32Max);

void virtio_set_status(struct gj_virtio_dev *pDev, u8 u8Status);
u8   virtio_get_status(struct gj_virtio_dev *pDev);
