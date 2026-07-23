/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3677: freestanding protobuf key (tag) pack
 * (_u surface).
 *
 * Surface (unique symbols):
 *   uint32_t gj_protobuf_key_u(uint32_t field, uint32_t wire);
 *     - Return the protobuf key integer (field << 3) | (wire & 7).
 *       Wire type is forced into the low 3 bits. Field is taken as the
 *       remaining high bits of the shifted value (field bits above bit
 *       28 are truncated by the uint32 result). Does not validate that
 *       field is non-zero or that wire is a known type — pure pack.
 *   uint32_t __gj_protobuf_key_u  (alias)
 *   __libcgj_batch3677_marker = "libcgj-batch3677"
 *
 * CREATE-ONLY exclusive continuum wave (3671-3680). Unique
 * gj_protobuf_key_u surface only; no multi-def. Distinct from
 * gj_pb_encode_tag (batch131). Pairs with gj_protobuf_wire_u
 * (batch3678). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3677_marker[] = "libcgj-batch3677";

/* ---- freestanding helpers ---------------------------------------------- */

/* Pack field number and wire type into a protobuf key integer. */
static uint32_t
b3677_key(uint32_t u32Field, uint32_t u32Wire)
{
	return (uint32_t)((u32Field << 3) | (u32Wire & 7u));
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_protobuf_key_u - pack (field, wire) into a protobuf key integer.
 *
 * field: protobuf field number (shifted left by 3)
 * wire:  wire type; only low 3 bits are retained
 *
 * Pure integer pack. No parent wires.
 */
uint32_t
gj_protobuf_key_u(uint32_t field, uint32_t wire)
{
	(void)NULL;
	return b3677_key(field, wire);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_protobuf_key_u(uint32_t field, uint32_t wire)
    __attribute__((alias("gj_protobuf_key_u")));
