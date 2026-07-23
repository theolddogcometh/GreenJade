/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch608: freestanding memory fill (octet u8).
 *
 * Surface (unique symbols):
 *   void gj_mem_fill_u8(void *p, size_t n, unsigned char v);
 *     — Fill n octets at p with value v. p == NULL → no-op.
 *       n == 0 → no-op.
 *   void __gj_mem_fill_u8  (alias)
 *   __libcgj_batch608_marker = "libcgj-batch608"
 *
 * Distinct from libc memset / gj_mem_set (batch396) — unique gj_ names
 * only; no multi-def with the existing string or mem graph.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc string helpers. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch608_marker[] = "libcgj-batch608";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mem_fill_u8 — fill n bytes at p with unsigned char v.
 *
 * p: destination region of at least n bytes when n > 0 (NULL → no-op)
 * n: number of octets to write (0 → no-op)
 * v: fill byte
 */
void
gj_mem_fill_u8(void *p, size_t n, unsigned char v)
{
	unsigned char *pb;
	size_t i;

	if (p == NULL) {
		return;
	}
	if (n == 0u) {
		return;
	}

	pb = (unsigned char *)p;
	for (i = 0u; i < n; i++) {
		pb[i] = v;
	}
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_mem_fill_u8(void *p, size_t n, unsigned char v)
    __attribute__((alias("gj_mem_fill_u8")));
