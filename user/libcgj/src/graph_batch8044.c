/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8044: memcpy-or-memmove dispatch stub.
 *
 * Surface (unique symbols):
 *   void *gj_memcpy_or_move_u_8044(void *dst, const void *src, size_t n);
 *     - If [dst,dst+n) and [src,src+n) overlap, copy with memmove
 *       direction; else forward memcpy. NULL with n > 0 → return dst;
 *       n == 0 → return dst.
 *   void *__gj_memcpy_or_move_u_8044  (alias)
 *   __libcgj_batch8044_marker = "libcgj-batch8044"
 *
 * Exclusive continuum CREATE-ONLY (8041-8050: memcpy overlap stubs —
 * memcpy_fwd, memmove, overlap_p, memcpy_or_move, memcpy_strict,
 * memmove_bytes, copy_dir, memmove_left, memmove_right, continuum).
 * Unique gj_memcpy_or_move_u_8044 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8044_marker[] = "libcgj-batch8044";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8044_overlap(uintptr_t uA, uintptr_t uB, size_t n)
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
b8044_fwd(unsigned char *pDst, const unsigned char *pSrc, size_t cb)
{
	size_t i;

	for (i = 0u; i < cb; i++) {
		pDst[i] = pSrc[i];
	}
}

static void
b8044_bwd(unsigned char *pDst, const unsigned char *pSrc, size_t cb)
{
	size_t i;

	i = cb;
	while (i > 0u) {
		i--;
		pDst[i] = pSrc[i];
	}
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_memcpy_or_move_u_8044 - overlap-aware copy dispatch stub.
 *
 * dst: destination of at least n bytes (NULL with n > 0 → no-op)
 * src: source of at least n bytes (NULL with n > 0 → no-op)
 * n:   octet count
 *
 * Non-overlapping → forward; overlapping with dst > src → backward;
 * overlapping with dst < src → forward. Returns dst. No parent wires.
 */
void *
gj_memcpy_or_move_u_8044(void *pDst, const void *pSrc, size_t n)
{
	unsigned char *pD;
	const unsigned char *pS;
	uintptr_t uDst;
	uintptr_t uSrc;

	(void)NULL;
	if (n == 0u || pDst == NULL || pSrc == NULL) {
		return pDst;
	}
	if (pDst == pSrc) {
		return pDst;
	}

	pD = (unsigned char *)pDst;
	pS = (const unsigned char *)pSrc;
	uDst = (uintptr_t)pD;
	uSrc = (uintptr_t)pS;

	if (b8044_overlap(uDst, uSrc, n) != 0u && uDst > uSrc) {
		b8044_bwd(pD, pS, n);
	} else {
		b8044_fwd(pD, pS, n);
	}
	return pDst;
}

/* ---- underscored alias ------------------------------------------------- */

void *__gj_memcpy_or_move_u_8044(void *pDst, const void *pSrc, size_t n)
    __attribute__((alias("gj_memcpy_or_move_u_8044")));
