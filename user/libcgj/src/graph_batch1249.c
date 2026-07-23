/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1249: constant-time all-zero byte buffer test.
 *
 * Surface (unique symbols):
 *   int gj_bytes_is_zero_consttime(const void *p, size_t n);
 *     — 1 if all n bytes are zero (or n==0), else 0.
 *       No data-dependent early exit. p==NULL with n>0 → 0.
 *   int __gj_bytes_is_zero_consttime  (alias)
 *   __libcgj_batch1249_marker = "libcgj-batch1249"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1249_marker[] = "libcgj-batch1249";

/*
 * gj_bytes_is_zero_consttime — constant-time "all zero" test.
 */
int
gj_bytes_is_zero_consttime(const void *p, size_t cb)
{
	const unsigned char *pX;
	unsigned char uAcc;
	size_t i;

	if (cb == 0u) {
		return 1;
	}
	if (p == NULL) {
		return 0;
	}

	pX = (const unsigned char *)p;
	uAcc = 0u;
	for (i = 0u; i < cb; i++) {
		uAcc = (unsigned char)(uAcc | pX[i]);
	}

	return (int)((((unsigned)uAcc - 1u) >> 8) & 1u);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_bytes_is_zero_consttime(const void *p, size_t cb)
    __attribute__((alias("gj_bytes_is_zero_consttime")));
