/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch268: gj_crc64_ecma — CRC-64/ECMA-182 one-shot.
 *
 * Surface (unique symbols — do NOT collide with existing CRC-64 exports):
 *   uint64_t gj_crc64_ecma(const void *data, size_t n);
 *     — CRC-64/ECMA-182 one-shot over n bytes (poly 0x42F0E1EBA9EA3693,
 *       refin=false, refout=false, init=0, xorout=0).
 *       Check("123456789") == 0x6c40df5f0b497347.
 *       NULL data with n != 0 → 0. Empty (n == 0) → 0.
 *   __gj_crc64_ecma  (alias)
 *   __libcgj_batch268_marker = "libcgj-batch268"
 *
 * Already present elsewhere (NOT redefined — avoid multi-def):
 *   crc64 / crc64_ecma / __crc64 / __crc64_ecma     → graph_batch41.c
 *   crc64_update / __crc64_update                   → graph_batch67.c
 *   crc64_xz / crc64_xz_update                      → graph_batch67.c
 *
 * Clean-room freestanding pure C (integer/pointer only). Table-driven
 * byte loop; table built once from the ECMA poly. Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno.
 * No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch268_marker[] = "libcgj-batch268";

/* ECMA-182 / CRC-64/ECMA polynomial (non-reflected). */
#define B268_POLY 0x42F0E1EBA9EA3693ull

/* ---- table (lazy init, freestanding; no constructor required) ---------- */

static uint64_t s_b268_tab[256];
static int s_b268_tab_ready;

static void
b268_tab_init(void)
{
	unsigned i;
	unsigned b;
	uint64_t c;

	if (s_b268_tab_ready) {
		return;
	}
	for (i = 0u; i < 256u; i++) {
		c = (uint64_t)i << 56;
		for (b = 0u; b < 8u; b++) {
			if (c & 0x8000000000000000ull) {
				c = (c << 1) ^ B268_POLY;
			} else {
				c <<= 1;
			}
		}
		s_b268_tab[i] = c;
	}
	s_b268_tab_ready = 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_crc64_ecma — one-shot CRC-64/ECMA-182.
 *
 * Same catalog parameters as batch41 crc64()/crc64_ecma(0, ...):
 *   width=64 poly=0x42F0E1EBA9EA3693 init=0 refin=false refout=false xorout=0
 *
 * Table path is equivalent to the bit-serial MSB-first loop used elsewhere.
 */
uint64_t
gj_crc64_ecma(const void *data, size_t n)
{
	const unsigned char *p = (const unsigned char *)data;
	uint64_t crc = 0ull;
	size_t i;

	if (p == NULL && n != 0u) {
		return 0ull;
	}
	if (n == 0u) {
		return 0ull;
	}

	b268_tab_init();

	for (i = 0u; i < n; i++) {
		unsigned idx = (unsigned)((crc >> 56) ^ (uint64_t)p[i]) & 0xffu;

		crc = (crc << 8) ^ s_b268_tab[idx];
	}
	return crc;
}

uint64_t __gj_crc64_ecma(const void *data, size_t n)
    __attribute__((alias("gj_crc64_ecma")));
