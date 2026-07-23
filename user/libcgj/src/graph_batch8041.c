/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8041: forward memcpy stub (non-overlap).
 *
 * Surface (unique symbols):
 *   void *gj_memcpy_fwd_u_8041(void *dst, const void *src, size_t n);
 *     - Forward octet copy of n bytes from src to dst. Regions must not
 *       overlap unless dst == src (no-op). NULL with n > 0 → return dst
 *       without write; n == 0 → return dst. Not a memmove substitute.
 *   void *__gj_memcpy_fwd_u_8041  (alias)
 *   __libcgj_batch8041_marker = "libcgj-batch8041"
 *
 * Exclusive continuum CREATE-ONLY (8041-8050: memcpy overlap stubs —
 * memcpy_fwd, memmove, overlap_p, memcpy_or_move, memcpy_strict,
 * memmove_bytes, copy_dir, memmove_left, memmove_right, continuum).
 * Unique gj_memcpy_fwd_u_8041 surface only; no multi-def with libc
 * memcpy. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8041_marker[] = "libcgj-batch8041";

/* ---- freestanding helpers ---------------------------------------------- */

static void
b8041_fwd(unsigned char *pDst, const unsigned char *pSrc, size_t cb)
{
	size_t i;

	for (i = 0u; i < cb; i++) {
		pDst[i] = pSrc[i];
	}
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_memcpy_fwd_u_8041 - forward-only memcpy stub.
 *
 * dst: destination of at least n bytes (NULL with n > 0 → no-op)
 * src: source of at least n bytes (NULL with n > 0 → no-op)
 * n:   octet count
 *
 * Forward copy only. Overlapping windows other than dst == src are
 * undefined for this stub (use gj_memmove_u_8042). Returns dst.
 * No parent wires.
 */
void *
gj_memcpy_fwd_u_8041(void *pDst, const void *pSrc, size_t n)
{
	(void)NULL;
	if (n == 0u || pDst == NULL || pSrc == NULL) {
		return pDst;
	}
	if (pDst == pSrc) {
		return pDst;
	}
	b8041_fwd((unsigned char *)pDst, (const unsigned char *)pSrc, n);
	return pDst;
}

/* ---- underscored alias ------------------------------------------------- */

void *__gj_memcpy_fwd_u_8041(void *pDst, const void *pSrc, size_t n)
    __attribute__((alias("gj_memcpy_fwd_u_8041")));
