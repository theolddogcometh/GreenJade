/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch897: integer online variance add (Welford M2).
 *
 * Distinct from gj_welford_update (batch309, double). Pair with
 * gj_online_var_init (batch896).
 *
 * Surface (unique symbols):
 *   void gj_online_var_add(uint64_t *n, int64_t *mean, int64_t *m2,
 *                          int64_t x);
 *     — One integer Welford step:
 *         n++; delta = x - mean; mean += floor(delta / n);
 *         m2 += delta * (x - mean)
 *   void __gj_online_var_add  (alias)
 *   __libcgj_batch897_marker = "libcgj-batch897"
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no libm. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch897_marker[] = "libcgj-batch897";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * b897_floor_div — floor(a / b) for b > 0 (mathematical floor, not trunc).
 */
static int64_t
b897_floor_div(int64_t a, int64_t b)
{
	int64_t q;
	int64_t r;

	q = a / b;
	r = a % b;
	if (r != (int64_t)0 && a < (int64_t)0) {
		q -= (int64_t)1;
	}
	return q;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_online_var_add — fold one int64 sample into online mean / M2.
 *
 * Integer Welford:
 *   n    ← n + 1
 *   mean ← mean + floor((x - mean) / n)
 *   m2   ← m2 + (x - mean_old) * (x - mean_new)
 *
 * NULL any of n/mean/m2 is a no-op. When n would exceed INT64_MAX the
 * mean/m2 are left unchanged after the count increment (avoids a non-
 * representable divisor). Caller owns signed overflow of products.
 */
void
gj_online_var_add(uint64_t *n, int64_t *mean, int64_t *m2, int64_t x)
{
	uint64_t nn;
	int64_t m;
	int64_t delta;
	int64_t delta2;

	if (n == NULL || mean == NULL || m2 == NULL) {
		return;
	}

	nn = *n + (uint64_t)1;
	*n = nn;

	if (nn > (uint64_t)INT64_MAX) {
		return;
	}

	m = *mean;
	delta = x - m;
	m = m + b897_floor_div(delta, (int64_t)nn);
	*mean = m;

	delta2 = x - m;
	*m2 = *m2 + delta * delta2;
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_online_var_add(uint64_t *n, int64_t *mean, int64_t *m2, int64_t x)
    __attribute__((alias("gj_online_var_add")));
