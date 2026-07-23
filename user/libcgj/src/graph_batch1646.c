/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1646: Bitcoin base58 decode length upper bound.
 *
 * Surface (unique symbols):
 *   size_t gj_base58_decode_len_hint(size_t n);
 *     — Safe upper bound on decoded raw bytes for an n-character base58
 *       string. log(58)/log(256) ≈ 0.732 → n * 733 / 1000 + 1. Also covers
 *       leading '1' → 0x00 expansion (at most n leading zeros). n == 0 → 1.
 *   size_t __gj_base58_decode_len_hint  (alias)
 *   __libcgj_batch1646_marker = "libcgj-batch1646"
 *
 * Does NOT redefine base58_decode (graph_batch42) — length-hint only.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1646_marker[] = "libcgj-batch1646";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * n * 733 / 1000 + 1 is a standard ceiling used as a base58 decode buffer
 * size hint (Bitcoin Core style). Always at least 1.
 */
static size_t
b1646_dec_hint(size_t cEnc)
{
	/* Avoid wrap: require cEnc * 733 fits in size_t before /1000 + 1. */
	if (cEnc > (SIZE_MAX - 1u) / 733u) {
		return SIZE_MAX;
	}
	return cEnc * 733u / 1000u + 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_base58_decode_len_hint — upper bound on raw bytes for n base58 chars.
 */
size_t
gj_base58_decode_len_hint(size_t cEnc)
{
	return b1646_dec_hint(cEnc);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_base58_decode_len_hint(size_t cEnc)
    __attribute__((alias("gj_base58_decode_len_hint")));
