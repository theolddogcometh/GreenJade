/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3674: freestanding CBOR major-type extract
 * (_u surface).
 *
 * Surface (unique symbols):
 *   unsigned gj_cbor_major_u(uint8_t initial);
 *     - Return the CBOR major type from the initial byte (bits 7..5),
 *       range 0..7 (RFC 8949 §3). Distinct from gj_cbor_major_type
 *       (batch1132) by public name — exclusive _u wave surface.
 *   unsigned __gj_cbor_major_u  (alias)
 *   __libcgj_batch3674_marker = "libcgj-batch3674"
 *
 * CREATE-ONLY exclusive continuum wave (3671-3680). Unique
 * gj_cbor_major_u surface only; no multi-def. Pairs with gj_cbor_ai_u
 * (batch3675). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3674_marker[] = "libcgj-batch3674";

/* ---- freestanding helpers ---------------------------------------------- */

/* Major type = top 3 bits of the CBOR initial byte. */
static unsigned
b3674_major(uint8_t u8Initial)
{
	return (unsigned)((u8Initial >> 5) & 0x7u);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cbor_major_u - CBOR major type from initial byte (bits 7..5).
 *
 * initial: first octet of a CBOR data item
 *
 * Result is always in 0..7. No parent wires.
 */
unsigned
gj_cbor_major_u(uint8_t initial)
{
	(void)NULL;
	return b3674_major(initial);
}

/* ---- underscored alias ------------------------------------------------- */

unsigned __gj_cbor_major_u(uint8_t initial)
    __attribute__((alias("gj_cbor_major_u")));
