/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * UDX virtqueue client helpers (host + freestanding shapes).
 * Kernel maps rings via GJ_NET_OP_MAP_RING / EXPORT_RING;
 * DMA via MAP_DMA; submit via DESC_ALLOC + user desc/avail + USER_AVAIL/KICK.
 * Store door mirrors the same export layout (EXPORT_RING / MAP_RING).
 *
 * Pure C, header-only programming helpers (no GPL, no OOP).
 * Soft client state: struct udx_virtq + attach from mapped export.
 *
 * Product path residual (Soft!=product; Dual DoD OPEN; G-AC-1):
 *   EXPORT_RING → MAP_RING (vaBase + off_*) → optional MAP_DMA
 *     → DESC_ALLOC (or soft free-list) → program desc/avail
 *     → USER_AVAIL (+ KICK bit) → USED_REAP
 * Soft path never mints product caps, never claims notify MMIO kick
 * as product, never freestanding class-driver product wire.
 *
 * Soft residual deepen (UDX ring clients; Soft!=product; G-AC-1; C2):
 *   attach residual   -- MAP_RING vaBase + export offs → soft client view;
 *                         free_head/num_free are export-time snapshot only
 *                         (Soft!=product: never invent free when num_free=0)
 *   detach residual   -- clear soft view; no product unmap / revoke claim
 *   export residual   -- layout mirror of gj_virtq_export (ABI stable);
 *                         map_layout gate (classic page offs Soft residual)
 *   dma residual      -- layout mirror of gj_virtq_dma_export + slot PA/VA;
 *                         Soft!=product: never mints DMA window caps
 *   desc residual     -- set / chain / soft free-list alloc+free (next chain)
 *   avail residual    -- userspace avail.idx push with mfence; != product
 *                         notify write (USER_AVAIL/KICK door owns kick)
 *   used residual     -- soft last_used cursor reap; optional free chain;
 *                         used_reap_free_n bulk free residual
 *   free_list residual-- soft free_head/num_free bookkeeping only; live
 *                         free counts stay protocol-owned (door DESC_ALLOC
 *                         or exclusive client ownership after attach);
 *                         free_list_reset rebuilds exclusive soft free only
 *   kick residual     -- soft avail push only; product kick OPEN via door
 *   notify residual   -- export notify_off*mult geometry observation only;
 *                         Soft!=product: never writes notify MMIO (product
 *                         kick remains USER_AVAIL/KICK / store KICK OPEN)
 *   mint honesty      -- header never mints MAP_RING / MAP_DMA / DESC caps,
 *                         never claims notify MMIO product kick, never
 *                         freestanding class-driver product wire
 *   freestanding thrash refuse -- no freestanding class re-enable / ring
 *                         thrash from this header; product = UDX+ABI
 *                         userspace ring clients (netstackd/storaged)
 *   dual_dod_open residual -- Dual DoD A/B OPEN; ring residual != product
 *                         close (G-AC-1; Soft!=product)
 *   product_host residual -- netstackd / storaged / UDX+ABI ring clients
 *                         (not freestanding kernel class thrash)
 *
 * Soft API honesty (Soft!=product; dual MIT OR Apache-2.0; G-AC-1):
 *   soft path = header inlines over MAP_RING VA + export snapshot
 *   product   = door USER_AVAIL/KICK + live free-list / notify OPEN
 *   free_list = soft bookkeeping; export snapshot != live kernel free
 *   mint      = 0 from this header (Soft!=product; caps OPEN elsewhere)
 *   Dual DoD  = OPEN (never closed from this header)
 * greppable: udx: virtq soft residual lean
 * greppable: udx: virtq soft residual attach
 * greppable: udx: virtq soft residual detach
 * greppable: udx: virtq soft residual export
 * greppable: udx: virtq soft residual map_layout
 * greppable: udx: virtq soft residual dma
 * greppable: udx: virtq soft residual desc
 * greppable: udx: virtq soft residual avail
 * greppable: udx: virtq soft residual used
 * greppable: udx: virtq soft residual free_list
 * greppable: udx: virtq soft residual kick
 * greppable: udx: virtq soft residual notify
 * greppable: udx: virtq soft residual mint
 * greppable: udx: virtq soft residual freestanding thrash refuse
 * greppable: udx: virtq soft residual dual_dod_open
 * greppable: udx: virtq soft residual product_host
 * greppable: udx: virtq soft api honesty
 * No stamp storms. No version stamp. No GJ_IMAGE_VERSION.
 * Dual MIT OR Apache-2.0. Soft!=product. G-AC-1. No GPL.
 */
