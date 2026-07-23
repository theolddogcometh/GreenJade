/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch522: freestanding little-endian uint64 unpack.
 *
 * Surface (unique symbols):
 *   uint64_t gj_unpack_u64le(const unsigned char *in);
 *     — Read a 64-bit little-endian unsigned integer from in[0..7].
 *       NULL in → 0.
 *   uint64_t __gj_unpack_u64le  (alias)
 *   __libcgj_batch522_marker = "libcgj-batch522"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch522_marker[] = "libcgj-batch522";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_unpack_u64le — load one little-endian uint64 from eight bytes.
 *
 * in: pointer to at least eight readable bytes (NULL → 0)
 *
 * Wire layout: in[0] is the least-significant byte, in[7] is the MSB.
 *   value = in[0]
 *         | (in[1] << 8)  | (in[2] << 16) | (in[3] << 24)
 *         | (in[4] << 32) | (in[5] << 40) | (in[6] << 48)
 *         | (in[7] << 56)
 *
 * Returns the host-order uint64_t value, or 0 if in is NULL.
 * Unaligned-safe (byte loads only; no multi-byte pointer cast).
 */
uint64_t
gj_unpack_u64le(const unsigned char *in)
{
	if (in == NULL) {
		return 0ull;
	}
	return (uint64_t)in[0] |
	       ((uint64_t)in[1] << 8) |
	       ((uint64_t)in[2] << 16) |
	       ((uint64_t)in[3] << 24) |
	       ((uint64_t)in[4] << 32) |
	       ((uint64_t)in[5] << 40) |
	       ((uint64_t)in[6] << 48) |
	       ((uint64_t)in[7] << 56);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_unpack_u64le(const unsigned char *in)
    __attribute__((alias("gj_unpack_u64le")));
