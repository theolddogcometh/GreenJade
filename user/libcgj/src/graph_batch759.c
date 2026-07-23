/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch759: freestanding little-endian 64-bit store.
 *
 * Surface (unique symbols):
 *   void gj_le64_store(unsigned char *p, uint64_t v);
 *     — Store v as eight little-endian bytes at p. p == NULL → no-op.
 *   void __gj_le64_store  (alias)
 *   __libcgj_batch759_marker = "libcgj-batch759"
 *
 * Distinct from file-static b70_store_le64 and from gj_be64_store
 * (batch758). Unique gj_le64_store names only; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch759_marker[] = "libcgj-batch759";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_le64_store — write uint64_t v as little-endian octets at p.
 *
 * p: destination of at least 8 writable bytes (NULL → no-op)
 * v: host-order 64-bit value
 *
 * Layout: p[0] is the least-significant byte, p[7] the most.
 * Does not require natural alignment of p.
 */
void
gj_le64_store(unsigned char *p, uint64_t v)
{
	if (p == NULL) {
		return;
	}
	p[0] = (unsigned char)(v & 0xffu);
	p[1] = (unsigned char)((v >> 8) & 0xffu);
	p[2] = (unsigned char)((v >> 16) & 0xffu);
	p[3] = (unsigned char)((v >> 24) & 0xffu);
	p[4] = (unsigned char)((v >> 32) & 0xffu);
	p[5] = (unsigned char)((v >> 40) & 0xffu);
	p[6] = (unsigned char)((v >> 48) & 0xffu);
	p[7] = (unsigned char)((v >> 56) & 0xffu);
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_le64_store(unsigned char *p, uint64_t v)
    __attribute__((alias("gj_le64_store")));
