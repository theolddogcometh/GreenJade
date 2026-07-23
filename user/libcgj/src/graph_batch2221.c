/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2221: open-addressed u32 key table lookup.
 *
 * Surface (unique symbols):
 *   int gj_table_u32_lookup(const uint32_t *keys, const uint32_t *vals,
 *                           size_t cap, uint32_t key, uint32_t *out);
 *     - Linear-probe lookup in a caller-owned open-addressed table.
 *       keys[i] == 0xFFFFFFFFu is empty (ends probe). keys[i] ==
 *       0xFFFFFFFEu is a tombstone (skipped). On hit write vals[i] to
 *       *out and return 0. On miss or bad args return -1 (*out
 *       untouched). Reserved sentinel keys are rejected as -1.
 *   int __gj_table_u32_lookup  (alias)
 *   __libcgj_batch2221_marker = "libcgj-batch2221"
 *
 * Post-2220 table exclusive wave (2221-2230). Distinct from gj_shmap_get
 * (batch323 string keys) and gj_imap_find (batch178 ordered ints).
 * Unique gj_table_u32_lookup surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2221_marker[] = "libcgj-batch2221";

/* Open-address slot sentinels (local to this TU; not exported macros). */
#define B2221_EMPTY  0xFFFFFFFFu
#define B2221_TOMB   0xFFFFFFFEu

/* Multiplicative mix constant (2^32 / phi). */
#define B2221_GOLDEN 0x9E3779B9u

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if slot is empty sentinel. */
static int
b2221_empty_p(uint32_t uKey)
{
	return (uKey == B2221_EMPTY) ? 1 : 0;
}

/* 1 if slot is tombstone sentinel. */
static int
b2221_tomb_p(uint32_t uKey)
{
	return (uKey == B2221_TOMB) ? 1 : 0;
}

/* First probe index for key in a table of capacity cCap (cCap > 0). */
static size_t
b2221_start(uint32_t uKey, size_t cCap)
{
	uint32_t uH;

	uH = uKey * B2221_GOLDEN;
	return (size_t)((uint64_t)uH % (uint64_t)cCap);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_table_u32_lookup - look up key in open-addressed parallel arrays.
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
gj_table_u32_lookup(const uint32_t *keys, const uint32_t *vals, size_t cap,
                    uint32_t key, uint32_t *out)
{
	size_t iStart;
	size_t i;

	(void)NULL;

	if (keys == NULL || vals == NULL || out == NULL || cap == 0u) {
		return -1;
	}
	if (b2221_empty_p(key) != 0 || b2221_tomb_p(key) != 0) {
		return -1;
	}

	iStart = b2221_start(key, cap);
	i = iStart;
	do {
		if (b2221_empty_p(keys[i]) != 0) {
			return -1;
		}
		if (b2221_tomb_p(keys[i]) == 0 && keys[i] == key) {
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

int __gj_table_u32_lookup(const uint32_t *keys, const uint32_t *vals,
                          size_t cap, uint32_t key, uint32_t *out)
    __attribute__((alias("gj_table_u32_lookup")));
