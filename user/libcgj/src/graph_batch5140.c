/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5140: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_5140(void);
 *     - Returns the compile-time graph batch number for this TU (5140).
 *   uint32_t __gj_batch_id_5140  (alias)
 *   __libcgj_batch5140_marker = "libcgj-batch5140"
 *
 * Exclusive continuum CREATE-ONLY (5131-5140: RCU/seqlock unique —
 * seq_begin_u, seq_retry_u, seq_end_u, seq_write_begin_u, seq_write_end_u,
 * rcu_quiescent_u, rcu_gp_next_u, rcu_gp_done_u, rcu_is_odd_u,
 * batch_id_5140). Does NOT redefine gj_batch_id / gj_graph_milestone /
 * prior batch_id_* symbols — avoid multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5140_marker[] = "libcgj-batch5140";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5140_id(void)
{
	return 5140u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_5140 - report this TU's graph batch number.
 *
 * Always returns 5140. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_5140(void)
{
	(void)NULL;
	return b5140_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_5140(void)
    __attribute__((alias("gj_batch_id_5140")));
