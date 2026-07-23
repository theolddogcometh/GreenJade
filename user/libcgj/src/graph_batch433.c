/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch433: freestanding byte-buffer zero fill.
 *
 * Surface (unique symbols):
 *   void gj_buf_zero(uint8_t *p, size_t n);
 *     — Write n zero bytes starting at p. No-op when n == 0 or p == NULL.
 *   void __gj_buf_zero  (alias)
 *   __libcgj_batch433_marker = "libcgj-batch433"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch433_marker[] = "libcgj-batch433";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_buf_zero — fill p[0..n) with 0.
 *
 * p: destination base (NULL is a no-op)
 * n: byte count
 */
void
gj_buf_zero(uint8_t *pBuf, size_t cbN)
{
	size_t i;

	if (pBuf == NULL || cbN == 0u) {
		return;
	}

	for (i = 0u; i < cbN; i++) {
		pBuf[i] = 0u;
	}
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_buf_zero(uint8_t *pBuf, size_t cbN)
    __attribute__((alias("gj_buf_zero")));
