/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch757: freestanding little-endian 32-bit store.
 *
 * Surface (unique symbols):
 *   void gj_le32_store(unsigned char *p, uint32_t v);
 *     — Store v as four little-endian bytes at p. p == NULL → no-op.
 *   void __gj_le32_store  (alias)
 *   __libcgj_batch757_marker = "libcgj-batch757"
 *
 * Distinct from file-static b70_store_le32 and from gj_be32_store
 * (batch755). Unique gj_le32_store names only; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch757_marker[] = "libcgj-batch757";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_le32_store — write uint32_t v as little-endian octets at p.
 *
 * p: destination of at least 4 writable bytes (NULL → no-op)
 * v: host-order 32-bit value
 *
 * Layout: p[0] is the least-significant byte, p[3] the most.
 * Does not require natural alignment of p.
 */
void
gj_le32_store(unsigned char *p, uint32_t v)
{
	if (p == NULL) {
		return;
	}
	p[0] = (unsigned char)(v & 0xffu);
	p[1] = (unsigned char)((v >> 8) & 0xffu);
	p[2] = (unsigned char)((v >> 16) & 0xffu);
	p[3] = (unsigned char)((v >> 24) & 0xffu);
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_le32_store(unsigned char *p, uint32_t v)
    __attribute__((alias("gj_le32_store")));
