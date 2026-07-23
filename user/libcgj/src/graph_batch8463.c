/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8463: UUID nil-ok layout tag stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_uuid_nil_ok_u_8463(void);
 *     - Return 1 (nil UUID layout accepted as a valid soft state).
 *   uint32_t __gj_uuid_nil_ok_u_8463  (alias)
 *   __libcgj_batch8463_marker = "libcgj-batch8463"
 *
 * Exclusive continuum CREATE-ONLY (8461-8470: uuid/layout tag stubs —
 * ver4_id, var_rfc_id, nil_ok, str_len_id, bin_len_id, dash_pos_ok,
 * hex_ok, parse_ready, fmt_ready, batch_id_8470).
 * Unique gj_uuid_nil_ok_u_8463 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8463_marker[] = "libcgj-batch8463";

/* Soft nil-UUID layout acceptance lamp. */
#define B8463_UUID_NIL_OK  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8463_uuid_nil_ok(void)
{
	return B8463_UUID_NIL_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_uuid_nil_ok_u_8463 - soft nil-UUID layout ok tag.
 *
 * Always returns 1 (nil UUID accepted as a soft layout state). Does not
 * inspect bytes or call libc. No parent wires.
 */
uint32_t
gj_uuid_nil_ok_u_8463(void)
{
	(void)NULL;
	return b8463_uuid_nil_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_uuid_nil_ok_u_8463(void)
    __attribute__((alias("gj_uuid_nil_ok_u_8463")));