#pragma once

#include <stddef.h>
#include <stdint.h>

#include <udx/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Soft residual lean layout version (eng only; Soft!=host ABI; not stamp).
 * Bump when client residual surface grows (attach/free_list/dma/reap).
 * Not GJ_IMAGE_VERSION. Soft!=product. stamp_storm=0.
 * ver1: export + attach + desc_set/chain + avail_push + used_reap_one
 * ver2: residual honesty catalog; dma_ok; detach; soft free_list;
 *       free_chain; used_reap_free / used_reap_n; dma slot PA/VA;
 *       add1/add2; shape_ok; product_host residual notes.
 * ver3: Soft!=product deepen (C2): honest free snapshot (no invent free);
 *       map_layout gate; free_list_reset exclusive soft rebuild;
 *       notify geometry residual (no MMIO write); mint / dual_dod_open /
 *       freestanding thrash refuse honesty; used_reap_free_n; export_ok
 *       requires pa_avail+pa_used; free_list_ok observation.
 * Grep: udx: virtq soft residual lean layout_ver=
 */
#define UDX_VIRTQ_LAYOUT_VER           3u

/** Max split-VQ size for static ring structs (matches GJ_VIRTQ_MAX_SIZE). */
#define UDX_VIRTQ_MAX_SIZE             256u

/** Soft page geometry note for MAP_RING (desc | avail | used pages). */
#define UDX_VIRTQ_PAGE_CB              4096u

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

/*
 * Avail / used ring flags (OASIS virtio 1.x shape; soft residual).
 * Soft client rarely sets these; product EVENT_IDX path remains OPEN.
 */
#define UDX_VIRTQ_AVAIL_F_NO_INTERRUPT 1u
#define UDX_VIRTQ_USED_F_NO_NOTIFY     1u

struct udx_virtq_avail {
    uint16_t flags;
    uint16_t idx;
    uint16_t ring[UDX_VIRTQ_MAX_SIZE]; /* max; use export.size / q.u16Size */
} __attribute__((packed));

struct udx_virtq_used_elem {
    uint32_t id;
    uint32_t len;
} __attribute__((packed));

struct udx_virtq_used {
    uint16_t flags;
    uint16_t idx;
    struct udx_virtq_used_elem ring[UDX_VIRTQ_MAX_SIZE];
} __attribute__((packed));

/* GreenJade net door ops used by UDX porters (soft residual catalog). */
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

/*
 * Store door ring ops residual (storaged / store_door; Soft!=product).
 * Layout of udx_virtq_export is shared; op numbers differ from net door.
 * greppable: udx: virtq soft residual product_host
 */
#define UDX_GJ_STORE_EXPORT_RING 9u
#define UDX_GJ_STORE_KICK        10u
#define UDX_GJ_STORE_RING_STATE  11u
#define UDX_GJ_STORE_MAP_RING    12u

/** USER_AVAIL flags */
#define UDX_USER_AVAIL_KICK        1u
#define UDX_USER_AVAIL_KERNEL_PUSH 2u

/**
 * Soft client view of a mapped split virtqueue.
 * Fill via udx_virtq_attach after MAP_RING grants VAs.
 * free_head / num_free are soft bookkeeping (export snapshot or
 * exclusive client free_list residual) — not a live kernel free claim.
 * greppable residual: udx: virtq soft residual free_list
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
 * Soft shape gate (observation + fail-closed; never Dual DoD close).
 * greppable residual: udx: virtq soft residual lean
 */
static inline int
udx_virtq_shape_ok(const struct udx_virtq *pQ)
{
    if (pQ == NULL || pQ->pDesc == NULL || pQ->pAvail == NULL ||
        pQ->pUsed == NULL) {
        return 0;
    }
    if (pQ->u16Size == 0 || pQ->u16Size > UDX_VIRTQ_MAX_SIZE) {
        return 0;
    }
    return 1;
}

/**
 * Host-side documentation smoke: validate export layout size.
 * Real MAP/KICK/AVAIL go through GJ_SYS_NET / store door on GreenJade.
 * Soft!=product: observation gate only; never mints EXPORT/MAP caps.
 * greppable residual: udx: virtq soft residual export
 * greppable residual: udx: virtq soft residual mint
 */
