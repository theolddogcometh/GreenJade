/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4774: job state is-running predicate (_u).
 *
 * Surface (unique symbols):
 *   int gj_job_state_is_run_u(uint32_t s);
 *     - Return 1 if s == 1 (running), else 0. Job state encoding:
 *       0 = pending, 1 = running, 2 = done.
 *   int __gj_job_state_is_run_u  (alias)
 *   __libcgj_batch4774_marker = "libcgj-batch4774"
 *
 * Exclusive continuum CREATE-ONLY (4771-4780: job_priority_u,
 * job_id_next_u, job_state_is_done_u, job_state_is_run_u,
 * job_state_is_pend_u, job_timeout_u, job_retry_ok_u,
 * job_backoff_u, job_score_u, batch_id_4780).
 * Unique gj_job_state_is_run_u surface only; no multi-def. Pairs with
 * gj_job_state_is_done_u (batch4773) and gj_job_state_is_pend_u
 * (batch4775). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4774_marker[] = "libcgj-batch4774";

/* Job state class: running. */
#define B4774_STATE_RUN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static int
b4774_is_run(uint32_t u32S)
{
	return (u32S == B4774_STATE_RUN) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_job_state_is_run_u - 1 iff job state s is running (s == 1).
 *
 * s: job state code (0 pend / 1 run / 2 done)
 *
 * Returns 1 when s == 1, else 0. Self-contained; no parent wires.
 */
int
gj_job_state_is_run_u(uint32_t u32S)
{
	(void)NULL;
	return b4774_is_run(u32S);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_job_state_is_run_u(uint32_t u32S)
    __attribute__((alias("gj_job_state_is_run_u")));
