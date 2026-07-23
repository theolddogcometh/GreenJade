/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2929: freestanding big-endian uint32 unpack (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_unpack_u32_be_u(const unsigned char *in);
 *     - Read big-endian u32 from in[0..3]. NULL in → 0.
 *   uint32_t __gj_unpack_u32_be_u  (alias)
 *   __libcgj_batch2929_marker = "libcgj-batch2929"
 *
 * Exclusive continuum CREATE-ONLY (2921-2930). Distinct from
 * gj_unpack_u32be (batch514) — unique gj_unpack_u32_be_u surface only;
 * no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2929_marker[] = "libcgj-batch2929";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * b2929_load_be32 - load big-endian u32 from four bytes.
 */
static uint32_t
b2929_load_be32(const unsigned char *pIn)
{
	return ((uint32_t)pIn[0] << 24) | ((uint32_t)pIn[1] << 16) |
	       ((uint32_t)pIn[2] << 8) | (uint32_t)pIn[3];
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_unpack_u32_be_u - load a big-endian uint32_t from four bytes.
 *
 * in:  pointer to at least 4 readable bytes in network/big-endian order.
 *      If NULL, returns 0 without reading.
 *
 * Returns (in[0]<<24)|(in[1]<<16)|(in[2]<<8)|in[3], or 0 when in is NULL.
 */
uint32_t
gj_unpack_u32_be_u(const unsigned char *pIn)
{
	(void)NULL;
	if (pIn == NULL) {
		return 0u;
	}
	return b2929_load_be32(pIn);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_unpack_u32_be_u(const unsigned char *pIn)
    __attribute__((alias("gj_unpack_u32_be_u")));
