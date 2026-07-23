/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4775: job state is-pending predicate (_u).
 *
 * Surface (unique symbols):
 *   int gj_job_state_is_pend_u(uint32_t s);
 *     - Return 1 if s == 0 (pending), else 0. Job state encoding:
 *       0 = pending, 1 = running, 2 = done.
 *   int __gj_job_state_is_pend_u  (alias)
 *   __libcgj_batch4775_marker = "libcgj-batch4775"
 *
 * Exclusive continuum CREATE-ONLY (4771-4780: job_priority_u,
 * job_id_next_u, job_state_is_done_u, job_state_is_run_u,
 * job_state_is_pend_u, job_timeout_u, job_retry_ok_u,
 * job_backoff_u, job_score_u, batch_id_4780).
 * Unique gj_job_state_is_pend_u surface only; no multi-def. Pairs with
 * gj_job_state_is_run_u (batch4774) and gj_job_state_is_done_u
 * (batch4773). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4775_marker[] = "libcgj-batch4775";

/* Job state class: pending. */
#define B4775_STATE_PEND  0u

/* ---- freestanding helpers ---------------------------------------------- */

static int
b4775_is_pend(uint32_t u32S)
{
	return (u32S == B4775_STATE_PEND) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_job_state_is_pend_u - 1 iff job state s is pending (s == 0).
 *
 * s: job state code (0 pend / 1 run / 2 done)
 *
 * Returns 1 when s == 0, else 0. Self-contained; no parent wires.
 */
int
gj_job_state_is_pend_u(uint32_t u32S)
{
	(void)NULL;
	return b4775_is_pend(u32S);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_job_state_is_pend_u(uint32_t u32S)
    __attribute__((alias("gj_job_state_is_pend_u")));
