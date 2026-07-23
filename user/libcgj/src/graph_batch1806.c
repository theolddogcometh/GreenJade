/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1806: byte-ring clear (reset cursors).
 *
 * Surface (unique symbols):
 *   void gj_ring_u8_clear(size_t *head, size_t *tail, size_t *count);
 *     — Zero head, tail, and count. Leaves buffer contents untouched.
 *       NULL any pointer → no-op (same contract as gj_ring_u8_init).
 *   void __gj_ring_u8_clear  (alias)
 *   __libcgj_batch1806_marker = "libcgj-batch1806"
 *
 * Distinct from gj_ring_u8_init (batch281) by public name — clear is the
 * advanced-exclusive surface for discarding occupancy without re-binding
 * storage. Behaviour matches init on the three cursors.
 *
 * Ring buffer advanced exclusive wave (1801–1809). Clean-room freestanding
 * pure C (integer/pointer only). Compiles with -ffreestanding -msse2
 * -Wall -Wextra -Werror. No malloc, no errno, no libc. No third-party
 * source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1806_marker[] = "libcgj-batch1806";

/* ---- freestanding helpers ---------------------------------------------- */

/* Write three size_t slots to zero when all non-NULL. */
static void
b1806_zero3(size_t *pHead, size_t *pTail, size_t *pCount)
{
	*pHead = 0u;
	*pTail = 0u;
	*pCount = 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ring_u8_clear — reset ring cursors on caller storage.
 *
 * head/tail/count: caller-owned cursor slots. Any NULL → no-op.
 * Buffer payload is not modified (caller may reuse or zero separately).
 */
void
gj_ring_u8_clear(size_t *head, size_t *tail, size_t *count)
{
	if (head == NULL || tail == NULL || count == NULL) {
		return;
	}
	b1806_zero3(head, tail, count);
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_ring_u8_clear(size_t *head, size_t *tail, size_t *count)
    __attribute__((alias("gj_ring_u8_clear")));