static inline int
udx_virtq_export_ok(const struct udx_virtq_export *p)
{
    if (p == NULL || p->ready == 0 || p->size == 0 || p->pa_desc == 0) {
        return 0;
    }
    /* Soft!=product honesty: split VQ needs all three ring PAs. */
    if (p->pa_avail == 0 || p->pa_used == 0) {
        return 0;
    }
    if (p->size > UDX_VIRTQ_MAX_SIZE) {
        return 0;
    }
    return 1;
}

/**
 * Soft MAP_RING map_layout residual (classic desc|avail|used pages).
 * Kernel export uses page-aligned, distinct offs (see virtio_*_map_q_user).
 * Soft!=product: geometric honesty only; never MAP_RING product mint.
 * greppable residual: udx: virtq soft residual map_layout
 * greppable residual: udx: virtq soft residual export
 */
static inline int
udx_virtq_export_map_layout_ok(const struct udx_virtq_export *p)
{
    if (!udx_virtq_export_ok(p)) {
        return 0;
    }
    if ((p->off_desc % UDX_VIRTQ_PAGE_CB) != 0u ||
        (p->off_avail % UDX_VIRTQ_PAGE_CB) != 0u ||
        (p->off_used % UDX_VIRTQ_PAGE_CB) != 0u) {
        return 0;
    }
    if (p->off_desc == p->off_avail || p->off_desc == p->off_used ||
        p->off_avail == p->off_used) {
        return 0;
    }
    return 1;
}

/**
 * Soft free_list snapshot gate (export-time residual; Soft!=product).
 * Non-zero when free_head/num_free are in-range for size.
 * greppable residual: udx: virtq soft residual free_list
 */
static inline int
udx_virtq_export_free_snapshot_ok(const struct udx_virtq_export *p)
{
    if (p == NULL || p->size == 0 || p->size > UDX_VIRTQ_MAX_SIZE) {
        return 0;
    }
    if (p->num_free > p->size) {
        return 0;
    }
    if (p->num_free != 0u && p->free_head >= p->size) {
        return 0;
    }
    return 1;
}

/**
 * Soft notify geometry residual (export notify_off * notify_mult).
 * Returns byte offset for product notify MMIO — Soft!=product never writes.
 * Product kick remains door USER_AVAIL/KICK / store KICK (OPEN).
 * greppable residual: udx: virtq soft residual notify
 * greppable residual: udx: virtq soft residual kick
 * greppable residual: udx: virtq soft residual mint
 */
static inline uint32_t
udx_virtq_notify_off_bytes(const struct udx_virtq_export *p)
{
    if (!udx_virtq_export_ok(p)) {
        return 0;
    }
    return (uint32_t)p->notify_off * p->notify_mult;
}

/**
 * Soft DMA export ready gate (MAP_DMA residual).
 * Soft!=product: observation only; never mints MAP_DMA / DMA window caps.
 * greppable residual: udx: virtq soft residual dma
 * greppable residual: udx: virtq soft residual mint
 */
static inline int
udx_virtq_dma_export_ok(const struct udx_virtq_dma_export *p)
{
    if (p == NULL || p->ready == 0 || p->n_slots == 0 || p->slot_sz == 0) {
        return 0;
    }
    if (p->n_slots > UDX_VIRTQ_DMA_MAX_SLOTS) {
        return 0;
    }
    if (p->pa[0] == 0 && p->va_base == 0) {
        return 0;
    }
    return 1;
}

/**
 * Soft DMA slot physical address (desc.addr residual).
 * Returns 0 if export not ready or slot out of range.
 */
static inline uint64_t
udx_virtq_dma_slot_pa(const struct udx_virtq_dma_export *p, uint32_t uSlot)
{
    if (!udx_virtq_dma_export_ok(p) || uSlot >= p->n_slots) {
        return 0;
    }
    return p->pa[uSlot];
}

/**
 * Soft DMA slot CPU VA (contiguous pool residual from va_base).
 * Returns NULL if export not ready / slot out of range / no va_base.
 */
static inline void *
udx_virtq_dma_slot_va(const struct udx_virtq_dma_export *p, uint32_t uSlot)
{
    if (!udx_virtq_dma_export_ok(p) || uSlot >= p->n_slots ||
        p->va_base == 0) {
        return NULL;
    }
    return (void *)(uintptr_t)(p->va_base +
                               (uint64_t)uSlot * (uint64_t)p->slot_sz);
}

