/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4810: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_4810(void);
 *     - Returns the compile-time graph batch number for this TU (4810).
 *   uint32_t __gj_batch_id_4810  (alias)
 *   __libcgj_batch4810_marker = "libcgj-batch4810"
 *
 * Exclusive continuum CREATE-ONLY (4801-4810: ev_fd_valid_u,
 * ev_timeout_ms_u, ev_interest_read_u, ev_interest_write_u,
 * ev_interest_err_u, ev_mask_has_u, ev_mask_or_u, ev_ready_count_u,
 * ev_id_next_u, batch_id_4810). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* symbols — avoid multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4810_marker[] = "libcgj-batch4810";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4810_id(void)
{
	return 4810u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_4810 - report this TU's graph batch number.
 *
 * Always returns 4810. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_4810(void)
{
	(void)NULL;
	return b4810_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_4810(void)
    __attribute__((alias("gj_batch_id_4810")));
