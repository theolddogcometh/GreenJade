/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4516: atomic-less plain counter load/get.
 *
 * Surface (unique symbols):
 *   uint32_t gj_counter_get_u(const uint32_t *p);
 *     - Return *p. NULL -> 0 without load fault. Read-only view of a
 *       plain counter word.
 *   uint32_t __gj_counter_get_u  (alias)
 *   __libcgj_batch4516_marker = "libcgj-batch4516"
 *
 * Exclusive continuum CREATE-ONLY (4511-4520: atomic-less counters
 * unique). Unique gj_counter_get_u surface only; no multi-def. No
 * parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no atomics. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4516_marker[] = "libcgj-batch4516";

/* ---- freestanding helpers ---------------------------------------------- */

/* Identity load of a u32 word. */
static uint32_t
b4516_load(uint32_t u32V)
{
	return u32V;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_counter_get_u - atomic-less read of a plain u32 counter.
 *
 * p: pointer to counter word; NULL -> 0.
 *
 * Returns the observed value (0 if p is NULL).
 * Self-contained; no parent wires.
 */
uint32_t
gj_counter_get_u(const uint32_t *pWord)
{
	(void)NULL;

	if (pWord == NULL) {
		return 0u;
	}
	return b4516_load(*pWord);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_counter_get_u(const uint32_t *pWord)
    __attribute__((alias("gj_counter_get_u")));
