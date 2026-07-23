/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch437: freestanding uint32 ring-queue helpers
 * on caller storage (ring-buffer style, unique names vs batch281 u8 ring).
 *
 * Head/tail/count live in caller-owned size_t slots (no struct, no heap).
 * Buffer is also caller-owned. Single-threaded / cooperative use (no atomics).
 *
 * Surface (unique symbols):
 *   void gj_queue_u32_init(size_t *h, size_t *t, size_t *n);
 *     — Zero head, tail, and count. NULL any pointer → no-op.
 *   int  gj_queue_u32_push(uint32_t *buf, size_t cap, size_t *h, size_t *t,
 *                          size_t *n, uint32_t v);
 *     — Enqueue one value at head. Returns 0 on success, -1 if full or bad.
 *   int  gj_queue_u32_pop(uint32_t *buf, size_t cap, size_t *h, size_t *t,
 *                         size_t *n, uint32_t *out);
 *     — Dequeue one value from tail into *out. Returns 0 on success, -1 if
 *       empty or bad args.
 *   __gj_queue_u32_init / __gj_queue_u32_push / __gj_queue_u32_pop  (aliases)
 *   __libcgj_batch437_marker = "libcgj-batch437"
 *
 * Semantics:
 *   h / *h — next write index (mod cap)
 *   t / *t — next read index (mod cap)
 *   n / *n — elements currently stored (0..cap)
 *   full when *n == cap; empty when *n == 0.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch437_marker[] = "libcgj-batch437";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_queue_u32_init — reset queue cursors on caller storage.
 * Leaves buf contents untouched (caller may reuse or zero separately).
 */
void
gj_queue_u32_init(size_t *pH, size_t *pT, size_t *pN)
{
	if (pH == NULL || pT == NULL || pN == NULL) {
		return;
	}
	*pH = 0u;
	*pT = 0u;
	*pN = 0u;
}

/*
 * gj_queue_u32_push — enqueue one uint32_t.
 *
 * Advances head modulo cap and increments count. Does not touch tail.
 * Returns 0 on success; -1 if full, zero capacity, or NULL required args.
 * (t is accepted for API symmetry with pop; unused here.)
 */
int
gj_queue_u32_push(uint32_t *pBuf, size_t cCap, size_t *pH, size_t *pT,
                  size_t *pN, uint32_t u32V)
{
	size_t iH;

	(void)pT;

	if (pBuf == NULL || pH == NULL || pN == NULL || cCap == 0u) {
		return -1;
	}
	if (*pN >= cCap) {
		return -1;
	}

	iH = *pH;
	if (iH >= cCap) {
		/* Corrupt cursor — refuse rather than write out of bounds. */
		return -1;
	}
	pBuf[iH] = u32V;
	iH++;
	if (iH >= cCap) {
		iH = 0u;
	}
	*pH = iH;
	(*pN)++;
	return 0;
}

/*
 * gj_queue_u32_pop — dequeue one uint32_t into *out.
 *
 * Advances tail modulo cap and decrements count. Does not touch head.
 * Returns 0 on success; -1 if empty, zero capacity, or NULL required args.
 * (h is accepted for API symmetry with push; unused here.)
 */
int
gj_queue_u32_pop(uint32_t *pBuf, size_t cCap, size_t *pH, size_t *pT,
                 size_t *pN, uint32_t *pOut)
{
	size_t iT;

	(void)pH;

	if (pBuf == NULL || pT == NULL || pN == NULL || pOut == NULL ||
	    cCap == 0u) {
		return -1;
	}
	if (*pN == 0u) {
		return -1;
	}

	iT = *pT;
	if (iT >= cCap) {
		/* Corrupt cursor — refuse rather than read out of bounds. */
		return -1;
	}
	*pOut = pBuf[iT];
	iT++;
	if (iT >= cCap) {
		iT = 0u;
	}
	*pT = iT;
	(*pN)--;
	return 0;
}

/* ---- underscored aliases ----------------------------------------------- */

void __gj_queue_u32_init(size_t *pH, size_t *pT, size_t *pN)
    __attribute__((alias("gj_queue_u32_init")));
int __gj_queue_u32_push(uint32_t *pBuf, size_t cCap, size_t *pH, size_t *pT,
                        size_t *pN, uint32_t u32V)
    __attribute__((alias("gj_queue_u32_push")));
int __gj_queue_u32_pop(uint32_t *pBuf, size_t cCap, size_t *pH, size_t *pT,
                       size_t *pN, uint32_t *pOut)
    __attribute__((alias("gj_queue_u32_pop")));
