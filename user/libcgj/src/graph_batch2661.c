/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2661: freestanding uint32 queue init on caller
 * storage (_u exclusive surface).
 *
 * Surface (unique symbols):
 *   void gj_queue_u32_init_u(size_t *h, size_t *t, size_t *n);
 *     - Zero head, tail, and count. NULL any pointer -> no-op.
 *       Leaves buffer contents untouched.
 *   void __gj_queue_u32_init_u  (alias)
 *   __libcgj_batch2661_marker = "libcgj-batch2661"
 *
 * Queue design (count-based head/tail, same model as batch437):
 *   h / *h — next write index (mod cap)
 *   t / *t — next read index (mod cap)
 *   n / *n — elements currently stored (0..cap)
 *
 * Distinct from gj_queue_u32_init (batch437) by public _u name —
 * unique surface; no multi-def. Queue/stack u32 exclusive wave
 * (2661-2670). Caller buffers only.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2661_marker[] = "libcgj-batch2661";

/* ---- freestanding helpers ---------------------------------------------- */

/* Write three size_t slots to zero when all non-NULL. */
static void
b2661_zero3(size_t *pH, size_t *pT, size_t *pN)
{
	*pH = 0u;
	*pT = 0u;
	*pN = 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_queue_u32_init_u - reset queue cursors on caller storage.
 *
 * h/t/n: caller-owned cursor slots. Any NULL -> no-op.
 * Buffer payload is not modified (caller may reuse or zero separately).
 */
void
gj_queue_u32_init_u(size_t *pH, size_t *pT, size_t *pN)
{
	(void)NULL;

	if (pH == NULL || pT == NULL || pN == NULL) {
		return;
	}
	b2661_zero3(pH, pT, pN);
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_queue_u32_init_u(size_t *pH, size_t *pT, size_t *pN)
    __attribute__((alias("gj_queue_u32_init_u")));
