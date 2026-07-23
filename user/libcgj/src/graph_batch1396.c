/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1396: fill a byte buffer with a u32 pattern.
 *
 * Surface (unique symbols):
 *   void *gj_mem_set_u32(void *p, uint32_t v, size_t n);
 *     — Fill n octets at p by repeating the little-endian encoding of v.
 *       A trailing partial unit (n % 4 != 0) receives the low-order
 *       octets of v in LE order. Returns p. p == NULL → NULL. n == 0 → p.
 *   void *__gj_mem_set_u32  (alias)
 *   __libcgj_batch1396_marker = "libcgj-batch1396"
 *
 * Distinct from gj_mem_set / gj_u32_fill / gj_mem_set_u16 — unique surface.
 * Does NOT redefine prior batch symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1396_marker[] = "libcgj-batch1396";

/* ---- freestanding helpers ---------------------------------------------- */

static void
b1396_store_le32(unsigned char *p, uint32_t u32V)
{
	p[0] = (unsigned char)(u32V & 0xffu);
	p[1] = (unsigned char)((u32V >> 8) & 0xffu);
	p[2] = (unsigned char)((u32V >> 16) & 0xffu);
	p[3] = (unsigned char)((u32V >> 24) & 0xffu);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mem_set_u32 — fill n bytes at p with LE u32 pattern v.
 */
void *
gj_mem_set_u32(void *p, uint32_t u32V, size_t n)
{
	unsigned char *pb;
	unsigned char aPat[4];
	size_t i;
	size_t iRem;

	if (p == NULL) {
		return NULL;
	}
	if (n == 0u) {
		return p;
	}

	pb = (unsigned char *)p;
	b1396_store_le32(aPat, u32V);

	i = 0u;
	while (i + 4u <= n) {
		pb[i] = aPat[0];
		pb[i + 1u] = aPat[1];
		pb[i + 2u] = aPat[2];
		pb[i + 3u] = aPat[3];
		i += 4u;
	}
	iRem = 0u;
	while (i < n) {
		pb[i++] = aPat[iRem++];
	}
	return p;
}

/* ---- underscored alias ------------------------------------------------- */

void *__gj_mem_set_u32(void *p, uint32_t u32V, size_t n)
    __attribute__((alias("gj_mem_set_u32")));
