/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * UDX virtqueue client helpers (host + freestanding shapes).
 * Kernel maps rings via GJ_NET_OP_MAP_RING / EXPORT_RING;
 * DMA via MAP_DMA; submit via DESC_ALLOC + user desc/avail + USER_AVAIL/KICK.
 *
 * Pure C, header-only programming helpers (no GPL, no OOP).
 * Soft client state: struct udx_virtq + attach from mapped export.
 */
#pragma once

#include <stddef.h>
#include <stdint.h>

#include <udx/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Mirrors kernel gj_virtq_export (keep layout stable). */
struct udx_virtq_export {
    uint16_t which;
    uint16_t size;
    uint16_t queue_idx;
    uint16_t notify_off;
    uint64_t pa_desc;
    uint64_t pa_avail;
    uint64_t pa_used;
    uint32_t notify_mult;
    uint32_t ready;
    uint32_t off_desc;
    uint32_t off_avail;
    uint32_t off_used;
    uint16_t free_head;
    uint16_t num_free;
};

/** Mirrors kernel gj_virtq_dma_export */
#define UDX_VIRTQ_DMA_MAX_SLOTS 8u
struct udx_virtq_dma_export {
    uint32_t n_slots;
    uint32_t slot_sz;
    uint64_t va_base;
    uint64_t pa[UDX_VIRTQ_DMA_MAX_SLOTS];
    uint32_t ready;
};

/** Virtio split-ring descriptor (LE). */
struct udx_virtq_desc {
    uint64_t addr;
    uint32_t len;
    uint16_t flags;
    uint16_t next;
} __attribute__((packed));

#define UDX_VIRTQ_DESC_F_NEXT  1u
#define UDX_VIRTQ_DESC_F_WRITE 2u
#define UDX_VIRTQ_DESC_F_INDIRECT 4u

struct udx_virtq_avail {
    uint16_t flags;
    uint16_t idx;
    uint16_t ring[256]; /* max; use export.size / q.u16Size */
} __attribute__((packed));

struct udx_virtq_used_elem {
    uint32_t id;
    uint32_t len;
} __attribute__((packed));

struct udx_virtq_used {
    uint16_t flags;
    uint16_t idx;
    struct udx_virtq_used_elem ring[256];
} __attribute__((packed));

/* GreenJade net door ops used by UDX porters */
#define UDX_GJ_NET_EXPORT_RING 15u
#define UDX_GJ_NET_MAP_RING    16u
#define UDX_GJ_NET_KICK        17u
#define UDX_GJ_NET_AVAIL_PUSH  18u
#define UDX_GJ_NET_USED_REAP   19u
#define UDX_GJ_NET_RING_STATE  20u
#define UDX_GJ_NET_MAP_DMA     21u
#define UDX_GJ_NET_DESC_ALLOC  22u
#define UDX_GJ_NET_USER_AVAIL  23u
#define UDX_GJ_NET_BOUNCE_FILL 24u

/** USER_AVAIL flags */
#define UDX_USER_AVAIL_KICK        1u
#define UDX_USER_AVAIL_KERNEL_PUSH 2u

/**
 * Soft client view of a mapped split virtqueue.
 * Fill via udx_virtq_attach after MAP_RING grants VAs.
 */
struct udx_virtq {
    uint16_t                 u16Size;
    uint16_t                 u16LastUsedIdx;
    uint16_t                 u16FreeHead;
    uint16_t                 u16NumFree;
    struct udx_virtq_desc   *pDesc;
    struct udx_virtq_avail  *pAvail;
    struct udx_virtq_used   *pUsed;
};

/**
 * Host-side documentation smoke: validate export layout size.
 * Real MAP/KICK/AVAIL go through GJ_SYS_NET on GreenJade.
 */
static inline int
udx_virtq_export_ok(const struct udx_virtq_export *p)
{
    return p != NULL && p->ready != 0 && p->size != 0 && p->pa_desc != 0;
}

/**
 * Attach soft client from MAP_RING base + export offsets.
 * pVaBase is the userspace VA returned by map; export supplies off_*.
 * Returns UDX_OK or UDX_ERR_INVAL.
 */
