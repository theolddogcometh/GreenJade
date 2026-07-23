/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2431: uint32_t ring init on caller storage.
 *
 * Surface (unique symbols):
 *   void gj_ring_u32_init(size_t *head, size_t *tail, size_t *count);
 *     — Zero head, tail, and count. NULL any pointer → no-op.
 *       Leaves buffer contents untouched.
 *   void __gj_ring_u32_init  (alias)
 *   __libcgj_batch2431_marker = "libcgj-batch2431"
 *
 * Ring design (count-based head/tail, exclusive advanced u32 wave):
 *   head  — next write index (mod cap)
 *   tail  — next read index (mod cap)
 *   count — elements currently stored (0..cap)
 *   full when count == cap; empty when count == 0.
 *
 * Distinct from classic one-empty-slot gj_ring_u32_push/pop (batch533/534)
 * which omit a count field. Distinct from gj_ring_u8_init / gj_queue_u32_init
 * by public name — no multi-def.
 *
 * Ring advanced exclusive wave (2431–2439). Clean-room freestanding pure C
 * (integer/pointer only). Compiles with -ffreestanding -msse2 -Wall
 * -Wextra -Werror. No malloc, no errno, no libc. No third-party source
 * copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2431_marker[] = "libcgj-batch2431";

/* ---- freestanding helpers ---------------------------------------------- */

/* Write three size_t slots to zero when all non-NULL. */
static void
b2431_zero3(size_t *pHead, size_t *pTail, size_t *pCount)
{
	*pHead = 0u;
	*pTail = 0u;
	*pCount = 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ring_u32_init — reset ring cursors on caller storage.
 *
 * head/tail/count: caller-owned cursor slots. Any NULL → no-op.
 * Buffer payload is not modified (caller may reuse or zero separately).
 */
void
gj_ring_u32_init(size_t *head, size_t *tail, size_t *count)
{
	if (head == NULL || tail == NULL || count == NULL) {
		return;
	}
	b2431_zero3(head, tail, count);
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_ring_u32_init(size_t *head, size_t *tail, size_t *count)
    __attribute__((alias("gj_ring_u32_init")));
