/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch519: freestanding little-endian uint32 pack.
 *
 * Surface (unique symbols):
 *   void gj_pack_u32le(unsigned char *out, uint32_t v);
 *     — Write v as little-endian into out[0..3]. NULL out → no-op.
 *   void __gj_pack_u32le  (alias)
 *   __libcgj_batch519_marker = "libcgj-batch519"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc calls. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch519_marker[] = "libcgj-batch519";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pack_u32le — store uint32_t in little-endian byte order.
 *
 * out: destination of at least 4 bytes, or NULL (no-op).
 * v:   value; low byte at out[0], high byte at out[3].
 */
void
gj_pack_u32le(unsigned char *out, uint32_t v)
{
	if (out == NULL) {
		return;
	}
	out[0] = (unsigned char)(v & 0xffu);
	out[1] = (unsigned char)((v >> 8) & 0xffu);
	out[2] = (unsigned char)((v >> 16) & 0xffu);
	out[3] = (unsigned char)((v >> 24) & 0xffu);
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_pack_u32le(unsigned char *out, uint32_t v)
    __attribute__((alias("gj_pack_u32le")));
