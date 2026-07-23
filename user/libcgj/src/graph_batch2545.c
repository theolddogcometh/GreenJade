/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2545: non-atomic cooperative counter increment.
 *
 * Surface (unique symbols):
 *   uint32_t gj_counter_inc_u(volatile uint32_t *p);
 *     - Fetch-inc style: read *p, write *p + 1 (mod 2^32), return the
 *       previous value. NULL -> 0 without store.
 *       Plain volatile RMW as separate load+store; NOT atomic XADD.
 *   uint32_t __gj_counter_inc_u  (alias)
 *   __libcgj_batch2545_marker = "libcgj-batch2545"
 *
 * NON-ATOMIC COOPERATIVE SEMANTICS: load and store are not indivisible.
 * Concurrent cooperative callers can lose updates. No fence. For
 * single-thread counters or externally serialized RMW only. Not C11
 * atomic_fetch_add.
 *
 * Sync exclusive pure-volatile hints wave (2541-2550). Unique surface.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no atomics. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2545_marker[] = "libcgj-batch2545";

/* ---- freestanding helpers ---------------------------------------------- */

/* Modular u32 successor (wraps naturally in unsigned arithmetic). */
static uint32_t
b2545_succ(uint32_t u32V)
{
	return u32V + 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_counter_inc_u - cooperative fetch-inc on a volatile u32.
 *
 * p: pointer to volatile word; NULL -> 0.
 *
 * Returns the value observed before the increment (0 if p is NULL).
 */
uint32_t
gj_counter_inc_u(volatile uint32_t *pWord)
{
	uint32_t u32Prev;

	(void)NULL;

	if (pWord == NULL) {
		return 0u;
	}
	u32Prev = *pWord;
	*pWord = b2545_succ(u32Prev);
	return u32Prev;
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_counter_inc_u(volatile uint32_t *pWord)
    __attribute__((alias("gj_counter_inc_u")));
