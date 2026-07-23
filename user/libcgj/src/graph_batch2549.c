/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2549: non-atomic cooperative u64 sequence next.
 *
 * Surface (unique symbols):
 *   uint64_t gj_seq_next_u64(volatile uint64_t *p);
 *     - Ticket-style: sample *p, store *p + 1 (mod 2^64), return the
 *       previous value as the issued sequence id. NULL -> 0 without store.
 *       Plain volatile RMW as separate load+store; NOT atomic XADD.
 *   uint64_t __gj_seq_next_u64  (alias)
 *   __libcgj_batch2549_marker = "libcgj-batch2549"
 *
 * NON-ATOMIC COOPERATIVE SEMANTICS: load and store are not indivisible.
 * Concurrent cooperative callers can issue duplicate tickets. No fence.
 * On 32-bit targets a 64-bit volatile RMW may tear - this TU is
 * freestanding x86_64 (-msse2) oriented. Cooperative / single-thread or
 * external sync only.
 *
 * Sync exclusive pure-volatile hints wave (2541-2550). Unique surface.
 * Distinct from gj_seq_next_u32 (batch2095 pure value step).
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no atomics. No __int128. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2549_marker[] = "libcgj-batch2549";

/* ---- freestanding helpers ---------------------------------------------- */

/* Modular u64 successor (wraps naturally in unsigned arithmetic). */
static uint64_t
b2549_succ(uint64_t u64V)
{
	return u64V + 1ull;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_seq_next_u64 - cooperative ticket issue on a volatile u64 sequence.
 *
 * p: pointer to volatile 64-bit sequence word; NULL -> 0.
 *
 * Returns the value observed before the increment (the issued ticket),
 * or 0 if p is NULL.
 */
uint64_t
gj_seq_next_u64(volatile uint64_t *pWord)
{
	uint64_t u64Prev;

	(void)NULL;

	if (pWord == NULL) {
		return 0ull;
	}
	u64Prev = *pWord;
	*pWord = b2549_succ(u64Prev);
	return u64Prev;
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_seq_next_u64(volatile uint64_t *pWord)
    __attribute__((alias("gj_seq_next_u64")));
