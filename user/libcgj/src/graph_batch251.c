/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch251: minimum coins for amount (unbounded knapsack).
 *
 * Surface (unique symbols):
 *   int gj_coin_change_min(const int *coins, size_t ncoins, int amount);
 *     — Fewest coins that sum exactly to amount; each coin type may be
 *       used any number of times (classic unbounded-knapsack coin change).
 *       Returns the minimum coin count, or -1 if impossible.
 *       Caps: amount in [0, 10000], ncoins <= 32. Stack-only DP.
 *   __gj_coin_change_min  (alias)
 *   __libcgj_batch251_marker = "libcgj-batch251"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2. No malloc, no errno. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch251_marker[] = "libcgj-batch251";

/* Freestanding caps for stack DP (dp[0..amount], amount <= B251_MAX_AMOUNT). */
#define B251_MAX_NCOINS 32u
#define B251_MAX_AMOUNT 10000
/* Sentinel: larger than any feasible coin count (amount + 1 <= 10001). */
#define B251_INF        (B251_MAX_AMOUNT + 1)

/* ---- freestanding helpers ---------------------------------------------- */

static int
b251_min2(int a, int b)
{
	return (a < b) ? a : b;
}

/*
 * b251_coin_change — bottom-up unbounded knapsack on a stack DP table.
 *
 * dp[x] = fewest coins to make sum x; B251_INF means unreachable.
 * Forward scan over amounts with unlimited reuse of each coin (outer
 * amount / inner coins). Non-positive denominations are skipped.
 * Caller guarantees: coins != NULL, 0 < ncoins <= 32, 0 < amount <= 10000.
 */
static int
b251_coin_change(const int *coins, size_t ncoins, int amount)
{
	/* Fixed max so the array is always valid for amount in range. */
	int dp[B251_MAX_AMOUNT + 1];
	int a;
	size_t i;

	dp[0] = 0;
	for (a = 1; a <= amount; a++) {
		dp[a] = B251_INF;
	}

	for (a = 1; a <= amount; a++) {
		for (i = 0u; i < ncoins; i++) {
			int c = coins[i];

			if (c <= 0 || c > a) {
				continue;
			}
			if (dp[a - c] < B251_INF) {
				dp[a] = b251_min2(dp[a], dp[a - c] + 1);
			}
		}
	}

	return (dp[amount] >= B251_INF) ? -1 : dp[amount];
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_coin_change_min — minimum number of coins summing to amount.
 *
 * coins:  coin denominations (non-positive values are ignored in the DP)
 * ncoins: number of entries (0..B251_MAX_NCOINS)
 * amount: target sum (0..B251_MAX_AMOUNT)
 *
 * Recurrence (unbounded):
 *   dp[0] = 0
 *   dp[a] = min over coin c of dp[a - c] + 1   (when a >= c and reachable)
 *
 * Returns min coin count, or -1 on impossible / invalid input:
 *   amount < 0, amount > 10000, ncoins > 32, NULL coins when amount > 0,
 *   or no combination of coins reaches amount.
 * amount == 0 always yields 0 (empty selection).
 */
int
gj_coin_change_min(const int *coins, size_t ncoins, int amount)
{
	if (amount == 0) {
		return 0;
	}
	if (amount < 0 || amount > B251_MAX_AMOUNT) {
		return -1;
	}
	if (ncoins > (size_t)B251_MAX_NCOINS) {
		return -1;
	}
	if (coins == NULL || ncoins == 0u) {
		return -1;
	}

	return b251_coin_change(coins, ncoins, amount);
}

int __gj_coin_change_min(const int *coins, size_t ncoins, int amount)
    __attribute__((alias("gj_coin_change_min")));
