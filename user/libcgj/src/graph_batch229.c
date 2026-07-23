/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch229: unsigned 64-bit GCD / LCM and extended
 * Euclidean GCD (Bézout coefficients).
 *
 * Clean-room freestanding pure C — integer/pointer only (builds under
 * userspace -msse2; this TU does not require SSE). No third-party number
 * theory source was copied.
 *
 * Surface (unique symbols):
 *   uint64_t gj_gcd_u64(uint64_t a, uint64_t b);
 *     — greatest common divisor of a and b (Euclidean algorithm).
 *       gcd(0, 0) → 0; gcd(a, 0) → a; gcd(0, b) → b.
 *   uint64_t gj_lcm_u64(uint64_t a, uint64_t b);
 *     — least common multiple via a/gcd * b with overflow-safe mul.
 *       lcm(a, 0) / lcm(0, b) / lcm(0, 0) → 0.
 *       Returns 0 if the mathematical LCM does not fit in uint64_t.
 *   uint64_t gj_egcd(uint64_t a, uint64_t b, int64_t *x, int64_t *y);
 *     — extended Euclidean: returns g = gcd(a, b) and writes (if non-NULL)
 *       integers x, y such that (int64_t)a * x + (int64_t)b * y == (int64_t)g
 *       when a and b fit in int64_t (both < 2^63). For full-range uint64
 *       inputs the returned g is still exact; x and y outputs are
 *       best-effort and may not satisfy the identity if intermediate
 *       coeffs overflow int64_t (in that case written as 0 if non-NULL).
 *   __gj_gcd_u64 / __gj_lcm_u64 / __gj_egcd  (aliases)
 *   __libcgj_batch229_marker = "libcgj-batch229"
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch229_marker[] = "libcgj-batch229";

/* ---- helpers ----------------------------------------------------------- */

/*
 * Binary / Euclidean GCD on uint64. Standard remainder loop is fine for
 * freestanding pure C and keeps the code short and branch-predictable.
 */
static uint64_t
b229_gcd(uint64_t a, uint64_t b)
{
	while (b != 0u) {
		uint64_t t = a % b;
		a = b;
		b = t;
	}
	return a;
}

/*
 * Multiply a * b into *out if the product fits in uint64_t.
 * Returns 0 on success, -1 on overflow (or if out is NULL).
 */
