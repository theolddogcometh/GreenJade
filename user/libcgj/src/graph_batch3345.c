/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3345: non-atomic refcount increment (u-suffix).
 *
 * Surface (unique symbols):
 *   uint32_t gj_refcount_inc_u(uint32_t *rc);
 *     - Increment *rc by one. Returns the new count after the
 *       increment. NULL or wrap-at-UINT32_MAX leaves the counter
 *       unchanged and returns 0 (refuse to wrap). Soft pure-data
 *       refcount helper; not atomic.
 *   uint32_t __gj_refcount_inc_u  (alias)
 *   __libcgj_batch3345_marker = "libcgj-batch3345"
 *
 * Milestone 3350 exclusive continuum CREATE-ONLY (3341-3350). Distinct
 * from gj_ref_inc (batch326) — unique gj_refcount_inc_u surface only;
 * no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no atomics. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3345_marker[] = "libcgj-batch3345";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b3345_inc(uint32_t *pRc)
{
	uint32_t uCur;

	if (pRc == NULL) {
		return 0u;
	}
	uCur = *pRc;
	if (uCur == 0xffffffffu) {
		return 0u;
	}
	uCur++;
	*pRc = uCur;
	return uCur;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_refcount_inc_u - take one additional reference (non-atomic).
 *
 * rc: pointer to caller-owned counter; NULL → 0.
 *
 * Returns post-increment value, or 0 if NULL / already UINT32_MAX.
 * Single-thread / external-sync only. No parent wires.
 */
uint32_t
gj_refcount_inc_u(uint32_t *pRc)
{
	(void)NULL;
	return b3345_inc(pRc);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_refcount_inc_u(uint32_t *pRc)
    __attribute__((alias("gj_refcount_inc_u")));
