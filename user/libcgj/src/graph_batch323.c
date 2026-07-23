/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch323: open-addressing string→u64 map on
 * caller-owned parallel arrays.
 *
 * Surface (unique symbols):
 *   int gj_shmap_put(const char **keys, uint64_t *vals, size_t cap,
 *                    const char *k, uint64_t v);
 *     — Upsert (k, v) into an open-addressed table of capacity cap.
 *       keys[i] == NULL marks an empty slot. Linear probe from
 *       hash(k) % cap. On match: overwrite vals[i] = v (pointer
 *       keys[i] left as-is). On first empty: store k and v.
 *       Content is not copied; caller must keep k live while the
 *       slot is occupied. Returns 0 on success; -1 on full table
 *       or bad args (NULL keys/vals/k, or cap == 0).
 *   int gj_shmap_get(const char *const *keys, const uint64_t *vals,
 *                    size_t cap, const char *k, uint64_t *out);
 *     — Lookup k. On hit write vals[i] to *out and return 0.
 *       On miss or bad args return -1 (*out untouched).
 *   __gj_shmap_put / __gj_shmap_get  (aliases)
 *   __libcgj_batch323_marker = "libcgj-batch323"
 *
 * Notes:
 *   - Empty key slot = NULL (no tombstones; no delete API).
 *   - Distinct from ordered int map (batch178) and linear intern
 *     table (batch177). Hash is local FNV-1a 64 (static) — does not
 *     redefine gj_fnv1a64 / fnv1a_64.
 *   - No errno (freestanding-safe); callers use return codes only.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno.
 * No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch323_marker[] = "libcgj-batch323";

/* FNV-1a 64-bit parameters (public domain constants; local only). */
#define B323_FNV64_OFFSET 14695981039346656037ULL
#define B323_FNV64_PRIME  1099511628211ULL

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * b323_streq — NUL-terminated string equality. Returns 1 if equal, 0 else.
 * Both pointers must be non-NULL.
 */
static int
b323_streq(const char *szA, const char *szB)
{
	size_t i;

	for (i = 0u;; i++) {
		if (szA[i] != szB[i]) {
			return 0;
		}
		if (szA[i] == '\0') {
			return 1;
		}
	}
}

/*
 * b323_hash — FNV-1a 64 over a NUL-terminated C string.
 * Empty string yields the offset basis. sz must be non-NULL.
 */
static uint64_t
b323_hash(const char *sz)
{
	const unsigned char *p = (const unsigned char *)sz;
	uint64_t h = B323_FNV64_OFFSET;

	while (*p != 0u) {
		h ^= (uint64_t)*p;
		h *= B323_FNV64_PRIME;
		p++;
	}
	return h;
}

/*
 * b323_probe_start — first slot index for key k in a table of capacity cap.
 * cap must be non-zero.
 */
static size_t
b323_probe_start(const char *szK, size_t cCap)
{
	return (size_t)(b323_hash(szK) % (uint64_t)cCap);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shmap_put — insert or update (k, v) in open-addressed parallel arrays.
 *
 * keys:  capacity-cap array of string pointers; NULL = empty slot
 * vals:  parallel values
 * cap:   table capacity (must be > 0)
 * k:     key string (stored by pointer on insert; not copied)
 * v:     value to associate
 *
 * Returns 0 on success (insert or update), -1 if full or bad args.
 */
int
gj_shmap_put(const char **keys, uint64_t *vals, size_t cap, const char *k,
             uint64_t v)
{
	size_t iStart;
	size_t i;

	if (keys == NULL || vals == NULL || k == NULL || cap == 0u) {
		return -1;
	}

	iStart = b323_probe_start(k, cap);
	i = iStart;
	do {
		if (keys[i] == NULL) {
			/* Empty slot: claim it. */
			keys[i] = k;
			vals[i] = v;
			return 0;
		}
		if (b323_streq(keys[i], k) != 0) {
			/* Match: update value only (keep existing key ptr). */
			vals[i] = v;
			return 0;
		}
		i++;
		if (i == cap) {
			i = 0u;
		}
	} while (i != iStart);

	/* Full table, no match and no empty slot. */
	return -1;
}

/*
 * gj_shmap_get — look up k in open-addressed parallel arrays.
 *
 * On hit write the paired value to *out and return 0.
 * On miss or bad args return -1 (*out untouched).
 */
int
gj_shmap_get(const char *const *keys, const uint64_t *vals, size_t cap,
             const char *k, uint64_t *out)
{
	size_t iStart;
	size_t i;

	if (keys == NULL || vals == NULL || k == NULL || out == NULL ||
	    cap == 0u) {
		return -1;
	}

	iStart = b323_probe_start(k, cap);
	i = iStart;
	do {
		if (keys[i] == NULL) {
			/* Empty slot ends the probe chain (no tombstones). */
			return -1;
		}
		if (b323_streq(keys[i], k) != 0) {
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

int __gj_shmap_put(const char **keys, uint64_t *vals, size_t cap, const char *k,
                   uint64_t v) __attribute__((alias("gj_shmap_put")));

int __gj_shmap_get(const char *const *keys, const uint64_t *vals, size_t cap,
                   const char *k, uint64_t *out)
    __attribute__((alias("gj_shmap_get")));
