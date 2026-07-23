/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1559: non-atomic counter-barrier arrive hint.
 *
 * Surface (unique symbols):
 *   int gj_barrier_hint_arrive(volatile uint32_t *count, uint32_t parties);
 *     — Cooperative arrive on a shared u32 counter barrier.
 *       Increments *count. When the new value reaches parties, resets
 *       *count to 0 and returns 1 (this arriver released the barrier).
 *       Otherwise stores the incremented value and returns 0.
 *       NULL count or parties==0 → 0.
 *   int __gj_barrier_hint_arrive  (alias)
 *   __libcgj_batch1559_marker = "libcgj-batch1559"
 *
 * Distinct from gj_barrier_generation / gj_barrier_wait_count
 * (batch1314/1315). Unique arrive surface only.
 *
 * NON-ATOMIC HINT: plain volatile RMW; racy under true concurrency.
 * For cooperative / single-thread harnesses, or under an external
 * serializer. No wait/park — only the counter side of a barrier.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no atomics. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1559_marker[] = "libcgj-batch1559";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_barrier_hint_arrive — increment shared counter; release at parties.
 *
 * count:   shared arrive counter (reset to 0 on release)
 * parties: number of arrives that complete one barrier epoch (>= 1)
 *
 * Returns 1 if this call completed the epoch (last arriver), else 0.
 */
int
gj_barrier_hint_arrive(volatile uint32_t *pCount, uint32_t u32Parties)
{
	uint32_t u32N;

	if (pCount == NULL || u32Parties == 0u) {
		return 0;
	}

	u32N = *pCount + 1u;
	if (u32N >= u32Parties) {
		*pCount = 0u;
		return 1;
	}
	*pCount = u32N;
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_barrier_hint_arrive(volatile uint32_t *pCount, uint32_t u32Parties)
    __attribute__((alias("gj_barrier_hint_arrive")));
