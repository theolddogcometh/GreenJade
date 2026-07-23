/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1501: RLE encode worst-case bound.
 *
 * Surface (unique symbols):
 *   size_t gj_rle_encode_bound(size_t n);
 *     — Worst-case output size in bytes for simple (byte,count) RLE of
 *       n input bytes. Each input byte can form its own run → 2 bytes
 *       of output per input byte. Returns 0 when n == 0. Saturates to
 *       SIZE_MAX when 2*n would overflow size_t.
 *   size_t __gj_rle_encode_bound  (alias)
 *   __libcgj_batch1501_marker = "libcgj-batch1501"
 *
 * Distinct from gj_rle_compress (batch288). Format pairs with
 * gj_rle_encode_simple (batch1502) / gj_rle_decode_simple (batch1503).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1501_marker[] = "libcgj-batch1501";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_rle_encode_bound — worst-case (byte,count) RLE output size for n bytes.
 *
 * Simple RLE emits two octets per run chunk (value + count in 1..255).
 * The adversarial input is n singleton runs → exactly 2*n output bytes.
 */
size_t
gj_rle_encode_bound(size_t cbN)
{
	if (cbN == 0u) {
		return 0u;
	}
	/* Saturate on overflow: 2*cbN does not fit in size_t. */
	if (cbN > (SIZE_MAX / 2u)) {
		return SIZE_MAX;
	}
	return cbN * 2u;
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_rle_encode_bound(size_t cbN)
    __attribute__((alias("gj_rle_encode_bound")));
