/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch785: XOR-fold of n bytes into a uint32_t.
 *
 * Surface (unique symbols):
 *   uint32_t gj_xor32_n(const void *data, size_t n);
 *     — Fold n bytes into a running uint32_t via successive XOR of each
 *       octet (zero-extended). data == NULL → 0. n == 0 → 0.
 *   uint32_t __gj_xor32_n  (alias)
 *   __libcgj_batch785_marker = "libcgj-batch785"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch785_marker[] = "libcgj-batch785";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_xor32_n — XOR-fold of bytes into u32.
 *
 * acc starts at 0; for each byte b: acc ^= (uint32_t)b.
 */
uint32_t
gj_xor32_n(const void *pData, size_t cb)
{
	const unsigned char *p;
	uint32_t u32Acc;
	size_t i;

	if (pData == NULL || cb == 0u) {
		return 0u;
	}

	p = (const unsigned char *)pData;
	u32Acc = 0u;
	for (i = 0u; i < cb; i++) {
		u32Acc ^= (uint32_t)p[i];
	}
	return u32Acc;
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_xor32_n(const void *pData, size_t cb)
    __attribute__((alias("gj_xor32_n")));
