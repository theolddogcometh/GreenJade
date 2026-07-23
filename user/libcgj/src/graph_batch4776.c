/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4776: job timeout predicate (_u).
 *
 * Surface (unique symbols):
 *   int gj_job_timeout_u(uint64_t start, uint64_t now, uint64_t limit);
 *     - Return 1 if modular elapsed (now - start) is >= limit, else 0.
 *       Free-running u64 tick domain; unsigned wrap is correct.
 *       Callers wanting "no timeout" pass UINT64_MAX as limit.
 *   int __gj_job_timeout_u  (alias)
 *   __libcgj_batch4776_marker = "libcgj-batch4776"
 *
 * Exclusive continuum CREATE-ONLY (4771-4780: job_priority_u,
 * job_id_next_u, job_state_is_done_u, job_state_is_run_u,
 * job_state_is_pend_u, job_timeout_u, job_retry_ok_u,
 * job_backoff_u, job_score_u, batch_id_4780).
 * Distinct from gj_job_deadline_miss_p (batch1616) / gj_tick_expired_u
 * (batch4604) — unique gj_job_timeout_u surface only; no multi-def.
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4776_marker[] = "libcgj-batch4776";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * b4776_timeout — 1 when elapsed ticks meet/exceed the budget.
 * Elapsed uses modular (now - start) for free-running counters.
 */
static int
b4776_timeout(uint64_t u64Start, uint64_t u64Now, uint64_t u64Limit)
{
	uint64_t u64Elapsed;

	u64Elapsed = u64Now - u64Start;
	return (u64Elapsed >= u64Limit) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_job_timeout_u - 1 if job elapsed time has reached its limit.
 *
 * start: tick sample when the job began (or last reset)
 * now:   current free-running tick sample
 * limit: max allowed elapsed ticks (UINT64_MAX ≈ never times out)
 *
 * Returns 1 when (now - start) >= limit (mod 2^64), else 0.
 * Self-contained; no parent wires.
 */
int
gj_job_timeout_u(uint64_t u64Start, uint64_t u64Now, uint64_t u64Limit)
{
	(void)NULL;
	return b4776_timeout(u64Start, u64Now, u64Limit);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_job_timeout_u(uint64_t u64Start, uint64_t u64Now, uint64_t u64Limit)
    __attribute__((alias("gj_job_timeout_u")));
