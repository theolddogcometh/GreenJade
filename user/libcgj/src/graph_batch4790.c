/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4790: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_4790(void);
 *     - Returns the compile-time graph batch number for this TU (4790).
 *   uint32_t __gj_batch_id_4790  (alias)
 *   __libcgj_batch4790_marker = "libcgj-batch4790"
 *
 * Exclusive continuum CREATE-ONLY (4781-4790: cfg_is_true_u,
 * cfg_is_false_u, cfg_parse_u32_u, cfg_parse_u64_u, cfg_skip_ws_u,
 * cfg_is_key_char_u, cfg_is_sep_u, cfg_hash_key_u,
 * cfg_line_is_comment_u, batch_id_4790). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* symbols — avoid multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4790_marker[] = "libcgj-batch4790";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4790_id(void)
{
	return 4790u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_4790 - report this TU's graph batch number.
 *
 * Always returns 4790. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_4790(void)
{
	(void)NULL;
	return b4790_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_4790(void)
    __attribute__((alias("gj_batch_id_4790")));
