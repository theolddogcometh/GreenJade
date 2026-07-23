/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch185: lock-free SPSC uint64_t ring (power-of-2 cap).
 *
 * Single-producer / single-consumer ring over a caller-owned uint64_t[] buffer.
 * Capacity must be a power of two so indexing is a cheap mask (index & mask)
 * rather than a modulo. Free-running size_t cursors (r = consumer, w = producer);
 * occupancy is (w - r) and the ring is full when (w - r) == (mask + 1).
 *
 * Memory ordering (SPSC-safe, no locks):
 *   producer: load r ACQUIRE, write payload, store w RELEASE
 *   consumer: load w ACQUIRE, read payload, store r RELEASE
 *
 * Existing surface NOT redefined (avoid multi-def):
 *   gj_ring_*  → graph_batch117.c  (byte ring, any cap)
 *   gj_wq_*    → graph_batch175.c  (void* work queue)
 *
 * This TU adds only unique symbols:
 *   typedef struct {
 *       uint64_t *buf;
 *       size_t mask, r, w;
 *   } gj_u64ring_t;
 *   int gj_u64ring_init(gj_u64ring_t *ring, uint64_t *buf, size_t cap);
 *     — bind ring to buf[0..cap-1]; cap must be power-of-2 and > 0.
 *       Sets mask = cap - 1, r = w = 0. Returns 0 / -1.
 *   int gj_u64ring_push(gj_u64ring_t *ring, uint64_t v);
 *     — producer: enqueue v. Returns 0 on success, -1 if full/NULL.
 *   int gj_u64ring_pop(gj_u64ring_t *ring, uint64_t *out);
 *     — consumer: dequeue into *out. Returns 0 on success, -1 if empty/NULL.
 *   __gj_u64ring_init / __gj_u64ring_push / __gj_u64ring_pop  (aliases)
 *   __libcgj_batch185_marker = "libcgj-batch185"
 *
 * Clean-room freestanding pure C (integer/pointer + GCC __atomic_*).
 * Compiles with -ffreestanding -msse2. No third-party ring source copied.
 * No malloc, no errno (freestanding-safe).
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch185_marker[] = "libcgj-batch185";

/* ---- public type ------------------------------------------------------- */

typedef struct {
	uint64_t *buf;
	size_t mask;
	size_t r;
	size_t w;
} gj_u64ring_t;

/* ---- helpers ----------------------------------------------------------- */

/*
 * Return non-zero if n is a power of two and non-zero.
 * Uses the classic (n & (n - 1)) == 0 test.
 */
static int
b185_is_pow2(size_t n)
{
	return n != 0u && (n & (n - 1u)) == 0u;
}

/* ---- gj_u64ring_init --------------------------------------------------- */

/*
 * Bind ring to caller-owned buf[0 .. cap-1]. cap must be a power of two
 * and greater than zero. mask is stored as (cap - 1). Cursors start at 0.
 *
 * Returns 0 on success, -1 if ring is NULL, buf is NULL, or cap is not a
 * positive power of two.
 */
int
gj_u64ring_init(gj_u64ring_t *ring, uint64_t *buf, size_t cap)
{
	if (ring == NULL || buf == NULL || !b185_is_pow2(cap)) {
		return -1;
	}

	ring->buf = buf;
	ring->mask = cap - 1u;
	/*
	 * Publish empty indices with release so a concurrent consumer on
	 * another core cannot observe a stale non-zero cursor against a
	 * freshly assigned buffer.
	 */
	__atomic_store_n(&ring->r, (size_t)0u, __ATOMIC_RELAXED);
	__atomic_store_n(&ring->w, (size_t)0u, __ATOMIC_RELEASE);
	return 0;
}

int __gj_u64ring_init(gj_u64ring_t *ring, uint64_t *buf, size_t cap)
    __attribute__((alias("gj_u64ring_init")));

/* ---- gj_u64ring_push --------------------------------------------------- */

/*
 * Producer: enqueue a single uint64_t. Only one producer may call this.
 * Returns 0 on success, -1 if ring is NULL/unbound or the ring is full.
 */
int
gj_u64ring_push(gj_u64ring_t *ring, uint64_t v)
{
	size_t uW;
	size_t uR;
	size_t uCap;
	size_t uUsed;

	if (ring == NULL || ring->buf == NULL) {
		return -1;
	}

	/* cap == mask + 1; mask is always cap-1 for a valid init. */
	uCap = ring->mask + 1u;
	if (uCap == 0u) {
		/* mask overflow (cap was SIZE_MAX, not a practical pow2 ring). */
		return -1;
	}

	uR = __atomic_load_n(&ring->r, __ATOMIC_ACQUIRE);
	uW = __atomic_load_n(&ring->w, __ATOMIC_RELAXED);
	uUsed = uW - uR;
	if (uUsed >= uCap) {
		return -1;
	}

	ring->buf[uW & ring->mask] = v;

	/* Release: payload store becomes visible before the new w. */
	__atomic_store_n(&ring->w, uW + 1u, __ATOMIC_RELEASE);
	return 0;
}

int __gj_u64ring_push(gj_u64ring_t *ring, uint64_t v)
    __attribute__((alias("gj_u64ring_push")));

/* ---- gj_u64ring_pop ---------------------------------------------------- */

/*
 * Consumer: dequeue one uint64_t into *out. Only one consumer may call this.
 * Returns 0 on success (writes *out), -1 if ring/out is NULL or the ring
 * is empty. *out is left untouched on failure.
 */
int
gj_u64ring_pop(gj_u64ring_t *ring, uint64_t *out)
{
	size_t uW;
	size_t uR;
	size_t uHave;

	if (ring == NULL || out == NULL || ring->buf == NULL) {
		return -1;
	}

	uW = __atomic_load_n(&ring->w, __ATOMIC_ACQUIRE);
	uR = __atomic_load_n(&ring->r, __ATOMIC_RELAXED);
	uHave = uW - uR;
	if (uHave == 0u) {
		return -1;
	}

	*out = ring->buf[uR & ring->mask];

	/* Release: finished consuming before advancing r (frees a slot). */
	__atomic_store_n(&ring->r, uR + 1u, __ATOMIC_RELEASE);
	return 0;
}

int __gj_u64ring_pop(gj_u64ring_t *ring, uint64_t *out)
    __attribute__((alias("gj_u64ring_pop")));
