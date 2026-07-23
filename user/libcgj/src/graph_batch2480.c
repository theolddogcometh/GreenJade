/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2480: milestone 2480 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_2480(void);
 *     — Returns the compile-time graph batch number for this TU (2480).
 *   uint32_t gj_graph_milestone_2480(void);
 *     — Returns the current graph milestone revision (2480).
 *   uint32_t __gj_batch_id_2480  (alias)
 *   uint32_t __gj_graph_milestone_2480  (alias)
 *   __libcgj_batch2480_marker = "libcgj-batch2480"
 *
 * Milestone for the path exclusive wave (batches 2471–2479:
 * path_is_sep_u, path_skip_dots_u, path_last_sep_u, path_basename_u,
 * path_dirname_copy_u, path_join3_len_u, path_has_drive_u, path_is_unc_u,
 * path_normalize_sep_u). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols —
 * avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2480_marker[] = "libcgj-batch2480";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2480_id(void)
{
	return 2480u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_2480 — report this TU's graph batch number.
 *
 * Always returns 2480.
 */
uint32_t
gj_batch_id_2480(void)
{
	(void)NULL;
	return b2480_id();
}

/*
 * gj_graph_milestone_2480 — report this TU's graph milestone revision.
 *
 * Always returns 2480 (this milestone TU). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link time.
 */
uint32_t
gj_graph_milestone_2480(void)
{
	return b2480_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_2480(void)
    __attribute__((alias("gj_batch_id_2480")));

uint32_t __gj_graph_milestone_2480(void)
    __attribute__((alias("gj_graph_milestone_2480")));
