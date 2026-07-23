/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4771: clamp job priority to 0..255 (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_job_priority_u(uint32_t p);
 *     - Confine job priority p to the closed byte range [0, 255].
 *       Values above 255 clamp to 255; unsigned domain already
 *       saturates the lower bound at 0.
 *   uint32_t __gj_job_priority_u  (alias)
 *   __libcgj_batch4771_marker = "libcgj-batch4771"
 *
 * Exclusive continuum CREATE-ONLY (4771-4780: job_priority_u,
 * job_id_next_u, job_state_is_done_u, job_state_is_run_u,
 * job_state_is_pend_u, job_timeout_u, job_retry_ok_u,
 * job_backoff_u, job_score_u, batch_id_4780).
 * Distinct from gj_job_priority_clamp (batch1309, 0..31) — unique
 * gj_job_priority_u surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4771_marker[] = "libcgj-batch4771";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4771_priority(uint32_t u32P)
{
	if (u32P > 255u) {
		return 255u;
	}
	return u32P;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_job_priority_u - clamp job priority into [0, 255].
 *
 * p: raw priority class (caller-defined meaning; higher = more urgent)
 *
 * Returns p when p <= 255, else 255. Self-contained; no parent wires.
 */
uint32_t
gj_job_priority_u(uint32_t u32P)
{
	(void)NULL;
	return b4771_priority(u32P);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_job_priority_u(uint32_t u32P)
    __attribute__((alias("gj_job_priority_u")));
