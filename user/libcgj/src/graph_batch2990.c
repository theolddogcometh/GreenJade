/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2990: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_2990(void);
 *     - Returns the compile-time graph batch number for this TU (2990).
 *   uint32_t __gj_batch_id_2990  (alias)
 *   __libcgj_batch2990_marker = "libcgj-batch2990"
 *
 * Exclusive continuum CREATE-ONLY (2981-2990): pe_is_pe_u,
 * coff_is_coff_u, macho_is_mh_u, json_skip_ws_u, json_is_null_lit_u,
 * json_is_true_lit_u, json_is_false_lit_u, toml_is_comment_u,
 * ini_is_section_u, batch_id_2990. Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* symbols — avoid multi-def.
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2990_marker[] = "libcgj-batch2990";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2990_id(void)
{
	return 2990u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_2990 - report this TU's graph batch number.
 *
 * Always returns 2990.
 */
uint32_t
gj_batch_id_2990(void)
{
	(void)NULL;
	return b2990_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_2990(void)
    __attribute__((alias("gj_batch_id_2990")));
