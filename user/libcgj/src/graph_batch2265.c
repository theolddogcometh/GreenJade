/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2265: freestanding milliseconds to timeval
 * (sec, usec) split (post-2260 time exclusive surface).
 *
 * Surface (unique symbols):
 *   void gj_ms_to_timeval(int64_t ms, int64_t *sec, int32_t *usec);
 *     - Split a signed millisecond duration into timeval-shaped
 *       (seconds, microseconds) with floor division toward -inf:
 *         sec  = floor(ms / 1000)
 *         usec = (ms - sec * 1000) * 1000   in [0, 999000]
 *       so usec is always a multiple of 1000 in [0, 999000].
 *       NULL sec or NULL usec -> no-op.
 *   void __gj_ms_to_timeval  (alias)
 *   __libcgj_batch2265_marker = "libcgj-batch2265"
 *
 * Post-2260 time exclusive wave (2261-2270). Inverse-shaped partner of
 * gj_timeval_to_ms (batch2264). Distinct from gj_ms_to_secs (batch485)
 * and gj_ms_to_us (batch1971). Unique surface; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no struct timeval. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2265_marker[] = "libcgj-batch2265";

#define B2265_MS_PER_SEC   1000ll
#define B2265_USEC_PER_MS  1000ll

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * b2265_split - floor-split ms into (sec, usec_ms_part * 1000).
 */
static void
b2265_split(int64_t ms, int64_t *p_sec, int32_t *p_usec)
{
	int64_t s;
	int64_t rem;
	const int64_t m = B2265_MS_PER_SEC;

	/* C99 division truncates toward zero; fix negative remainder. */
	s = ms / m;
	rem = ms % m;
	if (rem < (int64_t)0) {
		rem += m;
		s -= (int64_t)1;
	}

	*p_sec = s;
	/* rem in [0, 999] -> usec in [0, 999000] step 1000. */
	*p_usec = (int32_t)(rem * B2265_USEC_PER_MS);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ms_to_timeval - convert whole milliseconds to (sec, usec).
 *
 * ms:   signed millisecond duration
 * sec:  out seconds (NULL -> no-op)
 * usec: out microseconds in [0, 999000] step 1000 (NULL -> no-op)
 *
 * Uses mathematical floor so negative ms yield non-positive sec and a
 * non-negative usec remainder. Does not set errno.
 */
void
gj_ms_to_timeval(int64_t ms, int64_t *sec, int32_t *usec)
{
	(void)NULL;
	if (sec == NULL || usec == NULL) {
		return;
	}
	b2265_split(ms, sec, usec);
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_ms_to_timeval(int64_t ms, int64_t *sec, int32_t *usec)
    __attribute__((alias("gj_ms_to_timeval")));
