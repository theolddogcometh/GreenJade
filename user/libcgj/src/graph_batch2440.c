/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2440: milestone 2440 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_2440(void);
 *     — Returns the compile-time graph batch number for this TU (2440).
 *       Milestone for the ring advanced exclusive u32 series
 *       (batches 2431–2439: init / push_u / pop_u / peek_u / count_u /
 *       space_u / full_u_p / empty_u_p / clear_u).
 *   uint32_t __gj_batch_id_2440  (alias)
 *   __libcgj_batch2440_marker = "libcgj-batch2440"
 *
 * Does NOT redefine gj_batch_id / gj_graph_milestone / prior batch_id_*
 * symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2440_marker[] = "libcgj-batch2440";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2440_id(void)
{
	return 2440u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_2440 — report this TU's graph batch number.
 *
 * Always returns 2440. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_2440(void)
{
	(void)NULL;
	return b2440_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_2440(void)
    __attribute__((alias("gj_batch_id_2440")));
