/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8461: UUID version-4 layout id tag stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_uuid_ver4_id_8461(void);
 *     - Return soft UUID version-4 layout id (4).
 *   uint32_t __gj_uuid_ver4_id_8461  (alias)
 *   __libcgj_batch8461_marker = "libcgj-batch8461"
 *
 * Exclusive continuum CREATE-ONLY (8461-8470: uuid/layout tag stubs —
 * ver4_id, var_rfc_id, nil_ok, str_len_id, bin_len_id, dash_pos_ok,
 * hex_ok, parse_ready, fmt_ready, batch_id_8470).
 * Unique gj_uuid_ver4_id_8461 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8461_marker[] = "libcgj-batch8461";

/* Soft UUID version-4 layout catalog id (RFC 4122 version field). */
#define B8461_UUID_VER4_ID ((uint32_t)4u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8461_uuid_ver4_id(void)
{
	return B8461_UUID_VER4_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_uuid_ver4_id_8461 - soft UUID version-4 layout id tag.
 *
 * Always returns 4. Catalog id only; does not generate or parse UUIDs.
 * No parent wires.
 */
uint32_t
gj_uuid_ver4_id_8461(void)
{
	(void)NULL;
	return b8461_uuid_ver4_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_uuid_ver4_id_8461(void)
    __attribute__((alias("gj_uuid_ver4_id_8461")));
