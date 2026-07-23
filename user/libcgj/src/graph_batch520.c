/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch520: freestanding little-endian uint32 unpack.
 *
 * Surface (unique symbols):
 *   uint32_t gj_unpack_u32le(const unsigned char *in);
 *     — Read little-endian uint32 from in[0..3]. NULL in → 0.
 *   uint32_t __gj_unpack_u32le  (alias)
 *   __libcgj_batch520_marker = "libcgj-batch520"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc calls. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch520_marker[] = "libcgj-batch520";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_unpack_u32le — load uint32_t from little-endian byte order.
 *
 * in: source of at least 4 bytes, or NULL (returns 0).
 *     low byte at in[0], high byte at in[3].
 *
 * Returns (in[0])|(in[1]<<8)|(in[2]<<16)|(in[3]<<24), or 0 when in is NULL.
 */
uint32_t
gj_unpack_u32le(const unsigned char *in)
{
	if (in == NULL) {
		return 0u;
	}
	return (uint32_t)in[0]
	     | ((uint32_t)in[1] << 8)
	     | ((uint32_t)in[2] << 16)
	     | ((uint32_t)in[3] << 24);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_unpack_u32le(const unsigned char *in)
    __attribute__((alias("gj_unpack_u32le")));
