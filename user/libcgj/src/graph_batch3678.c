/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3678: freestanding protobuf wire-type extract
 * (_u surface).
 *
 * Surface (unique symbols):
 *   unsigned gj_protobuf_wire_u(uint32_t key);
 *     - Return the protobuf wire type from a key integer (key & 7),
 *       range 0..7. Known types: 0 varint, 1 64-bit, 2 length-delimited,
 *       3 start group, 4 end group, 5 32-bit; 6 and 7 are reserved /
 *       unused on the wire but are returned as-is if present.
 *   unsigned __gj_protobuf_wire_u  (alias)
 *   __libcgj_batch3678_marker = "libcgj-batch3678"
 *
 * CREATE-ONLY exclusive continuum wave (3671-3680). Unique
 * gj_protobuf_wire_u surface only; no multi-def. Distinct from
 * gj_pb_decode_tag (batch131). Pairs with gj_protobuf_key_u
 * (batch3677). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3678_marker[] = "libcgj-batch3678";

/* ---- freestanding helpers ---------------------------------------------- */

/* Wire type = low 3 bits of the protobuf key. */
static unsigned
b3678_wire(uint32_t u32Key)
{
	return (unsigned)(u32Key & 7u);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_protobuf_wire_u - wire type from a protobuf key integer (key & 7).
 *
 * key: packed protobuf key ((field << 3) | wire)
 *
 * Result is always in 0..7. No parent wires.
 */
unsigned
gj_protobuf_wire_u(uint32_t key)
{
	(void)NULL;
	return b3678_wire(key);
}

/* ---- underscored alias ------------------------------------------------- */

unsigned __gj_protobuf_wire_u(uint32_t key)
    __attribute__((alias("gj_protobuf_wire_u")));
