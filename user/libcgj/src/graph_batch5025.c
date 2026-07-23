/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5025: scheduler vruntime saturating difference.
 *
 * Surface (unique symbols):
 *   uint64_t gj_sched_vruntime_diff_u(uint64_t a, uint64_t b);
 *     - Return a - b when a >= b, else 0 (unsigned saturating subtract).
 *   uint64_t __gj_sched_vruntime_diff_u  (alias)
 *   __libcgj_batch5025_marker = "libcgj-batch5025"
 *
 * Exclusive continuum CREATE-ONLY (5021-5030). Unique
 * gj_sched_vruntime_diff_u surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5025_marker[] = "libcgj-batch5025";

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b5025_sat_diff(uint64_t u64A, uint64_t u64B)
{
	if (u64A < u64B) {
		return 0ull;
	}
	return u64A - u64B;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sched_vruntime_diff_u - saturating unsigned vruntime difference.
 *
 * a: left (minuend) virtual runtime
 * b: right (subtrahend) virtual runtime
 *
 * Returns a - b when a >= b, otherwise 0. Useful for pure "how far ahead"
 * probes without wraparound. No parent wires.
 */
uint64_t
gj_sched_vruntime_diff_u(uint64_t u64A, uint64_t u64B)
{
	(void)NULL;
	return b5025_sat_diff(u64A, u64B);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_sched_vruntime_diff_u(uint64_t u64A, uint64_t u64B)
    __attribute__((alias("gj_sched_vruntime_diff_u")));
