/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3675: freestanding CBOR additional-info
 * extract (_u surface).
 *
 * Surface (unique symbols):
 *   unsigned gj_cbor_ai_u(uint8_t initial);
 *     - Return the CBOR additional information from the initial byte
 *       (bits 4..0), range 0..31 (RFC 8949 §3). ai 0..23 embed the
 *       argument; 24..27 select 1/2/4/8-byte follow; 31 is break in
 *       indefinite contexts; 28..30 are reserved.
 *   unsigned __gj_cbor_ai_u  (alias)
 *   __libcgj_batch3675_marker = "libcgj-batch3675"
 *
 * CREATE-ONLY exclusive continuum wave (3671-3680). Unique
 * gj_cbor_ai_u surface only; no multi-def. Pairs with gj_cbor_major_u
 * (batch3674). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3675_marker[] = "libcgj-batch3675";

/* ---- freestanding helpers ---------------------------------------------- */

/* Additional info = low 5 bits of the CBOR initial byte. */
static unsigned
b3675_ai(uint8_t u8Initial)
{
	return (unsigned)(u8Initial & 0x1fu);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cbor_ai_u - CBOR additional information from initial byte (bits 4..0).
 *
 * initial: first octet of a CBOR data item
 *
 * Result is always in 0..31. No parent wires.
 */
unsigned
gj_cbor_ai_u(uint8_t initial)
{
	(void)NULL;
	return b3675_ai(initial);
}

/* ---- underscored alias ------------------------------------------------- */

unsigned __gj_cbor_ai_u(uint8_t initial)
    __attribute__((alias("gj_cbor_ai_u")));