/**
 * Attach soft client from MAP_RING base + export offsets.
 * pVaBase is the userspace VA returned by map; export supplies off_*.
 * free_head/num_free copied as soft free_list snapshot (Soft!=product).
 * Soft!=product honesty (C2): never invent free when export num_free==0
 * (live free stays door DESC_ALLOC or exclusive free_list_reset).
 * Requires classic MAP_RING map_layout (page-aligned distinct offs).
 * Returns UDX_OK or UDX_ERR_INVAL.
 * greppable residual: udx: virtq soft residual attach
 * greppable residual: udx: virtq soft residual free_list
 * greppable residual: udx: virtq soft residual map_layout
 * greppable residual: udx: virtq soft residual mint
 */
static inline udx_status_t
udx_virtq_attach(struct udx_virtq *pQ, void *pVaBase,
                 const struct udx_virtq_export *pEx)
{
    uint8_t *pBase;

    if (pQ == NULL || pVaBase == NULL ||
        !udx_virtq_export_map_layout_ok(pEx) ||
        !udx_virtq_export_free_snapshot_ok(pEx)) {
        return UDX_ERR_INVAL;
    }
    pBase = (uint8_t *)pVaBase;
    pQ->u16Size = pEx->size;
    pQ->u16LastUsedIdx = 0;
    /* Honest export snapshot only — Soft!=product never invents free. */
    pQ->u16FreeHead = pEx->free_head;
    pQ->u16NumFree = pEx->num_free;
    pQ->pDesc = (struct udx_virtq_desc *)(pBase + pEx->off_desc);
    pQ->pAvail = (struct udx_virtq_avail *)(pBase + pEx->off_avail);
    pQ->pUsed = (struct udx_virtq_used *)(pBase + pEx->off_used);
    return UDX_OK;
}

/**
 * Soft detach residual: clear client view only.
 * Does not unmap rings, revoke doors, or free DMA (product OPEN).
 * Soft!=product: never Phase-A revoke / cap destroy claim.
 * greppable residual: udx: virtq soft residual detach
 * greppable residual: udx: virtq soft residual mint
 */
static inline void
udx_virtq_detach(struct udx_virtq *pQ)
{
    if (pQ == NULL) {
        return;
    }
    pQ->u16Size = 0;
    pQ->u16LastUsedIdx = 0;
    pQ->u16FreeHead = 0;
    pQ->u16NumFree = 0;
    pQ->pDesc = NULL;
    pQ->pAvail = NULL;
    pQ->pUsed = NULL;
}

/** Soft free descriptor count residual (bookkeeping only; Soft!=product). */
static inline uint16_t
udx_virtq_num_free(const struct udx_virtq *pQ)
{
    if (pQ == NULL) {
        return 0;
    }
    return pQ->u16NumFree;
}

/**
 * Soft free_list observation residual (client bookkeeping bounds).
 * Non-zero when shape ok and free_head/num_free in-range for size.
 * Soft!=product: not a live kernel free claim.
 * greppable residual: udx: virtq soft residual free_list
 */
static inline int
udx_virtq_free_list_ok(const struct udx_virtq *pQ)
{
    if (!udx_virtq_shape_ok(pQ)) {
        return 0;
    }
    if (pQ->u16NumFree > pQ->u16Size) {
        return 0;
    }
    if (pQ->u16NumFree != 0u && pQ->u16FreeHead >= pQ->u16Size) {
        return 0;
    }
    return 1;
}

/**
 * Soft free_list_reset residual (exclusive client ownership only).
 * Rebuilds local free chain 0..size-1 via desc.next; free_head=0;
 * num_free=size. Soft!=product: never claims live kernel free list /
 * door DESC_ALLOC ownership. Prefer door DESC_ALLOC when kernel owns free.
 * Returns UDX_OK or UDX_ERR_INVAL.
 * greppable residual: udx: virtq soft residual free_list
 * greppable residual: udx: virtq soft residual mint
 */
