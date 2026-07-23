/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3476: freestanding uint64 queue init on caller
 * storage (_u exclusive surface).
 *
 * Surface (unique symbols):
 *   void gj_queue_u64_init_u(size_t *h, size_t *t, size_t *n);
 *     - Zero head, tail, and count. NULL any pointer -> no-op.
 *       Leaves buffer contents untouched.
 *   void __gj_queue_u64_init_u  (alias)
 *   __libcgj_batch3476_marker = "libcgj-batch3476"
 *
 * Queue design (count-based head/tail, same model as batch437 / 2661):
 *   h / *h — next write index (mod cap)
 *   t / *t — next read index (mod cap)
 *   n / *n — elements currently stored (0..cap)
 *
 * Exclusive continuum CREATE-ONLY (3471-3480). Distinct from
 * gj_queue_u32_init_u (batch2661) / gj_queue_u32_init (batch437) —
 * unique queue_u64_init_u surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3476_marker[] = "libcgj-batch3476";

/* ---- freestanding helpers ---------------------------------------------- */

/* Write three size_t slots to zero when all non-NULL. */
static void
b3476_zero3(size_t *pH, size_t *pT, size_t *pN)
{
	*pH = 0u;
	*pT = 0u;
	*pN = 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_queue_u64_init_u - reset queue cursors on caller storage.
 *
 * h/t/n: caller-owned cursor slots. Any NULL -> no-op.
 * Buffer payload is not modified (caller may reuse or zero separately).
 * Does not call libc. No parent wires.
 */
void
gj_queue_u64_init_u(size_t *pH, size_t *pT, size_t *pN)
{
	(void)NULL;

	if (pH == NULL || pT == NULL || pN == NULL) {
		return;
	}
	b3476_zero3(pH, pT, pN);
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_queue_u64_init_u(size_t *pH, size_t *pT, size_t *pN)
    __attribute__((alias("gj_queue_u64_init_u")));
