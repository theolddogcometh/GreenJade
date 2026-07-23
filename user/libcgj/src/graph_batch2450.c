/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2450: milestone 2450 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_2450(void);
 *     — Returns the compile-time graph batch number for this TU (2450).
 *   uint32_t __gj_batch_id_2450  (alias)
 *   __libcgj_batch2450_marker = "libcgj-batch2450"
 *
 * Milestone for the exclusive bitmap helpers wave (batches 2441-2449:
 * set_bit_u, clear_bit_u, test_bit_u, toggle_bit_u, count_set_u,
 * find_first_set_u, find_first_zero_u, or_into_u, and_into_u).
 * Does NOT redefine gj_batch_id / gj_graph_milestone / prior batch_id_*
 * symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No __int128. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2450_marker[] = "libcgj-batch2450";

/* Exclusive-wave milestone id. */
#define B2450_ID  2450u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2450_id(void)
{
	return B2450_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_2450 — report this TU's graph batch number.
 *
 * Always returns 2450. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_2450(void)
{
	(void)NULL;
	return b2450_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_2450(void)
    __attribute__((alias("gj_batch_id_2450")));
