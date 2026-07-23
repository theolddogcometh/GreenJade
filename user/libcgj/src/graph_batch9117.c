/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9117: soft JSON depth-ok unit flag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_json_depth_ok_u_9117(void);
 *     - Returns 1 (json soft-parse depth continuum ok). Pure-data
 *       product tag; does not measure nesting or parse tokens.
 *   uint32_t __gj_json_depth_ok_u_9117  (alias)
 *   __libcgj_batch9117_marker = "libcgj-batch9117"
 *
 * Exclusive continuum CREATE-ONLY (9111-9120: json soft parse id stubs —
 * json_null_id_9111, json_bool_id_9112, json_number_id_9113,
 * json_string_id_9114, json_array_id_9115, json_object_id_9116,
 * json_depth_ok_u_9117, json_escape_ok_u_9118, json_ready_u_9119,
 * batch_id_9120).
 * Unique surface only; no multi-def. Distinct from
 * gj_json_object_id_9116 (batch9116). No parent wires. No __int128.
 * No full JSON parser implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9117_marker[] = "libcgj-batch9117";

#define B9117_JSON_DEPTH_OK  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9117_depth_ok(void)
{
	return B9117_JSON_DEPTH_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_json_depth_ok_u_9117 - JSON soft-parse depth continuum ok flag.
 *
 * Always returns 1. Soft pure-data product tag; does not measure
 * nesting depth or parse tokens. No parent wires.
 */
uint32_t
gj_json_depth_ok_u_9117(void)
{
	(void)NULL;
	return b9117_depth_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_json_depth_ok_u_9117(void)
    __attribute__((alias("gj_json_depth_ok_u_9117")));
