/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch117: lock-free SPSC byte ring helpers.
 *
 * Single-producer / single-consumer ring over a caller-owned buffer.
 * Free-running size_t indices (r = consumer, w = producer); occupancy is
 * (w - r) and free space is (cap - (w - r)). Indexing into the buffer uses
 * modular wrap (works for any cap > 0, not only power-of-two).
 *
 * Memory ordering (SPSC-safe, no locks):
 *   producer: load r ACQUIRE, write payload, store w RELEASE
 *   consumer: load w ACQUIRE, read payload, store r RELEASE
 *
 * Surface (unique — no multi-def with existing symbols):
 *   typedef struct { unsigned char *buf; size_t cap; size_t r; size_t w; }
 *       gj_ring_t;
 *   void   gj_ring_init(gj_ring_t *r, unsigned char *buf, size_t cap);
 *   size_t gj_ring_write(gj_ring_t *r, const void *data, size_t n);
 *   size_t gj_ring_read(gj_ring_t *r, void *data, size_t n);
 *   size_t gj_ring_avail(const gj_ring_t *r);
 *   size_t gj_ring_space(const gj_ring_t *r);
 *   __libcgj_batch117_marker = "libcgj-batch117"
 *
 * Clean-room freestanding pure C (integer/pointer + GCC __atomic_*).
 * Compiles with -ffreestanding -msse2. No third-party ring source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch117_marker[] = "libcgj-batch117";

/* ---- public type ------------------------------------------------------- */

typedef struct {
	unsigned char *buf;
	size_t cap;
	size_t r;
	size_t w;
} gj_ring_t;

/* ---- freestanding helpers ---------------------------------------------- */

static void
b117_copy(unsigned char *pDst, const unsigned char *pSrc, size_t cb)
{
	size_t i;

	for (i = 0u; i < cb; i++) {
		pDst[i] = pSrc[i];
	}
}

/*
 * Copy cb bytes from linear pSrc into ring pBuf of capacity cbCap starting
 * at free-running write index uW (modulo wrap).
 */
static void
b117_put(unsigned char *pBuf, size_t cbCap, size_t uW,
         const unsigned char *pSrc, size_t cb)
{
	size_t iOff;
	size_t cbFirst;

	if (cb == 0u || cbCap == 0u) {
		return;
	}
	iOff = uW % cbCap;
	cbFirst = cbCap - iOff;
	if (cbFirst > cb) {
		cbFirst = cb;
	}
	b117_copy(pBuf + iOff, pSrc, cbFirst);
	if (cb > cbFirst) {
		b117_copy(pBuf, pSrc + cbFirst, cb - cbFirst);
	}
}

/*
 * Copy cb bytes from ring pBuf of capacity cbCap starting at free-running
 * read index uR into linear pDst (modulo wrap).
 */
static void
b117_get(unsigned char *pDst, const unsigned char *pBuf, size_t cbCap,
         size_t uR, size_t cb)
{
	size_t iOff;
	size_t cbFirst;

	if (cb == 0u || cbCap == 0u) {
		return;
	}
	iOff = uR % cbCap;
	cbFirst = cbCap - iOff;
	if (cbFirst > cb) {
		cbFirst = cb;
	}
	b117_copy(pDst, pBuf + iOff, cbFirst);
	if (cb > cbFirst) {
		b117_copy(pDst + cbFirst, pBuf, cb - cbFirst);
	}
}

/* ---- public API -------------------------------------------------------- */

void
gj_ring_init(gj_ring_t *pRing, unsigned char *pBuf, size_t cbCap)
{
	if (pRing == NULL) {
		return;
	}
	pRing->buf = pBuf;
	pRing->cap = cbCap;
	/*
	 * Publish empty indices with release so a concurrent consumer on
	 * another core cannot observe a stale non-zero cursor against a
	 * freshly assigned buffer.
	 */
	__atomic_store_n(&pRing->r, (size_t)0u, __ATOMIC_RELAXED);
	__atomic_store_n(&pRing->w, (size_t)0u, __ATOMIC_RELEASE);
}

