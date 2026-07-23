/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3050: MILESTONE 3050 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_3050(void);
 *     - Returns the compile-time graph batch number for this TU (3050).
 *   uint32_t gj_graph_milestone_3050(void);
 *     - Returns the current graph milestone revision (3050).
 *   uint32_t __gj_batch_id_3050  (alias)
 *   uint32_t __gj_graph_milestone_3050  (alias)
 *   __libcgj_batch3050_marker = "libcgj-batch3050"
 *
 * MILESTONE 3050 for the exclusive CREATE-ONLY wave
 * (batches 3041-3049: utf8_is_lead_u, utf8_is_cont_u, ascii_fold_eq_uc,
 * path_ext_len_u2, path_stem_len_u2, env_key_ok_u, env_val_ok_u,
 * shell_token_ok_u, product_hint_3050). Does NOT redefine gj_batch_id
 * / gj_graph_milestone / prior batch_id_* or milestone_* symbols -
 * avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3050_marker[] = "libcgj-batch3050";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b3050_id(void)
{
	return 3050u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_3050 - report this TU's graph batch number.
 *
 * Always returns 3050.
 */
uint32_t
gj_batch_id_3050(void)
{
	(void)NULL;
	return b3050_id();
}

/*
 * gj_graph_milestone_3050 - report this TU's graph milestone revision.
 *
 * Always returns 3050 (MILESTONE 3050). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. No parent wires.
 */
uint32_t
gj_graph_milestone_3050(void)
{
	return b3050_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_3050(void)
    __attribute__((alias("gj_batch_id_3050")));

uint32_t __gj_graph_milestone_3050(void)
    __attribute__((alias("gj_graph_milestone_3050")));
