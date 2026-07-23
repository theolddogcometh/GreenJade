/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1523: bump pointer alloc on caller buffer state.
 *
 * Surface (unique symbols):
 *   void *gj_arena_bump_alloc(unsigned char *base, size_t cap, size_t *used,
 *                             size_t need, size_t align);
 *     — Bump-allocate need bytes from [base, base+cap) at alignment
 *       align (power of two, or 0/1 = byte). Advances *used. Returns
 *       pointer into the buffer, or NULL on OOM / NULL args / bad
 *       align / overflow. Does not zero memory.
 *   void *__gj_arena_bump_alloc  (alias)
 *   __libcgj_batch1523_marker = "libcgj-batch1523"
 *
 * Distinct from gj_arena_alloc (batch176 struct), gj_arena_bump
 * (batch545), gj_bump_alloc (batch282). Unique gj_arena_bump_alloc
 * surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1523_marker[] = "libcgj-batch1523";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Return 1 if uAlign is 0, 1, or a power of two; else 0.
 */
static int
b1523_align_ok(size_t uAlign)
{
	if (uAlign == 0u || uAlign == 1u) {
		return 1;
	}
	return (uAlign & (uAlign - 1u)) == 0u ? 1 : 0;
}

/*
 * Round uOff up to the next multiple of uAlign (0/1 or power of 2).
 * Returns 0 and sets *pOut on success; -1 on size_t overflow.
 */
static int
b1523_align_up(size_t uOff, size_t uAlign, size_t *pOut)
{
	size_t uMask;
	size_t uAdj;

	if (uAlign <= 1u) {
		*pOut = uOff;
		return 0;
	}
	uMask = uAlign - 1u;
	uAdj = (uOff + uMask) & ~uMask;
	if (uAdj < uOff) {
		return -1;
	}
	*pOut = uAdj;
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_arena_bump_alloc — bump-allocate need@align from caller buffer state.
 *
 * pBase:  buffer base (caller-owned)
 * cbCap:  total capacity in bytes
 * pUsed:  in/out watermark (advanced on success)
 * cbNeed: payload size (0 still aligns *used and returns that pointer)
 * uAlign: 0/1 or power of two
 *
 * Returns pointer into buffer, or NULL on failure.
 */
void *
gj_arena_bump_alloc(unsigned char *pBase, size_t cbCap, size_t *pUsed,
		    size_t cbNeed, size_t uAlign)
{
	size_t uStart;
	size_t uEnd;
	size_t uCur;

	if (pBase == NULL || pUsed == NULL) {
		return NULL;
	}
	if (!b1523_align_ok(uAlign)) {
		return NULL;
	}
	uCur = *pUsed;
	if (b1523_align_up(uCur, uAlign, &uStart) != 0) {
		return NULL;
	}
	if (uStart > cbCap) {
		return NULL;
	}
	if (cbNeed > cbCap - uStart) {
		return NULL;
	}
	uEnd = uStart + cbNeed;
	*pUsed = uEnd;
	return (void *)(pBase + uStart);
}

/* ---- underscored alias ------------------------------------------------- */

void *__gj_arena_bump_alloc(unsigned char *pBase, size_t cbCap, size_t *pUsed,
			    size_t cbNeed, size_t uAlign)
    __attribute__((alias("gj_arena_bump_alloc")));
