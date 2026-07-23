/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2811: padded base64 encode length (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_base64_enc_len_u(uint32_t n);
 *     - Return the number of base64 output characters needed to encode
 *       n input octets with standard '=' padding:
 *         4 * ((n + 2) / 3)
 *       n == 0 → 0. Saturates to 0xffffffffu on arithmetic overflow.
 *       Pure length math; does not read or write buffers.
 *   uint32_t __gj_base64_enc_len_u  (alias)
 *   __libcgj_batch2811_marker = "libcgj-batch2811"
 *
 * Encoding length exclusive wave (2811-2820). Distinct from
 * gj_base64_enc_len (size_t, batch693) — unique _u surface; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2811_marker[] = "libcgj-batch2811";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Padded base64 encode length for uN octets.
 * groups = ceil(n/3) = (n+2)/3; result = 4*groups, with sat.
 */
static uint32_t
b2811_enc_len(uint32_t uN)
{
	uint32_t uGroups;

	if (uN == 0u) {
		return 0u;
	}
	/* n+2 must not wrap: n > 0xfffffffd → saturate. */
	if (uN > 0xfffffffdu) {
		return 0xffffffffu;
	}
	uGroups = (uN + 2u) / 3u;
	/* 4 * groups must not wrap. */
	if (uGroups > (0xffffffffu / 4u)) {
		return 0xffffffffu;
	}
	return uGroups * 4u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_base64_enc_len_u - padded base64 encode output length for n octets.
 *
 * n: number of input octets
 *
 * Each group of up to 3 input octets becomes 4 output characters
 * (with '=' padding when n is not a multiple of 3). Equivalent to:
 *   4 * ceil(n / 3)  computed as  4 * ((n + 2) / 3)
 * with unsigned integer division. Returns 0 when n is 0. Returns
 * 0xffffffffu if the result would overflow uint32_t.
 */
uint32_t
gj_base64_enc_len_u(uint32_t uN)
{
	(void)NULL;
	return b2811_enc_len(uN);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_base64_enc_len_u(uint32_t uN)
    __attribute__((alias("gj_base64_enc_len_u")));
