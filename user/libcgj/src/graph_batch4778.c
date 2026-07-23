/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4778: job exponential backoff (base<<tries sat).
 *
 * Surface (unique symbols):
 *   uint32_t gj_job_backoff_u(uint32_t tries, uint32_t base);
 *     - Return base << tries with saturation at UINT32_MAX when the
 *       true product would overflow (or tries >= 32 and base != 0).
 *       base == 0 → 0. tries is 0-based (0 → base).
 *   uint32_t __gj_job_backoff_u  (alias)
 *   __libcgj_batch4778_marker = "libcgj-batch4778"
 *
 * Exclusive continuum CREATE-ONLY (4771-4780: job_priority_u,
 * job_id_next_u, job_state_is_done_u, job_state_is_run_u,
 * job_state_is_pend_u, job_timeout_u, job_retry_ok_u,
 * job_backoff_u, job_score_u, batch_id_4780).
 * Distinct from gj_retry_backoff_us (batch1195) / gj_u32_shl_sat
 * (batch2407, shifts to 0) — unique gj_job_backoff_u surface only;
 * no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4778_marker[] = "libcgj-batch4778";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * b4778_backoff — saturating base << tries.
 * Value saturates to UINT32_MAX on overflow (not to 0).
 */
static uint32_t
b4778_backoff(uint32_t u32Tries, uint32_t u32Base)
{
	unsigned uN;

	if (u32Base == 0u) {
		return 0u;
	}
	if (u32Tries >= 32u) {
		return UINT32_MAX;
	}
	uN = (unsigned)u32Tries;
	if (uN > 0u && u32Base > (UINT32_MAX >> uN)) {
		return UINT32_MAX;
	}
	return u32Base << uN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_job_backoff_u - exponential backoff delay: base << tries (sat).
 *
 * tries: 0-based attempt index (0 returns base)
 * base:  base delay unit (ticks/ms/us — caller-defined)
 *
 * Returns base shifted left by tries, or UINT32_MAX on overflow.
 * Self-contained; no parent wires.
 */
uint32_t
gj_job_backoff_u(uint32_t u32Tries, uint32_t u32Base)
{
	(void)NULL;
	return b4778_backoff(u32Tries, u32Base);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_job_backoff_u(uint32_t u32Tries, uint32_t u32Base)
    __attribute__((alias("gj_job_backoff_u")));
