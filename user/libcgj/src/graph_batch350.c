/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch350: tiny cuckoo filter on a caller-owned
 * table of uint8 fingerprints.
 *
 * Surface (unique symbols):
 *   int gj_cuckoo_insert(uint8_t *table, size_t buckets, size_t bucket_size,
 *                        const void *key, size_t n);
 *     — Insert key[0..n) into the filter. table is a flat array of
 *       buckets * bucket_size fingerprint bytes (caller zero-inits empty
 *       slots to 0). Returns 0 on success (including already-present
 *       fingerprint in either candidate bucket); -1 on bad args or when
 *       relocation fails after B350_MAX_KICKS kicks (table full / stuck).
 *   int gj_cuckoo_may(const uint8_t *table, size_t buckets,
 *                     size_t bucket_size, const void *key, size_t n);
 *     — Probabilistic membership. Returns 1 if the fingerprint may be
 *       present (false positives possible via fingerprint collision), 0 if
 *       definitely absent or on bad args. Successfully inserted keys always
 *       probe true (no false negatives) when storage was zero-inited and
 *       only mutated by this insert path (failed inserts roll back).
 *   __gj_cuckoo_insert / __gj_cuckoo_may  (aliases)
 *   __libcgj_batch350_marker = "libcgj-batch350"
 *
 * Layout (caller provides):
 *   uint8_t table[buckets * bucket_size];
 *   Slot value 0 is empty. Fingerprints are forced non-zero.
 *   Candidate buckets for fingerprint fp of key:
 *     i1 = hash(key) % buckets
 *     i2 = alt(i1, fp)   with alt(alt(i, fp), fp) == i
 *   Bucket b occupies table[b * bucket_size .. (b+1)*bucket_size).
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party cuckoo/Bloom source copied. Distinct from the
 * bit-array Bloom filter in graph_batch125.c.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch350_marker[] = "libcgj-batch350";

/* Relocation budget (deterministic kick sequence; no RNG). */
#define B350_MAX_KICKS 512u

/* FNV-1a 64 bases (static to this TU — not the batch41 exports). */
#define B350_FNV_OFF1  0xcbf29ce484222325ULL
#define B350_FNV_OFF2  0x84222325cbf29ce4ULL
#define B350_FNV_PRIME 0x100000001b3ULL

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b350_fnv1a64(uint64_t u64Off, const void *pKey, size_t cb)
{
	const uint8_t *p;
	uint64_t u64H;
	size_t i;

	u64H = u64Off;
	if (pKey == NULL || cb == 0u) {
		return u64H;
	}
	p = (const uint8_t *)pKey;
	for (i = 0u; i < cb; i++) {
		u64H ^= (uint64_t)p[i];
		u64H *= B350_FNV_PRIME;
	}
	return u64H;
}

/*
 * Non-zero 8-bit fingerprint. Slot 0 is reserved empty, so map a zero
 * digest fold to 1.
 */
static uint8_t
b350_fingerprint(const void *pKey, size_t cb)
{
	uint64_t u64H;
	uint8_t u8Fp;

	u64H = b350_fnv1a64(B350_FNV_OFF1, pKey, cb);
	u8Fp = (uint8_t)(u64H ^ (u64H >> 8) ^ (u64H >> 16) ^ (u64H >> 24) ^
	    (u64H >> 32) ^ (u64H >> 40) ^ (u64H >> 48) ^ (u64H >> 56));
	if (u8Fp == 0u) {
		u8Fp = 1u;
	}
	return u8Fp;
}

/* Primary bucket index in [0, cBuckets). */
static size_t
b350_bucket1(const void *pKey, size_t cb, size_t cBuckets)
{
	uint64_t u64H;

	u64H = b350_fnv1a64(B350_FNV_OFF2 ^ 0x9e3779b97f4a7c15ULL, pKey, cb);
	return (size_t)(u64H % (uint64_t)cBuckets);
}

/*
 * Alternate bucket. Symmetric for any cBuckets >= 1:
 *   alt(i, fp) = (H(fp) - i) mod cBuckets
 * so alt(alt(i, fp), fp) == i.
 */
static size_t
b350_alt(size_t iBucket, uint8_t u8Fp, size_t cBuckets)
{
	uint64_t u64H;
	uint64_t u64B;
	uint64_t u64I;

	/* Mix the single-byte fingerprint into a 64-bit hash. */
	u64H = (uint64_t)u8Fp * 0x9e3779b97f4a7c15ULL;
	u64H ^= u64H >> 33;
	u64H *= 0xff51afd7ed558ccdULL;
	u64H ^= u64H >> 33;

	u64B = (uint64_t)cBuckets;
	u64I = (uint64_t)iBucket % u64B;
	return (size_t)(((u64H % u64B) + u64B - u64I) % u64B);
}

/* True if u8Fp occupies any slot of bucket iBucket. */
static int
b350_bucket_has(const uint8_t *pTab, size_t cSlot, size_t iBucket,
    uint8_t u8Fp)
{
	const uint8_t *pSlot;
	size_t i;

	pSlot = pTab + iBucket * cSlot;
	for (i = 0u; i < cSlot; i++) {
		if (pSlot[i] == u8Fp) {
			return 1;
		}
	}
	return 0;
}

