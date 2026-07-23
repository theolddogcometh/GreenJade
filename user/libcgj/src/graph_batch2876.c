/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2876: copy n uint64_t elements dst <- src
 * (_u arr surface).
 *
 * Surface (unique symbols):
 *   void gj_arr_u64_copy_u(uint64_t *dst, const uint64_t *src, size_t n);
 *     - Copy n elements from src[0..n) into dst[0..n). Forward element
 *       walk (safe for non-overlapping regions; undefined for overlap).
 *       dst or src NULL with n > 0 -> no-op; n == 0 -> no-op.
 *   void __gj_arr_u64_copy_u  (alias)
 *   __libcgj_batch2876_marker = "libcgj-batch2876"
 *
 * Array exclusive wave (2871-2880). Distinct from gj_arr_u32_copy_u
 * (batch2875) — unique u64 arr _u copy surface; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No __int128. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2876_marker[] = "libcgj-batch2876";

/* ---- freestanding helpers ---------------------------------------------- */

/* Forward-copy n elements d[i] = s[i]. Both non-NULL when n > 0. */
static void
b2876_copy(uint64_t *d, const uint64_t *s, size_t n)
{
	size_t i;

	for (i = 0u; i < n; i++) {
		d[i] = s[i];
	}
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_arr_u64_copy_u - copy n uint64_t elements from src to dst.
 *
 * dst: destination base (NULL with n > 0 -> no-op)
 * src: source base (NULL with n > 0 -> no-op)
 * n:   element count
 *
 * Forward walk only. Caller must ensure non-overlapping ranges when
 * both point into the same object.
 */
void
gj_arr_u64_copy_u(uint64_t *dst, const uint64_t *src, size_t n)
{
	(void)NULL;
	if (n == 0u || dst == NULL || src == NULL) {
		return;
	}
	b2876_copy(dst, src, n);
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_arr_u64_copy_u(uint64_t *dst, const uint64_t *src, size_t n)
    __attribute__((alias("gj_arr_u64_copy_u")));
