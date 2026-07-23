/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2928: freestanding little-endian uint16 unpack (_u).
 *
 * Surface (unique symbols):
 *   uint16_t gj_unpack_u16_le_u(const unsigned char *in);
 *     - Read little-endian u16 from in[0..1]. NULL in → 0.
 *   uint16_t __gj_unpack_u16_le_u  (alias)
 *   __libcgj_batch2928_marker = "libcgj-batch2928"
 *
 * Exclusive continuum CREATE-ONLY (2921-2930). Distinct from
 * gj_unpack_u16le (batch518) — unique gj_unpack_u16_le_u surface only;
 * no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2928_marker[] = "libcgj-batch2928";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * b2928_load_le16 - load little-endian u16 from two bytes.
 */
static uint16_t
b2928_load_le16(const unsigned char *pIn)
{
	return (uint16_t)((uint16_t)pIn[0] | ((uint16_t)pIn[1] << 8));
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_unpack_u16_le_u - load uint16_t from little-endian byte order.
 *
 * in: source of at least 2 bytes, or NULL (returns 0).
 *     low byte at in[0], high byte at in[1].
 */
uint16_t
gj_unpack_u16_le_u(const unsigned char *pIn)
{
	(void)NULL;
	if (pIn == NULL) {
		return 0u;
	}
	return b2928_load_le16(pIn);
}

/* ---- underscored alias ------------------------------------------------- */

uint16_t __gj_unpack_u16_le_u(const unsigned char *pIn)
    __attribute__((alias("gj_unpack_u16_le_u")));
