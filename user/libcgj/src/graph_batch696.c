/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch696: freestanding timeval subtract with usec
 * normalize (OK/FAIL).
 *
 * Surface (unique symbols):
 *   int gj_timeval_sub(int64_t *sec, int32_t *usec,
 *                      int64_t dsec, int32_t dusec);
 *     — In-place: (*sec, *usec) -= (dsec, dusec), then normalize *usec
 *       into the closed range [0, 999999] by borrowing/carrying whole
 *       seconds. On success write the normalized pair and return 0.
 *       On failure return -1 and leave *sec / *usec untouched.
 *       FAIL when sec or usec is NULL, or when the second component
 *       overflows int64_t while applying the difference / carry.
 *   int __gj_timeval_sub  (alias)
 *   __libcgj_batch696_marker = "libcgj-batch696"
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no compiler builtins. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch696_marker[] = "libcgj-batch696";

/* One second in microseconds. */
#define B696_USEC_PER_SEC  1000000

/* ---- helpers (TU-private) ---------------------------------------------- */

/*
 * b696_i64_sub — a - b with signed overflow check.
 * Writes a - b to *out and returns 0 on success; returns -1 on overflow
 * (*out untouched). out may not be NULL (caller guarantees).
 */
static int
b696_i64_sub(int64_t a, int64_t b, int64_t *out)
{
	/*
	 * a - b overflows iff:
	 *   b > 0 && a < INT64_MIN + b
	 *   b < 0 && a > INT64_MAX + b
	 */
	if (b > 0) {
		if (a < (INT64_MIN + b)) {
			return -1;
		}
	} else if (b < 0) {
		if (a > (INT64_MAX + b)) {
			return -1;
		}
	}
	*out = a - b;
	return 0;
}

/*
 * b696_i64_add — a + b with signed overflow check.
 * Writes a + b to *out and returns 0 on success; returns -1 on overflow
 * (*out untouched). out may not be NULL (caller guarantees).
 */
static int
b696_i64_add(int64_t a, int64_t b, int64_t *out)
{
	/*
	 * a + b overflows iff:
	 *   b > 0 && a > INT64_MAX - b
	 *   b < 0 && a < INT64_MIN - b
	 */
	if (b > 0) {
		if (a > (INT64_MAX - b)) {
			return -1;
		}
	} else if (b < 0) {
		if (a < (INT64_MIN - b)) {
			return -1;
		}
	}
	*out = a + b;
	return 0;
}

/*
 * b696_usec_norm — split raw microsecond delta into (carry_sec, usec).
 *
 * Input:  raw usec difference (any int64 in the range of two int32 diffs
 *         plus reasonable carry; general int64 accepted).
 * Output: *pCarry = floor(u / 1000000)  (toward -inf)
 *         *pUsec  = u - *pCarry * 1000000  ∈ [0, 999999]
 *
 * Uses truncating C division and corrects the negative case so the
 * remainder is always non-negative (floor division semantics).
 */
static void
b696_usec_norm(int64_t u, int64_t *pCarry, int32_t *pUsec)
{
	int64_t carry;
	int64_t rem;
	const int64_t m = (int64_t)B696_USEC_PER_SEC;

	/* C99 division truncates toward zero. */
	carry = u / m;
	rem = u % m;
	if (rem < 0) {
		/* Move remainder into [0, m): borrow one more second. */
		rem += m;
		carry -= 1;
	}

	*pCarry = carry;
	*pUsec = (int32_t)rem;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_timeval_sub — subtract (dsec, dusec) from (*sec, *usec) with normalize.
 *
 * sec, usec: in/out timeval components (both required; NULL → -1)
 * dsec:      seconds to subtract
 * dusec:     microseconds to subtract (need not be pre-normalized)
 *
 * Semantics (success path):
 *   raw_usec = *usec - dusec
 *   (carry, nu) = normalize(raw_usec)   // nu ∈ [0, 999999]
 *   *sec  = *sec - dsec + carry
 *   *usec = nu
 *
 * Returns 0 (OK) on success; -1 (FAIL) on NULL pointers or int64 overflow
 * of the second component (inputs left unchanged on FAIL).
 */
int
gj_timeval_sub(int64_t *sec, int32_t *usec, int64_t dsec, int32_t dusec)
{
	int64_t raw_usec;
	int64_t carry;
	int32_t nu;
	int64_t s;
	int64_t s2;

	if (sec == NULL || usec == NULL) {
		return -1;
	}

	/* Wider intermediate so int32 usec diffs never overflow. */
	raw_usec = (int64_t)(*usec) - (int64_t)dusec;
	b696_usec_norm(raw_usec, &carry, &nu);

	/* s = *sec - dsec */
	if (b696_i64_sub(*sec, dsec, &s) != 0) {
		return -1;
	}

	/* s2 = s + carry  (carry may be negative when borrowing) */
	if (b696_i64_add(s, carry, &s2) != 0) {
		return -1;
	}

	*sec = s2;
	*usec = nu;
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_timeval_sub(int64_t *sec, int32_t *usec, int64_t dsec, int32_t dusec)
    __attribute__((alias("gj_timeval_sub")));
