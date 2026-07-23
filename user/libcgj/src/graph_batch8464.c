/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8464: UUID string length layout id tag stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_uuid_str_len_id_8464(void);
 *     - Return soft UUID canonical string length without NUL (36).
 *   uint32_t __gj_uuid_str_len_id_8464  (alias)
 *   __libcgj_batch8464_marker = "libcgj-batch8464"
 *
 * Exclusive continuum CREATE-ONLY (8461-8470: uuid/layout tag stubs —
 * ver4_id, var_rfc_id, nil_ok, str_len_id, bin_len_id, dash_pos_ok,
 * hex_ok, parse_ready, fmt_ready, batch_id_8470).
 * Unique gj_uuid_str_len_id_8464 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8464_marker[] = "libcgj-batch8464";

/* Soft UUID canonical string length (8-4-4-4-12 hex, no NUL). */
#define B8464_UUID_STR_LEN ((uint32_t)36u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8464_uuid_str_len_id(void)
{
	return B8464_UUID_STR_LEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_uuid_str_len_id_8464 - soft UUID string length layout id tag.
 *
 * Always returns 36. Catalog id only; does not format or measure strings.
 * No parent wires.
 */
uint32_t
gj_uuid_str_len_id_8464(void)
{
	(void)NULL;
	return b8464_uuid_str_len_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_uuid_str_len_id_8464(void)
    __attribute__((alias("gj_uuid_str_len_id_8464")));
