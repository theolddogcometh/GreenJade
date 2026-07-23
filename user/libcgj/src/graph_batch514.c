/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch514: big-endian uint32 unpack from 4 bytes.
 *
 * Surface (unique symbols):
 *   uint32_t gj_unpack_u32be(const unsigned char *in);
 *     — Read a big-endian 32-bit value from the four bytes at in.
 *       NULL in returns 0.
 *   uint32_t __gj_unpack_u32be  (alias)
 *   __libcgj_batch514_marker = "libcgj-batch514"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc string calls. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch514_marker[] = "libcgj-batch514";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_unpack_u32be — load a big-endian uint32_t from four bytes.
 *
 * in:  pointer to at least 4 readable bytes in network/big-endian order.
 *      If NULL, returns 0 without reading.
 *
 * Returns (in[0]<<24)|(in[1]<<16)|(in[2]<<8)|in[3], or 0 when in is NULL.
 */
uint32_t
gj_unpack_u32be(const unsigned char *in)
{
	if (in == NULL) {
		return 0u;
	}
	return ((uint32_t)in[0] << 24) | ((uint32_t)in[1] << 16) |
	       ((uint32_t)in[2] << 8) | (uint32_t)in[3];
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_unpack_u32be(const unsigned char *in)
    __attribute__((alias("gj_unpack_u32be")));
