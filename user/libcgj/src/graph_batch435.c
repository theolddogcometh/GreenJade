/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch435: freestanding in-place byte-buffer reverse.
 *
 * Surface (unique symbols):
 *   void gj_buf_rev(uint8_t *p, size_t n);
 *     — Reverse the n bytes at p in place. No-op when n < 2 or p == NULL.
 *   void __gj_buf_rev  (alias)
 *   __libcgj_batch435_marker = "libcgj-batch435"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch435_marker[] = "libcgj-batch435";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_buf_rev — reverse p[0..n) in place.
 *
 * p: buffer base (NULL is a no-op)
 * n: byte count
 */
void
gj_buf_rev(uint8_t *pBuf, size_t cbN)
{
	size_t iLo;
	size_t iHi;
	uint8_t u8Tmp;

	if (pBuf == NULL || cbN < 2u) {
		return;
	}

	iLo = 0u;
	iHi = cbN - 1u;
	while (iLo < iHi) {
		u8Tmp = pBuf[iLo];
		pBuf[iLo] = pBuf[iHi];
		pBuf[iHi] = u8Tmp;
		iLo++;
		iHi--;
	}
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_buf_rev(uint8_t *pBuf, size_t cbN)
    __attribute__((alias("gj_buf_rev")));
