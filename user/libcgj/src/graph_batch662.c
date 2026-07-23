/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch662: freestanding in-place memory reverse (octets).
 *
 * Surface (unique symbols):
 *   void gj_mem_reverse(void *p, size_t n);
 *     — Reverse the n bytes at p in place. p == NULL → no-op.
 *       n < 2 → no-op.
 *   void __gj_mem_reverse  (alias)
 *   __libcgj_batch662_marker = "libcgj-batch662"
 *
 * Distinct from gj_buf_rev (batch435, uint8_t *) and gj_reverse_u32
 * (batch401) — unique gj_mem_reverse name only; no multi-def with the
 * existing mem/string graph.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc string helpers. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch662_marker[] = "libcgj-batch662";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mem_reverse — reverse p[0..n) in place via two-pointer octet swap.
 *
 * p: region of at least n bytes when n > 0 (NULL → no-op)
 * n: number of octets to reverse (n < 2 → no-op)
 *
 * Does not call libc. Operates on unsigned char octets.
 */
void
gj_mem_reverse(void *p, size_t n)
{
	unsigned char *pb;
	size_t iLo;
	size_t iHi;
	unsigned char u8Tmp;

	if (p == NULL) {
		return;
	}
	if (n < 2u) {
		return;
	}

	pb = (unsigned char *)p;
	iLo = 0u;
	iHi = n - 1u;
	while (iLo < iHi) {
		u8Tmp = pb[iLo];
		pb[iLo] = pb[iHi];
		pb[iHi] = u8Tmp;
		iLo++;
		iHi--;
	}
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_mem_reverse(void *p, size_t n)
    __attribute__((alias("gj_mem_reverse")));
