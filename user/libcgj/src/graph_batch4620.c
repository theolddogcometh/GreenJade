/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4620: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_4620(void);
 *     - Returns the compile-time graph batch number for this TU (4620).
 *   uint32_t __gj_batch_id_4620  (alias)
 *   __libcgj_batch4620_marker = "libcgj-batch4620"
 *
 * Exclusive continuum CREATE-ONLY (4611-4620: list_next_u, list_prev_u,
 * list_is_end_u, list_count_u, idx_of_u, idx_valid_u, idx_clamp_u,
 * idx_wrap_u, idx_advance_u, batch_id_4620). Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* symbols — avoid
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4620_marker[] = "libcgj-batch4620";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4620_id(void)
{
	return 4620u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_4620 - report this TU's graph batch number.
 *
 * Always returns 4620. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_4620(void)
{
	(void)NULL;
	return b4620_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_4620(void)
    __attribute__((alias("gj_batch_id_4620")));
