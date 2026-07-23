/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1618: load average EMA update (milli alpha).
 *
 * Surface (unique symbols):
 *   uint32_t gj_load_avg_update_milli(uint32_t old_avg, uint32_t instant,
 *                                     uint32_t alpha_milli);
 *     — Exponential moving average in integer milli-units:
 *         new = old * (1000 - a) / 1000 + instant * a / 1000
 *       alpha_milli is clamped to [0, 1000]. Pure; no timers.
 *   uint32_t __gj_load_avg_update_milli  (alias)
 *   __libcgj_batch1618_marker = "libcgj-batch1618"
 *
 * Does NOT redefine prior batch symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1618_marker[] = "libcgj-batch1618";

#define B1618_MILLI 1000u

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_load_avg_update_milli — one EMA step for a load / runnable metric.
 *
 * alpha_milli = 0 keeps old_avg; alpha_milli = 1000 snaps to instant.
 * Intermediate values blend. Uses 64-bit intermediates for mul safety.
 */
uint32_t
gj_load_avg_update_milli(uint32_t u32Old, uint32_t u32Instant,
    uint32_t u32AlphaMilli)
{
	uint32_t u32A;
	uint32_t u32Keep;
	uint64_t u64Acc;

	u32A = u32AlphaMilli;
	if (u32A > B1618_MILLI) {
		u32A = B1618_MILLI;
	}
	u32Keep = B1618_MILLI - u32A;

	u64Acc = (uint64_t)u32Old * (uint64_t)u32Keep;
	u64Acc += (uint64_t)u32Instant * (uint64_t)u32A;
	return (uint32_t)(u64Acc / (uint64_t)B1618_MILLI);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_load_avg_update_milli(uint32_t u32Old, uint32_t u32Instant,
    uint32_t u32AlphaMilli)
    __attribute__((alias("gj_load_avg_update_milli")));
