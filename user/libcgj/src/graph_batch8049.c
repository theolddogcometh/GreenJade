/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8049: right-shift memmove stub (dst > src).
 *
 * Surface (unique symbols):
 *   void *gj_memmove_right_u_8049(void *dst, const void *src, size_t n);
 *     - Backward octet copy intended for right-overlap (dst address
 *       above src). Always copies backward. NULL with n > 0 → return
 *       dst; n == 0 → return dst. Callers requiring left-overlap
 *       safety should use gj_memmove_left_u_8048 or gj_memmove_u_8042.
 *   void *__gj_memmove_right_u_8049  (alias)
 *   __libcgj_batch8049_marker = "libcgj-batch8049"
 *
 * Exclusive continuum CREATE-ONLY (8041-8050: memcpy overlap stubs —
 * memcpy_fwd, memmove, overlap_p, memcpy_or_move, memcpy_strict,
 * memmove_bytes, copy_dir, memmove_left, memmove_right, continuum).
 * Unique gj_memmove_right_u_8049 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8049_marker[] = "libcgj-batch8049";

/* ---- freestanding helpers ---------------------------------------------- */

static void
b8049_bwd(unsigned char *pDst, const unsigned char *pSrc, size_t cb)
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
 * gj_memmove_right_u_8049 - backward memmove for right-sliding windows.
 *
 * dst: destination of at least n bytes (NULL with n > 0 → no-op)
 * src: source of at least n bytes (NULL with n > 0 → no-op)
 * n:   octet count
 *
 * Correct when dst address >= src (including non-overlap and right
 * overlap). Returns dst. Soft half of the memmove pair. No parent wires.
 */
void *
gj_memmove_right_u_8049(void *pDst, const void *pSrc, size_t n)
{
	(void)NULL;
	if (n == 0u || pDst == NULL || pSrc == NULL) {
		return pDst;
	}
	if (pDst == pSrc) {
		return pDst;
	}
	b8049_bwd((unsigned char *)pDst, (const unsigned char *)pSrc, n);
	return pDst;
}

/* ---- underscored alias ------------------------------------------------- */

void *__gj_memmove_right_u_8049(void *pDst, const void *pSrc, size_t n)
    __attribute__((alias("gj_memmove_right_u_8049")));
