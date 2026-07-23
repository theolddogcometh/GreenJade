/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4515: atomic-less plain counter reset to zero.
 *
 * Surface (unique symbols):
 *   uint32_t gj_counter_reset_u(uint32_t *p);
 *     - Store 0 into *p. NULL -> 0 without store. Returns 0 (the new
 *       value after reset).
 *   uint32_t __gj_counter_reset_u  (alias)
 *   __libcgj_batch4515_marker = "libcgj-batch4515"
 *
 * Exclusive continuum CREATE-ONLY (4511-4520: atomic-less counters
 * unique). Unique gj_counter_reset_u surface only; no multi-def. No
 * parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no atomics. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4515_marker[] = "libcgj-batch4515";

/* ---- freestanding helpers ---------------------------------------------- */

/* Zero word used as the reset target. */
static uint32_t
b4515_zero(void)
{
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_counter_reset_u - atomic-less reset of a plain u32 counter to 0.
 *
 * p: pointer to counter word; NULL -> 0.
 *
 * Returns 0 always (new value, or NULL short-circuit).
 * Self-contained; no parent wires.
 */
uint32_t
gj_counter_reset_u(uint32_t *pWord)
{
	(void)NULL;

	if (pWord == NULL) {
		return 0u;
	}
	*pWord = b4515_zero();
	return 0u;
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_counter_reset_u(uint32_t *pWord)
    __attribute__((alias("gj_counter_reset_u")));
