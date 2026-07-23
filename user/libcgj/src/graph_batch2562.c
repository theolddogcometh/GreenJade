/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2562: RLE encode worst-case bound (_u surface).
 *
 * Surface (unique symbols):
 *   size_t gj_rle_bound_u(size_t n);
 *     — Worst-case output size in bytes for simple (byte,count) RLE of
 *       n input bytes. Each input byte can form its own run → 2 bytes
 *       of output per input byte. Returns 0 when n == 0. Saturates to
 *       SIZE_MAX when 2*n would overflow size_t.
 *   size_t __gj_rle_bound_u  (alias)
 *   __libcgj_batch2562_marker = "libcgj-batch2562"
 *
 * Compression exclusive wave (2561–2570). Distinct from
 * gj_rle_encode_bound (batch1501) — unique _u symbol only.
 * Pairs with gj_rle_encode_u (batch2563) / gj_rle_decode_u (batch2564).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2562_marker[] = "libcgj-batch2562";

/* ---- freestanding helpers ---------------------------------------------- */

/* Worst-case (byte,count) RLE size: 2*cbN, saturating. */
static size_t
b2562_bound(size_t cbN)
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

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_rle_bound_u — worst-case (byte,count) RLE output size for n bytes.
 *
 * Simple RLE emits two octets per run chunk (value + count in 1..255).
 * The adversarial input is n singleton runs → exactly 2*n output bytes.
 */
size_t
gj_rle_bound_u(size_t n)
{
	(void)NULL;
	return b2562_bound(n);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_rle_bound_u(size_t n)
    __attribute__((alias("gj_rle_bound_u")));
