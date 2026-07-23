/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3346: non-atomic refcount decrement (u-suffix).
 *
 * Surface (unique symbols):
 *   uint32_t gj_refcount_dec_u(uint32_t *rc);
 *     - Decrement *rc by one when *rc > 0. Returns the remaining count
 *       after the decrement (0 means the last reference was dropped).
 *       NULL or already-zero counter returns 0 without underflow.
 *       Soft pure-data refcount helper; not atomic.
 *   uint32_t __gj_refcount_dec_u  (alias)
 *   __libcgj_batch3346_marker = "libcgj-batch3346"
 *
 * Milestone 3350 exclusive continuum CREATE-ONLY (3341-3350). Distinct
 * from gj_ref_dec (batch326) and gj_refcount_inc_u (batch3345) — unique
 * gj_refcount_dec_u surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no atomics. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3346_marker[] = "libcgj-batch3346";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b3346_dec(uint32_t *pRc)
{
	uint32_t uCur;

	if (pRc == NULL) {
		return 0u;
	}
	uCur = *pRc;
	if (uCur == 0u) {
		return 0u;
	}
	uCur--;
	*pRc = uCur;
	return uCur;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_refcount_dec_u - drop one reference; return remaining count.
 *
 * rc: pointer to caller-owned counter; NULL → 0.
 *
 * Returns *rc after a successful decrement. A return of 0 after a prior
 * non-zero count means the last reference was released. No underflow.
 * Single-thread / external-sync only. No parent wires.
 */
uint32_t
gj_refcount_dec_u(uint32_t *pRc)
{
	(void)NULL;
	return b3346_dec(pRc);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_refcount_dec_u(uint32_t *pRc)
    __attribute__((alias("gj_refcount_dec_u")));
