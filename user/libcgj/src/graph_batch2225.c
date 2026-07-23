/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2225: open-addressed u64 table insert with hint.
 *
 * Surface (unique symbols):
 *   int gj_table_u64_insert_hint(uint64_t *keys, uint64_t *vals, size_t cap,
 *                                uint64_t key, uint64_t val, size_t *hint);
 *     - Upsert (key, val) into an open-addressed u64 table. EMPTY =
 *       0xFFFFFFFFFFFFFFFFull, TOMB = 0xFFFFFFFFFFFFFFFEull. Probe
 *       starts at *hint when hint != NULL and *hint < cap, else at
 *       hash(key) % cap. First empty or tomb is claimed; matching key
 *       updates val only. On success write final slot index to *hint
 *       (if non-NULL) and return 0. On full table or bad args return
 *       -1. Reserved sentinel keys are rejected as -1.
 *   int __gj_table_u64_insert_hint  (alias)
 *   __libcgj_batch2225_marker = "libcgj-batch2225"
 *
 * Post-2220 table exclusive wave (2221-2230). Distinct from
 * gj_table_u32_insert_hint (batch2222) and gj_shmap_put (batch323).
 * Unique surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2225_marker[] = "libcgj-batch2225";

#define B2225_EMPTY  0xFFFFFFFFFFFFFFFFull
#define B2225_TOMB   0xFFFFFFFFFFFFFFFEull
#define B2225_GOLDEN 0x9E3779B97F4A7C15ull

/* ---- freestanding helpers ---------------------------------------------- */

static int
b2225_empty_p(uint64_t u64Key)
{
	return (u64Key == B2225_EMPTY) ? 1 : 0;
}

static int
b2225_tomb_p(uint64_t u64Key)
{
	return (u64Key == B2225_TOMB) ? 1 : 0;
}

static size_t
b2225_hash_start(uint64_t u64Key, size_t cCap)
{
	uint64_t u64H;

	u64H = u64Key * B2225_GOLDEN;
	return (size_t)(u64H % (uint64_t)cCap);
}

static size_t
b2225_probe_start(uint64_t u64Key, size_t cCap, const size_t *pHint)
{
	if (pHint != NULL && *pHint < cCap) {
		return *pHint;
	}
	return b2225_hash_start(u64Key, cCap);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_table_u64_insert_hint - upsert with optional probe-start hint.
 *
 * keys/vals/cap: open-addressed parallel arrays
 * key/val:       live key (not EMPTY/TOMB) and value to store
 * hint:          optional preferred start; on success receives final index
 *
 * Returns 0 on insert or update, -1 if full or bad args.
 */
int
gj_table_u64_insert_hint(uint64_t *keys, uint64_t *vals, size_t cap,
                         uint64_t key, uint64_t val, size_t *hint)
{
	size_t iStart;
	size_t i;
	size_t iTomb;
	int fTomb;

	(void)NULL;

	if (keys == NULL || vals == NULL || cap == 0u) {
		return -1;
	}
	if (b2225_empty_p(key) != 0 || b2225_tomb_p(key) != 0) {
		return -1;
	}

	iStart = b2225_probe_start(key, cap, hint);
	i = iStart;
	iTomb = cap;
	fTomb = 0;

	do {
		if (b2225_empty_p(keys[i]) != 0) {
			if (fTomb != 0) {
				i = iTomb;
			}
			keys[i] = key;
			vals[i] = val;
			if (hint != NULL) {
				*hint = i;
			}
			return 0;
		}
		if (b2225_tomb_p(keys[i]) != 0) {
			if (fTomb == 0) {
				iTomb = i;
				fTomb = 1;
			}
		} else if (keys[i] == key) {
			vals[i] = val;
			if (hint != NULL) {
				*hint = i;
			}
			return 0;
		}
		i++;
		if (i == cap) {
			i = 0u;
		}
	} while (i != iStart);

	if (fTomb != 0) {
		keys[iTomb] = key;
		vals[iTomb] = val;
		if (hint != NULL) {
			*hint = iTomb;
		}
		return 0;
	}
	return -1;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_table_u64_insert_hint(uint64_t *keys, uint64_t *vals, size_t cap,
                               uint64_t key, uint64_t val, size_t *hint)
    __attribute__((alias("gj_table_u64_insert_hint")));
