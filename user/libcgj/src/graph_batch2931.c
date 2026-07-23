/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2931: freestanding little-endian uint32 unpack
 * (_u continuum).
 *
 * Surface (unique symbols):
 *   uint32_t gj_unpack_u32_le_u(const unsigned char *in);
 *     - Read little-endian uint32 from in[0..3]. NULL in -> 0.
 *       Unaligned-safe (byte loads only).
 *   uint32_t __gj_unpack_u32_le_u  (alias)
 *   __libcgj_batch2931_marker = "libcgj-batch2931"
 *
 * Pack/mod exclusive wave (2931-2940). Distinct from gj_unpack_u32le
 * (batch520) and gj_bytes_to_u32_le_u (batch2787) — unique
 * gj_unpack_u32_le_u surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No __int128. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2931_marker[] = "libcgj-batch2931";

/* ---- freestanding helpers ---------------------------------------------- */

/* Load one LE u32 from four octets (may be unaligned). pB non-NULL. */
static uint32_t
b2931_load(const unsigned char *pB)
{
	return (uint32_t)pB[0]
	    | ((uint32_t)pB[1] << 8)
	    | ((uint32_t)pB[2] << 16)
	    | ((uint32_t)pB[3] << 24);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_unpack_u32_le_u - load uint32_t from little-endian byte order.
 *
 * in: source of at least 4 bytes, or NULL (returns 0).
 *     low byte at in[0], high byte at in[3].
 *
 * Returns (in[0])|(in[1]<<8)|(in[2]<<16)|(in[3]<<24), or 0 when in is NULL.
 */
uint32_t
gj_unpack_u32_le_u(const unsigned char *pIn)
{
	(void)NULL;
	if (pIn == NULL) {
		return 0u;
	}
	return b2931_load(pIn);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_unpack_u32_le_u(const unsigned char *pIn)
    __attribute__((alias("gj_unpack_u32_le_u")));
