/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2669: freestanding uint32 stack top peek
 * (_u exclusive surface).
 *
 * Surface (unique symbols):
 *   int gj_stack_u32_peek_u(const uint32_t *buf, size_t sp, uint32_t *out);
 *     - Read the top element (buf[sp - 1]) into *out without pop.
 *       Returns 0 on success; -1 if empty or bad args. Does not mutate
 *       depth or buffer.
 *   int __gj_stack_u32_peek_u  (alias)
 *   __libcgj_batch2669_marker = "libcgj-batch2669"
 *
 * Stack design (depth by value):
 *   sp — number of elements currently stored; empty when sp == 0
 *   top is at buf[sp - 1]
 *
 * Distinct from gj_queue_u32_peek_u (batch2664) / gj_fifo_u32_peek
 * (batch2123) by public stack_u32_peek_u name — unique surface;
 * no multi-def. Queue/stack u32 exclusive wave (2661-2670).
 * Caller buffers only.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2669_marker[] = "libcgj-batch2669";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 iff depth is non-zero (valid for a top peek). */
static int
b2669_can_peek(size_t cSp)
{
	return cSp != 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_stack_u32_peek_u - non-destructive read of the top stack element.
 *
 * buf: caller-owned element storage
 * sp:  live depth (by value; not modified)
 * out: destination for the peeked value (must be non-NULL)
 *
 * Returns 0 on success with *out written. Returns -1 if empty or NULL
 * required args. *out is left untouched on failure.
 */
int
gj_stack_u32_peek_u(const uint32_t *pBuf, size_t cSp, uint32_t *pOut)
{
	(void)NULL;

	if (pBuf == NULL || pOut == NULL) {
		return -1;
	}
	if (b2669_can_peek(cSp) == 0) {
		return -1;
	}

	*pOut = pBuf[cSp - 1u];
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_stack_u32_peek_u(const uint32_t *pBuf, size_t cSp, uint32_t *pOut)
    __attribute__((alias("gj_stack_u32_peek_u")));
