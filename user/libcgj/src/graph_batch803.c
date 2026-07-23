/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch803: freestanding memory zero-fill.
 *
 * Surface (unique symbols):
 *   void gj_mem_fill_zero(void *p, size_t n);
 *     — Fill n octets at p with 0x00. p == NULL or n == 0 → no-op.
 *   void __gj_mem_fill_zero  (alias)
 *   __libcgj_batch803_marker = "libcgj-batch803"
 *
 * Distinct from gj_mem_fill_u8 (batch608) / gj_mem_fill_ff (batch804)
 * and libc memset — unique gj_mem_fill_zero surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc string helpers. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch803_marker[] = "libcgj-batch803";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mem_fill_zero — fill n bytes at p with 0x00.
 *
 * p: destination region of at least n bytes when n > 0 (NULL → no-op)
 * n: number of octets to write (0 → no-op)
 */
void
gj_mem_fill_zero(void *p, size_t n)
{
	unsigned char *pb;
	size_t i;

	if (p == NULL || n == 0u) {
		return;
	}

	pb = (unsigned char *)p;
	for (i = 0u; i < n; i++) {
		pb[i] = 0u;
	}
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_mem_fill_zero(void *p, size_t n)
    __attribute__((alias("gj_mem_fill_zero")));
