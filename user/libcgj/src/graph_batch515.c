/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch515: freestanding big-endian uint64 pack.
 *
 * Surface (unique symbols):
 *   void gj_pack_u64be(unsigned char *out, uint64_t v);
 *     — write v as 8 big-endian octets into out[0..7].
 *       No-op if out is NULL.
 *   void __gj_pack_u64be(...)  (alias)
 *   __libcgj_batch515_marker = "libcgj-batch515"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch515_marker[] = "libcgj-batch515";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pack_u64be — store v as big-endian into 8 octets.
 *
 * out: destination buffer of at least 8 bytes, or NULL for no-op.
 * v:   value to pack (MSB first).
 */
void
gj_pack_u64be(unsigned char *out, uint64_t v)
{
	if (out == NULL) {
		return;
	}

	out[0] = (unsigned char)((v >> 56) & 0xffull);
	out[1] = (unsigned char)((v >> 48) & 0xffull);
	out[2] = (unsigned char)((v >> 40) & 0xffull);
	out[3] = (unsigned char)((v >> 32) & 0xffull);
	out[4] = (unsigned char)((v >> 24) & 0xffull);
	out[5] = (unsigned char)((v >> 16) & 0xffull);
	out[6] = (unsigned char)((v >> 8) & 0xffull);
	out[7] = (unsigned char)(v & 0xffull);
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_pack_u64be(unsigned char *out, uint64_t v)
    __attribute__((alias("gj_pack_u64be")));
