/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2439: uint32_t ring clear (reset cursors, _u).
 *
 * Surface (unique symbols):
 *   void gj_ring_u32_clear_u(size_t *head, size_t *tail, size_t *count);
 *     — Zero head, tail, and count. Leaves buffer contents untouched.
 *       NULL any pointer → no-op (same contract as gj_ring_u32_init).
 *   void __gj_ring_u32_clear_u  (alias)
 *   __libcgj_batch2439_marker = "libcgj-batch2439"
 *
 * Distinct from gj_ring_u32_init (batch2431) by public name — clear is the
 * advanced-exclusive surface for discarding occupancy without re-binding
 * storage. Behaviour matches init on the three cursors.
 * Distinct from gj_ring_u8_clear (batch1806). Unique _u surface.
 *
 * Ring advanced exclusive wave (2431–2439). Clean-room freestanding pure C
 * (integer/pointer only). Compiles with -ffreestanding -msse2 -Wall
 * -Wextra -Werror. No malloc, no errno, no libc. No third-party source
 * copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2439_marker[] = "libcgj-batch2439";

/* ---- freestanding helpers ---------------------------------------------- */

/* Write three size_t slots to zero when all non-NULL. */
static void
b2439_zero3(size_t *pHead, size_t *pTail, size_t *pCount)
{
	*pHead = 0u;
	*pTail = 0u;
	*pCount = 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ring_u32_clear_u — reset ring cursors on caller storage.
 *
 * head/tail/count: caller-owned cursor slots. Any NULL → no-op.
 * Buffer payload is not modified (caller may reuse or zero separately).
 */
void
gj_ring_u32_clear_u(size_t *head, size_t *tail, size_t *count)
{
	if (head == NULL || tail == NULL || count == NULL) {
		return;
	}
	b2439_zero3(head, tail, count);
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_ring_u32_clear_u(size_t *head, size_t *tail, size_t *count)
    __attribute__((alias("gj_ring_u32_clear_u")));
