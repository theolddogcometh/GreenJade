/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2546: non-atomic cooperative counter decrement.
 *
 * Surface (unique symbols):
 *   uint32_t gj_counter_dec_u(volatile uint32_t *p);
 *     - Fetch-dec style: read *p, write *p - 1 (mod 2^32), return the
 *       previous value. NULL -> 0 without store.
 *       Plain volatile RMW as separate load+store; NOT atomic XADD.
 *   uint32_t __gj_counter_dec_u  (alias)
 *   __libcgj_batch2546_marker = "libcgj-batch2546"
 *
 * NON-ATOMIC COOPERATIVE SEMANTICS: load and store are not indivisible.
 * Underflow wraps in uint32_t modular arithmetic by design (matches
 * unsigned C semantics; not a saturating counter). Cooperative /
 * single-thread or external sync only. Not C11 atomic_fetch_sub.
 *
 * Sync exclusive pure-volatile hints wave (2541-2550). Unique surface.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no atomics. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2546_marker[] = "libcgj-batch2546";

/* ---- freestanding helpers ---------------------------------------------- */

/* Modular u32 predecessor (wraps naturally in unsigned arithmetic). */
static uint32_t
b2546_pred(uint32_t u32V)
{
	return u32V - 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_counter_dec_u - cooperative fetch-dec on a volatile u32.
 *
 * p: pointer to volatile word; NULL -> 0.
 *
 * Returns the value observed before the decrement (0 if p is NULL).
 */
uint32_t
gj_counter_dec_u(volatile uint32_t *pWord)
{
	uint32_t u32Prev;

	(void)NULL;

	if (pWord == NULL) {
		return 0u;
	}
	u32Prev = *pWord;
	*pWord = b2546_pred(u32Prev);
	return u32Prev;
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_counter_dec_u(volatile uint32_t *pWord)
    __attribute__((alias("gj_counter_dec_u")));
