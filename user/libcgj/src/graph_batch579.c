/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch579: floor base-2 logarithm for uint64_t.
 *
 * Surface (unique symbols for this TU):
 *   int gj_floor_log2_u64(uint64_t x);
 *     — Floor of log2(x) for x >= 1 (index of the highest set bit,
 *       0-based: gj_floor_log2_u64(1)==0, gj_floor_log2_u64(2)==1,
 *       gj_floor_log2_u64(3)==1, gj_floor_log2_u64(UINT64_C(1)<<63)==63).
 *       Returns -1 when x == 0 (log2(0) is undefined).
 *   int __gj_floor_log2_u64  (alias)
 *   __libcgj_batch579_marker = "libcgj-batch579"
 *
 * Avoid multi-def: do not also export bare ilog2 / fls64 / other
 * non-gj_ names. Self-contained bit peel (no call into batch367/564
 * clz helpers). Clean-room freestanding pure C (integer only).
 * Compiles with -ffreestanding -msse2 -Wall -Wextra -Werror.
 * No malloc, no errno, no floating point, no libc, no compiler
 * builtins required. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch579_marker[] = "libcgj-batch579";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_floor_log2_u64 — floor(log2(x)), or -1 if x == 0.
 *
 * Binary bit-scan (high half first) so the path is a fixed number of
 * shift/compare steps rather than up to 63 single-bit shifts.
 */
int
gj_floor_log2_u64(uint64_t x)
{
	int r;
	uint64_t v;

	if (x == 0ull) {
		return -1;
	}

	r = 0;
	v = x;

	if (v >= 0x100000000ull) {
		v >>= 32;
		r += 32;
	}
	if (v >= 0x10000ull) {
		v >>= 16;
		r += 16;
	}
	if (v >= 0x100ull) {
		v >>= 8;
		r += 8;
	}
	if (v >= 0x10ull) {
		v >>= 4;
		r += 4;
	}
	if (v >= 0x4ull) {
		v >>= 2;
		r += 2;
	}
	if (v >= 0x2ull) {
		r += 1;
	}
	return r;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_floor_log2_u64(uint64_t x)
    __attribute__((alias("gj_floor_log2_u64")));
