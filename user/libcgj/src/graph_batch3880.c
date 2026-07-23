/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3880: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_3880(void);
 *     - Returns the compile-time graph batch number for this TU (3880).
 *   uint32_t __gj_batch_id_3880  (alias)
 *   __libcgj_batch3880_marker = "libcgj-batch3880"
 *
 * Exclusive continuum CREATE-ONLY (3871-3880: u64_to_i64_sat_u,
 * str_is_ident_start_u, str_is_ident_cont_u, str_is_ident_u,
 * str_is_keyword_u, str_is_number_u, str_is_hex_u, str_is_ws_only_u,
 * str_trim_both_u, batch_id_3880). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* symbols — avoid multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3880_marker[] = "libcgj-batch3880";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b3880_id(void)
{
	return 3880u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_3880 - report this TU's graph batch number.
 *
 * Always returns 3880. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_3880(void)
{
	(void)NULL;
	return b3880_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_3880(void)
    __attribute__((alias("gj_batch_id_3880")));
