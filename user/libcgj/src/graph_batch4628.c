/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4628: entropy fold from a byte buffer (_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_entropy_from_bytes_u(const uint8_t *p, size_t n);
 *     - Fold n bytes at p into a 64-bit entropy word via a rolling
 *       odd-multiply mix (length mixed in). NULL p with n != 0 -> 0.
 *       Empty (n == 0) -> 0.
 *   uint64_t __gj_entropy_from_bytes_u  (alias)
 *   __libcgj_batch4628_marker = "libcgj-batch4628"
 *
 * Exclusive continuum CREATE-ONLY (4621-4630). Unique
 * gj_entropy_from_bytes_u surface only; no multi-def. Distinct from
 * gj_buf_hash_crc32c_u (batch4023), gj_buf_hash_adler_u (batch4024),
 * gj_entropy_shannon_u8 (batch1091). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4628_marker[] = "libcgj-batch4628";

/* Rolling mix constants unique to this TU. */
#define B4628_M0   0x9e3779b97f4a7c15ULL
#define B4628_M1   0xbf58476d1ce4e5b9ULL
#define B4628_INIT 0xa0761d6478bd642full

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b4628_from_bytes(const uint8_t *p, size_t cbN)
{
	uint64_t u64H;
	size_t i;

	u64H = B4628_INIT ^ ((uint64_t)cbN * B4628_M0);
	for (i = 0u; i < cbN; i++) {
		u64H ^= (uint64_t)p[i] + ((uint64_t)i * 0x100000001b3ULL);
		u64H *= B4628_M0;
		u64H ^= u64H >> 31;
	}
	u64H ^= u64H >> 33;
	u64H *= B4628_M1;
	u64H ^= u64H >> 29;
	return u64H;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_entropy_from_bytes_u - fold a byte buffer into a 64-bit entropy word.
 *
 * p: byte buffer (may be NULL only when n == 0)
 * n: length in bytes
 *
 * Returns a mixed 64-bit entropy digest. NULL p with n != 0 yields 0.
 * Empty input (n == 0) yields 0. Does not call libc. No parent wires.
 */
uint64_t
gj_entropy_from_bytes_u(const uint8_t *p, size_t n)
{
	(void)NULL;
	if (p == NULL && n != 0u) {
		return 0ULL;
	}
	if (n == 0u) {
		return 0ULL;
	}
	return b4628_from_bytes(p, n);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_entropy_from_bytes_u(const uint8_t *p, size_t n)
    __attribute__((alias("gj_entropy_from_bytes_u")));
