/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2262: freestanding timespec subtract nanoseconds
 * with nsec normalization (post-2260 time exclusive surface).
 *
 * Surface (unique symbols):
 *   void gj_timespec_sub_ns(int64_t *sec, int32_t *nsec, int64_t delta_ns);
 *     - *sec / *nsec -= delta_ns nanoseconds, then normalize *nsec into
 *       the closed-open range [0, 999999999] by borrowing from *sec
 *       (1 second = 1000000000 ns). NULL sec or NULL nsec -> no-op.
 *   void __gj_timespec_sub_ns  (alias)
 *   __libcgj_batch2262_marker = "libcgj-batch2262"
 *
 * Post-2260 time exclusive wave (2261-2270). Distinct from
 * gj_time_sub_ns (batch1036, flat uint64 ns) and gj_timeval_sub
 * (batch696, usec). Unique gj_timespec_sub_ns surface; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no struct timespec. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2262_marker[] = "libcgj-batch2262";

/* Nanoseconds per second. */
#define B2262_NSEC_PER_SEC  1000000000ll

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * b2262_split_ns - split a signed ns delta into (sec_carry, nsec_rem)
 * with nsec_rem in [0, 999999999] (floor division).
 */
static void
b2262_split_ns(int64_t delta_ns, int64_t *p_sec, int64_t *p_nsec)
{
	int64_t s;
	int64_t r;
	const int64_t n = B2262_NSEC_PER_SEC;

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
 * b2262_norm_sub - subtract (dsec, dnsec) from (*sec, *nsec), normalize.
 * dnsec is expected in [0, 999999999]; *nsec may be any int32.
 */
static void
b2262_norm_sub(int64_t *sec, int32_t *nsec, int64_t dsec, int64_t dnsec)
{
	int64_t s;
	int64_t n;
	int64_t carry;
	int64_t rem;
	const int64_t m = B2262_NSEC_PER_SEC;

	s = *sec - dsec;
	n = (int64_t)(*nsec) - dnsec;

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
 * gj_timespec_sub_ns - subtract delta_ns nanoseconds from (*sec, *nsec).
 *
 * sec:      seconds accumulator (NULL -> no-op)
 * nsec:     nanoseconds accumulator (NULL -> no-op)
 * delta_ns: signed nanosecond delta to subtract (any int64_t)
 *
 * After the subtract, *nsec is forced into [0, 999999999] by borrowing
 * whole seconds from *sec. Existing *nsec values outside that range are
 * also corrected. Seconds wrap on int64 overflow. Mutates only *sec and
 * *nsec. Does not set errno.
 */
void
gj_timespec_sub_ns(int64_t *sec, int32_t *nsec, int64_t delta_ns)
{
	int64_t dsec;
	int64_t dnsec;

	(void)NULL;
	if (sec == NULL || nsec == NULL) {
		return;
	}

	b2262_split_ns(delta_ns, &dsec, &dnsec);
	b2262_norm_sub(sec, nsec, dsec, dnsec);
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_timespec_sub_ns(int64_t *sec, int32_t *nsec, int64_t delta_ns)
    __attribute__((alias("gj_timespec_sub_ns")));
