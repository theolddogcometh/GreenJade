/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3348: barrier phase/epoch word read (u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_barrier_phase_u(const uint32_t *phase);
 *     - Return *phase (barrier epoch/phase word). NULL → 0.
 *       Soft pure-data accessor for cooperative barrier harnesses.
 *   uint32_t __gj_barrier_phase_u  (alias)
 *   __libcgj_batch3348_marker = "libcgj-batch3348"
 *
 * Milestone 3350 exclusive continuum CREATE-ONLY (3341-3350). Distinct
 * from gj_barrier_generation (batch1314), gj_barrier_wait_count
 * (batch1315), and gj_barrier_hint_arrive (batch1559) — unique
 * gj_barrier_phase_u surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3348_marker[] = "libcgj-batch3348";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b3348_phase(const uint32_t *pPhase)
{
	if (pPhase == NULL) {
		return 0u;
	}
	return *pPhase;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_barrier_phase_u - read a barrier phase/epoch word.
 *
 * phase: pointer to caller-owned phase counter; NULL → 0.
 *
 * Callers bump phase when the last waiter arrives so late threads can
 * observe a new epoch. No parent wires.
 */
uint32_t
gj_barrier_phase_u(const uint32_t *pPhase)
{
	(void)NULL;
	return b3348_phase(pPhase);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_barrier_phase_u(const uint32_t *pPhase)
    __attribute__((alias("gj_barrier_phase_u")));
