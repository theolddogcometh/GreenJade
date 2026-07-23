/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch307: integer simple moving average (int32 series).
 *
 * Surface (unique symbols):
 *   int gj_movavg_i32(const int32_t *in, size_t n, size_t win, int32_t *out);
 *     — Trailing simple moving average of n int32 samples with window win.
 *       For each index i in 0..n-1, out[i] is the integer average of the
 *       trailing window of up to win samples ending at i:
 *         start = (i + 1 < win) ? 0 : (i + 1 - win)
 *         count = i - start + 1
 *         out[i] = sum(in[start..i]) / count   (toward-zero C division)
 *       Partial windows at the head (i < win-1) use the available prefix.
 *       Full windows (i >= win-1) always average exactly win samples.
 *       Summation uses int64_t so intermediate window sums do not wrap
 *       for realistic win; the final store narrows to int32_t.
 *       Returns 0 on success, -1 on error:
 *         win == 0, or n > 0 with in == NULL or out == NULL.
 *       n == 0 with win > 0 is success (no-op).
 *       in and out must not alias (out-of-place only).
 *   __gj_movavg_i32  (alias)
 *   __libcgj_batch307_marker = "libcgj-batch307"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno.
 * No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch307_marker[] = "libcgj-batch307";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_movavg_i32 — simple moving average of n int32 samples, window win.
 *
 * O(n) sliding sum: add in[i], drop in[i - win] once the window is full,
 * then divide by the current window length (grows from 1 to win, then
 * stays at win). Division is signed C toward-zero.
 *
 * in:  input series of length n (read-only)
 * n:   number of samples; 0 → success, no stores
 * win: window length in samples; must be > 0
 * out: output series of length n (caller-allocated; must not overlap in)
 */
int
gj_movavg_i32(const int32_t *in, size_t n, size_t win, int32_t *out)
{
	size_t i;
	int64_t i64Sum;
	size_t cWin;

	if (win == 0u) {
		return -1;
	}
	if (n == 0u) {
		return 0;
	}
	if (in == NULL || out == NULL) {
		return -1;
	}

	i64Sum = 0;
	for (i = 0u; i < n; i++) {
		i64Sum += (int64_t)in[i];
		if (i >= win) {
			i64Sum -= (int64_t)in[i - win];
			cWin = win;
		} else {
			cWin = i + 1u;
		}
		/* Toward-zero divide (C99 signed /); cWin is always in 1..win. */
		out[i] = (int32_t)(i64Sum / (int64_t)cWin);
	}

	return 0;
}

int __gj_movavg_i32(const int32_t *in, size_t n, size_t win, int32_t *out)
    __attribute__((alias("gj_movavg_i32")));
