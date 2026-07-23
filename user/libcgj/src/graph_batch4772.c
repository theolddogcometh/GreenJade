/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4772: next job id (saturating, _u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_job_id_next_u(uint64_t id);
 *     - Return the next job id after id (id + 1). Saturates at
 *       UINT64_MAX (no wrap to 0). Id 0 remains a valid start so the
 *       first issued id can be 1 when callers begin at 0.
 *   uint64_t __gj_job_id_next_u  (alias)
 *   __libcgj_batch4772_marker = "libcgj-batch4772"
 *
 * Exclusive continuum CREATE-ONLY (4771-4780: job_priority_u,
 * job_id_next_u, job_state_is_done_u, job_state_is_run_u,
 * job_state_is_pend_u, job_timeout_u, job_retry_ok_u,
 * job_backoff_u, job_score_u, batch_id_4780).
 * Distinct from gj_task_id_next (batch1312, pointer counter) — unique
 * gj_job_id_next_u surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4772_marker[] = "libcgj-batch4772";

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b4772_id_next(uint64_t u64Id)
{
	if (u64Id == UINT64_MAX) {
		return UINT64_MAX;
	}
	return u64Id + 1ull;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_job_id_next_u - next job id after id (no wrap).
 *
 * id: last issued / current job id
 *
 * Returns id + 1, or UINT64_MAX when id is already UINT64_MAX.
 * Self-contained; no parent wires.
 */
uint64_t
gj_job_id_next_u(uint64_t u64Id)
{
	(void)NULL;
	return b4772_id_next(u64Id);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_job_id_next_u(uint64_t u64Id)
    __attribute__((alias("gj_job_id_next_u")));
