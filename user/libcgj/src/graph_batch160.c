/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch160: userspace counting semaphore helpers.
 * Clean-room freestanding pure C — integer/pointer only (no SSE required;
 * builds under userspace -msse2). No third-party semaphore source was copied.
 *
 * Existing surface NOT redefined (avoid multi-def / no kernel futex path):
 *   sem_init / sem_trywait / sem_post / sem_getvalue / sem_wait  → semaphore.c
 *   (POSIX sem_t uses futex syscalls; this TU is pure userspace CAS.)
 *
 * This TU adds only unique symbols:
 *   typedef struct { volatile int count; } gj_sem_t;
 *   int gj_sem_init(gj_sem_t *s, int value);
 *     — set count = value (value must be >= 0). Returns 0 / -1.
 *   int gj_sem_trywait(gj_sem_t *s);
 *     — non-blocking: if count > 0, decrement and return 0; else -1.
 *   int gj_sem_post(gj_sem_t *s);
 *     — increment count (saturates at INT_MAX → -1). Returns 0 / -1.
 *   int gj_sem_getvalue(gj_sem_t *s, int *val);
 *     — *val = current count. Returns 0 / -1.
 *   __gj_sem_init / __gj_sem_trywait / __gj_sem_post / __gj_sem_getvalue
 *   __libcgj_batch160_marker = "libcgj-batch160"
 *
 * Notes:
 *   - Process-private only; no futex, no spin-wait blocking API.
 *   - Atomic via __sync_bool_compare_and_swap / __sync_fetch_and_add.
 *   - No errno (freestanding-safe); callers use return codes only.
 */

#include <stddef.h>

const char __libcgj_batch160_marker[] = "libcgj-batch160";

/* Userspace counting semaphore (volatile count; CAS for trywait/post). */
typedef struct {
	volatile int count;
} gj_sem_t;

/* ---- gj_sem_init ------------------------------------------------------- */

/*
 * Initialise a counting semaphore to nValue (>= 0).
 * Returns 0 on success, -1 on NULL or negative value.
 */
int
gj_sem_init(gj_sem_t *pS, int nValue)
{
	if (pS == NULL || nValue < 0) {
		return -1;
	}
	pS->count = nValue;
	return 0;
}

int __gj_sem_init(gj_sem_t *pS, int nValue)
    __attribute__((alias("gj_sem_init")));

/* ---- gj_sem_trywait ---------------------------------------------------- */

/*
 * Non-blocking P: if count > 0, atomically decrement and return 0.
 * If count is already 0 (or negative), return -1 without blocking.
 */
int
gj_sem_trywait(gj_sem_t *pS)
{
	int nCur;

	if (pS == NULL) {
		return -1;
	}
	for (;;) {
		nCur = pS->count;
		if (nCur <= 0) {
			return -1;
		}
		if (__sync_bool_compare_and_swap(&pS->count, nCur, nCur - 1)) {
			return 0;
		}
	}
}

int __gj_sem_trywait(gj_sem_t *pS)
    __attribute__((alias("gj_sem_trywait")));

/* ---- gj_sem_post ------------------------------------------------------- */

/*
 * V operation: atomically increment count. Fails if count would exceed
 * INT_MAX (0x7fffffff). Does not wake waiters (no blocking wait in this TU).
 */
int
gj_sem_post(gj_sem_t *pS)
{
	int nCur;

	if (pS == NULL) {
		return -1;
	}
	for (;;) {
		nCur = pS->count;
		if (nCur >= 0x7fffffff) {
			return -1;
		}
		if (__sync_bool_compare_and_swap(&pS->count, nCur, nCur + 1)) {
			return 0;
		}
	}
}

int __gj_sem_post(gj_sem_t *pS)
    __attribute__((alias("gj_sem_post")));

/* ---- gj_sem_getvalue --------------------------------------------------- */

/*
 * Snapshot current count into *pVal (may be 0). Not a memory barrier beyond
 * the volatile read; sufficient for single-process coordination snapshots.
 */
int
gj_sem_getvalue(gj_sem_t *pS, int *pVal)
{
	if (pS == NULL || pVal == NULL) {
		return -1;
	}
	*pVal = pS->count;
	return 0;
}

int __gj_sem_getvalue(gj_sem_t *pS, int *pVal)
    __attribute__((alias("gj_sem_getvalue")));
