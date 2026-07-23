/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2224: open-addressed u64 key table lookup.
 *
 * Surface (unique symbols):
 *   int gj_table_u64_lookup(const uint64_t *keys, const uint64_t *vals,
 *                           size_t cap, uint64_t key, uint64_t *out);
 *     - Linear-probe lookup. keys[i] == 0xFFFFFFFFFFFFFFFFull is empty
 *       (ends probe). keys[i] == 0xFFFFFFFFFFFFFFFEull is a tombstone
 *       (skipped). On hit write vals[i] to *out and return 0. On miss
 *       or bad args return -1. Reserved sentinel keys are rejected.
 *   int __gj_table_u64_lookup  (alias)
 *   __libcgj_batch2224_marker = "libcgj-batch2224"
 *
 * Post-2220 table exclusive wave (2221-2230). Distinct from
 * gj_table_u32_lookup (batch2221) and gj_shmap_get (batch323). Unique
 * surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2224_marker[] = "libcgj-batch2224";

#define B2224_EMPTY  0xFFFFFFFFFFFFFFFFull
#define B2224_TOMB   0xFFFFFFFFFFFFFFFEull
#define B2224_GOLDEN 0x9E3779B97F4A7C15ull

/* ---- freestanding helpers ---------------------------------------------- */

static int
b2224_empty_p(uint64_t u64Key)
{
	return (u64Key == B2224_EMPTY) ? 1 : 0;
}

static int
b2224_tomb_p(uint64_t u64Key)
{
	return (u64Key == B2224_TOMB) ? 1 : 0;
}

static size_t
b2224_start(uint64_t u64Key, size_t cCap)
{
	uint64_t u64H;

	u64H = u64Key * B2224_GOLDEN;
	return (size_t)(u64H % (uint64_t)cCap);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_table_u64_lookup - look up key in open-addressed u64 parallel arrays.
 *
 * keys: capacity-cap key slots (EMPTY ends probe, TOMB is skippable)
 * vals: parallel values
 * cap:  table capacity (must be > 0)
 * key:  live key (not EMPTY or TOMB)
 * out:  on hit receives the paired value
 *
 * Returns 0 on hit, -1 on miss or bad args.
 */
int
gj_table_u64_lookup(const uint64_t *keys, const uint64_t *vals, size_t cap,
                    uint64_t key, uint64_t *out)
{
	size_t iStart;
	size_t i;

	(void)NULL;

	if (keys == NULL || vals == NULL || out == NULL || cap == 0u) {
		return -1;
	}
	if (b2224_empty_p(key) != 0 || b2224_tomb_p(key) != 0) {
		return -1;
	}

	iStart = b2224_start(key, cap);
	i = iStart;
	do {
		if (b2224_empty_p(keys[i]) != 0) {
			return -1;
		}
		if (b2224_tomb_p(keys[i]) == 0 && keys[i] == key) {
			*out = vals[i];
			return 0;
		}
		i++;
		if (i == cap) {
			i = 0u;
		}
	} while (i != iStart);

	return -1;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_table_u64_lookup(const uint64_t *keys, const uint64_t *vals,
                          size_t cap, uint64_t key, uint64_t *out)
    __attribute__((alias("gj_table_u64_lookup")));