/* Place u8Fp in the first empty slot of bucket iBucket; 1 if placed. */
static int
b350_bucket_put(uint8_t *pTab, size_t cSlot, size_t iBucket, uint8_t u8Fp)
{
	uint8_t *pSlot;
	size_t i;

	pSlot = pTab + iBucket * cSlot;
	for (i = 0u; i < cSlot; i++) {
		if (pSlot[i] == 0u) {
			pSlot[i] = u8Fp;
			return 1;
		}
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cuckoo_insert — insert fingerprint of key into caller table.
 *
 * table:       flat fingerprint store, length buckets * bucket_size
 * buckets:     number of buckets (>0)
 * bucket_size: slots per bucket (>0); each slot is one uint8_t
 * key / n:     key bytes (key may be NULL only when n == 0)
 *
 * Returns 0 on success, -1 on invalid args or failed relocation.
 */
int
gj_cuckoo_insert(uint8_t *table, size_t buckets, size_t bucket_size,
    const void *key, size_t n)
{
	uint8_t u8Fp;
	uint8_t u8Kick;
	size_t i1;
	size_t i2;
	size_t iCur;
	size_t iKick;
	size_t iSlot;
	uint8_t *pSlot;

	if (table == NULL || buckets == 0u || bucket_size == 0u) {
		return -1;
	}
	if (key == NULL && n != 0u) {
		return -1;
	}

	u8Fp = b350_fingerprint(key, n);
	i1 = b350_bucket1(key, n, buckets);
	i2 = b350_alt(i1, u8Fp, buckets);

	/* Already present in either candidate bucket → success. */
	if (b350_bucket_has(table, bucket_size, i1, u8Fp) ||
	    b350_bucket_has(table, bucket_size, i2, u8Fp)) {
		return 0;
	}

	/* Prefer an empty slot in i1, then i2. */
	if (b350_bucket_put(table, bucket_size, i1, u8Fp)) {
		return 0;
	}
	if (b350_bucket_put(table, bucket_size, i2, u8Fp)) {
		return 0;
	}

	/*
	 * Both buckets full: cuckoo relocation. Deterministic victim:
	 * slot (kick % bucket_size) of the current bucket. Start at i1.
	 * Record each swap so a failed insert can roll back and leave the
	 * table unchanged (preserves no-false-negative for prior keys).
	 */
	{
		size_t aPathBucket[B350_MAX_KICKS];
		size_t aPathSlot[B350_MAX_KICKS];
		uint8_t aPathOld[B350_MAX_KICKS];
		size_t cPath;

		iCur = i1;
		u8Kick = u8Fp;
		cPath = 0u;

		for (iKick = 0u; iKick < (size_t)B350_MAX_KICKS; iKick++) {
			iSlot = iKick % bucket_size;
			pSlot = table + iCur * bucket_size + iSlot;

			aPathBucket[cPath] = iCur;
			aPathSlot[cPath] = iSlot;
			aPathOld[cPath] = *pSlot;
			cPath++;

			/* Swap victim with the fingerprint we must place. */
			{
				uint8_t u8Tmp = *pSlot;

				*pSlot = u8Kick;
				u8Kick = u8Tmp;
			}

			/* Empty victim slot (should be rare if we reached kicks). */
			if (u8Kick == 0u) {
				return 0;
			}

			iCur = b350_alt(iCur, u8Kick, buckets);
			if (b350_bucket_put(table, bucket_size, iCur, u8Kick)) {
				return 0;
			}
			/* Bucket full again — continue kicking from iCur. */
		}

		/* Roll back swaps in reverse order; insert fails cleanly. */
		while (cPath > 0u) {
			cPath--;
			pSlot = table + aPathBucket[cPath] * bucket_size +
			    aPathSlot[cPath];
			*pSlot = aPathOld[cPath];
		}
	}

	return -1;
}

/*
 * gj_cuckoo_may — possible membership of key in the filter.
 *
 * Returns 1 if fingerprint is found in either candidate bucket, else 0.
 * Bad arguments return 0 (definite non-member / unusable table).
 */
int
gj_cuckoo_may(const uint8_t *table, size_t buckets, size_t bucket_size,
    const void *key, size_t n)
{
	uint8_t u8Fp;
	size_t i1;
	size_t i2;

	if (table == NULL || buckets == 0u || bucket_size == 0u) {
		return 0;
	}
	if (key == NULL && n != 0u) {
		return 0;
	}

	u8Fp = b350_fingerprint(key, n);
	i1 = b350_bucket1(key, n, buckets);
	i2 = b350_alt(i1, u8Fp, buckets);

	if (b350_bucket_has(table, bucket_size, i1, u8Fp)) {
		return 1;
	}
	if (b350_bucket_has(table, bucket_size, i2, u8Fp)) {
		return 1;
	}
	return 0;
}

int __gj_cuckoo_insert(uint8_t *table, size_t buckets, size_t bucket_size,
    const void *key, size_t n)
    __attribute__((alias("gj_cuckoo_insert")));

int __gj_cuckoo_may(const uint8_t *table, size_t buckets, size_t bucket_size,
    const void *key, size_t n)
    __attribute__((alias("gj_cuckoo_may")));
