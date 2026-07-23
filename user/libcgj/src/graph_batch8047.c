/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8047: memcpy/memmove copy-direction stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_copy_dir_u_8047(const void *dst, const void *src, size_t n);
 *     - Classify copy strategy for n-byte transfer:
 *         0 = no-op / invalid (n == 0, NULL, or dst == src)
 *         1 = forward copy (dst address < src, or non-overlapping)
 *         2 = backward copy (overlapping and dst address > src)
 *       Does not read or write the pointed-to memory.
 *   uint32_t __gj_copy_dir_u_8047  (alias)
 *   __libcgj_batch8047_marker = "libcgj-batch8047"
 *
 * Exclusive continuum CREATE-ONLY (8041-8050: memcpy overlap stubs —
 * memcpy_fwd, memmove, overlap_p, memcpy_or_move, memcpy_strict,
 * memmove_bytes, copy_dir, memmove_left, memmove_right, continuum).
 * Unique gj_copy_dir_u_8047 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8047_marker[] = "libcgj-batch8047";

/* Direction codes for this continuum. */
#define B8047_DIR_NONE  0u
#define B8047_DIR_FWD   1u
#define B8047_DIR_BWD   2u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8047_overlap(uintptr_t uA, uintptr_t uB, size_t n)
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

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_copy_dir_u_8047 - classify forward vs backward copy need.
 *
 * dst, src: region bases (NULL with n > 0 → 0)
 * n:        octet count
 *
 * Returns direction code (0/1/2) as above. Soft planner for memcpy
 * overlap handling; no memory access. No parent wires.
 */
uint32_t
gj_copy_dir_u_8047(const void *pDst, const void *pSrc, size_t n)
{
	uintptr_t uDst;
	uintptr_t uSrc;

	(void)NULL;
	if (n == 0u || pDst == NULL || pSrc == NULL) {
		return B8047_DIR_NONE;
	}
	if (pDst == pSrc) {
		return B8047_DIR_NONE;
	}

	uDst = (uintptr_t)pDst;
	uSrc = (uintptr_t)pSrc;
	if (b8047_overlap(uDst, uSrc, n) != 0u && uDst > uSrc) {
		return B8047_DIR_BWD;
	}
	return B8047_DIR_FWD;
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_copy_dir_u_8047(const void *pDst, const void *pSrc, size_t n)
    __attribute__((alias("gj_copy_dir_u_8047")));
