/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch328: fixed-capacity void* work queue over a
 * caller-owned dense array (no control-block struct).
 *
 * Surface (unique symbols for this TU's object file):
 *   int   gj_jobq_push(void **q, size_t cap, size_t *n, void *job);
 *     — Append job at q[*n] when *n < cap. Returns 0 on success, -1 if
 *       q/n is NULL, cap is 0, or the queue is full (*n >= cap).
 *       job may be NULL (stored as a valid payload).
 *   void *gj_jobq_pop(void **q, size_t *n);
 *     — Remove and return the head (FIFO: q[0]), compacting the remaining
 *       slots left by one. Returns NULL if q/n is NULL or *n == 0.
 *       Note: a previously pushed NULL payload is indistinguishable from
 *       empty; callers that store NULL should track *n externally.
 *   __gj_jobq_push / __gj_jobq_pop  (aliases)
 *   __libcgj_batch328_marker = "libcgj-batch328"
 *
 * Layout: caller supplies q[0 .. cap-1] and a live count *n in [0, cap].
 * Push grows the dense prefix; pop is FIFO from index 0 with O(n) shift.
 * Distinct API shape from batch175 (gj_wqueue_t ring with head/tail).
 * When both TUs are linked, resolve the shared gj_wq_* names at the
 * product level (map / weak / rename); this TU stands alone as specified.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno.
 * Single-threaded / external-sync only. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch328_marker[] = "libcgj-batch328";

/* ---- gj_jobq_push -------------------------------------------------------- */

/*
 * gj_jobq_push — enqueue job at the dense tail (index *n).
 *
 * q:   caller-owned slot array of length cap
 * cap: maximum number of jobs
 * n:   in/out live count; advanced by 1 on success
 * job: payload pointer (NULL allowed)
 *
 * Returns 0 on success, -1 on bad args or full queue.
 */
int
gj_jobq_push(void **q, size_t cap, size_t *n, void *job)
{
	size_t cLive;

	if (q == NULL || n == NULL || cap == 0u) {
		return -1;
	}
	cLive = *n;
	if (cLive >= cap) {
		return -1;
	}
	q[cLive] = job;
	*n = cLive + 1u;
	return 0;
}

int __gj_jobq_push(void **q, size_t cap, size_t *n, void *job)
    __attribute__((alias("gj_jobq_push")));

/* ---- gj_jobq_pop --------------------------------------------------------- */

/*
 * gj_jobq_pop — dequeue FIFO head at q[0], shift remaining jobs left.
 *
 * q: caller-owned slot array (length at least *n)
 * n: in/out live count; decremented by 1 on success
 *
 * Returns the removed job, or NULL if empty / bad args.
 */
void *
gj_jobq_pop(void **q, size_t *n)
{
	void *pJob;
	size_t cLive;
	size_t iSlot;

	if (q == NULL || n == NULL) {
		return NULL;
	}
	cLive = *n;
	if (cLive == 0u) {
		return NULL;
	}

	pJob = q[0];
	/* Compact dense prefix: move q[1..cLive) down to q[0..cLive-1). */
	for (iSlot = 1u; iSlot < cLive; iSlot++) {
		q[iSlot - 1u] = q[iSlot];
	}
	q[cLive - 1u] = NULL;
	*n = cLive - 1u;
	return pJob;
}

void *__gj_jobq_pop(void **q, size_t *n)
    __attribute__((alias("gj_jobq_pop")));
