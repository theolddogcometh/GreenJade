/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch233: two-congruence Chinese Remainder Theorem.
 *
 * Surface (unique symbols):
 *   int gj_crt_pair(int64_t a1, int64_t m1, int64_t a2, int64_t m2,
 *                   int64_t *x, int64_t *m);
 *     — solve x ≡ a1 (mod m1), x ≡ a2 (mod m2).
 *       On success: *x in [0, *m), *m = lcm(|m1|, |m2|), return 0.
 *       On failure (NULL outs, zero modulus, |m| = 2^63, inconsistent
 *       system, or lcm overflow): return -1 (outs unchanged).
 *   __gj_crt_pair  (alias)
 *   __libcgj_batch233_marker = "libcgj-batch233"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2. No malloc, no errno. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch233_marker[] = "libcgj-batch233";

/* ---- freestanding helpers ---------------------------------------------- */

/* Positive absolute value; caller must reject INT64_MIN. */
static int64_t
b233_iabs(int64_t v)
{
	return (v < 0) ? -v : v;
}

/* Euclidean gcd for non-negative inputs. */
static int64_t
b233_gcd(int64_t a, int64_t b)
{
	while (b != 0) {
		int64_t t = a % b;

		a = b;
		b = t;
	}
	return a;
}

/*
 * Residue of a modulo m in [0, m). m must be > 0.
 * C99 % truncates toward zero; negative a needs a positive lift.
 */
static int64_t
b233_mod(int64_t a, int64_t m)
{
	int64_t r = a % m;

	if (r < 0) {
		r += m;
	}
	return r;
}

/*
 * Modular inverse of a modulo m (m > 1), assuming gcd(|a| mod m, m) == 1.
 * Returns inv in [0, m), or -1 if no inverse (should not occur for CRT
 * after dividing out gcd).
 */
static int64_t
b233_modinv(int64_t a, int64_t m)
{
	int64_t t = 0;
	int64_t newt = 1;
	int64_t r = m;
	int64_t newr = b233_mod(a, m);

	while (newr != 0) {
		int64_t q = r / newr;
		int64_t tmp;

		tmp = t - q * newt;
		t = newt;
		newt = tmp;

		tmp = r - q * newr;
		r = newr;
		newr = tmp;
	}

	if (r != 1) {
		return -1;
	}
	if (t < 0) {
		t += m;
	}
	return t;
}

/*
 * Multiply a * b for a, b >= 0 into *out. Returns 0 on success, -1 if the
 * product would exceed INT64_MAX (so lcm cannot be represented).
 */
static int
b233_mul_ok(int64_t a, int64_t b, int64_t *out)
{
	if (a == 0 || b == 0) {
		*out = 0;
		return 0;
	}
	if (a > (INT64_MAX / b)) {
		return -1;
	}
	*out = a * b;
	return 0;
}

/*
 * (a * b) mod m for a, b in [0, m), m > 1. Russian-peasant multiply with
 * running reduction. Uses uint64_t so a+a stays in-range: both operands
 * are < m <= 2^63-1, so sums are < 2^64 and never wrap uint64_t.
 * No __int128 / libgcc soft-div helpers (freestanding-safe).
 */
static int64_t
b233_mulmod(int64_t a, int64_t b, int64_t m)
{
	uint64_t ua = (uint64_t)b233_mod(a, m);
	uint64_t ub = (uint64_t)b233_mod(b, m);
	uint64_t um = (uint64_t)m;
	uint64_t r = 0u;

	while (ub > 0u) {
		if ((ub & 1u) != 0u) {
			r += ua;
			if (r >= um) {
				r -= um;
			}
		}
		ua += ua;
		if (ua >= um) {
			ua -= um;
		}
		ub >>= 1;
	}
	return (int64_t)r;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_crt_pair — solve the simultaneous system
 *   x ≡ a1 (mod m1)
 *   x ≡ a2 (mod m2)
 *
 * Existence: a1 ≡ a2 (mod gcd(|m1|, |m2|)).
 * Solution modulus: M = lcm(|m1|, |m2|) = |m1| / g * |m2|.
 *
 * Construction (after normalizing residues into [0, |mi|)):
 *   g   = gcd(m1, m2)
 *   m1g = m1/g,  m2g = m2/g   (coprime)
 *   k   = ((a2 - a1)/g) * inv(m1g mod m2g)  mod m2g
 *   x   = a1 + m1 * k         already in [0, M) when a1 ∈ [0, m1)
 *
 * Returns 0 and writes *x, *m on success; -1 on error (no write).
 */
int
gj_crt_pair(int64_t a1, int64_t m1, int64_t a2, int64_t m2,
            int64_t *x, int64_t *m)
{
	int64_t um1;
	int64_t um2;
	int64_t g;
	int64_t m1g;
	int64_t m2g;
	int64_t r1;
	int64_t r2;
	int64_t diff;
	int64_t inv;
	int64_t k;
	int64_t M;
	int64_t prod;

	if (x == NULL || m == NULL) {
		return -1;
	}
	/* Zero modulus is undefined; INT64_MIN has no positive abs in int64. */
	if (m1 == 0 || m2 == 0 || m1 == INT64_MIN || m2 == INT64_MIN) {
		return -1;
	}

	um1 = b233_iabs(m1);
	um2 = b233_iabs(m2);

	r1 = b233_mod(a1, um1);
	r2 = b233_mod(a2, um2);

	g = b233_gcd(um1, um2);
	/* Consistency: r1 ≡ r2 (mod g). */
	if (b233_mod(r1 - r2, g) != 0) {
		return -1;
	}

	m1g = um1 / g;
	m2g = um2 / g;

	/* M = lcm = (um1 / g) * um2 */
	if (b233_mul_ok(m1g, um2, &M) != 0) {
		return -1;
	}

	/* Trivial when one modulus divides the other and residues agree. */
	if (m2g == 1) {
		/* M == um1; r1 already satisfies both congruences. */
		*x = r1;
		*m = M;
		return 0;
	}

	/*
	 * k solves: m1g * k ≡ (r2 - r1)/g  (mod m2g)
	 * inv(m1g mod m2g) exists because gcd(m1g, m2g) == 1.
	 */
	inv = b233_modinv(m1g, m2g);
	if (inv < 0) {
		return -1;
	}

	diff = (r2 - r1) / g;
	diff = b233_mod(diff, m2g);
	/* k = (diff * inv) mod m2g */
	k = b233_mulmod(diff, inv, m2g);

	/*
	 * x = r1 + um1 * k.  Max = (um1-1) + um1*(m2g-1) = um1*m2g - 1 = M-1.
	 * um1 * k: k < m2g so um1*k <= um1*(m2g-1) < M <= INT64_MAX.
	 */
	if (b233_mul_ok(um1, k, &prod) != 0) {
		return -1;
	}
	*x = r1 + prod;
	*m = M;
	return 0;
}

int __gj_crt_pair(int64_t a1, int64_t m1, int64_t a2, int64_t m2,
                  int64_t *x, int64_t *m)
    __attribute__((alias("gj_crt_pair")));
