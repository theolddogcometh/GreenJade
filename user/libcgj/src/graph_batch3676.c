/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3676: freestanding MessagePack fix-family
 * payload extract (_u surface).
 *
 * Surface (unique symbols):
 *   unsigned gj_msgpack_fix_u(uint8_t b);
 *     - If b is a MessagePack "fix" family type byte, return the
 *       embedded unsigned payload; otherwise return 0xffffffffu:
 *         positive fixint 0x00..0x7f → value (0..127)
 *         fixmap          0x80..0x8f → n (0..15)
 *         fixarray        0x90..0x9f → n (0..15)
 *         fixstr          0xa0..0xbf → n (0..31)
 *         negative fixint 0xe0..0xff → low 5 bits (0..31)
 *         all other type bytes       → 0xffffffffu
 *       Does not decode multi-byte MessagePack headers.
 *   unsigned __gj_msgpack_fix_u  (alias)
 *   __libcgj_batch3676_marker = "libcgj-batch3676"
 *
 * CREATE-ONLY exclusive continuum wave (3671-3680). Unique
 * gj_msgpack_fix_u surface only; no multi-def. Distinct from
 * gj_msgpack_is_map / gj_msgpack_is_array (batch1133/1134) and
 * gj_msgpack_encode_* (batch129). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3676_marker[] = "libcgj-batch3676";

/* Sentinel: not a fix-family type byte. */
#define B3676_NOT_FIX 0xffffffffu

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Extract fix-family embedded payload, or B3676_NOT_FIX.
 * MessagePack format family (single-byte headers only).
 */
static unsigned
b3676_fix(uint8_t u8B)
{
	if ((u8B & 0x80u) == 0u) {
		/* positive fixint: 0xxxxxxx */
		return (unsigned)u8B;
	}
	if ((u8B & 0xf0u) == 0x80u) {
		/* fixmap: 1000xxxx */
		return (unsigned)(u8B & 0x0fu);
	}
	if ((u8B & 0xf0u) == 0x90u) {
		/* fixarray: 1001xxxx */
		return (unsigned)(u8B & 0x0fu);
	}
	if ((u8B & 0xe0u) == 0xa0u) {
		/* fixstr: 101xxxxx */
		return (unsigned)(u8B & 0x1fu);
	}
	if ((u8B & 0xe0u) == 0xe0u) {
		/* negative fixint: 111xxxxx → low 5 bits */
		return (unsigned)(u8B & 0x1fu);
	}
	return (unsigned)B3676_NOT_FIX;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_msgpack_fix_u - embedded payload of a MessagePack fix-family byte.
 *
 * b: MessagePack type / value byte
 *
 * Returns 0..127 for positive fixint, 0..15/0..31 for fix container
 * headers, 0..31 for negative fixint payload bits, or 0xffffffffu when
 * b is not a fix-family header. No parent wires.
 */
unsigned
gj_msgpack_fix_u(uint8_t b)
{
	(void)NULL;
	return b3676_fix(b);
}

/* ---- underscored alias ------------------------------------------------- */

unsigned __gj_msgpack_fix_u(uint8_t b)
    __attribute__((alias("gj_msgpack_fix_u")));
