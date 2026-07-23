/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5137: pure-value RCU grace-period next (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_rcu_gp_next_u(uint32_t gp);
 *     - Advance a grace-period counter by one. Returns gp + 1.
 *   uint32_t __gj_rcu_gp_next_u  (alias)
 *   __libcgj_batch5137_marker = "libcgj-batch5137"
 *
 * Exclusive continuum CREATE-ONLY (5131-5140: RCU/seqlock unique —
 * seq_begin_u, seq_retry_u, seq_end_u, seq_write_begin_u, seq_write_end_u,
 * rcu_quiescent_u, rcu_gp_next_u, rcu_gp_done_u, rcu_is_odd_u,
 * batch_id_5140). Distinct from gj_rcu_tick (batch547) — unique
 * gj_rcu_gp_next_u surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5137_marker[] = "libcgj-batch5137";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5137_gp_next(uint32_t u32Gp)
{
	return u32Gp + 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_rcu_gp_next_u - advance pure-value RCU grace-period counter.
 *
 * gp: current grace-period number
 *
 * Returns gp + 1. Pair with gj_rcu_gp_done_u (batch5138). Wraps
 * naturally in uint32_t modular arithmetic. No parent wires.
 */
uint32_t
gj_rcu_gp_next_u(uint32_t u32Gp)
{
	(void)NULL;
	return b5137_gp_next(u32Gp);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_rcu_gp_next_u(uint32_t u32Gp)
    __attribute__((alias("gj_rcu_gp_next_u")));
