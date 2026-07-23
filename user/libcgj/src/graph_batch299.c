/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch299: Bech32 polymod + charset helpers (BIP-173 lite).
 *
 * Surface (unique symbols — distinct from batch165 encode/decode wrappers):
 *   uint32_t gj_bech32_polymod(const uint8_t *values, size_t n);
 *     — BIP-0173 30-bit BCH polymod over n 5-bit values. Initial chk = 1.
 *       values may be NULL when n == 0 (empty input → returns 1).
 *   int gj_bech32_charset_idx(char c);
 *     — Map BIP-0173 charset glyph → 5-bit index 0..31, or -1 if invalid.
 *       Exact match against "qpzry9x8gf2tvdw0s3jn54khce6mua7l" (no case fold).
 *   __gj_bech32_polymod  (alias)
 *   __gj_bech32_charset_idx  (alias)
 *   __libcgj_batch299_marker = "libcgj-batch299"
 *
 * Already present elsewhere (NOT redefined — avoid multi-def):
 *   gj_bech32_encode / gj_bech32_decode → graph_batch165.c
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno.
 * No third-party BIP reference implementation or wallet source was copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch299_marker[] = "libcgj-batch299";

/* BIP-0173 Bech32 character set (index == 5-bit value). */
static const char s_b299_charset[32] =
    "qpzry9x8gf2tvdw0s3jn54khce6mua7l";

/* Polymod generator coefficients (BIP-0173). */
static const uint32_t s_b299_gen[5] = {
	0x3b6a57b2u,
	0x26508e6du,
	0x1ea119fau,
	0x3d4233ddu,
	0x2a1462b3u
};

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bech32_polymod — BIP-0173 polymod over 5-bit values.
 *
 * values[0..n): sequence of 5-bit symbols (caller supplies 0..31).
 * chk is a 30-bit BCH state; starts at 1 for an empty sequence.
 *
 * NULL values with n == 0 is accepted (returns 1). NULL values with n > 0
 * is treated as empty (returns 1) to avoid deref — no errno in freestanding.
 */
uint32_t
gj_bech32_polymod(const uint8_t *values, size_t n)
{
	uint32_t uChk = 1u;
	size_t i;

	if (values == NULL || n == 0u) {
		return 1u;
	}

	for (i = 0u; i < n; i++) {
		uint32_t uB = uChk >> 25;
		unsigned j;

		uChk = ((uChk & 0x1ffffffu) << 5) ^ (uint32_t)values[i];
		for (j = 0u; j < 5u; j++) {
			if ((uB >> j) & 1u) {
				uChk ^= s_b299_gen[j];
			}
		}
	}
	return uChk;
}

uint32_t __gj_bech32_polymod(const uint8_t *values, size_t n)
    __attribute__((alias("gj_bech32_polymod")));

/*
 * gj_bech32_charset_idx — BIP-0173 charset glyph → 5-bit index.
 *
 * Returns 0..31 when c is in "qpzry9x8gf2tvdw0s3jn54khce6mua7l",
 * or -1 if c is not a valid Bech32 data character (exact match, no fold).
 */
int
gj_bech32_charset_idx(char c)
{
	unsigned char ch = (unsigned char)c;
	unsigned i;

	for (i = 0u; i < 32u; i++) {
		if ((unsigned char)s_b299_charset[i] == ch) {
			return (int)i;
		}
	}
	return -1;
}

int __gj_bech32_charset_idx(char c)
    __attribute__((alias("gj_bech32_charset_idx")));
