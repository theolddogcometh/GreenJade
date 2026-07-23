/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2268: freestanding events-per-second rate from
 * a count and elapsed milliseconds (post-2260 time exclusive surface).
 *
 * Surface (unique symbols):
 *   uint64_t gj_rate_per_sec_u(uint64_t count, uint64_t elapsed_ms);
 *     - Compute floor(count * 1000 / elapsed_ms) as an unsigned rate in
 *       units per second. elapsed_ms == 0 -> 0 (no rate). Overflow-safe
 *       wide multiply; saturates the quotient at UINT64_MAX.
 *   uint64_t __gj_rate_per_sec_u  (alias)
 *   __libcgj_batch2268_marker = "libcgj-batch2268"
 *
 * Post-2260 time exclusive wave (2261-2270). Distinct from
 * gj_rate_limit_tokens (batch1369). Unique _u surface; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2268_marker[] = "libcgj-batch2268";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * b2268_muldiv - portable floor(a * b / d) without __int128 / libgcc ti3.
 * Saturates at UINT64_MAX. Caller guarantees d > 0.
 */
static uint64_t
b2268_muldiv(uint64_t a, uint64_t b, uint64_t d)
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
	/* a*b overflows 64: use a = q*d + r  =>  a*b/d = q*b + r*b/d */
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
		/* r*b overflows: r*b/d = r*(b/d) + r*(b%d)/d */
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
 * b2268_rate - floor(count * 1000 / elapsed_ms).
 * Caller guarantees elapsed_ms > 0.
 */
static uint64_t
b2268_rate(uint64_t count, uint64_t elapsed_ms)
{
	if (count == 0ull) {
		return 0ull;
	}
	return b2268_muldiv(count, 1000ull, elapsed_ms);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_rate_per_sec_u - unsigned rate (units per second) from count / time.
 *
 * count:       number of events (or bytes, etc.) in the window
 * elapsed_ms:  window length in milliseconds (0 -> return 0)
 *
 * Returns floor(count * 1000 / elapsed_ms), saturating at UINT64_MAX
 * if the true quotient would exceed that. Pure; no side effects.
 */
uint64_t
gj_rate_per_sec_u(uint64_t count, uint64_t elapsed_ms)
{
	(void)NULL;
	if (elapsed_ms == 0ull) {
		return 0ull;
	}
	return b2268_rate(count, elapsed_ms);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_rate_per_sec_u(uint64_t count, uint64_t elapsed_ms)
    __attribute__((alias("gj_rate_per_sec_u")));
