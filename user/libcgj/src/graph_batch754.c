/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch754: freestanding big-endian 16-bit store.
 *
 * Surface (unique symbols):
 *   void gj_be16_store(unsigned char *p, uint16_t v);
 *     — Store v as two big-endian bytes at p. p == NULL → no-op.
 *   void __gj_be16_store  (alias)
 *   __libcgj_batch754_marker = "libcgj-batch754"
 *
 * Distinct from htons/ntohs (byteorder.c) and from le16 store
 * (batch756). Unique gj_be16_store names only; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch754_marker[] = "libcgj-batch754";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_be16_store — write uint16_t v as big-endian octets at p.
 *
 * p: destination of at least 2 writable bytes (NULL → no-op)
 * v: host-order 16-bit value
 *
 * Layout: p[0] = high byte, p[1] = low byte.
 * Does not require natural alignment of p.
 */
void
gj_be16_store(unsigned char *p, uint16_t v)
{
	if (p == NULL) {
		return;
	}
	p[0] = (unsigned char)((v >> 8) & 0xffu);
	p[1] = (unsigned char)(v & 0xffu);
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_be16_store(unsigned char *p, uint16_t v)
    __attribute__((alias("gj_be16_store")));