static inline udx_status_t
udx_virtq_attach(struct udx_virtq *pQ, void *pVaBase,
                 const struct udx_virtq_export *pEx)
{
    uint8_t *pBase;

    if (pQ == NULL || pVaBase == NULL || !udx_virtq_export_ok(pEx)) {
        return UDX_ERR_INVAL;
    }
    if (pEx->size == 0 || pEx->size > 256u) {
        return UDX_ERR_INVAL;
    }
    pBase = (uint8_t *)pVaBase;
    pQ->u16Size = pEx->size;
    pQ->u16LastUsedIdx = 0;
    pQ->u16FreeHead = pEx->free_head;
    pQ->u16NumFree = pEx->num_free ? pEx->num_free : pEx->size;
    pQ->pDesc = (struct udx_virtq_desc *)(pBase + pEx->off_desc);
    pQ->pAvail = (struct udx_virtq_avail *)(pBase + pEx->off_avail);
    pQ->pUsed = (struct udx_virtq_used *)(pBase + pEx->off_used);
    return UDX_OK;
}

/**
 * Program one TX descriptor at mapped ring VA (userspace ring programming).
 * pDescBase = vaBase + off_desc from MAP_RING.
 */
static inline void
udx_virtq_desc_set(struct udx_virtq_desc *pDescBase, uint16_t head,
                   uint64_t pa, uint32_t len, uint16_t flags)
{
    struct udx_virtq_desc *d = pDescBase + head;

    d->addr = pa;
    d->len = len;
    d->flags = flags;
    d->next = 0;
}

/** Chain two descriptors (head → next) with F_NEXT on head. */
static inline void
udx_virtq_desc_chain(struct udx_virtq_desc *pDescBase, uint16_t head,
                     uint16_t next, uint64_t pa, uint32_t len, uint16_t flags)
{
    struct udx_virtq_desc *d = pDescBase + head;

    d->addr = pa;
    d->len = len;
    d->flags = (uint16_t)(flags | UDX_VIRTQ_DESC_F_NEXT);
    d->next = next;
}

/**
 * Push head onto avail ring at mapped VA (userspace avail programming).
 * Returns new avail.idx after push.
 */
static inline uint16_t
udx_virtq_avail_push(struct udx_virtq_avail *pAvail, uint16_t qsize,
                     uint16_t head)
{
    uint16_t slot;

    __asm__ volatile("mfence" ::: "memory");
    slot = (uint16_t)(pAvail->idx % qsize);
    pAvail->ring[slot] = head;
    __asm__ volatile("mfence" ::: "memory");
    pAvail->idx = (uint16_t)(pAvail->idx + 1);
    __asm__ volatile("mfence" ::: "memory");
    return pAvail->idx;
}

/** Soft attach: push via struct udx_virtq. */
static inline uint16_t
udx_virtq_kick_avail(struct udx_virtq *pQ, uint16_t head)
{
    if (pQ == NULL || pQ->pAvail == NULL || pQ->u16Size == 0) {
        return 0;
    }
    return udx_virtq_avail_push(pQ->pAvail, pQ->u16Size, head);
}

/**
 * Reap one used element if available.
 * Returns 1 and fills *pu32Id and *pu32Len; 0 if ring empty vs last_used.
 */
static inline int
udx_virtq_used_reap_one(struct udx_virtq *pQ, uint32_t *pu32Id,
                        uint32_t *pu32Len)
{
    uint16_t u16Idx;
    uint16_t u16Slot;
    struct udx_virtq_used_elem *pEl;

    if (pQ == NULL || pQ->pUsed == NULL || pQ->u16Size == 0) {
        return 0;
    }
    __asm__ volatile("mfence" ::: "memory");
    u16Idx = pQ->pUsed->idx;
    if (u16Idx == pQ->u16LastUsedIdx) {
        return 0;
    }
    u16Slot = (uint16_t)(pQ->u16LastUsedIdx % pQ->u16Size);
    pEl = &pQ->pUsed->ring[u16Slot];
    if (pu32Id) {
        *pu32Id = pEl->id;
    }
    if (pu32Len) {
        *pu32Len = pEl->len;
    }
    pQ->u16LastUsedIdx = (uint16_t)(pQ->u16LastUsedIdx + 1);
    return 1;
}

/** How many used entries are pending vs soft last_used cursor. */
static inline uint16_t
udx_virtq_used_pending(const struct udx_virtq *pQ)
{
    uint16_t u16Idx;

    if (pQ == NULL || pQ->pUsed == NULL) {
        return 0;
    }
    __asm__ volatile("mfence" ::: "memory");
    u16Idx = pQ->pUsed->idx;
    return (uint16_t)(u16Idx - pQ->u16LastUsedIdx);
}

#ifdef __cplusplus
}
#endif