static inline udx_status_t
udx_virtq_free_list_reset(struct udx_virtq *pQ)
{
    uint16_t i;

    if (!udx_virtq_shape_ok(pQ)) {
        return UDX_ERR_INVAL;
    }
    for (i = 0; i < pQ->u16Size; i++) {
        pQ->pDesc[i].addr = 0;
        pQ->pDesc[i].len = 0;
        pQ->pDesc[i].flags = 0;
        pQ->pDesc[i].next = (uint16_t)(i + 1u);
    }
    if (pQ->u16Size > 0u) {
        pQ->pDesc[pQ->u16Size - 1u].next = 0;
    }
    pQ->u16FreeHead = 0;
    pQ->u16NumFree = pQ->u16Size;
    return UDX_OK;
}

/**
 * Soft free_list alloc residual (mirror virtio_q_alloc_desc spirit).
 * Returns descriptor index or -1 if soft free list empty / shape bad.
 * Prefer door DESC_ALLOC when kernel owns the live free list.
 * greppable residual: udx: virtq soft residual free_list
 * greppable residual: udx: virtq soft residual desc
 */
static inline int
udx_virtq_desc_alloc(struct udx_virtq *pQ)
{
    uint16_t u16Head;

    if (!udx_virtq_shape_ok(pQ) || pQ->u16NumFree == 0) {
        return -1;
    }
    u16Head = pQ->u16FreeHead;
    if (u16Head >= pQ->u16Size) {
        return -1;
    }
    pQ->u16FreeHead = pQ->pDesc[u16Head].next;
    pQ->u16NumFree = (uint16_t)(pQ->u16NumFree - 1u);
    return (int)u16Head;
}

/**
 * Soft free one descriptor onto free_list (no chain walk).
 * greppable residual: udx: virtq soft residual free_list
 */
static inline void
udx_virtq_desc_free_one(struct udx_virtq *pQ, uint16_t u16Idx)
{
    if (!udx_virtq_shape_ok(pQ) || u16Idx >= pQ->u16Size) {
        return;
    }
    if (pQ->u16NumFree >= pQ->u16Size) {
        return;
    }
    pQ->pDesc[u16Idx].next = pQ->u16FreeHead;
    pQ->pDesc[u16Idx].flags = 0;
    pQ->pDesc[u16Idx].len = 0;
    pQ->pDesc[u16Idx].addr = 0;
    pQ->u16FreeHead = u16Idx;
    pQ->u16NumFree = (uint16_t)(pQ->u16NumFree + 1u);
}

/**
 * Soft free descriptor chain residual (NEXT walk; mirror kernel poll free).
 * head is used-ring id / chain head. Stops at first desc without F_NEXT.
 * greppable residual: udx: virtq soft residual free_list
 */
static inline void
udx_virtq_desc_free_chain(struct udx_virtq *pQ, uint16_t u16Head)
{
    uint16_t u16Cur;
    uint32_t u32Guard;

    if (!udx_virtq_shape_ok(pQ) || u16Head >= pQ->u16Size) {
        return;
    }
    u16Cur = u16Head;
    /* Bound walk by queue size (soft residual refuse infinite NEXT loops). */
    for (u32Guard = 0; u32Guard < (uint32_t)pQ->u16Size; u32Guard++) {
        uint16_t u16Next;
        uint16_t u16Flags;

        if (u16Cur >= pQ->u16Size) {
            break;
        }
        u16Next = pQ->pDesc[u16Cur].next;
        u16Flags = pQ->pDesc[u16Cur].flags;
        udx_virtq_desc_free_one(pQ, u16Cur);
        if ((u16Flags & UDX_VIRTQ_DESC_F_NEXT) == 0) {
            break;
        }
        u16Cur = u16Next;
    }
}

/**
 * Program one TX descriptor at mapped ring VA (userspace ring programming).
 * pDescBase = vaBase + off_desc from MAP_RING.
 * greppable residual: udx: virtq soft residual desc
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
 * Soft attach desc_set via struct udx_virtq (bounds-checked residual).
 * Returns UDX_OK or UDX_ERR_INVAL.
 */
static inline udx_status_t
udx_virtq_desc_set_q(struct udx_virtq *pQ, uint16_t head, uint64_t pa,
                     uint32_t len, uint16_t flags)
{
    if (!udx_virtq_shape_ok(pQ) || head >= pQ->u16Size || pa == 0 ||
        len == 0) {
        return UDX_ERR_INVAL;
    }
    udx_virtq_desc_set(pQ->pDesc, head, pa, len, flags);
    return UDX_OK;
}

