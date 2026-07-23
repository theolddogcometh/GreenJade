/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2222: open-addressed u32 table insert with hint.
 *
 * Surface (unique symbols):
 *   int gj_table_u32_insert_hint(uint32_t *keys, uint32_t *vals, size_t cap,
 *                                uint32_t key, uint32_t val, size_t *hint);
 *     - Upsert (key, val) into an open-addressed table. keys[i] ==
 *       0xFFFFFFFFu empty, 0xFFFFFFFEu tomb. Probe starts at *hint when
 *       hint != NULL and *hint < cap, else at hash(key) % cap. First
 *       empty or tomb is claimed; matching key updates val only. On
 *       success write the final slot index to *hint (if non-NULL) and
 *       return 0. On full table or bad args return -1. Reserved
 *       sentinel keys are rejected as -1.
 *   int __gj_table_u32_insert_hint  (alias)
 *   __libcgj_batch2222_marker = "libcgj-batch2222"
 *
 * Post-2220 table exclusive wave (2221-2230). Distinct from gj_shmap_put
 * (batch323) and gj_imap_insert (batch178). Unique surface only; no
 * multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2222_marker[] = "libcgj-batch2222";

#define B2222_EMPTY  0xFFFFFFFFu
#define B2222_TOMB   0xFFFFFFFEu
#define B2222_GOLDEN 0x9E3779B9u

/* ---- freestanding helpers ---------------------------------------------- */

static int
b2222_empty_p(uint32_t uKey)
{
	return (uKey == B2222_EMPTY) ? 1 : 0;
}

static int
b2222_tomb_p(uint32_t uKey)
{
	return (uKey == B2222_TOMB) ? 1 : 0;
}

static size_t
b2222_hash_start(uint32_t uKey, size_t cCap)
{
	uint32_t uH;

	uH = uKey * B2222_GOLDEN;
	return (size_t)((uint64_t)uH % (uint64_t)cCap);
}

/*
 * Resolve probe start from optional hint. hint may be NULL.
 * cCap must be > 0.
 */
static size_t
b2222_probe_start(uint32_t uKey, size_t cCap, const size_t *pHint)
{
	if (pHint != NULL && *pHint < cCap) {
		return *pHint;
	}
	return b2222_hash_start(uKey, cCap);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_table_u32_insert_hint - upsert with optional probe-start hint.
 *
 * keys/vals/cap: open-addressed parallel arrays
 * key/val:       live key (not EMPTY/TOMB) and value to store
 * hint:          optional preferred start; on success receives final index
 *
 * Returns 0 on insert or update, -1 if full or bad args.
 */
int
gj_table_u32_insert_hint(uint32_t *keys, uint32_t *vals, size_t cap,
                         uint32_t key, uint32_t val, size_t *hint)
{
	size_t iStart;
	size_t i;
	size_t iTomb;
	int fTomb;

	(void)NULL;

	if (keys == NULL || vals == NULL || cap == 0u) {
		return -1;
	}
	if (b2222_empty_p(key) != 0 || b2222_tomb_p(key) != 0) {
		return -1;
	}

	iStart = b2222_probe_start(key, cap, hint);
	i = iStart;
	iTomb = cap;
	fTomb = 0;

	do {
		if (b2222_empty_p(keys[i]) != 0) {
			/* Prefer earlier tomb if seen; else claim empty. */
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
		if (b2222_tomb_p(keys[i]) != 0) {
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

	/* Full probe with no empty; may still have a tomb to reclaim. */
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

int __gj_table_u32_insert_hint(uint32_t *keys, uint32_t *vals, size_t cap,
                               uint32_t key, uint32_t val, size_t *hint)
    __attribute__((alias("gj_table_u32_insert_hint")));
