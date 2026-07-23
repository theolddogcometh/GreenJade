/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3790: milestone 3790 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_3790(void);
 *     - Returns the compile-time graph batch number for this TU (3790).
 *       Milestone for the exclusive continuum CREATE-ONLY series
 *       (batches 3781-3789: sa_lcp_u / suffix_cmp_u / manacher_expand_u /
 *       palindrome_odd_u / palindrome_even_u / rolling_hash_u /
 *       string_hash_pair_u / aho_fail_u / aho_output_u).
 *   uint32_t __gj_batch_id_3790  (alias)
 *   __libcgj_batch3790_marker = "libcgj-batch3790"
 *
 * Does NOT redefine gj_batch_id / gj_graph_milestone / prior batch_id_*
 * symbols — avoid multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3790_marker[] = "libcgj-batch3790";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b3790_id(void)
{
	return 3790u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_3790 — report this TU's graph batch number.
 *
 * Always returns 3790. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_3790(void)
{
	(void)NULL;
	return b3790_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_3790(void)
    __attribute__((alias("gj_batch_id_3790")));
