/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1522: arena remaining-capacity probe.
 *
 * Surface (unique symbols):
 *   int gj_arena_can_alloc(size_t used, size_t cap, size_t need,
 *                          size_t align);
 *     — Return 1 if a bump allocation of need bytes at alignment align
 *       would fit in remaining capacity (cap - aligned_used), else 0.
 *       Does not mutate state. align must be 0, 1, or a power of two;
 *       other values fail (return 0). Overflow / used > cap → 0.
 *   int __gj_arena_can_alloc  (alias)
 *   __libcgj_batch1522_marker = "libcgj-batch1522"
 *
 * Distinct from gj_arena_alloc / gj_arena_bump / gj_bump_alloc (which
 * actually consume). Unique probe surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1522_marker[] = "libcgj-batch1522";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Return 1 if uAlign is 0, 1, or a power of two; else 0.
 */
static int
b1522_align_ok(size_t uAlign)
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
b1522_align_up(size_t uOff, size_t uAlign, size_t *pOut)
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
 * gj_arena_can_alloc — would a bump of need@align fit in [used, cap)?
 *
 * cbUsed: current watermark
 * cbCap:  total buffer capacity
 * cbNeed: payload bytes requested (0 still requires the aligned start
 *         to lie within cap)
 * uAlign: 0/1 or power of two
 *
 * Returns 1 if fit, 0 if not (bad align, overflow, OOM).
 */
int
gj_arena_can_alloc(size_t cbUsed, size_t cbCap, size_t cbNeed, size_t uAlign)
{
	size_t uStart;

	if (!b1522_align_ok(uAlign)) {
		return 0;
	}
	if (b1522_align_up(cbUsed, uAlign, &uStart) != 0) {
		return 0;
	}
	if (uStart > cbCap) {
		return 0;
	}
	if (cbNeed > cbCap - uStart) {
		return 0;
	}
	return 1;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_arena_can_alloc(size_t cbUsed, size_t cbCap, size_t cbNeed,
			 size_t uAlign)
    __attribute__((alias("gj_arena_can_alloc")));
