/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1645: Bitcoin base58 encode length upper bound.
 *
 * Surface (unique symbols):
 *   size_t gj_base58_encode_len_hint(size_t n);
 *     — Safe upper bound on the number of base58 characters produced by
 *       encoding n raw bytes (excludes trailing NUL). Uses the common
 *       log(256)/log(58) ≈ 1.365 bound: n * 138 / 100 + 1. Covers leading
 *       zero → '1' expansion. n == 0 → 1 (empty payload encodes as empty
 *       in some codecs; return 1 so a single-NUL buffer always fits).
 *   size_t __gj_base58_encode_len_hint  (alias)
 *   __libcgj_batch1645_marker = "libcgj-batch1645"
 *
 * Does NOT redefine base58_encode (graph_batch42) — length-hint only.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1645_marker[] = "libcgj-batch1645";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * n * 138 / 100 + 1 is a well-known safe ceiling used by Bitcoin Core
 * style base58 for intermediate digit buffers; also bounds the output.
 */
static size_t
b1645_enc_hint(size_t cbIn)
{
	/* Avoid wrap: require cbIn * 138 fits in size_t before /100 + 1. */
	if (cbIn > (SIZE_MAX - 1u) / 138u) {
		return SIZE_MAX;
	}
	return cbIn * 138u / 100u + 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_base58_encode_len_hint — upper bound on base58 chars for n raw bytes.
 */
size_t
gj_base58_encode_len_hint(size_t cbIn)
{
	return b1645_enc_hint(cbIn);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_base58_encode_len_hint(size_t cbIn)
    __attribute__((alias("gj_base58_encode_len_hint")));
