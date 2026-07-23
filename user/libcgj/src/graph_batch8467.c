/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8467: UUID hex alphabet layout ok tag stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_uuid_hex_ok_u_8467(void);
 *     - Return 1 (hex alphabet layout [0-9a-fA-F] accepted).
 *   uint32_t __gj_uuid_hex_ok_u_8467  (alias)
 *   __libcgj_batch8467_marker = "libcgj-batch8467"
 *
 * Exclusive continuum CREATE-ONLY (8461-8470: uuid/layout tag stubs —
 * ver4_id, var_rfc_id, nil_ok, str_len_id, bin_len_id, dash_pos_ok,
 * hex_ok, parse_ready, fmt_ready, batch_id_8470).
 * Unique gj_uuid_hex_ok_u_8467 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8467_marker[] = "libcgj-batch8467";

/* Soft hex-alphabet layout acceptance lamp. */
#define B8467_UUID_HEX_OK  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8467_uuid_hex_ok(void)
{
	return B8467_UUID_HEX_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_uuid_hex_ok_u_8467 - soft UUID hex alphabet layout ok tag.
 *
 * Always returns 1 (hex digits accepted for UUID layout). Does not
 * classify characters or call libc. No parent wires.
 */
uint32_t
gj_uuid_hex_ok_u_8467(void)
{
	(void)NULL;
	return b8467_uuid_hex_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_uuid_hex_ok_u_8467(void)
    __attribute__((alias("gj_uuid_hex_ok_u_8467")));
