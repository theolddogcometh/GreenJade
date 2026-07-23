/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1395: fill a byte buffer with a u16 pattern.
 *
 * Surface (unique symbols):
 *   void *gj_mem_set_u16(void *p, uint16_t v, size_t n);
 *     — Fill n octets at p by repeating the little-endian encoding of v.
 *       Odd trailing byte (when n is odd) receives the low octet of v.
 *       Returns p. p == NULL → NULL (no write). n == 0 → return p.
 *   void *__gj_mem_set_u16  (alias)
 *   __libcgj_batch1395_marker = "libcgj-batch1395"
 *
 * Distinct from gj_mem_set (batch396) / gj_u16_fill (batch805) — fills a
 * raw octet region with a multi-byte pattern, not an array of uint16_t.
 * Does NOT redefine prior batch symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1395_marker[] = "libcgj-batch1395";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mem_set_u16 — fill n bytes at p with LE u16 pattern v.
 */
void *
gj_mem_set_u16(void *p, uint16_t u16V, size_t n)
{
	unsigned char *pb;
	unsigned char chLo;
	unsigned char chHi;
	size_t i;

	if (p == NULL) {
		return NULL;
	}
	if (n == 0u) {
		return p;
	}

	pb = (unsigned char *)p;
	chLo = (unsigned char)(u16V & 0xffu);
	chHi = (unsigned char)((u16V >> 8) & 0xffu);

	i = 0u;
	while (i + 1u < n) {
		pb[i] = chLo;
		pb[i + 1u] = chHi;
		i += 2u;
	}
	if (i < n) {
		pb[i] = chLo;
	}
	return p;
}

/* ---- underscored alias ------------------------------------------------- */

void *__gj_mem_set_u16(void *p, uint16_t u16V, size_t n)
    __attribute__((alias("gj_mem_set_u16")));
