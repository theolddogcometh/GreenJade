/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5136: pure-value RCU quiescent note (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_rcu_quiescent_u(void);
 *     - Report a freestanding quiescent state. Always returns 1
 *       (single-thread / cooperative continuum: always quiescent).
 *   uint32_t __gj_rcu_quiescent_u  (alias)
 *   __libcgj_batch5136_marker = "libcgj-batch5136"
 *
 * Exclusive continuum CREATE-ONLY (5131-5140: RCU/seqlock unique —
 * seq_begin_u, seq_retry_u, seq_end_u, seq_write_begin_u, seq_write_end_u,
 * rcu_quiescent_u, rcu_gp_next_u, rcu_gp_done_u, rcu_is_odd_u,
 * batch_id_5140). Distinct from gj_rcu_stamp / gj_rcu_tick (batch547) —
 * unique gj_rcu_quiescent_u surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5136_marker[] = "libcgj-batch5136";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5136_quiescent(void)
{
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_rcu_quiescent_u - freestanding RCU quiescent-state note.
 *
 * Always returns 1. Models a cooperative / single-thread continuum
 * where the caller is always outside a read-side critical section.
 * No parent wires.
 */
uint32_t
gj_rcu_quiescent_u(void)
{
	(void)NULL;
	return b5136_quiescent();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_rcu_quiescent_u(void)
    __attribute__((alias("gj_rcu_quiescent_u")));