/**
 * Push head onto avail ring at mapped VA (userspace avail programming).
 * Returns new avail.idx after push.
 * greppable residual: udx: virtq soft residual avail
 */
static inline uint16_t
udx_virtq_avail_push(struct udx_virtq_avail *pAvail, uint16_t qsize,
                     uint16_t head)
{
    uint16_t slot;

    if (pAvail == NULL || qsize == 0) {
        return 0;
    }
    __asm__ volatile("mfence" ::: "memory");
    slot = (uint16_t)(pAvail->idx % qsize);
    pAvail->ring[slot] = head;
    __asm__ volatile("mfence" ::: "memory");
    pAvail->idx = (uint16_t)(pAvail->idx + 1);
    __asm__ volatile("mfence" ::: "memory");
    return pAvail->idx;
}

/**
 * Soft attach: push via struct udx_virtq (avail residual only).
 * Product notify kick remains door USER_AVAIL/KICK (OPEN).
 * Soft!=product: never writes notify MMIO (see notify_off_bytes geometry).
 * greppable residual: udx: virtq soft residual kick
 * greppable residual: udx: virtq soft residual notify
 * greppable residual: udx: virtq soft residual mint
 */
static inline uint16_t
udx_virtq_kick_avail(struct udx_virtq *pQ, uint16_t head)
{
    if (!udx_virtq_shape_ok(pQ) || head >= pQ->u16Size) {
        return 0;
    }
    return udx_virtq_avail_push(pQ->pAvail, pQ->u16Size, head);
}

/**
 * Soft add1 residual: free_list alloc + desc_set + avail push.
 * Returns head index or -1. Does not door-kick (product kick OPEN).
 * Prefer DESC_ALLOC + USER_AVAIL when kernel owns free list / kick.
 * greppable residual: udx: virtq soft residual desc
 * greppable residual: udx: virtq soft residual avail
 */
static inline int
udx_virtq_add1(struct udx_virtq *pQ, uint64_t pa, uint32_t len,
               uint16_t flags)
{
    int iHead;

    if (!udx_virtq_shape_ok(pQ) || pa == 0 || len == 0) {
        return -1;
    }
    iHead = udx_virtq_desc_alloc(pQ);
    if (iHead < 0) {
        return -1;
    }
    udx_virtq_desc_set(pQ->pDesc, (uint16_t)iHead, pa, len, flags);
    (void)udx_virtq_kick_avail(pQ, (uint16_t)iHead);
    return iHead;
}

/**
 * Soft add2 residual: two-desc chain + avail push (head = first).
 * fWriteN non-zero sets F_WRITE on that segment. Returns head or -1.
 * greppable residual: udx: virtq soft residual desc
 */
static inline int
udx_virtq_add2(struct udx_virtq *pQ,
               uint64_t pa0, uint32_t len0, int fWrite0,
               uint64_t pa1, uint32_t len1, int fWrite1)
{
    int ia;
    int ib;
    uint16_t u16A;
    uint16_t u16B;
    uint16_t u16F0;
    uint16_t u16F1;

    if (!udx_virtq_shape_ok(pQ) || pa0 == 0 || pa1 == 0 || len0 == 0 ||
        len1 == 0) {
        return -1;
    }
    if (pQ->u16NumFree < 2u) {
        return -1;
    }
    ia = udx_virtq_desc_alloc(pQ);
    ib = udx_virtq_desc_alloc(pQ);
    if (ia < 0 || ib < 0) {
        /* Soft residual: best-effort return on partial alloc. */
        if (ia >= 0) {
            udx_virtq_desc_free_one(pQ, (uint16_t)ia);
        }
        if (ib >= 0) {
            udx_virtq_desc_free_one(pQ, (uint16_t)ib);
        }
        return -1;
    }
    u16A = (uint16_t)ia;
    u16B = (uint16_t)ib;
    u16F0 = (uint16_t)((fWrite0 ? UDX_VIRTQ_DESC_F_WRITE : 0u) |
                       UDX_VIRTQ_DESC_F_NEXT);
    u16F1 = (uint16_t)(fWrite1 ? UDX_VIRTQ_DESC_F_WRITE : 0u);
    udx_virtq_desc_chain(pQ->pDesc, u16A, u16B, pa0, len0, u16F0);
    udx_virtq_desc_set(pQ->pDesc, u16B, pa1, len1, u16F1);
    (void)udx_virtq_kick_avail(pQ, u16A);
    return (int)u16A;
}

