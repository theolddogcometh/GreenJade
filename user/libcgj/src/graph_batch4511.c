/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4511: atomic-less plain counter post-increment.
 *
 * Surface (unique symbols):
 *   uint32_t gj_counter_inc_u2(uint32_t *p);
 *     - *p += 1 (mod 2^32). NULL -> 0 without store. Returns the new
 *       value after the increment (post-update).
 *   uint32_t __gj_counter_inc_u2  (alias)
 *   __libcgj_batch4511_marker = "libcgj-batch4511"
 *
 * Exclusive continuum CREATE-ONLY (4511-4520: atomic-less counters
 * unique — counter_inc_u2, counter_dec_u2, counter_add_u2,
 * counter_sub_u, counter_reset_u, counter_get_u, counter_max_u,
 * counter_min_u, counter_cas_u2, batch_id_4520). Distinct from
 * gj_counter_inc_u (batch2545: volatile fetch-inc returning previous)
 * — rename on collision; unique gj_counter_inc_u2 surface only; no
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no atomics. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4511_marker[] = "libcgj-batch4511";

/* ---- freestanding helpers ---------------------------------------------- */

/* Modular u32 successor (wraps naturally in unsigned arithmetic). */
static uint32_t
b4511_succ(uint32_t u32V)
{
	return u32V + 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_counter_inc_u2 - atomic-less post-increment on a plain u32 counter.
 *
 * p: pointer to counter word; NULL -> 0.
 *
 * Returns the value after the increment (0 if p is NULL).
 * Self-contained; no parent wires.
 */
uint32_t
gj_counter_inc_u2(uint32_t *pWord)
{
	uint32_t u32New;

	(void)NULL;

	if (pWord == NULL) {
		return 0u;
	}
	u32New = b4511_succ(*pWord);
	*pWord = u32New;
	return u32New;
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_counter_inc_u2(uint32_t *pWord)
    __attribute__((alias("gj_counter_inc_u2")));
