/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch610: freestanding XOR-reduce of a uint32_t
 * array (fold all elements with bitwise XOR).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_xor_all(const uint32_t *a, size_t n);
 *     — Return a[0] ^ a[1] ^ ... ^ a[n-1]. NULL a with n > 0 yields 0;
 *       n == 0 yields 0 (XOR identity). Pure integer fold; no carry.
 *   uint32_t __gj_u32_xor_all  (alias)
 *   __libcgj_batch610_marker = "libcgj-batch610"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch610_marker[] = "libcgj-batch610";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_xor_all — XOR-reduce n uint32_t elements.
 *
 * a: base (NULL with n > 0 → return 0)
 * n: element count
 *
 * Returns a[0] ^ ... ^ a[n-1] (0 if a is NULL or n is 0).
 */
uint32_t
gj_u32_xor_all(const uint32_t *a, size_t n)
{
	size_t i;
	uint32_t x;

	if (n == 0u || a == NULL) {
		return 0u;
	}

	x = 0u;
	for (i = 0u; i < n; i++) {
		x ^= a[i];
	}
	return x;
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_xor_all(const uint32_t *a, size_t n)
    __attribute__((alias("gj_u32_xor_all")));
