/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3680: batch identity for exclusive wire-format
 * continuum wave (3671-3680).
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_3680(void);
 *     - Returns the compile-time graph batch number for this TU (3680).
 *   uint32_t __gj_batch_id_3680  (alias)
 *   __libcgj_batch3680_marker = "libcgj-batch3680"
 *
 * CREATE-ONLY exclusive continuum wave (3671-3680: tlv_tag_ok_u,
 * tlv_len_ok_u, ber_len_need_u, cbor_major_u, cbor_ai_u, msgpack_fix_u,
 * protobuf_key_u, protobuf_wire_u, varint_u32_len_u, batch_id_3680).
 * Unique gj_batch_id_3680 surface only; no multi-def. Does NOT redefine
 * gj_batch_id / prior batch_id_* symbols. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3680_marker[] = "libcgj-batch3680";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b3680_id(void)
{
	return 3680u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_3680 - report this TU's graph batch number.
 *
 * Always returns 3680. Link-time presence tags the 3671-3680 wave.
 * No parent wires.
 */
uint32_t
gj_batch_id_3680(void)
{
	(void)NULL;
	return b3680_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_3680(void)
    __attribute__((alias("gj_batch_id_3680")));
