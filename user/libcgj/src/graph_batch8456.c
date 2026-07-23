/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8456: hex decode buffer need stub.
 *
 * Surface (unique symbols):
 *   size_t gj_hex_decode_need_u_8456(size_t n);
 *     - Binary buffer octets needed to decode a hex string of length n:
 *       n even → n / 2; n odd → 0 (incomplete pair stream).
 *   size_t __gj_hex_decode_need_u_8456  (alias)
 *   __libcgj_batch8456_marker = "libcgj-batch8456"
 *
 * Exclusive continuum CREATE-ONLY (8451-8460: hex encode/decode stubs).
 * Distinct from gj_hex_dec_len (batch823) — unique
 * gj_hex_decode_need_u_8456 surface only; no multi-def. Companion to
 * gj_hex_encode_need_u_8455. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8456_marker[] = "libcgj-batch8456";

/* ---- freestanding helpers ---------------------------------------------- */

/* Even n → n/2 whole octets; odd n cannot form complete pairs → 0. */
static size_t
b8456_decode_need(size_t cbHex)
{
	if ((cbHex & 1u) != 0u) {
		return 0u;
	}
	return cbHex / 2u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hex_decode_need_u_8456 - binary buffer size for n hex characters.
 *
 * n: number of hex digit characters (not including a trailing NUL).
 *
 * Returns n/2 when n is even (including 0), else 0. Pure length math.
 * No parent wires.
 */
size_t
gj_hex_decode_need_u_8456(size_t cbHex)
{
	(void)NULL;
	return b8456_decode_need(cbHex);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_hex_decode_need_u_8456(size_t cbHex)
    __attribute__((alias("gj_hex_decode_need_u_8456")));
