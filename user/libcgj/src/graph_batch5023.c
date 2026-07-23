/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5023: scheduler timeslice from weight.
 *
 * Surface (unique symbols):
 *   uint32_t gj_sched_timeslice_u(uint32_t weight, uint32_t base);
 *     - Scale a base quantum by weight relative to the nice-0 unit
 *       (1024): slice = base * weight / 1024. weight 0 is treated as 1.
 *       base 0 yields 0. Non-zero base yields at least 1.
 *   uint32_t __gj_sched_timeslice_u  (alias)
 *   __libcgj_batch5023_marker = "libcgj-batch5023"
 *
 * Exclusive continuum CREATE-ONLY (5021-5030). Distinct from
 * gj_timeslice_from_nice (batch1615) — unique gj_sched_timeslice_u
 * surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5023_marker[] = "libcgj-batch5023";

/* Nice-0 weight unit used as the timeslice scale denominator. */
#define B5023_NICE0_WEIGHT 1024u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5023_timeslice(uint32_t u32Weight, uint32_t u32Base)
{
	uint32_t u32W;
	uint32_t u32Out;

	if (u32Base == 0u) {
		return 0u;
	}

	u32W = (u32Weight == 0u) ? 1u : u32Weight;

	/* base * weight fits in 64-bit for any uint32 pair. */
	u32Out = (uint32_t)(((uint64_t)u32Base * (uint64_t)u32W) /
	    (uint64_t)B5023_NICE0_WEIGHT);
	if (u32Out == 0u) {
		return 1u;
	}
	return u32Out;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sched_timeslice_u - pure timeslice hint from weight and base.
 *
 * weight: relative share (0 treated as 1)
 * base:   caller-supplied quantum at nice-0 weight 1024
 *
 * Returns base * weight / 1024, floored, with min 1 when base > 0.
 * Self-contained; no parent wires.
 */
uint32_t
gj_sched_timeslice_u(uint32_t u32Weight, uint32_t u32Base)
{
	(void)NULL;
	return b5023_timeslice(u32Weight, u32Base);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_sched_timeslice_u(uint32_t u32Weight, uint32_t u32Base)
    __attribute__((alias("gj_sched_timeslice_u")));
