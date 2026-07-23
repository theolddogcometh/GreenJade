/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2927: freestanding big-endian uint16 unpack (_u).
 *
 * Surface (unique symbols):
 *   uint16_t gj_unpack_u16_be_u(const unsigned char *in);
 *     - Read big-endian u16 from in[0..1]. NULL in → 0.
 *   uint16_t __gj_unpack_u16_be_u  (alias)
 *   __libcgj_batch2927_marker = "libcgj-batch2927"
 *
 * Exclusive continuum CREATE-ONLY (2921-2930). Distinct from
 * gj_unpack_u16be (batch512) — unique gj_unpack_u16_be_u surface only;
 * no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2927_marker[] = "libcgj-batch2927";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * b2927_load_be16 - load big-endian u16 from two bytes.
 */
static uint16_t
b2927_load_be16(const unsigned char *pIn)
{
	return (uint16_t)(((uint16_t)pIn[0] << 8) | (uint16_t)pIn[1]);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_unpack_u16_be_u - load uint16_t from network / big-endian byte order.
 *
 * in: pointer to at least 2 readable bytes, or NULL (returns 0).
 *     high byte at in[0], low byte at in[1].
 */
uint16_t
gj_unpack_u16_be_u(const unsigned char *pIn)
{
	(void)NULL;
	if (pIn == NULL) {
		return 0u;
	}
	return b2927_load_be16(pIn);
}

/* ---- underscored alias ------------------------------------------------- */

uint16_t __gj_unpack_u16_be_u(const unsigned char *pIn)
    __attribute__((alias("gj_unpack_u16_be_u")));
