/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2269: freestanding ETA hint in milliseconds from
 * progress counters (post-2260 time exclusive surface).
 *
 * Surface (unique symbols):
 *   uint64_t gj_eta_ms_hint(uint64_t done, uint64_t total,
 *                           uint64_t elapsed_ms);
 *     - Soft remaining-time hint:
 *         remaining = total - done   (when total > done)
 *         eta_ms    = floor(remaining * elapsed_ms / done)
 *       Returns 0 when done == 0 (no pace yet), total <= done (finished
 *       or empty), or elapsed_ms == 0. Overflow-safe wide multiply;
 *       saturates at UINT64_MAX.
 *   uint64_t __gj_eta_ms_hint  (alias)
 *   __libcgj_batch2269_marker = "libcgj-batch2269"
 *
 * Post-2260 time exclusive wave (2261-2270). Pure arithmetic hint; not a
 * wall-clock API. Unique surface; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2269_marker[] = "libcgj-batch2269";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * b2269_muldiv - portable floor(a * b / d) without __int128 / libgcc ti3.
 * Saturates at UINT64_MAX. Caller guarantees d > 0.
 */
static uint64_t
b2269_muldiv(uint64_t a, uint64_t b, uint64_t d)
{
	uint64_t q;
	uint64_t r;
	uint64_t term1;
	uint64_t term2;
	uint64_t bq;
	uint64_t br;
	uint64_t t;

	if (a == 0ull || b == 0ull) {
		return 0ull;
	}
	if (a <= UINT64_MAX / b) {
		return (a * b) / d;
	}
	q = a / d;
	r = a % d;
	if (q > 0ull && q > UINT64_MAX / b) {
		return UINT64_MAX;
	}
	term1 = q * b;
	if (r == 0ull) {
		return term1;
	}
	if (r <= UINT64_MAX / b) {
		term2 = (r * b) / d;
	} else {
		bq = b / d;
		br = b % d;
		if (bq > 0ull && r > UINT64_MAX / bq) {
			return UINT64_MAX;
		}
		term2 = r * bq;
		if (br != 0ull) {
			if (r <= UINT64_MAX / br) {
				t = (r * br) / d;
			} else {
				return UINT64_MAX;
			}
			if (term2 > UINT64_MAX - t) {
				return UINT64_MAX;
			}
			term2 += t;
		}
	}
	if (term1 > UINT64_MAX - term2) {
		return UINT64_MAX;
	}
	return term1 + term2;
}

/*
 * b2269_eta - floor(remaining * elapsed_ms / done).
 * Caller guarantees done > 0 and remaining is well-formed.
 */
static uint64_t
b2269_eta(uint64_t remaining, uint64_t elapsed_ms, uint64_t done)
{
	if (remaining == 0ull || elapsed_ms == 0ull) {
		return 0ull;
	}
	return b2269_muldiv(remaining, elapsed_ms, done);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_eta_ms_hint - estimated remaining milliseconds from progress.
 *
 * done:        units completed so far (0 -> return 0, no pace)
 * total:       total units of work (total <= done -> return 0)
 * elapsed_ms:  milliseconds spent to complete "done" (0 -> return 0)
 *
 * Assumes a constant rate done/elapsed_ms. Returns a soft hint only;
 * not a deadline or timer. Pure; no side effects.
 */
uint64_t
gj_eta_ms_hint(uint64_t done, uint64_t total, uint64_t elapsed_ms)
{
	uint64_t remaining;

	(void)NULL;
	if (done == 0ull || elapsed_ms == 0ull || total <= done) {
		return 0ull;
	}
	remaining = total - done;
	return b2269_eta(remaining, elapsed_ms, done);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_eta_ms_hint(uint64_t done, uint64_t total, uint64_t elapsed_ms)
    __attribute__((alias("gj_eta_ms_hint")));
