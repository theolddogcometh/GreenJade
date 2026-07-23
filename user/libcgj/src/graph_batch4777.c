/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4777: job retry-allowed predicate (_u).
 *
 * Surface (unique symbols):
 *   int gj_job_retry_ok_u(uint32_t tries, uint32_t max);
 *     - Return 1 if tries < max (another retry is allowed), else 0.
 *       max == 0 means no retries remain for any tries value.
 *   int __gj_job_retry_ok_u  (alias)
 *   __libcgj_batch4777_marker = "libcgj-batch4777"
 *
 * Exclusive continuum CREATE-ONLY (4771-4780: job_priority_u,
 * job_id_next_u, job_state_is_done_u, job_state_is_run_u,
 * job_state_is_pend_u, job_timeout_u, job_retry_ok_u,
 * job_backoff_u, job_score_u, batch_id_4780).
 * Unique gj_job_retry_ok_u surface only; no multi-def. Pairs with
 * gj_job_backoff_u (batch4778). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4777_marker[] = "libcgj-batch4777";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b4777_retry_ok(uint32_t u32Tries, uint32_t u32Max)
{
	return (u32Tries < u32Max) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_job_retry_ok_u - 1 if another retry is still allowed.
 *
 * tries: number of attempts already consumed
 * max:   maximum attempts permitted
 *
 * Returns 1 when tries < max, else 0. Self-contained; no parent wires.
 */
int
gj_job_retry_ok_u(uint32_t u32Tries, uint32_t u32Max)
{
	(void)NULL;
	return b4777_retry_ok(u32Tries, u32Max);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_job_retry_ok_u(uint32_t u32Tries, uint32_t u32Max)
    __attribute__((alias("gj_job_retry_ok_u")));
