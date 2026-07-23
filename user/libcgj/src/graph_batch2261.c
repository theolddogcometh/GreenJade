/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2261: freestanding timespec add nanoseconds
 * with nsec normalization (post-2260 time exclusive surface).
 *
 * Surface (unique symbols):
 *   void gj_timespec_add_ns(int64_t *sec, int32_t *nsec, int64_t delta_ns);
 *     - *sec / *nsec += delta_ns nanoseconds, then normalize *nsec into
 *       the closed-open range [0, 999999999] by carrying into *sec
 *       (1 second = 1000000000 ns). NULL sec or NULL nsec -> no-op.
 *   void __gj_timespec_add_ns  (alias)
 *   __libcgj_batch2261_marker = "libcgj-batch2261"
 *
 * Post-2260 time exclusive wave (2261-2270). Distinct from
 * gj_time_add_ns (batch1035, flat uint64 ns) and gj_timeval_add
 * (batch695, usec). Unique gj_timespec_add_ns surface; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no struct timespec. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2261_marker[] = "libcgj-batch2261";

/* Nanoseconds per second. */
#define B2261_NSEC_PER_SEC  1000000000ll

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * b2261_split_ns - split a signed ns delta into (sec_carry, nsec_rem)
 * with nsec_rem in [0, 999999999] (floor division).
 */
static void
b2261_split_ns(int64_t delta_ns, int64_t *p_sec, int64_t *p_nsec)
{
	int64_t s;
	int64_t r;
	const int64_t n = B2261_NSEC_PER_SEC;

	s = delta_ns / n;
	r = delta_ns % n;
	if (r < (int64_t)0) {
		r += n;
		s -= (int64_t)1;
	}
	*p_sec = s;
	*p_nsec = r;
}

/*
 * b2261_norm_add - add (dsec, dnsec) into (*sec, *nsec), normalize.
 * dnsec is expected in [0, 999999999]; *nsec may be any int32.
 */
static void
b2261_norm_add(int64_t *sec, int32_t *nsec, int64_t dsec, int64_t dnsec)
{
	int64_t s;
	int64_t n;
	int64_t carry;
	int64_t rem;
	const int64_t m = B2261_NSEC_PER_SEC;

	s = *sec + dsec;
	n = (int64_t)(*nsec) + dnsec;

	carry = n / m;
	rem = n % m;
	if (rem < (int64_t)0) {
		rem += m;
		carry -= (int64_t)1;
	}

	*sec = s + carry;
	*nsec = (int32_t)rem;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_timespec_add_ns - add delta_ns nanoseconds into (*sec, *nsec).
 *
 * sec:      seconds accumulator (NULL -> no-op)
 * nsec:     nanoseconds accumulator (NULL -> no-op)
 * delta_ns: signed nanosecond delta (any int64_t; split then applied)
 *
 * After the add, *nsec is forced into [0, 999999999] by carrying whole
 * seconds into *sec. Existing *nsec values outside that range are also
 * corrected. Seconds wrap on int64 overflow (same model as timeval_add).
 * Mutates only *sec and *nsec. Does not set errno.
 */
void
gj_timespec_add_ns(int64_t *sec, int32_t *nsec, int64_t delta_ns)
{
	int64_t dsec;
	int64_t dnsec;

	(void)NULL;
	if (sec == NULL || nsec == NULL) {
		return;
	}

	b2261_split_ns(delta_ns, &dsec, &dnsec);
	b2261_norm_add(sec, nsec, dsec, dnsec);
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_timespec_add_ns(int64_t *sec, int32_t *nsec, int64_t delta_ns)
    __attribute__((alias("gj_timespec_add_ns")));
