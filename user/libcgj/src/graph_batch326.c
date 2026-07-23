/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch326: non-atomic reference-count helpers on a
 * caller-owned uint32_t word.
 *
 * Surface (unique symbols):
 *   void     gj_ref_init(uint32_t *r);
 *     — Initialise *r to 1 (single live reference). NULL is a no-op.
 *   uint32_t gj_ref_inc(uint32_t *r);
 *     — Increment *r by one. Returns the new count after the increment.
 *       NULL or wrap-at-UINT32_MAX leaves the counter unchanged and
 *       returns 0 (refuse to wrap).
 *   uint32_t gj_ref_dec(uint32_t *r);
 *     — Decrement *r by one when *r > 0. Returns the remaining count
 *       after the decrement (0 means the last reference was dropped).
 *       NULL or already-zero counter returns 0 without underflow.
 *   __gj_ref_init / __gj_ref_inc / __gj_ref_dec  (aliases)
 *   __libcgj_batch326_marker = "libcgj-batch326"
 *
 * Single-threaded / external-sync only: plain loads and stores, no
 * __atomic_* and no memory barriers. Callers that share a counter across
 * threads must provide their own synchronisation.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno.
 * No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch326_marker[] = "libcgj-batch326";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ref_init — seed a non-atomic refcount to one live reference.
 *
 * r: pointer to caller-owned counter; NULL is ignored.
 */
void
gj_ref_init(uint32_t *pR)
{
	if (pR == NULL) {
		return;
	}
	*pR = 1u;
}

void __gj_ref_init(uint32_t *pR) __attribute__((alias("gj_ref_init")));

/*
 * gj_ref_inc — take one additional reference.
 *
 * Returns the post-increment value on success. Returns 0 if pR is NULL
 * or if *pR is already UINT32_MAX (saturate / refuse wrap so a corrupt
 * counter cannot roll to zero and free early).
 */
uint32_t
gj_ref_inc(uint32_t *pR)
{
	uint32_t uCur;

	if (pR == NULL) {
		return 0u;
	}
	uCur = *pR;
	if (uCur == 0xffffffffu) {
		return 0u;
	}
	uCur++;
	*pR = uCur;
	return uCur;
}

uint32_t __gj_ref_inc(uint32_t *pR) __attribute__((alias("gj_ref_inc")));

/*
 * gj_ref_dec — drop one reference; return remaining count.
 *
 * Returns *pR after a successful decrement. Returns 0 if pR is NULL or
 * if the counter was already zero (no underflow past zero). A return of
 * 0 after a prior non-zero count means the last reference was released
 * and the object may be destroyed by the caller.
 */
uint32_t
gj_ref_dec(uint32_t *pR)
{
	uint32_t uCur;

	if (pR == NULL) {
		return 0u;
	}
	uCur = *pR;
	if (uCur == 0u) {
		return 0u;
	}
	uCur--;
	*pR = uCur;
	return uCur;
}

uint32_t __gj_ref_dec(uint32_t *pR) __attribute__((alias("gj_ref_dec")));
