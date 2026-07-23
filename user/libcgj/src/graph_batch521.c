/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch521: freestanding little-endian uint64 pack.
 *
 * Surface (unique symbols):
 *   void gj_pack_u64le(unsigned char *out, uint64_t v);
 *     — Write v as little-endian into out[0..7]. NULL out → no-op.
 *   void __gj_pack_u64le  (alias)
 *   __libcgj_batch521_marker = "libcgj-batch521"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc calls. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch521_marker[] = "libcgj-batch521";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pack_u64le — store uint64_t in little-endian byte order.
 *
 * out: destination of at least 8 bytes, or NULL (no-op).
 * v:   value; least-significant byte at out[0], most-significant at out[7].
 */
void
gj_pack_u64le(unsigned char *out, uint64_t v)
{
	if (out == NULL) {
		return;
	}
	out[0] = (unsigned char)(v & 0xffu);
	out[1] = (unsigned char)((v >> 8) & 0xffu);
	out[2] = (unsigned char)((v >> 16) & 0xffu);
	out[3] = (unsigned char)((v >> 24) & 0xffu);
	out[4] = (unsigned char)((v >> 32) & 0xffu);
	out[5] = (unsigned char)((v >> 40) & 0xffu);
	out[6] = (unsigned char)((v >> 48) & 0xffu);
	out[7] = (unsigned char)((v >> 56) & 0xffu);
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_pack_u64le(unsigned char *out, uint64_t v)
    __attribute__((alias("gj_pack_u64le")));
