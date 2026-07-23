/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2126: freestanding uint32 FIFO clear.
 *
 * Surface (unique symbols):
 *   void gj_fifo_u32_clear(size_t *head, size_t *tail, size_t *count);
 *     - Zero head, tail, and count. Leaves buffer contents untouched.
 *       NULL any pointer -> no-op.
 *   void __gj_fifo_u32_clear  (alias)
 *   __libcgj_batch2126_marker = "libcgj-batch2126"
 *
 * Distinct from gj_queue_u32_init / gj_ring_u8_clear by public name -
 * clear is the fifo_u32 exclusive surface for discarding occupancy
 * without re-binding storage. Behaviour matches init on the three
 * cursors.
 *
 * Post-2100 fifo_u32 exclusive wave (2121-2130). Clean-room freestanding
 * pure C (integer/pointer only). Compiles with -ffreestanding -msse2
 * -Wall -Wextra -Werror. No malloc, no errno, no libc. No third-party
 * source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2126_marker[] = "libcgj-batch2126";

/* ---- freestanding helpers ---------------------------------------------- */

/* Write three size_t slots to zero when all non-NULL. */
static void
b2126_zero3(size_t *pHead, size_t *pTail, size_t *pCount)
{
	*pHead = 0u;
	*pTail = 0u;
	*pCount = 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fifo_u32_clear - reset FIFO cursors on caller storage.
 *
 * head/tail/count: caller-owned cursor slots. Any NULL -> no-op.
 * Buffer payload is not modified (caller may reuse or zero separately).
 */
void
gj_fifo_u32_clear(size_t *pHead, size_t *pTail, size_t *pCount)
{
	(void)NULL;

	if (pHead == NULL || pTail == NULL || pCount == NULL) {
		return;
	}
	b2126_zero3(pHead, pTail, pCount);
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_fifo_u32_clear(size_t *pHead, size_t *pTail, size_t *pCount)
    __attribute__((alias("gj_fifo_u32_clear")));
