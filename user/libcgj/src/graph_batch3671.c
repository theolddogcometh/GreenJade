/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3671: freestanding TLV tag acceptability
 * predicate (_u surface).
 *
 * Surface (unique symbols):
 *   int gj_tlv_tag_ok_u(uint32_t tag);
 *     - Return 1 if tag is a usable TLV type identifier (non-zero),
 *       else 0. Tag 0 is reserved/invalid for this surface. Does not
 *       parse multi-octet BER tag streams — value is the already-
 *       decoded tag number / type field.
 *   int __gj_tlv_tag_ok_u  (alias)
 *   __libcgj_batch3671_marker = "libcgj-batch3671"
 *
 * CREATE-ONLY exclusive continuum wave (3671-3680): tlv_tag_ok_u,
 * tlv_len_ok_u, ber_len_need_u, cbor_major_u, cbor_ai_u, msgpack_fix_u,
 * protobuf_key_u, protobuf_wire_u, varint_u32_len_u, batch_id_3680.
 * Unique gj_tlv_tag_ok_u surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3671_marker[] = "libcgj-batch3671";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if tag is a non-zero usable type id. */
static int
b3671_tag_ok(uint32_t u32Tag)
{
	return (u32Tag != 0u) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_tlv_tag_ok_u - true if tag is an acceptable TLV type identifier.
 *
 * tag: decoded type / tag number (0 is rejected)
 *
 * Pure predicate; does not allocate or touch memory. No parent wires.
 */
int
gj_tlv_tag_ok_u(uint32_t tag)
{
	(void)NULL;
	return b3671_tag_ok(tag);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_tlv_tag_ok_u(uint32_t tag)
    __attribute__((alias("gj_tlv_tag_ok_u")));