static int
b229_umul_overflow(uint64_t a, uint64_t b, uint64_t *out)
{
	if (out == NULL) {
		return -1;
	}
	if (a == 0u || b == 0u) {
		*out = 0u;
		return 0;
	}
	/* a * b overflows iff a > floor(UINT64_MAX / b). */
	if (a > (UINT64_C(0xffffffffffffffff) / b)) {
		return -1;
	}
	*out = a * b;
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_gcd_u64 — greatest common divisor of two unsigned 64-bit integers.
 *
 * gcd(0, 0) is defined as 0 (common practical convention; mathematically
 * every integer divides 0 so there is no unique positive GCD).
 */
uint64_t
gj_gcd_u64(uint64_t a, uint64_t b)
{
	return b229_gcd(a, b);
}

uint64_t __gj_gcd_u64(uint64_t a, uint64_t b)
    __attribute__((alias("gj_gcd_u64")));

/*
 * gj_lcm_u64 — least common multiple of two unsigned 64-bit integers.
 *
 * Computed as (a / gcd(a, b)) * b after reducing a by the GCD so the
 * intermediate product is as small as possible. Returns 0 when either
 * argument is 0, or when the true LCM exceeds UINT64_MAX.
 */
uint64_t
gj_lcm_u64(uint64_t a, uint64_t b)
{
	uint64_t g;
	uint64_t prod;

	if (a == 0u || b == 0u) {
		return 0u;
	}

	g = b229_gcd(a, b);
	/* a / g is exact and <= a; multiply by b with overflow check. */
	if (b229_umul_overflow(a / g, b, &prod) != 0) {
		return 0u;
	}
	return prod;
}

uint64_t __gj_lcm_u64(uint64_t a, uint64_t b)
    __attribute__((alias("gj_lcm_u64")));

/*
 * gj_egcd — extended Euclidean algorithm.
 *
 * Returns g = gcd(a, b). When x and/or y are non-NULL and the iterative
 * coefficients remain representable as int64_t, writes x, y satisfying
 *   a*x + b*y = g
 * (with a, b interpreted in the non-negative sense). If a coefficient
 * would overflow int64_t during the iteration, the corresponding pointer
 * (both, for simplicity) is written as 0 and g is still returned.
 *
 * For a = b = 0: g = 0, x = 0, y = 0.
 * For b = 0, a > 0: g = a, x = 1, y = 0  (when a fits in int64).
 * For a = 0, b > 0: g = b, x = 0, y = 1  (when b fits in int64).
 */
uint64_t
gj_egcd(uint64_t a, uint64_t b, int64_t *x, int64_t *y)
{
	/*
	 * Iterative extended Euclidean on unsigned remainders, tracking
	 * Bézout coeffs as signed 64-bit. old_s/old_t track the coeff pair
	 * for the current `old_r`; s/t for current `r`.
	 *
	 * Invariant (when no overflow):
	 *   old_r = a * old_s + b * old_t
	 *   r     = a * s     + b * t
	 */
	uint64_t old_r = a;
	uint64_t r = b;
	int64_t old_s = 1;
	int64_t s = 0;
	int64_t old_t = 0;
	int64_t t = 1;
	int overflow = 0;

	if (a == 0u && b == 0u) {
		if (x != NULL) {
			*x = 0;
		}
		if (y != NULL) {
			*y = 0;
		}
		return 0u;
	}

	while (r != 0u) {
		uint64_t q = old_r / r;
		uint64_t next_r = old_r - q * r;
		int64_t next_s = 0;
		int64_t next_t = 0;

		/*
		 * next_s = old_s - (int64_t)q * s
		 * next_t = old_t - (int64_t)q * t
		 * Guard: q must fit in int64, and the mul/sub must not
		 * overflow int64_t. If q >= 2^63 we cannot even cast it.
		 */
		if (q > (uint64_t)INT64_MAX) {
			overflow = 1;
			/* Still advance remainder state; coeffs abandoned. */
			old_r = r;
			r = next_r;
			old_s = 0;
			s = 0;
			old_t = 0;
			t = 0;
			continue;
		}

		if (!overflow) {
			int64_t q64 = (int64_t)q;
			int ok = 1;
			int64_t qs = 0;
			int64_t qt = 0;

			/*
			 * Safe signed multiply of q64 * {s,t}. Avoid
			 * INT64_MIN / -1 (undefined) by special-casing.
			 */
			if (q64 != 0 && s != 0) {
				if (s == -1) {
					if (q64 == INT64_MIN) {
						ok = 0;
					} else {
						qs = -q64;
					}
				} else if (q64 > 0 && s > 0) {
					if (q64 > INT64_MAX / s) {
						ok = 0;
					} else {
						qs = q64 * s;
					}
				} else if (q64 < 0 && s < 0) {
					if (q64 < INT64_MAX / s) {
						ok = 0;
					} else {
						qs = q64 * s;
					}
				} else if (s > 0) { /* q64 < 0 */
					if (q64 < INT64_MIN / s) {
						ok = 0;
					} else {
						qs = q64 * s;
					}
				} else { /* q64 > 0, s < 0 */
					if (q64 > INT64_MIN / s) {
						ok = 0;
					} else {
						qs = q64 * s;
					}
				}
			}

			if (ok && q64 != 0 && t != 0) {
				if (t == -1) {
					if (q64 == INT64_MIN) {
						ok = 0;
					} else {
						qt = -q64;
					}
				} else if (q64 > 0 && t > 0) {
					if (q64 > INT64_MAX / t) {
						ok = 0;
					} else {
						qt = q64 * t;
					}
				} else if (q64 < 0 && t < 0) {
					if (q64 < INT64_MAX / t) {
						ok = 0;
					} else {
						qt = q64 * t;
					}
				} else if (t > 0) {
					if (q64 < INT64_MIN / t) {
						ok = 0;
					} else {
						qt = q64 * t;
					}
				} else {
					if (q64 > INT64_MIN / t) {
						ok = 0;
					} else {
						qt = q64 * t;
					}
				}
			}

			/* next = old - q*coeff  (checked subtract). */
			if (ok) {
				if ((qs > 0 && old_s < INT64_MIN + qs) ||
				    (qs < 0 && old_s > INT64_MAX + qs)) {
					ok = 0;
				} else {
					next_s = old_s - qs;
				}
			}
			if (ok) {
				if ((qt > 0 && old_t < INT64_MIN + qt) ||
				    (qt < 0 && old_t > INT64_MAX + qt)) {
					ok = 0;
				} else {
					next_t = old_t - qt;
				}
			}

			if (ok) {
				old_s = s;
				s = next_s;
				old_t = t;
				t = next_t;
			} else {
				overflow = 1;
				old_s = 0;
				s = 0;
				old_t = 0;
				t = 0;
			}
		}

		old_r = r;
		r = next_r;
	}

	/*
	 * old_r is gcd(a, b). old_s / old_t are Bézout coeffs when no
	 * overflow occurred. Note gcd is always <= max(a,b) so fits in
	 * uint64_t (and is non-negative by construction).
	 */
	if (x != NULL) {
		*x = overflow ? 0 : old_s;
	}
	if (y != NULL) {
		*y = overflow ? 0 : old_t;
	}
	return old_r;
}

uint64_t __gj_egcd(uint64_t a, uint64_t b, int64_t *x, int64_t *y)
    __attribute__((alias("gj_egcd")));
