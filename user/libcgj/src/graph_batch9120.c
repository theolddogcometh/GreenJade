/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9120: json soft parse id continuum + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_9120(void);
 *     - Returns the compile-time graph batch number for this TU (9120).
 *   uint32_t __gj_batch_id_9120  (alias)
 *   __libcgj_batch9120_marker = "libcgj-batch9120"
 *
 * Exclusive continuum CREATE-ONLY (9111-9120: json soft parse id stubs —
 * json_null_id_9111, json_bool_id_9112, json_number_id_9113,
 * json_string_id_9114, json_array_id_9115, json_object_id_9116,
 * json_depth_ok_u_9117, json_escape_ok_u_9118, json_ready_u_9119,
 * batch_id_9120).
 * Unique surfaces only; no multi-def. Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* symbols — avoid multi-def.
 * No parent wires. No __int128. No full JSON parser implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9120_marker[] = "libcgj-batch9120";

#define B9120_BATCH_ID  9120u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9120_id(void)
{
	return B9120_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_9120 - report this TU's graph batch number.
 *
 * Always returns 9120. Link-time presence tags the exclusive
 * json soft-parse id continuum wave end. No parent wires.
 */
uint32_t
gj_batch_id_9120(void)
{
	(void)NULL;
	return b9120_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_9120(void)
    __attribute__((alias("gj_batch_id_9120")));
