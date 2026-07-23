/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch250: 0-1 knapsack maximum value (stack DP).
 *
 * Surface (unique symbols):
 *   int64_t gj_knapsack01(const int *w, const int64_t *v, size_t n, int W);
 *     — classic 0-1 knapsack: each of n items may be taken at most once.
 *       w[i] are positive integer weights; v[i] are values; capacity is W.
 *       Returns the maximum attainable total value, or -1 on bad args.
 *       Caps: n <= 64, 0 <= W <= 512. Stack-only DP table of size W+1.
 *   __gj_knapsack01  (alias)
 *   __libcgj_batch250_marker = "libcgj-batch250"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2. No malloc, no errno. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch250_marker[] = "libcgj-batch250";

/* Freestanding caps for stack DP (dp[0..W], W <= B250_MAX_W). */
#define B250_MAX_N 64u
#define B250_MAX_W 512

/* ---- freestanding helpers ---------------------------------------------- */

/* Max of two signed 64-bit values. */
static int64_t
b250_max64(int64_t a, int64_t b)
{
	return (a > b) ? a : b;
}

/* Zero the DP table (size nWords = W+1). */
static void
b250_dp_clear(int64_t *dp, int nWords)
{
	int i;

	for (i = 0; i < nWords; i++) {
		dp[i] = 0;
	}
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_knapsack01 — 0-1 knapsack maximum total value.
 *
 * w: item weights (must be > 0 for each of the n items)
 * v: item values
 * n: number of items (0..B250_MAX_N)
 * W: knapsack capacity (0..B250_MAX_W)
 *
 * Uses the classic backward 1-D recurrence on a stack buffer:
 *   for each item i with weight wi and value vi:
 *     for j = W down to wi:
 *       dp[j] = max(dp[j], dp[j - wi] + vi)
 *
 * Returns max value in [0, W] capacity, or -1 on invalid input:
 *   NULL w/v when n > 0, n > 64, W < 0, W > 512, or any weight <= 0.
 * Empty instance (n == 0 or W == 0 with valid args) yields 0.
 */
int64_t
gj_knapsack01(const int *w, const int64_t *v, size_t n, int W)
{
	/* W+1 slots; fixed max so the array is always valid for W in range. */
	int64_t dp[B250_MAX_W + 1];
	size_t i;
	int j;

	if (n > (size_t)B250_MAX_N) {
		return (int64_t)-1;
	}
	if (W < 0 || W > B250_MAX_W) {
		return (int64_t)-1;
	}
	if (n > 0u && (w == NULL || v == NULL)) {
		return (int64_t)-1;
	}

	/* Validate positive weights before mutating DP. */
	for (i = 0u; i < n; i++) {
		if (w[i] <= 0) {
			return (int64_t)-1;
		}
	}

	if (n == 0u || W == 0) {
		return 0;
	}

	b250_dp_clear(dp, W + 1);

	for (i = 0u; i < n; i++) {
		int wi = w[i];
		int64_t vi = v[i];

		/* Backward scan preserves 0-1 (each item once). */
		for (j = W; j >= wi; j--) {
			dp[j] = b250_max64(dp[j], dp[j - wi] + vi);
		}
	}

	return dp[W];
}

int64_t __gj_knapsack01(const int *w, const int64_t *v, size_t n, int W)
    __attribute__((alias("gj_knapsack01")));
