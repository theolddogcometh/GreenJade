/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8045: strict non-overlap memcpy stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_memcpy_strict_u_8045(void *dst, const void *src, size_t n);
 *     - If regions overlap (and n > 0), do not copy and return 0.
 *       If non-overlapping (or n == 0), forward-copy and return 1.
 *       NULL with n > 0 → 0 (no write). dst == src with n > 0 → 0
 *       (treated as overlap / refused).
 *   uint32_t __gj_memcpy_strict_u_8045  (alias)
 *   __libcgj_batch8045_marker = "libcgj-batch8045"
 *
 * Exclusive continuum CREATE-ONLY (8041-8050: memcpy overlap stubs —
 * memcpy_fwd, memmove, overlap_p, memcpy_or_move, memcpy_strict,
 * memmove_bytes, copy_dir, memmove_left, memmove_right, continuum).
 * Unique gj_memcpy_strict_u_8045 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8045_marker[] = "libcgj-batch8045";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8045_overlap(uintptr_t uA, uintptr_t uB, size_t n)
{
	uintptr_t uAEnd;
	uintptr_t uBEnd;

	if (n == 0u) {
		return 0u;
	}
	uAEnd = uA + (uintptr_t)n;
	uBEnd = uB + (uintptr_t)n;
	if (uA < uBEnd && uB < uAEnd) {
		return 1u;
	}
	return 0u;
}

static void
b8045_fwd(unsigned char *pDst, const unsigned char *pSrc, size_t cb)
{
	size_t i;

	for (i = 0u; i < cb; i++) {
		pDst[i] = pSrc[i];
	}
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_memcpy_strict_u_8045 - memcpy that refuses overlapping regions.
 *
 * dst: destination of at least n bytes
 * src: source of at least n bytes
 * n:   octet count
 *
 * Returns 1 on successful forward copy (including n == 0), 0 if
 * refused (NULL args with n > 0, or overlapping regions). Soft
 * fortify-style guard stub. No parent wires.
 */
uint32_t
gj_memcpy_strict_u_8045(void *pDst, const void *pSrc, size_t n)
{
	uintptr_t uDst;
	uintptr_t uSrc;

	(void)NULL;
	if (n == 0u) {
		return 1u;
	}
	if (pDst == NULL || pSrc == NULL) {
		return 0u;
	}

	uDst = (uintptr_t)pDst;
	uSrc = (uintptr_t)pSrc;
	if (b8045_overlap(uDst, uSrc, n) != 0u) {
		return 0u;
	}

	b8045_fwd((unsigned char *)pDst, (const unsigned char *)pSrc, n);
	return 1u;
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_memcpy_strict_u_8045(void *pDst, const void *pSrc, size_t n)
    __attribute__((alias("gj_memcpy_strict_u_8045")));
