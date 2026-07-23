/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2223: open-addressed u32 table delete with hint.
 *
 * Surface (unique symbols):
 *   int gj_table_u32_delete_hint(uint32_t *keys, size_t cap, uint32_t key,
 *                                size_t *hint);
 *     - Linear-probe delete. keys[i] == 0xFFFFFFFFu empty (ends probe),
 *       0xFFFFFFFEu tomb (skipped). Probe starts at *hint when hint !=
 *       NULL and *hint < cap, else at hash(key) % cap. On hit mark the
 *       slot as tombstone, write its index to *hint (if non-NULL), and
 *       return 0. On miss or bad args return -1. Reserved sentinel keys
 *       are rejected as -1. Values array is not required (key-only).
 *   int __gj_table_u32_delete_hint  (alias)
 *   __libcgj_batch2223_marker = "libcgj-batch2223"
 *
 * Post-2220 table exclusive wave (2221-2230). Tombstones keep probe
 * chains intact for later lookups. Unique surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2223_marker[] = "libcgj-batch2223";

#define B2223_EMPTY  0xFFFFFFFFu
#define B2223_TOMB   0xFFFFFFFEu
#define B2223_GOLDEN 0x9E3779B9u

/* ---- freestanding helpers ---------------------------------------------- */

static int
b2223_empty_p(uint32_t uKey)
{
	return (uKey == B2223_EMPTY) ? 1 : 0;
}

static int
b2223_tomb_p(uint32_t uKey)
{
	return (uKey == B2223_TOMB) ? 1 : 0;
}

static size_t
b2223_hash_start(uint32_t uKey, size_t cCap)
{
	uint32_t uH;

	uH = uKey * B2223_GOLDEN;
	return (size_t)((uint64_t)uH % (uint64_t)cCap);
}

static size_t
b2223_probe_start(uint32_t uKey, size_t cCap, const size_t *pHint)
{
	if (pHint != NULL && *pHint < cCap) {
		return *pHint;
	}
	return b2223_hash_start(uKey, cCap);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_table_u32_delete_hint - delete key, leaving a tombstone, with hint.
 *
 * keys: open-addressed key slots
 * cap:  capacity (must be > 0)
 * key:  live key to remove (not EMPTY/TOMB)
 * hint: optional preferred start; on success receives tomb index
 *
 * Returns 0 on delete, -1 on miss or bad args.
 */
int
gj_table_u32_delete_hint(uint32_t *keys, size_t cap, uint32_t key,
                         size_t *hint)
{
	size_t iStart;
	size_t i;

	(void)NULL;

	if (keys == NULL || cap == 0u) {
		return -1;
	}
	if (b2223_empty_p(key) != 0 || b2223_tomb_p(key) != 0) {
		return -1;
	}

	iStart = b2223_probe_start(key, cap, hint);
	i = iStart;
	do {
		if (b2223_empty_p(keys[i]) != 0) {
			return -1;
		}
		if (b2223_tomb_p(keys[i]) == 0 && keys[i] == key) {
			keys[i] = B2223_TOMB;
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

	return -1;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_table_u32_delete_hint(uint32_t *keys, size_t cap, uint32_t key,
                               size_t *hint)
    __attribute__((alias("gj_table_u32_delete_hint")));
