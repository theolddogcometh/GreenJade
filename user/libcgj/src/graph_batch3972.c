/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3972: in-place arena watermark align (_u).
 *
 * Surface (unique symbols):
 *   size_t gj_arena_align_u(size_t *used, size_t align);
 *     - Round *used up to a multiple of align (0/1 = identity; power of
 *       two preferred; general multiple also accepted). Stores the
 *       aligned value back into *used and returns it. NULL used, or
 *       size_t overflow of the round-up → (size_t)-1 (*used unchanged
 *       on failure).
 *   size_t __gj_arena_align_u  (alias)
 *   __libcgj_batch3972_marker = "libcgj-batch3972"
 *
 * Exclusive continuum CREATE-ONLY (3971-3980). Distinct from
 * gj_arena_align_up_size (batch1521 pure size round-up) — this form
 * mutates a watermark pointer. Unique gj_arena_align_u surface only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3972_marker[] = "libcgj-batch3972";

#define B3972_FAIL  ((size_t)-1)

/* ---- freestanding helpers ---------------------------------------------- */

static size_t
b3972_align(size_t *pUsed, size_t uAlign)
{
	size_t uCur;
	size_t uMask;
	size_t uAdj;
	size_t uRem;

	if (pUsed == NULL) {
		return B3972_FAIL;
	}
	uCur = *pUsed;
	if (uAlign <= 1u) {
		return uCur;
	}

	/* Power-of-two fast path. */
	if ((uAlign & (uAlign - 1u)) == 0u) {
		uMask = uAlign - 1u;
		if (uCur > (SIZE_MAX - uMask)) {
			return B3972_FAIL;
		}
		uAdj = (uCur + uMask) & ~uMask;
		*pUsed = uAdj;
		return uAdj;
	}

	/* General multiple. */
	uRem = uCur % uAlign;
	if (uRem == 0u) {
		return uCur;
	}
	uAdj = uCur + (uAlign - uRem);
	if (uAdj < uCur) {
		return B3972_FAIL;
	}
	*pUsed = uAdj;
	return uAdj;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_arena_align_u - align a bump watermark in place; return new value.
 *
 * used:  address of the size_t watermark (mutated on success)
 * align: alignment (0 or 1 → identity; power-of-two preferred)
 *
 * Returns aligned *used, or (size_t)-1 on NULL used / overflow.
 * No parent wires.
 */
size_t
gj_arena_align_u(size_t *pUsed, size_t uAlign)
{
	(void)NULL;
	return b3972_align(pUsed, uAlign);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_arena_align_u(size_t *pUsed, size_t uAlign)
    __attribute__((alias("gj_arena_align_u")));
