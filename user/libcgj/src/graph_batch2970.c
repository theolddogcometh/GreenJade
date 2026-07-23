/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2970: milestone 2970 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_2970(void);
 *     - Returns the compile-time graph batch number for this TU (2970).
 *   uint32_t __gj_batch_id_2970  (alias)
 *   __libcgj_batch2970_marker = "libcgj-batch2970"
 *
 * Milestone for the post-2960 mem exclusive wave (batches 2961-2969:
 * mem_set_u, mem_cpy_u, mem_move_u, mem_cmp_u, mem_chr_u, mem_rchr_u,
 * mem_xor_n_u, mem_and_n_u, mem_or_n_u). Does NOT redefine gj_batch_id
 * / gj_graph_milestone / prior batch_id_* symbols - avoid multi-def.
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2970_marker[] = "libcgj-batch2970";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2970_id(void)
{
	return 2970u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_2970 - report this TU's graph batch number.
 *
 * Always returns 2970. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_2970(void)
{
	(void)NULL;
	return b2970_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_2970(void)
    __attribute__((alias("gj_batch_id_2970")));
