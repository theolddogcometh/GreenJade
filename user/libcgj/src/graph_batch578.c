/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch578: freestanding floor integer cube root.
 * (gj_isqrt_u64 already owned by batch371 — this TU is icbrt.)
 *
 * Surface:
 *   uint64_t gj_icbrt_u64(uint64_t x);
 *   uint64_t __gj_icbrt_u64 (alias)
 *   __libcgj_batch578_marker = "libcgj-batch578"
 *
 * Returns largest y such that y*y*y <= x (y fits in 32 bits for all
 * uint64 x; overflow-safe multiply).
 *
 * Clean-room freestanding pure C. No malloc/errno/libc.
 */

#include <stdint.h>

const char __libcgj_batch578_marker[] = "libcgj-batch578";

static int
b578_mul_ok(uint64_t a, uint64_t b, uint64_t *out)
{
	if (a != 0ull && b > (uint64_t)-1 / a) {
		return 0;
	}
	*out = a * b;
	return 1;
}

/*
 * gj_icbrt_u64 — floor cube root of x.
 */
uint64_t
gj_icbrt_u64(uint64_t x)
{
	uint64_t lo;
	uint64_t hi;
	uint64_t mid;
	uint64_t sq;
	uint64_t cu;

	if (x < 2ull) {
		return x;
	}
	/* cube root of UINT64_MAX is 2642245 */
	lo = 1ull;
	hi = 2642246ull;
	while (lo < hi) {
		mid = lo + ((hi - lo + 1ull) >> 1);
		if (!b578_mul_ok(mid, mid, &sq)) {
			hi = mid - 1ull;
			continue;
		}
		if (!b578_mul_ok(sq, mid, &cu)) {
			hi = mid - 1ull;
			continue;
		}
		if (cu <= x) {
			lo = mid;
		} else {
			hi = mid - 1ull;
		}
	}
	return lo;
}

uint64_t __gj_icbrt_u64(uint64_t x)
    __attribute__((alias("gj_icbrt_u64")));
