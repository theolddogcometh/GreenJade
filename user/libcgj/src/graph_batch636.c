/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch636: freestanding sum of an unsigned char
 * (u8) array into a uint32_t accumulator.
 *
 * Surface (unique symbols):
 *   uint32_t gj_u8_sum(const unsigned char *a, size_t n);
 *     — Return a[0] + ... + a[n-1] as uint32_t. NULL a with n > 0
 *       yields 0; n == 0 yields 0. Each byte is widened before add.
 *   uint32_t __gj_u8_sum  (alias)
 *   __libcgj_batch636_marker = "libcgj-batch636"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch636_marker[] = "libcgj-batch636";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u8_sum — sum n unsigned char elements into a uint32_t.
 *
 * a: base (NULL with n > 0 → return 0)
 * n: element count
 *
 * Returns the sum of a[0..n) as uint32_t (0 if a is NULL or n is 0).
 */
uint32_t
gj_u8_sum(const unsigned char *a, size_t n)
{
	size_t i;
	uint32_t sum;

	if (n == 0u || a == NULL) {
		return 0u;
	}

	sum = 0u;
	for (i = 0u; i < n; i++) {
		sum += (uint32_t)a[i];
	}
	return sum;
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u8_sum(const unsigned char *a, size_t n)
    __attribute__((alias("gj_u8_sum")));
