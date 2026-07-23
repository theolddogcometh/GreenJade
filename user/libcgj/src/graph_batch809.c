/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch809: freestanding int64_t array element copy.
 *
 * Surface (unique symbols):
 *   void gj_i64_copy(int64_t *dst, const int64_t *src, size_t n);
 *     — Copy n elements from src to dst. Regions must not overlap
 *       unless dst == src (no-op). Either pointer NULL with n > 0 →
 *       no-op; n == 0 → no-op.
 *   void __gj_i64_copy  (alias)
 *   __libcgj_batch809_marker = "libcgj-batch809"
 *
 * Distinct from gj_u16_copy / gj_u32_copy / gj_u64_copy and libc
 * memcpy — unique gj_i64_copy surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch809_marker[] = "libcgj-batch809";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_i64_copy — copy n int64_t elements from src to dst.
 *
 * dst: destination of at least n elements (NULL with n > 0 → no-op)
 * src: source of at least n elements (NULL with n > 0 → no-op)
 * n:   element count
 *
 * Forward copy; not a memmove substitute for overlapping windows
 * other than the identical-pointer case.
 */
void
gj_i64_copy(int64_t *pDst, const int64_t *pSrc, size_t n)
{
	size_t i;

	if (n == 0u || pDst == NULL || pSrc == NULL) {
		return;
	}
	if (pDst == pSrc) {
		return;
	}
	for (i = 0u; i < n; i++) {
		pDst[i] = pSrc[i];
	}
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_i64_copy(int64_t *pDst, const int64_t *pSrc, size_t n)
    __attribute__((alias("gj_i64_copy")));
