/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4518: atomic-less plain counter store-min.
 *
 * Surface (unique symbols):
 *   uint32_t gj_counter_min_u(uint32_t *p, uint32_t v);
 *     - Store min(*p, v) into *p. NULL -> 0 without store. Returns the
 *       new value after the store-min.
 *   uint32_t __gj_counter_min_u  (alias)
 *   __libcgj_batch4518_marker = "libcgj-batch4518"
 *
 * Exclusive continuum CREATE-ONLY (4511-4520: atomic-less counters
 * unique). Unique gj_counter_min_u surface only; no multi-def. No
 * parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no atomics. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4518_marker[] = "libcgj-batch4518";

/* ---- freestanding helpers ---------------------------------------------- */

/* Unsigned minimum of two u32 words. */
static uint32_t
b4518_min(uint32_t u32A, uint32_t u32B)
{
	if (u32A < u32B) {
		return u32A;
	}
	return u32B;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_counter_min_u - atomic-less store-min on a plain u32 counter.
 *
 * p: pointer to counter word; NULL -> 0.
 * v: candidate value; *p becomes min(*p, v).
 *
 * Returns the value after the store-min (0 if p is NULL).
 * Self-contained; no parent wires.
 */
uint32_t
gj_counter_min_u(uint32_t *pWord, uint32_t u32V)
{
	uint32_t u32New;

	(void)NULL;

	if (pWord == NULL) {
		return 0u;
	}
	u32New = b4518_min(*pWord, u32V);
	*pWord = u32New;
	return u32New;
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_counter_min_u(uint32_t *pWord, uint32_t u32V)
    __attribute__((alias("gj_counter_min_u")));
