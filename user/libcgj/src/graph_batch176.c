/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch176: bump (linear) arena allocator.
 * Clean-room freestanding pure C — integer/pointer only (no SSE required;
 * builds under userspace -msse2). No third-party arena source was copied.
 *
 * Existing malloc / free / realloc / posix_memalign / aligned_alloc live in
 * malloc.c / graph_batch15.c and are NOT redefined here.
 *
 * This TU adds only unique symbols:
 *   typedef struct { unsigned char *base; size_t cap, used; } gj_arena_t;
 *   void  gj_arena_init(gj_arena_t *a, void *mem, size_t cap);
 *     — bind arena to caller-owned buffer [mem, mem+cap). used = 0.
 *   void *gj_arena_alloc(gj_arena_t *a, size_t n, size_t align);
 *     — bump-allocate n bytes with alignment align (power of two, or 0/1 =
 *       byte). Returns NULL on OOM / bad args / overflow. Does not zero.
 *   void  gj_arena_reset(gj_arena_t *a);
 *     — set used = 0 (retains base/cap; prior pointers become invalid).
 *   __gj_arena_init / __gj_arena_alloc / __gj_arena_reset  (aliases)
 *   __libcgj_batch176_marker = "libcgj-batch176"
 *
 * Notes:
 *   - No free of individual blocks; only reset reclaims.
 *   - No errno (freestanding-safe); failures return NULL.
 *   - align must be 0, 1, or a power of two; other values fail.
 */

#include <stddef.h>

const char __libcgj_batch176_marker[] = "libcgj-batch176";

/* Bump arena over a fixed external buffer. */
typedef struct {
	unsigned char *base;
	size_t cap;
	size_t used;
} gj_arena_t;

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Return 1 if uAlign is 0, 1, or a power of two; else 0.
 * 0 is treated as "byte align" (same as 1) by the allocator.
 */
static int
b176_align_ok(size_t uAlign)
{
	if (uAlign == 0u || uAlign == 1u) {
		return 1;
	}
	return (uAlign & (uAlign - 1u)) == 0u ? 1 : 0;
}

/*
 * Round uOff up to the next multiple of uAlign (uAlign is 0/1 or power of 2).
 * Returns 0 and sets *pOut on success; returns -1 on size_t overflow.
 */
static int
b176_align_up(size_t uOff, size_t uAlign, size_t *pOut)
{
	size_t uMask;
	size_t uAdj;

	if (uAlign <= 1u) {
		*pOut = uOff;
		return 0;
	}
	uMask = uAlign - 1u;
	uAdj = (uOff + uMask) & ~uMask;
	/* overflow: (uOff + uMask) wrapped */
	if (uAdj < uOff) {
		return -1;
	}
	*pOut = uAdj;
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_arena_init — bind arena to caller buffer and clear the watermark.
 * NULL a or NULL mem with non-zero cap leaves the arena zeroed/invalid.
 */
void
gj_arena_init(gj_arena_t *pA, void *pMem, size_t cbCap)
{
	if (pA == NULL) {
		return;
	}
	if (pMem == NULL && cbCap != 0u) {
		pA->base = NULL;
		pA->cap = 0u;
		pA->used = 0u;
		return;
	}
	pA->base = (unsigned char *)pMem;
	pA->cap = cbCap;
	pA->used = 0u;
}

void __gj_arena_init(gj_arena_t *pA, void *pMem, size_t cbCap)
    __attribute__((alias("gj_arena_init")));

/*
 * gj_arena_alloc — bump-allocate cbN bytes at alignment uAlign.
 * Returns pointer into the arena buffer, or NULL on failure.
 */
void *
gj_arena_alloc(gj_arena_t *pA, size_t cbN, size_t uAlign)
{
	size_t uStart;
	size_t uEnd;

	if (pA == NULL || pA->base == NULL) {
		return NULL;
	}
	if (!b176_align_ok(uAlign)) {
		return NULL;
	}
	/* n == 0: still advance to aligned offset, return that pointer
	 * without consuming payload bytes (common bump-arena convention). */
	if (b176_align_up(pA->used, uAlign, &uStart) != 0) {
		return NULL;
	}
	if (uStart > pA->cap) {
		return NULL;
	}
	/* end = start + n; reject overflow */
	if (cbN > pA->cap - uStart) {
		return NULL;
	}
	uEnd = uStart + cbN;
	pA->used = uEnd;
	return (void *)(pA->base + uStart);
}

void *__gj_arena_alloc(gj_arena_t *pA, size_t cbN, size_t uAlign)
    __attribute__((alias("gj_arena_alloc")));

/*
 * gj_arena_reset — discard all allocations; keep base/cap.
 */
void
gj_arena_reset(gj_arena_t *pA)
{
	if (pA == NULL) {
		return;
	}
	pA->used = 0u;
}

void __gj_arena_reset(gj_arena_t *pA)
    __attribute__((alias("gj_arena_reset")));