/*
 * Bytes currently readable (producer-published payload not yet consumed).
 * Safe for either endpoint; pure observation.
 */
size_t
gj_ring_avail(const gj_ring_t *pRing)
{
	size_t uW;
	size_t uR;

	if (pRing == NULL) {
		return 0u;
	}
	/* Consumer-side view: see producer's w before reading payload. */
	uW = __atomic_load_n((size_t *)&pRing->w, __ATOMIC_ACQUIRE);
	uR = __atomic_load_n((size_t *)&pRing->r, __ATOMIC_RELAXED);
	return uW - uR;
}

/*
 * Free bytes writable (capacity minus occupancy). Safe for either endpoint.
 */
size_t
gj_ring_space(const gj_ring_t *pRing)
{
	size_t uW;
	size_t uR;
	size_t cbUsed;

	if (pRing == NULL) {
		return 0u;
	}
	/* Producer-side view: see consumer's r before reclaiming slots. */
	uR = __atomic_load_n((size_t *)&pRing->r, __ATOMIC_ACQUIRE);
	uW = __atomic_load_n((size_t *)&pRing->w, __ATOMIC_RELAXED);
	cbUsed = uW - uR;
	if (cbUsed > pRing->cap) {
		/* Corrupt cursors — report no space rather than wrap trash. */
		return 0u;
	}
	return pRing->cap - cbUsed;
}

/*
 * Producer: copy up to n bytes from data into the ring. Returns bytes written
 * (0 if full, NULL args, or zero capacity). Only one producer may call this.
 */
size_t
gj_ring_write(gj_ring_t *pRing, const void *pData, size_t cb)
{
	const unsigned char *pSrc;
	size_t uW;
	size_t uR;
	size_t cbUsed;
	size_t cbSpace;
	size_t cbPut;

	if (pRing == NULL || pData == NULL || cb == 0u) {
		return 0u;
	}
	if (pRing->buf == NULL || pRing->cap == 0u) {
		return 0u;
	}

	uR = __atomic_load_n(&pRing->r, __ATOMIC_ACQUIRE);
	uW = __atomic_load_n(&pRing->w, __ATOMIC_RELAXED);
	cbUsed = uW - uR;
	if (cbUsed >= pRing->cap) {
		return 0u;
	}
	cbSpace = pRing->cap - cbUsed;
	cbPut = cb;
	if (cbPut > cbSpace) {
		cbPut = cbSpace;
	}

	pSrc = (const unsigned char *)pData;
	b117_put(pRing->buf, pRing->cap, uW, pSrc, cbPut);

	/* Release: payload stores become visible before the new w. */
	__atomic_store_n(&pRing->w, uW + cbPut, __ATOMIC_RELEASE);
	return cbPut;
}

/*
 * Consumer: copy up to n bytes from the ring into data. Returns bytes read
 * (0 if empty, NULL args, or zero capacity). Only one consumer may call this.
 */
size_t
gj_ring_read(gj_ring_t *pRing, void *pData, size_t cb)
{
	unsigned char *pDst;
	size_t uW;
	size_t uR;
	size_t cbHave;
	size_t cbGet;

	if (pRing == NULL || pData == NULL || cb == 0u) {
		return 0u;
	}
	if (pRing->buf == NULL || pRing->cap == 0u) {
		return 0u;
	}

	uW = __atomic_load_n(&pRing->w, __ATOMIC_ACQUIRE);
	uR = __atomic_load_n(&pRing->r, __ATOMIC_RELAXED);
	cbHave = uW - uR;
	if (cbHave == 0u) {
		return 0u;
	}
	if (cbHave > pRing->cap) {
		/* Corrupt cursors — refuse to read. */
		return 0u;
	}
	cbGet = cb;
	if (cbGet > cbHave) {
		cbGet = cbHave;
	}

	pDst = (unsigned char *)pData;
	b117_get(pDst, pRing->buf, pRing->cap, uR, cbGet);

	/* Release: finished consuming before advancing r (frees space). */
	__atomic_store_n(&pRing->r, uR + cbGet, __ATOMIC_RELEASE);
	return cbGet;
}
