/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5138: pure-value RCU grace-period done (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_rcu_gp_done_u(uint32_t start, uint32_t cur);
 *     - Return 1 if the grace-period counter has advanced past start
 *       (cur != start); else 0. Wrap-safe modular compare.
 *   uint32_t __gj_rcu_gp_done_u  (alias)
 *   __libcgj_batch5138_marker = "libcgj-batch5138"
 *
 * Exclusive continuum CREATE-ONLY (5131-5140: RCU/seqlock unique —
 * seq_begin_u, seq_retry_u, seq_end_u, seq_write_begin_u, seq_write_end_u,
 * rcu_quiescent_u, rcu_gp_next_u, rcu_gp_done_u, rcu_is_odd_u,
 * batch_id_5140). Distinct from gj_rcu_stamp (batch547) — unique
 * gj_rcu_gp_done_u surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5138_marker[] = "libcgj-batch5138";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Grace period observed as done once cur has left the start stamp.
 * (cur - start) != 0 is identical to cur != start for uint32_t.
 */
static uint32_t
b5138_gp_done(uint32_t u32Start, uint32_t u32Cur)
{
	if (u32Cur != u32Start) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_rcu_gp_done_u - 1 if grace-period counter advanced past start.
 *
 * start: GP stamp recorded before waiting
 * cur:   current GP counter
 *
 * Returns 1 when cur != start, else 0. Pair with gj_rcu_gp_next_u
 * (batch5137). No parent wires.
 */
uint32_t
gj_rcu_gp_done_u(uint32_t u32Start, uint32_t u32Cur)
{
	(void)NULL;
	return b5138_gp_done(u32Start, u32Cur);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_rcu_gp_done_u(uint32_t u32Start, uint32_t u32Cur)
    __attribute__((alias("gj_rcu_gp_done_u")));
