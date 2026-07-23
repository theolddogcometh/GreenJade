/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9111: soft JSON null type id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_json_null_id_9111(void);
 *     - Returns 0 (soft JSON null type catalog id). Pure-data type id
 *       stub; does not parse JSON or allocate tokens.
 *   uint32_t __gj_json_null_id_9111  (alias)
 *   __libcgj_batch9111_marker = "libcgj-batch9111"
 *
 * Exclusive continuum CREATE-ONLY (9111-9120: json soft parse id stubs —
 * json_null_id_9111, json_bool_id_9112, json_number_id_9113,
 * json_string_id_9114, json_array_id_9115, json_object_id_9116,
 * json_depth_ok_u_9117, json_escape_ok_u_9118, json_ready_u_9119,
 * batch_id_9120).
 * Unique surface only; no multi-def. No parent wires. No __int128.
 * No full JSON parser implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9111_marker[] = "libcgj-batch9111";

/* Soft JSON null type catalog id. */
#define B9111_JSON_NULL_ID  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9111_null_id(void)
{
	return B9111_JSON_NULL_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_json_null_id_9111 - soft JSON null type catalog id.
 *
 * Always returns 0. Soft continuum constant; does not parse JSON or
 * allocate tokens. No parent wires.
 */
uint32_t
gj_json_null_id_9111(void)
{
	(void)NULL;
	return b9111_null_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_json_null_id_9111(void)
    __attribute__((alias("gj_json_null_id_9111")));
