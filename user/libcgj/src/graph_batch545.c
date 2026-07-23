/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch545: freestanding arena bump over a raw buffer.
 * Clean-room pure C — integer/pointer only. Compiles with -ffreestanding
 * -msse2 -Wall -Wextra -Werror. No malloc, no errno, no libc string calls.
 * No third-party arena source was copied.
 *
 * Existing arena / bump surfaces (NOT redefined here):
 *   gj_arena_init / gj_arena_alloc / gj_arena_reset → graph_batch176.c
 *     (struct gj_arena_t { base, cap, used })
 *   gj_bump_init / gj_bump_alloc                   → graph_batch282.c
 *     (base, *used, cap, n, align) — different param order/names
 *
 * This TU adds only unique symbols:
 *   void *gj_arena_bump(unsigned char *base, size_t cap, size_t *off,
 *                        size_t need, size_t align);
 *     — bump-allocate need bytes from [base, base+cap) at alignment
 *       align (power of two, or 0/1 = byte). Advances *off. Returns
 *       pointer into the buffer, or NULL on OOM / NULL args / bad
 *       align / overflow. Does not zero memory.
 *   __gj_arena_bump  (alias)
 *   __libcgj_batch545_marker = "libcgj-batch545"
 *
 * Notes:
 *   - No free of individual blocks; set *off = 0 (or re-init) to reclaim.
 *   - No errno (freestanding-safe); failures return NULL.
 *   - align must be 0, 1, or a power of two; other values fail.
 */

#include <stddef.h>

const char __libcgj_batch545_marker[] = "libcgj-batch545";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Return 1 if uAlign is 0, 1, or a power of two; else 0.
 * 0 is treated as "byte align" (same as 1) by the allocator.
 */
static int
b545_align_ok(size_t uAlign)
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
b545_align_up(size_t uOff, size_t uAlign, size_t *pOut)
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
 * gj_arena_bump — bump-allocate need bytes at alignment align from caller
 * buffer [base, base+cap), advancing *off.
 *
 * Returns pointer into the buffer, or NULL on failure (NULL base/off,
 * non-power-of-two align other than 0/1, align overflow, or OOM).
 * need == 0 still aligns *off and returns that pointer without consuming
 * payload bytes (common bump-arena convention).
 */
void *
gj_arena_bump(unsigned char *base, size_t cap, size_t *off, size_t need,
	      size_t align)
{
	size_t uStart;
	size_t uEnd;
	size_t uCur;

	if (base == NULL || off == NULL) {
		return NULL;
	}
	if (!b545_align_ok(align)) {
		return NULL;
	}
	uCur = *off;
	if (b545_align_up(uCur, align, &uStart) != 0) {
		return NULL;
	}
	if (uStart > cap) {
		return NULL;
	}
	/* end = start + need; reject overflow / OOM */
	if (need > cap - uStart) {
		return NULL;
	}
	uEnd = uStart + need;
	*off = uEnd;
	return (void *)(base + uStart);
}

void *__gj_arena_bump(unsigned char *base, size_t cap, size_t *off, size_t need,
		      size_t align)
    __attribute__((alias("gj_arena_bump")));
