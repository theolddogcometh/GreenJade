/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2080: milestone 2080 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_2080(void);
 *     — Returns the compile-time graph batch number for this TU (2080).
 *   uint32_t gj_graph_milestone_2080(void);
 *     — Returns the current graph milestone revision (2080).
 *   uint32_t __gj_batch_id_2080  (alias)
 *   uint32_t __gj_graph_milestone_2080  (alias)
 *   __libcgj_batch2080_marker = "libcgj-batch2080"
 *
 * Milestone for the str encode exclusive wave (batches 2071–2079:
 * url_needs_encode_p, url_encode_len, json_escape_len, c_escape_len,
 * shell_escape_len, is_ident_ascii, is_number_ascii, is_hex_ascii,
 * is_base64_char). Does NOT redefine gj_batch_id / gj_graph_milestone /
 * prior batch_id_* or milestone_* symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2080_marker[] = "libcgj-batch2080";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2080_id(void)
{
	return 2080u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_2080 — report this TU's graph batch number.
 *
 * Always returns 2080.
 */
uint32_t
gj_batch_id_2080(void)
{
	(void)NULL;
	return b2080_id();
}

/*
 * gj_graph_milestone_2080 — report this TU's graph milestone revision.
 *
 * Always returns 2080 (this milestone TU). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link time.
 */
uint32_t
gj_graph_milestone_2080(void)
{
	return b2080_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_2080(void)
    __attribute__((alias("gj_batch_id_2080")));

uint32_t __gj_graph_milestone_2080(void)
    __attribute__((alias("gj_graph_milestone_2080")));
