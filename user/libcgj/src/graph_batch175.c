/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch175: fixed-slot void* work queue (ring buffer).
 * Clean-room freestanding pure C — integer/pointer only (no SSE required;
 * builds under userspace -msse2). No third-party queue source was copied.
 *
 * This TU adds only unique symbols:
 *   typedef struct {
 *       void **slots;
 *       size_t cap, head, tail, count;
 *   } gj_wqueue_t;
 *   int   gj_wq_init(gj_wqueue_t *q, void **slots, size_t cap);
 *   int   gj_wq_push(gj_wqueue_t *q, void *item);
 *   void *gj_wq_pop(gj_wqueue_t *q);
 *   __gj_wq_init / __gj_wq_push / __gj_wq_pop  (aliases)
 *   __libcgj_batch175_marker = "libcgj-batch175"
 *
 * Semantics:
 *   - Caller supplies the slots[] storage; the queue does not allocate.
 *   - FIFO ring: push at tail, pop at head; indices wrap with % cap.
 *   - Full when count == cap; empty when count == 0.
 *   - No errno (freestanding-safe); callers use return codes / NULL only.
 *   - Single-threaded / external-sync only (no atomics in this TU).
 */

#include <stddef.h>

const char __libcgj_batch175_marker[] = "libcgj-batch175";

/* Fixed-capacity work queue over caller-provided void* slots. */
typedef struct {
	void **slots;
	size_t cap;
	size_t head;
	size_t tail;
	size_t count;
} gj_wqueue_t;

/* ---- gj_wq_init -------------------------------------------------------- */

/*
 * Bind queue q to slots[0..cap-1]. Resets head/tail/count to 0.
 * Returns 0 on success, -1 if q is NULL, slots is NULL with cap > 0,
 * or cap is 0.
 */
int
gj_wq_init(gj_wqueue_t *q, void **slots, size_t cap)
{
	if (q == NULL || cap == 0 || slots == NULL) {
		return -1;
	}
	q->slots = slots;
	q->cap = cap;
	q->head = 0;
	q->tail = 0;
	q->count = 0;
	return 0;
}

int __gj_wq_init(gj_wqueue_t *q, void **slots, size_t cap)
    __attribute__((alias("gj_wq_init")));

/* ---- gj_wq_push -------------------------------------------------------- */

/*
 * Enqueue item at the tail. Returns 0 on success, -1 if q is NULL or full.
 * item may be NULL (stored as a valid payload).
 */
int
gj_wq_push(gj_wqueue_t *q, void *item)
{
	if (q == NULL || q->slots == NULL || q->cap == 0) {
		return -1;
	}
	if (q->count >= q->cap) {
		return -1;
	}
	q->slots[q->tail] = item;
	q->tail = q->tail + 1;
	if (q->tail >= q->cap) {
		q->tail = 0;
	}
	q->count = q->count + 1;
	return 0;
}

int __gj_wq_push(gj_wqueue_t *q, void *item)
    __attribute__((alias("gj_wq_push")));

/* ---- gj_wq_pop --------------------------------------------------------- */

/*
 * Dequeue from the head. Returns the item, or NULL if q is NULL/empty.
 * Note: a previously pushed NULL payload is indistinguishable from empty;
 * callers that store NULL should track count externally if needed.
 */
void *
gj_wq_pop(gj_wqueue_t *q)
{
	void *item;

	if (q == NULL || q->slots == NULL || q->cap == 0 || q->count == 0) {
		return NULL;
	}
	item = q->slots[q->head];
	q->slots[q->head] = NULL;
	q->head = q->head + 1;
	if (q->head >= q->cap) {
		q->head = 0;
	}
	q->count = q->count - 1;
	return item;
}

void *__gj_wq_pop(gj_wqueue_t *q)
    __attribute__((alias("gj_wq_pop")));
