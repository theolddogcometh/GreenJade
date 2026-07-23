/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch178: ordered integer map (fixed parallel arrays).
 * Clean-room freestanding pure C — integer/pointer only (no SSE required;
 * builds under userspace -msse2). No third-party source was copied.
 *
 * Parallel arrays keys[0..n) and vals[0..n) stay sorted by ascending key.
 * Lookup and insert both use binary search (lower-bound). Capacity is fixed
 * by the caller (cap); this TU never allocates.
 *
 * Note: gj_imap_* IMAP protocol helpers live in graph_batch145.c — different
 * symbols (is_untagged / parse_exists / parse_uid). No multi-def risk.
 *
 * This TU adds only unique symbols:
 *   int gj_imap_find(const int *keys, const int *vals, size_t n,
 *                    int key, int *out);
 *     — binary search for key; on hit write vals[i] to *out and return 0;
 *       on miss or bad args return -1 (*out untouched on miss/error).
 *   int gj_imap_insert(int *keys, int *vals, size_t *n, size_t cap,
 *                      int key, int val);
 *     — if key present: overwrite vals[i] = val, return 0;
 *       if absent and *n < cap: shift right at lower-bound, store key/val,
 *       bump *n, return 0;
 *       if full or bad args: return -1.
 *   __gj_imap_find / __gj_imap_insert  (aliases)
 *   __libcgj_batch178_marker = "libcgj-batch178"
 *
 * Notes:
 *   - No errno (freestanding-safe); callers use return codes only.
 *   - Duplicate keys are never introduced; insert is upsert.
 */

#include <stddef.h>

const char __libcgj_batch178_marker[] = "libcgj-batch178";

/* ---- lower-bound binary search ----------------------------------------- */

/*
 * Return the smallest index i in [0, n] such that keys[i] >= key
 * (or n if all keys are < key). keys must be sorted ascending.
 */
static size_t
b178_lower_bound(const int *keys, size_t n, int key)
{
	size_t lo = 0;
	size_t hi = n;

	while (lo < hi) {
		size_t mid = lo + ((hi - lo) >> 1);

		if (keys[mid] < key) {
			lo = mid + 1;
		} else {
			hi = mid;
		}
	}
	return lo;
}

/* ---- gj_imap_find ------------------------------------------------------ */

/*
 * Binary-search for key in ordered keys[0..n). On success write the paired
 * value to *out and return 0. Return -1 if not found or if keys/vals/out
 * is NULL when n > 0 (empty map with n == 0 is a clean miss).
 */
int
gj_imap_find(const int *keys, const int *vals, size_t n, int key, int *out)
{
	size_t i;

	if (out == NULL) {
		return -1;
	}
	if (n == 0) {
		return -1;
	}
	if (keys == NULL || vals == NULL) {
		return -1;
	}

	i = b178_lower_bound(keys, n, key);
	if (i < n && keys[i] == key) {
		*out = vals[i];
		return 0;
	}
	return -1;
}

int __gj_imap_find(const int *keys, const int *vals, size_t n, int key,
                   int *out)
    __attribute__((alias("gj_imap_find")));

/* ---- gj_imap_insert ---------------------------------------------------- */

/*
 * Upsert (key, val) into ordered parallel arrays.
 *   - existing key → update vals[i]
 *   - new key and *n < cap → insert at sorted position (shift right)
 *   - full map / NULL args → -1
 * Returns 0 on success.
 */
int
gj_imap_insert(int *keys, int *vals, size_t *n, size_t cap, int key, int val)
{
	size_t i;
	size_t cur;
	size_t j;

	if (keys == NULL || vals == NULL || n == NULL) {
		return -1;
	}
	cur = *n;
	if (cur > cap) {
		return -1;
	}

	i = b178_lower_bound(keys, cur, key);
	if (i < cur && keys[i] == key) {
		vals[i] = val;
		return 0;
	}
	if (cur >= cap) {
		return -1;
	}

	/* Shift [i .. cur) one slot right to open insertion point. */
	for (j = cur; j > i; j--) {
		keys[j] = keys[j - 1];
		vals[j] = vals[j - 1];
	}
	keys[i] = key;
	vals[i] = val;
	*n = cur + 1;
	return 0;
}

int __gj_imap_insert(int *keys, int *vals, size_t *n, size_t cap, int key,
                     int val)
    __attribute__((alias("gj_imap_insert")));
