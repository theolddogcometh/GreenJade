/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch637: freestanding XOR-reduce of an
 * unsigned char array (fold all elements with bitwise XOR).
 *
 * Surface (unique symbols):
 *   unsigned char gj_u8_xor_all(const unsigned char *a, size_t n);
 *     — Return a[0] ^ a[1] ^ ... ^ a[n-1]. NULL a with n > 0 yields 0;
 *       n == 0 yields 0 (XOR identity). Pure integer fold; no carry.
 *   unsigned char __gj_u8_xor_all  (alias)
 *   __libcgj_batch637_marker = "libcgj-batch637"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch637_marker[] = "libcgj-batch637";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u8_xor_all — XOR-reduce n unsigned char elements.
 *
 * a: base (NULL with n > 0 → return 0)
 * n: element count
 *
 * Returns a[0] ^ ... ^ a[n-1] (0 if a is NULL or n is 0).
 */
unsigned char
gj_u8_xor_all(const unsigned char *a, size_t n)
{
	size_t i;
	unsigned char x;

	if (n == 0u || a == NULL) {
		return (unsigned char)0u;
	}

	x = (unsigned char)0u;
	for (i = 0u; i < n; i++) {
		x = (unsigned char)(x ^ a[i]);
	}
	return x;
}

/* ---- underscored alias ------------------------------------------------- */

unsigned char __gj_u8_xor_all(const unsigned char *a, size_t n)
    __attribute__((alias("gj_u8_xor_all")));
