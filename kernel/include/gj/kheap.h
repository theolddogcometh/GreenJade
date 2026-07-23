/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Tiny kernel heap (freelist over PMM pages, HHDM-mapped).
 * Allocs zeroed; free scrubs payload. Max single alloc < one page payload.
 *
 * Greppable serial markers (implementation in kernel/mm/kheap.c):
 *   kheap: init
 *   kheap: stats used= free= free_blocks= soft_frag= soft_align= soft_unsplit=
 *   kheap: counters alloc= free= grow= split= fail= double_free=
 */
#pragma once

#include <gj/types.h>

/**
 * Snapshot of live heap accounting (cheap; no freelist walk for used/soft).
 * Free bytes / free_blocks are maintained on grow/alloc/free paths.
 */
struct kheap_stats {
    size_t cbUsed;        /* live payload bytes charged */
    size_t cbFree;        /* free-list payload sum */
    size_t cFreeBlocks;   /* free-list length */
    size_t cbSoftFrag;    /* live soft frag: align + unsplit waste */
    size_t cbSoftAlign;   /* live align waste (round-up - request) */
    size_t cbSoftUnsplit; /* live unsplit remainder absorbed into live */
    u64    cAlloc;        /* successful kheap_alloc */
    u64    cFree;         /* successful kheap_free */
    u64    cGrow;         /* PMM page grows */
    u64    cSplit;        /* free-block splits */
    u64    cFail;         /* failed alloc (OOM / oversize / uninit) */
    u64    cDoubleFree;   /* free of non-LIVE header */
};

void  kheap_init(void);

/** Zeroed allocation; NULL on OOM or oversized request. */
void *kheap_alloc(size_t cb);

/** Free; safe on NULL; double-free is a no-op (magic guard). */
void  kheap_free(void *p);

/** Bytes currently charged to live payloads (approx). */
size_t kheap_used_bytes(void);

/** Sum of free-list payload bytes. */
size_t kheap_free_bytes(void);

/** Free-list length (block count). */
size_t kheap_free_blocks(void);

/**
 * Soft (internal) fragmentation on live blocks:
 * align pad + unsplittable leftover absorbed into the live payload.
 */
size_t kheap_soft_frag_bytes(void);

/** Fill *pOut with current stats; no-op if pOut is NULL. */
void   kheap_get_stats(struct kheap_stats *pOut);

/** Emit greppable "kheap: stats …" / "kheap: counters …" lines. */
void   kheap_dump_stats(void);
