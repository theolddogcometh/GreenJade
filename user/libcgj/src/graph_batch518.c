/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch518: freestanding little-endian uint16 unpack.
 *
 * Surface (unique symbols):
 *   uint16_t gj_unpack_u16le(const unsigned char *in);
 *     — Read little-endian uint16 from in[0..1]. NULL in → 0.
 *   uint16_t __gj_unpack_u16le  (alias)
 *   __libcgj_batch518_marker = "libcgj-batch518"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc calls. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch518_marker[] = "libcgj-batch518";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_unpack_u16le — load uint16_t from little-endian byte order.
 *
 * in: source of at least 2 bytes, or NULL (returns 0).
 *     low byte at in[0], high byte at in[1].
 */
uint16_t
gj_unpack_u16le(const unsigned char *in)
{
	if (in == NULL) {
		return 0u;
	}
	return (uint16_t)((uint16_t)in[0] | ((uint16_t)in[1] << 8));
}

/* ---- underscored alias ------------------------------------------------- */

uint16_t __gj_unpack_u16le(const unsigned char *in)
    __attribute__((alias("gj_unpack_u16le")));
