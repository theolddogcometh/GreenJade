/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8465: UUID binary length layout id tag stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_uuid_bin_len_id_8465(void);
 *     - Return soft UUID binary layout length in octets (16).
 *   uint32_t __gj_uuid_bin_len_id_8465  (alias)
 *   __libcgj_batch8465_marker = "libcgj-batch8465"
 *
 * Exclusive continuum CREATE-ONLY (8461-8470: uuid/layout tag stubs —
 * ver4_id, var_rfc_id, nil_ok, str_len_id, bin_len_id, dash_pos_ok,
 * hex_ok, parse_ready, fmt_ready, batch_id_8470).
 * Unique gj_uuid_bin_len_id_8465 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8465_marker[] = "libcgj-batch8465";

/* Soft UUID binary layout length (128-bit / 16 octets). */
#define B8465_UUID_BIN_LEN ((uint32_t)16u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8465_uuid_bin_len_id(void)
{
	return B8465_UUID_BIN_LEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_uuid_bin_len_id_8465 - soft UUID binary length layout id tag.
 *
 * Always returns 16. Catalog id only; does not allocate or copy buffers.
 * No parent wires.
 */
uint32_t
gj_uuid_bin_len_id_8465(void)
{
	(void)NULL;
	return b8465_uuid_bin_len_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_uuid_bin_len_id_8465(void)
    __attribute__((alias("gj_uuid_bin_len_id_8465")));
