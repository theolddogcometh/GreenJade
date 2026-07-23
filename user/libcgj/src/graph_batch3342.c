/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3342: arena bump-alloc by offset (u-suffix).
 *
 * Surface (unique symbols):
 *   size_t gj_arena_alloc_u(size_t *used, size_t cap, size_t need,
 *                           size_t align);
 *     - Bump-allocate need bytes from a [0, cap) virtual span at
 *       alignment align (0/1 or power of two). Advances *used.
 *       Returns the start offset on success, or (size_t)-1 on
 *       failure (NULL used, bad align, overflow, or OOM). Does not
 *       require a base pointer — offset-only form.
 *   size_t __gj_arena_alloc_u  (alias)
 *   __libcgj_batch3342_marker = "libcgj-batch3342"
 *
 * Milestone 3350 exclusive continuum CREATE-ONLY (3341-3350). Distinct
 * from gj_arena_alloc (batch176 struct), gj_arena_bump (batch545), and
 * gj_arena_bump_alloc (batch1523) — unique gj_arena_alloc_u surface
 * only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3342_marker[] = "libcgj-batch3342";

/* Failure sentinel for offset-only bump alloc. */
#define B3342_FAIL  ((size_t)-1)

/* ---- freestanding helpers ---------------------------------------------- */

/* Return 1 if uAlign is 0, 1, or a power of two; else 0. */
static int
b3342_align_ok(size_t uAlign)
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
b3342_align_up(size_t uOff, size_t uAlign, size_t *pOut)
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

static size_t
b3342_alloc(size_t *pUsed, size_t cbCap, size_t cbNeed, size_t uAlign)
{
	size_t uStart;
	size_t uEnd;
	size_t uCur;

	if (pUsed == NULL) {
		return B3342_FAIL;
	}
	if (!b3342_align_ok(uAlign)) {
		return B3342_FAIL;
	}
	uCur = *pUsed;
	if (b3342_align_up(uCur, uAlign, &uStart) != 0) {
		return B3342_FAIL;
	}
	if (uStart > cbCap) {
		return B3342_FAIL;
	}
	if (cbNeed > cbCap - uStart) {
		return B3342_FAIL;
	}
	uEnd = uStart + cbNeed;
	*pUsed = uEnd;
	return uStart;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_arena_alloc_u - bump-allocate need@align; return start offset.
 *
 * used:  in/out watermark (advanced on success)
 * cap:   total capacity in bytes of the virtual span
 * need:  payload size (0 still aligns *used and returns that offset)
 * align: 0/1 or power of two
 *
 * Returns start offset on success, or (size_t)-1 on failure.
 * No parent wires.
 */
size_t
gj_arena_alloc_u(size_t *pUsed, size_t cbCap, size_t cbNeed, size_t uAlign)
{
	(void)NULL;
	return b3342_alloc(pUsed, cbCap, cbNeed, uAlign);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_arena_alloc_u(size_t *pUsed, size_t cbCap, size_t cbNeed,
			  size_t uAlign)
    __attribute__((alias("gj_arena_alloc_u")));
