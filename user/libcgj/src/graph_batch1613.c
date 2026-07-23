/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1613: fair-queue vruntime accumulate.
 *
 * Surface (unique symbols):
 *   uint64_t gj_fair_queue_vruntime_add(uint64_t vruntime, uint64_t delta,
 *                                       uint32_t weight);
 *     — CFS-style virtual runtime step: vruntime + delta * SCALE / weight
 *       with SCALE = 1024 (nice-0 unit). weight == 0 treated as 1 so the
 *       addition never divides by zero. Saturates at UINT64_MAX.
 *   uint64_t __gj_fair_queue_vruntime_add  (alias)
 *   __libcgj_batch1613_marker = "libcgj-batch1613"
 *
 * Does NOT redefine prior batch symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1613_marker[] = "libcgj-batch1613";

/* Nice-0 weight unit used as the fixed-point scale for vruntime. */
#define B1613_NICE0_SCALE 1024u

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b1613_sat_add_u64(uint64_t u64A, uint64_t u64B)
{
	if (u64A > UINT64_MAX - u64B) {
		return UINT64_MAX;
	}
	return u64A + u64B;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fair_queue_vruntime_add — add an execution delta to virtual runtime.
 *
 * Heavier weight advances vruntime more slowly (fair share). Pure data:
 * no queues, no locks — just the scaled increment.
 */
uint64_t
gj_fair_queue_vruntime_add(uint64_t u64Vr, uint64_t u64Delta, uint32_t u32Weight)
{
	uint64_t u64Step;
	uint32_t u32W;

	u32W = (u32Weight == 0u) ? 1u : u32Weight;

	/* step = delta * SCALE / weight; do mul first in 64-bit. */
	if (u64Delta > UINT64_MAX / (uint64_t)B1613_NICE0_SCALE) {
		/* Oversized delta: clamp product path via divide-first. */
		u64Step = (u64Delta / (uint64_t)u32W) * (uint64_t)B1613_NICE0_SCALE;
		u64Step = b1613_sat_add_u64(u64Step,
		    ((u64Delta % (uint64_t)u32W) * (uint64_t)B1613_NICE0_SCALE) /
		    (uint64_t)u32W);
	} else {
		u64Step = (u64Delta * (uint64_t)B1613_NICE0_SCALE) / (uint64_t)u32W;
	}

	return b1613_sat_add_u64(u64Vr, u64Step);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_fair_queue_vruntime_add(uint64_t u64Vr, uint64_t u64Delta,
    uint32_t u32Weight)
    __attribute__((alias("gj_fair_queue_vruntime_add")));
