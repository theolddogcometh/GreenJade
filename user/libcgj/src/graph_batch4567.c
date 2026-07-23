/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4567: base64 '=' pad count for n raw octets (_u).
 *
 * Surface (unique symbols):
 *   size_t gj_b64_pad_count_u(size_t n);
 *     - Canonical pad characters needed when encoding n bytes:
 *         n % 3 == 0 → 0
 *         n % 3 == 1 → 2
 *         n % 3 == 2 → 1
 *   size_t __gj_b64_pad_count_u  (alias)
 *   __libcgj_batch4567_marker = "libcgj-batch4567"
 *
 * Exclusive continuum CREATE-ONLY (4561-4570). Length/pad helper only;
 * not a codec. No multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4567_marker[] = "libcgj-batch4567";

/* ---- freestanding helpers ---------------------------------------------- */

/* Map remainder of n mod 3 to pad count 0 / 2 / 1. */
static size_t
b4567_pad_count(size_t cbN)
{
	size_t rem;

	rem = cbN % 3u;
	if (rem == 0u) {
		return 0u;
	}
	if (rem == 1u) {
		return 2u;
	}
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_b64_pad_count_u - number of '=' pads for encoding n raw octets.
 *
 * n: raw input byte count
 *
 * Returns 0, 1, or 2. Self-contained; no parent wires.
 */
size_t
gj_b64_pad_count_u(size_t n)
{
	(void)NULL;
	return b4567_pad_count(n);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_b64_pad_count_u(size_t n)
    __attribute__((alias("gj_b64_pad_count_u")));
