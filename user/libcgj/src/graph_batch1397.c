/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1397: fill a byte buffer with a u64 pattern.
 *
 * Surface (unique symbols):
 *   void *gj_mem_set_u64(void *p, uint64_t v, size_t n);
 *     — Fill n octets at p by repeating the little-endian encoding of v.
 *       A trailing partial unit (n % 8 != 0) receives the low-order
 *       octets of v in LE order. Returns p. p == NULL → NULL. n == 0 → p.
 *   void *__gj_mem_set_u64  (alias)
 *   __libcgj_batch1397_marker = "libcgj-batch1397"
 *
 * Distinct from gj_mem_set / gj_u64_fill / gj_mem_set_u16/u32 — unique.
 * Does NOT redefine prior batch symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1397_marker[] = "libcgj-batch1397";

/* ---- freestanding helpers ---------------------------------------------- */

static void
b1397_store_le64(unsigned char *p, uint64_t u64V)
{
	p[0] = (unsigned char)(u64V & 0xffu);
	p[1] = (unsigned char)((u64V >> 8) & 0xffu);
	p[2] = (unsigned char)((u64V >> 16) & 0xffu);
	p[3] = (unsigned char)((u64V >> 24) & 0xffu);
	p[4] = (unsigned char)((u64V >> 32) & 0xffu);
	p[5] = (unsigned char)((u64V >> 40) & 0xffu);
	p[6] = (unsigned char)((u64V >> 48) & 0xffu);
	p[7] = (unsigned char)((u64V >> 56) & 0xffu);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mem_set_u64 — fill n bytes at p with LE u64 pattern v.
 */
void *
gj_mem_set_u64(void *p, uint64_t u64V, size_t n)
{
	unsigned char *pb;
	unsigned char aPat[8];
	size_t i;
	size_t iRem;

	if (p == NULL) {
		return NULL;
	}
	if (n == 0u) {
		return p;
	}

	pb = (unsigned char *)p;
	b1397_store_le64(aPat, u64V);

	i = 0u;
	while (i + 8u <= n) {
		pb[i] = aPat[0];
		pb[i + 1u] = aPat[1];
		pb[i + 2u] = aPat[2];
		pb[i + 3u] = aPat[3];
		pb[i + 4u] = aPat[4];
		pb[i + 5u] = aPat[5];
		pb[i + 6u] = aPat[6];
		pb[i + 7u] = aPat[7];
		i += 8u;
	}
	iRem = 0u;
	while (i < n) {
		pb[i++] = aPat[iRem++];
	}
	return p;
}

/* ---- underscored alias ------------------------------------------------- */

void *__gj_mem_set_u64(void *p, uint64_t u64V, size_t n)
    __attribute__((alias("gj_mem_set_u64")));
