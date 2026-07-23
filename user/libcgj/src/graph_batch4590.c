/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4590: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_4590(void);
 *     - Returns the compile-time graph batch number for this TU (4590).
 *   uint32_t __gj_batch_id_4590  (alias)
 *   __libcgj_batch4590_marker = "libcgj-batch4590"
 *
 * Exclusive continuum CREATE-ONLY (4581-4590: flags_set_u, flags_clear_u,
 * flags_toggle_u, flags_test_all_u, flags_test_any_u, flags_test_none_u,
 * flags_mask_u, flags_diff_u, flags_subset_u, batch_id_4590).
 * Does NOT redefine gj_batch_id / gj_graph_milestone / prior batch_id_*
 * symbols — avoid multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4590_marker[] = "libcgj-batch4590";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4590_id(void)
{
	return 4590u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_4590 - report this TU's graph batch number.
 *
 * Always returns 4590. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_4590(void)
{
	(void)NULL;
	return b4590_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_4590(void)
    __attribute__((alias("gj_batch_id_4590")));
