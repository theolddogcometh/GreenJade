/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch516: freestanding big-endian uint64 unpack.
 *
 * Surface (unique symbols):
 *   uint64_t gj_unpack_u64be(const unsigned char *in);
 *     — Read 8 big-endian bytes from in[0..7] into a uint64_t.
 *       NULL in → 0.
 *   uint64_t __gj_unpack_u64be  (alias)
 *   __libcgj_batch516_marker = "libcgj-batch516"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc calls. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch516_marker[] = "libcgj-batch516";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_unpack_u64be — load uint64_t from network / big-endian byte order.
 *
 * in: source of at least 8 bytes, or NULL (returns 0).
 * Returns the reconstructed host-order uint64_t; high byte at in[0],
 * low byte at in[7].
 */
uint64_t
gj_unpack_u64be(const unsigned char *in)
{
	if (in == NULL) {
		return (uint64_t)0;
	}
	return ((uint64_t)in[0] << 56)
	     | ((uint64_t)in[1] << 48)
	     | ((uint64_t)in[2] << 40)
	     | ((uint64_t)in[3] << 32)
	     | ((uint64_t)in[4] << 24)
	     | ((uint64_t)in[5] << 16)
	     | ((uint64_t)in[6] << 8)
	     |  (uint64_t)in[7];
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_unpack_u64be(const unsigned char *in)
    __attribute__((alias("gj_unpack_u64be")));
