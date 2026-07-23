/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch511: freestanding big-endian uint16 pack.
 *
 * Surface (unique symbols):
 *   void gj_pack_u16be(unsigned char *out, uint16_t v);
 *     — Write v as big-endian into out[0..1]. NULL out → no-op.
 *   void __gj_pack_u16be  (alias)
 *   __libcgj_batch511_marker = "libcgj-batch511"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc calls. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch511_marker[] = "libcgj-batch511";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pack_u16be — store uint16_t in network / big-endian byte order.
 *
 * out: destination of at least 2 bytes, or NULL (no-op).
 * v:   value; high byte at out[0], low byte at out[1].
 */
void
gj_pack_u16be(unsigned char *out, uint16_t v)
{
	if (out == NULL) {
		return;
	}
	out[0] = (unsigned char)((v >> 8) & 0xffu);
	out[1] = (unsigned char)(v & 0xffu);
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_pack_u16be(unsigned char *out, uint16_t v)
    __attribute__((alias("gj_pack_u16be")));
