/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4514: atomic-less plain counter sat-subtract.
 *
 * Surface (unique symbols):
 *   uint32_t gj_counter_sub_u(uint32_t *p, uint32_t n);
 *     - *p = max(*p - n, 0) with unsigned sat (if *p < n then 0).
 *       NULL -> 0 without store. Returns the new value after the sub.
 *   uint32_t __gj_counter_sub_u  (alias)
 *   __libcgj_batch4514_marker = "libcgj-batch4514"
 *
 * Exclusive continuum CREATE-ONLY (4511-4520: atomic-less counters
 * unique). Unique gj_counter_sub_u surface only; no multi-def. No
 * parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no atomics. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4514_marker[] = "libcgj-batch4514";

/* ---- freestanding helpers ---------------------------------------------- */

/* Saturating u32 difference (floor at 0). */
static uint32_t
b4514_sub_sat(uint32_t u32A, uint32_t u32B)
{
	if (u32A < u32B) {
		return 0u;
	}
	return u32A - u32B;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_counter_sub_u - atomic-less saturating post-sub on a plain u32.
 *
 * p: pointer to counter word; NULL -> 0.
 * n: subtrahend applied to *p (saturates at 0).
 *
 * Returns the value after the sub (0 if p is NULL or *p < n).
 * Self-contained; no parent wires.
 */
uint32_t
gj_counter_sub_u(uint32_t *pWord, uint32_t u32N)
{
	uint32_t u32New;

	(void)NULL;

	if (pWord == NULL) {
		return 0u;
	}
	u32New = b4514_sub_sat(*pWord, u32N);
	*pWord = u32New;
	return u32New;
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_counter_sub_u(uint32_t *pWord, uint32_t u32N)
    __attribute__((alias("gj_counter_sub_u")));
