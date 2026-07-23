/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4779: job scheduling score from prio + age.
 *
 * Surface (unique symbols):
 *   uint32_t gj_job_score_u(uint32_t prio, uint32_t age);
 *     - Pack a comparable score: priority in the high byte (clamped to
 *       0..255) and age in the low 24 bits (capped at 0x00FFFFFF).
 *       Higher score ⇒ more urgent (higher prio, then older age).
 *   uint32_t __gj_job_score_u  (alias)
 *   __libcgj_batch4779_marker = "libcgj-batch4779"
 *
 * Exclusive continuum CREATE-ONLY (4771-4780: job_priority_u,
 * job_id_next_u, job_state_is_done_u, job_state_is_run_u,
 * job_state_is_pend_u, job_timeout_u, job_retry_ok_u,
 * job_backoff_u, job_score_u, batch_id_4780).
 * Distinct from gj_product_score_u / milestone product_score_* tags —
 * unique gj_job_score_u surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4779_marker[] = "libcgj-batch4779";

/* Age field width: 24 bits under an 8-bit priority class. */
#define B4779_AGE_MASK  0x00FFFFFFu

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4779_score(uint32_t u32Prio, uint32_t u32Age)
{
	uint32_t u32P;
	uint32_t u32A;

	if (u32Prio > 255u) {
		u32P = 255u;
	} else {
		u32P = u32Prio;
	}
	if (u32Age > B4779_AGE_MASK) {
		u32A = B4779_AGE_MASK;
	} else {
		u32A = u32Age;
	}
	return (u32P << 24) | u32A;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_job_score_u - pack priority and age into a comparable score.
 *
 * prio: job priority class (clamped to 0..255)
 * age:  wait / aging ticks (capped to 24 bits)
 *
 * Returns (prio << 24) | age_lo24. Higher score is more urgent.
 * Self-contained; no parent wires.
 */
uint32_t
gj_job_score_u(uint32_t u32Prio, uint32_t u32Age)
{
	(void)NULL;
	return b4779_score(u32Prio, u32Age);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_job_score_u(uint32_t u32Prio, uint32_t u32Age)
    __attribute__((alias("gj_job_score_u")));