/**
 * Reap one used element if available.
 * Returns 1 and fills *pu32Id and *pu32Len; 0 if ring empty vs last_used.
 * Does not free descriptor chain (see used_reap_free).
 * greppable residual: udx: virtq soft residual used
 */
static inline int
udx_virtq_used_reap_one(struct udx_virtq *pQ, uint32_t *pu32Id,
                        uint32_t *pu32Len)
{
    uint16_t u16Idx;
    uint16_t u16Slot;
    struct udx_virtq_used_elem *pEl;

    if (!udx_virtq_shape_ok(pQ)) {
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

/**
 * Reap one used element and soft free_list the descriptor chain.
 * Returns 1 on success (fills id/len); 0 if empty / shape bad.
 * Use when client owns free_list after attach; skip when door USED_REAP
 * owns free restoration (Soft!=product dual ownership honesty).
 * greppable residual: udx: virtq soft residual used
 * greppable residual: udx: virtq soft residual free_list
 */
static inline int
udx_virtq_used_reap_free(struct udx_virtq *pQ, uint32_t *pu32Id,
                         uint32_t *pu32Len)
{
    uint32_t u32Id;
    uint32_t u32Len;

    if (!udx_virtq_used_reap_one(pQ, &u32Id, &u32Len)) {
        return 0;
    }
    udx_virtq_desc_free_chain(pQ, (uint16_t)u32Id);
    if (pu32Id) {
        *pu32Id = u32Id;
    }
    if (pu32Len) {
        *pu32Len = u32Len;
    }
    return 1;
}

/**
 * Soft used_reap residual: reap up to u32Max used entries (no free).
 * Returns count reaped. Pair with desc_free_chain when client owns free.
 * Soft!=product: soft last_used cursor only; door USED_REAP remains OPEN.
 * greppable residual: udx: virtq soft residual used
 */
static inline uint32_t
udx_virtq_used_reap_n(struct udx_virtq *pQ, uint32_t u32Max)
{
    uint32_t n = 0;

    if (!udx_virtq_shape_ok(pQ) || u32Max == 0) {
        return 0;
    }
    while (n < u32Max) {
        if (!udx_virtq_used_reap_one(pQ, NULL, NULL)) {
            break;
        }
        n++;
    }
    return n;
}

/**
 * Soft used_reap_free residual bulk: reap up to u32Max and free chains.
 * Use only when exclusive client owns free_list after attach/reset.
 * Soft!=product dual ownership honesty: skip when door USED_REAP frees.
 * greppable residual: udx: virtq soft residual used
 * greppable residual: udx: virtq soft residual free_list
 */
static inline uint32_t
udx_virtq_used_reap_free_n(struct udx_virtq *pQ, uint32_t u32Max)
{
    uint32_t n = 0;

    if (!udx_virtq_shape_ok(pQ) || u32Max == 0) {
        return 0;
    }
    while (n < u32Max) {
        if (!udx_virtq_used_reap_free(pQ, NULL, NULL)) {
            break;
        }
        n++;
    }
    return n;
}

/**
 * How many used entries are pending vs soft last_used cursor.
 * Soft!=product observation only.
 * greppable residual: udx: virtq soft residual used
 */
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

/**
 * Soft Dual DoD OPEN residual seed (never closed from this header).
 * Returns 1 always — Dual DoD A/B remain OPEN (Soft!=product; G-AC-1).
 * greppable residual: udx: virtq soft residual dual_dod_open
 * greppable residual: udx: virtq soft residual product_host
 */
static inline int
udx_virtq_dual_dod_open(void)
{
    return 1;
}

/**
 * Soft mint honesty residual (header never mints product caps).
 * Returns 0 always — Soft!=product; MAP/KICK/DESC caps OPEN elsewhere.
 * greppable residual: udx: virtq soft residual mint
 * greppable residual: udx: virtq soft residual freestanding thrash refuse
 */
static inline int
udx_virtq_soft_mint_claim(void)
{
    return 0;
}

/**
 * Soft residual lean layout self-check (compile/host observation only).
 * Never gates product Dual DoD. Soft!=product. greppable: layout_ver=
 */
static inline uint32_t
udx_virtq_layout_ver(void)
{
    return UDX_VIRTQ_LAYOUT_VER;
}

#ifdef __cplusplus
}
#endif
