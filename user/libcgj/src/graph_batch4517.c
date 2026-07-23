/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4517: atomic-less plain counter store-max.
 *
 * Surface (unique symbols):
 *   uint32_t gj_counter_max_u(uint32_t *p, uint32_t v);
 *     - Store max(*p, v) into *p. NULL -> 0 without store. Returns the
 *       new value after the store-max.
 *   uint32_t __gj_counter_max_u  (alias)
 *   __libcgj_batch4517_marker = "libcgj-batch4517"
 *
 * Exclusive continuum CREATE-ONLY (4511-4520: atomic-less counters
 * unique). Unique gj_counter_max_u surface only; no multi-def. No
 * parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no atomics. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4517_marker[] = "libcgj-batch4517";

/* ---- freestanding helpers ---------------------------------------------- */

/* Unsigned maximum of two u32 words. */
static uint32_t
b4517_max(uint32_t u32A, uint32_t u32B)
{
	if (u32A > u32B) {
		return u32A;
	}
	return u32B;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_counter_max_u - atomic-less store-max on a plain u32 counter.
 *
 * p: pointer to counter word; NULL -> 0.
 * v: candidate value; *p becomes max(*p, v).
 *
 * Returns the value after the store-max (0 if p is NULL).
 * Self-contained; no parent wires.
 */
uint32_t
gj_counter_max_u(uint32_t *pWord, uint32_t u32V)
{
	uint32_t u32New;

	(void)NULL;

	if (pWord == NULL) {
		return 0u;
	}
	u32New = b4517_max(*pWord, u32V);
	*pWord = u32New;
	return u32New;
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_counter_max_u(uint32_t *pWord, uint32_t u32V)
    __attribute__((alias("gj_counter_max_u")));
