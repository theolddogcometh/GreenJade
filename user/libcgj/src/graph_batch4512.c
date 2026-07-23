/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4512: atomic-less plain counter sat-decrement.
 *
 * Surface (unique symbols):
 *   uint32_t gj_counter_dec_u2(uint32_t *p);
 *     - If *p > 0, *p -= 1; else leave *p at 0 (saturate). NULL -> 0
 *       without store. Returns the new value after the operation.
 *   uint32_t __gj_counter_dec_u2  (alias)
 *   __libcgj_batch4512_marker = "libcgj-batch4512"
 *
 * Exclusive continuum CREATE-ONLY (4511-4520: atomic-less counters
 * unique). Distinct from gj_counter_dec_u (batch2546: volatile
 * fetch-dec with modular wrap returning previous) — rename on
 * collision; unique gj_counter_dec_u2 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no atomics. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4512_marker[] = "libcgj-batch4512";

/* ---- freestanding helpers ---------------------------------------------- */

/* Saturating u32 predecessor (floor at 0). */
static uint32_t
b4512_pred_sat(uint32_t u32V)
{
	if (u32V == 0u) {
		return 0u;
	}
	return u32V - 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_counter_dec_u2 - atomic-less saturating post-decrement on a plain u32.
 *
 * p: pointer to counter word; NULL -> 0.
 *
 * Returns the value after the decrement (0 if p is NULL or already 0).
 * Self-contained; no parent wires.
 */
uint32_t
gj_counter_dec_u2(uint32_t *pWord)
{
	uint32_t u32New;

	(void)NULL;

	if (pWord == NULL) {
		return 0u;
	}
	u32New = b4512_pred_sat(*pWord);
	*pWord = u32New;
	return u32New;
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_counter_dec_u2(uint32_t *pWord)
    __attribute__((alias("gj_counter_dec_u2")));
