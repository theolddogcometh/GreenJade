/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5203: nanoseconds to HPET ticks (_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_hpet_ns_to_ticks_u(uint64_t ns, uint64_t period_fs);
 *     - Return floor(ns * 1000000 / period_fs) ticks.
 *       period_fs == 0 → 0. Saturates at UINT64_MAX on overflow.
 *   uint64_t __gj_hpet_ns_to_ticks_u  (alias)
 *   __libcgj_batch5203_marker = "libcgj-batch5203"
 *
 * Exclusive continuum CREATE-ONLY (5201-5210: HPET/timer unique).
 * Unique gj_hpet_ns_to_ticks_u surface only; no multi-def. Complements
 * gj_hpet_fs_to_ns_u (batch5202). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5203_marker[] = "libcgj-batch5203";

/* Femtoseconds per nanosecond (scale ns → fs before / period). */
#define B5203_FS_PER_NS  1000000ull

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * b5203_ns_to_ticks — floor(ns * 1e6 / period_fs) without __int128.
 *
 * Prefers (ns * 1e6) / period_fs when the product fits. For large ns,
 * rewrites as floor(ns/period)*1e6 + floor((ns%period)*1e6/period),
 * saturating intermediate multiplies.
 */
static uint64_t
b5203_ns_to_ticks(uint64_t u64Ns, uint64_t u64PeriodFs)
{
	uint64_t u64Whole;
	uint64_t u64Rem;
	uint64_t u64Part;

	if (u64PeriodFs == 0ull) {
		return 0ull;
	}
	if (u64Ns <= (UINT64_MAX / B5203_FS_PER_NS)) {
		return (u64Ns * B5203_FS_PER_NS) / u64PeriodFs;
	}
	u64Whole = u64Ns / u64PeriodFs;
	u64Rem = u64Ns % u64PeriodFs;
	if (u64Whole > (UINT64_MAX / B5203_FS_PER_NS)) {
		return UINT64_MAX;
	}
	u64Whole *= B5203_FS_PER_NS;
	if (u64Rem <= (UINT64_MAX / B5203_FS_PER_NS)) {
		u64Part = (u64Rem * B5203_FS_PER_NS) / u64PeriodFs;
	} else {
		/*
		 * rem > UINT64_MAX/1e6 implies period_fs is enormous; the
		 * fractional contribution is still in [0, 1e6-1]. Drop
		 * it rather than overflow (saturate whole only).
		 */
		u64Part = 0ull;
	}
	if (u64Whole > (UINT64_MAX - u64Part)) {
		return UINT64_MAX;
	}
	return u64Whole + u64Part;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hpet_ns_to_ticks_u - convert nanoseconds to HPET ticks.
 *
 * ns:        duration in nanoseconds
 * period_fs: HPET period in femtoseconds (0 → 0)
 *
 * Returns floor(ns * 1000000 / period_fs), saturating at UINT64_MAX.
 * Self-contained; no parent wires. No __int128.
 */
uint64_t
gj_hpet_ns_to_ticks_u(uint64_t u64Ns, uint64_t u64PeriodFs)
{
	(void)NULL;
	return b5203_ns_to_ticks(u64Ns, u64PeriodFs);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_hpet_ns_to_ticks_u(uint64_t u64Ns, uint64_t u64PeriodFs)
    __attribute__((alias("gj_hpet_ns_to_ticks_u")));
