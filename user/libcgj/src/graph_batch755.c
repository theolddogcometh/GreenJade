/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch755: freestanding big-endian 32-bit store.
 *
 * Surface (unique symbols):
 *   void gj_be32_store(unsigned char *p, uint32_t v);
 *     — Store v as four big-endian bytes at p. p == NULL → no-op.
 *   void __gj_be32_store  (alias)
 *   __libcgj_batch755_marker = "libcgj-batch755"
 *
 * Distinct from htonl/ntohl (byteorder.c) and from le32 store
 * (batch757). Unique gj_be32_store names only; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch755_marker[] = "libcgj-batch755";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_be32_store — write uint32_t v as big-endian octets at p.
 *
 * p: destination of at least 4 writable bytes (NULL → no-op)
 * v: host-order 32-bit value
 *
 * Layout: p[0] is the most-significant byte, p[3] the least.
 * Does not require natural alignment of p.
 */
void
gj_be32_store(unsigned char *p, uint32_t v)
{
	if (p == NULL) {
		return;
	}
	p[0] = (unsigned char)((v >> 24) & 0xffu);
	p[1] = (unsigned char)((v >> 16) & 0xffu);
	p[2] = (unsigned char)((v >> 8) & 0xffu);
	p[3] = (unsigned char)(v & 0xffu);
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_be32_store(unsigned char *p, uint32_t v)
    __attribute__((alias("gj_be32_store")));
