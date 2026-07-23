/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch512: freestanding big-endian uint16 unpack.
 *
 * Surface (unique symbols):
 *   uint16_t gj_unpack_u16be(const unsigned char *in);
 *     — Read big-endian u16 from in[0..1]. NULL in → 0.
 *   uint16_t __gj_unpack_u16be  (alias)
 *   __libcgj_batch512_marker = "libcgj-batch512"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc calls. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch512_marker[] = "libcgj-batch512";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_unpack_u16be — load uint16_t from network / big-endian byte order.
 *
 * in: pointer to at least 2 readable bytes, or NULL (returns 0).
 *     high byte at in[0], low byte at in[1].
 */
uint16_t
gj_unpack_u16be(const unsigned char *in)
{
	if (in == NULL) {
		return 0u;
	}
	return (uint16_t)(((uint16_t)in[0] << 8) | (uint16_t)in[1]);
}

/* ---- underscored alias ------------------------------------------------- */

uint16_t __gj_unpack_u16be(const unsigned char *in)
    __attribute__((alias("gj_unpack_u16be")));
