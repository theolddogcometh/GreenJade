/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch695: freestanding timeval (sec, usec) delta add
 * with microsecond normalization.
 *
 * Surface (unique symbols):
 *   void gj_timeval_add(int64_t *sec, int32_t *usec,
 *                       int64_t dsec, int32_t dusec);
 *     — *sec += dsec, *usec += dusec, then normalize *usec into the
 *       closed-open range [0, 999999] by carrying into / borrowing from
 *       *sec (1 second = 1000000 usec). NULL sec or NULL usec → no-op.
 *   void __gj_timeval_add  (alias)
 *   __libcgj_batch695_marker = "libcgj-batch695"
 *
 * Unique gj_ names only; no multi-def with libc timeradd / timeval ops.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch695_marker[] = "libcgj-batch695";

#define B695_USEC_PER_SEC 1000000

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_timeval_add — add (dsec, dusec) into (*sec, *usec), normalize usec.
 *
 * sec:  seconds accumulator (NULL → no-op)
 * usec: microseconds accumulator (NULL → no-op)
 * dsec: seconds delta to add
 * dusec: microseconds delta to add (may be outside [0, 999999];
 *        result is always normalized)
 *
 * After the add, *usec is forced into [0, 999999] by carrying whole
 * seconds into *sec (positive) or borrowing (negative usec). Existing
 * *usec values outside that range are also corrected. Mutates only
 * *sec and *usec. Does not set errno.
 */
void
gj_timeval_add(int64_t *sec, int32_t *usec, int64_t dsec, int32_t dusec)
{
	int64_t s;
	int64_t u;
	int64_t carry;
	int64_t rem;

	if (sec == NULL || usec == NULL) {
		return;
	}

	s = *sec + dsec;
	u = (int64_t)*usec + (int64_t)dusec;

	/*
	 * Normalize u into [0, 999999]. C99 / truncates toward zero, so
	 * a negative remainder needs one extra borrow to land in range.
	 */
	carry = u / (int64_t)B695_USEC_PER_SEC;
	rem = u % (int64_t)B695_USEC_PER_SEC;
	if (rem < 0) {
		rem += (int64_t)B695_USEC_PER_SEC;
		carry -= 1;
	}

	*sec = s + carry;
	*usec = (int32_t)rem;
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_timeval_add(int64_t *sec, int32_t *usec, int64_t dsec, int32_t dusec)
    __attribute__((alias("gj_timeval_add")));
