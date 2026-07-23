/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4513: atomic-less plain counter post-add.
 *
 * Surface (unique symbols):
 *   uint32_t gj_counter_add_u2(uint32_t *p, uint32_t n);
 *     - *p += n (mod 2^32). NULL -> 0 without store. Returns the new
 *       value after the add (post-update).
 *   uint32_t __gj_counter_add_u2  (alias)
 *   __libcgj_batch4513_marker = "libcgj-batch4513"
 *
 * Exclusive continuum CREATE-ONLY (4511-4520: atomic-less counters
 * unique). Distinct from gj_counter_add_u (batch2547: volatile
 * fetch-add returning previous) — rename on collision; unique
 * gj_counter_add_u2 surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no atomics. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4513_marker[] = "libcgj-batch4513";

/* ---- freestanding helpers ---------------------------------------------- */

/* Modular u32 sum (wraps naturally in unsigned arithmetic). */
static uint32_t
b4513_sum(uint32_t u32A, uint32_t u32B)
{
	return u32A + u32B;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_counter_add_u2 - atomic-less post-add on a plain u32 counter.
 *
 * p: pointer to counter word; NULL -> 0.
 * n: addend applied to *p.
 *
 * Returns the value after the add (0 if p is NULL).
 * Self-contained; no parent wires.
 */
uint32_t
gj_counter_add_u2(uint32_t *pWord, uint32_t u32N)
{
	uint32_t u32New;

	(void)NULL;

	if (pWord == NULL) {
		return 0u;
	}
	u32New = b4513_sum(*pWord, u32N);
	*pWord = u32New;
	return u32New;
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_counter_add_u2(uint32_t *pWord, uint32_t u32N)
    __attribute__((alias("gj_counter_add_u2")));
