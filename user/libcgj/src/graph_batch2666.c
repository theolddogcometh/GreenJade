/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2666: freestanding uint32 stack init on caller
 * storage (_u exclusive surface).
 *
 * Surface (unique symbols):
 *   void gj_stack_u32_init_u(size_t *sp);
 *     - Zero the stack pointer / depth. NULL sp -> no-op.
 *       Leaves buffer contents untouched.
 *   void __gj_stack_u32_init_u  (alias)
 *   __libcgj_batch2666_marker = "libcgj-batch2666"
 *
 * Stack design (depth in caller-owned size_t):
 *   *sp — number of elements currently stored (0..cap)
 *   top is at buf[*sp - 1]
 *
 * Distinct from gj_stack_u32_init (batch436) by public _u name —
 * unique surface; no multi-def. Queue/stack u32 exclusive wave
 * (2661-2670). Caller buffers only.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2666_marker[] = "libcgj-batch2666";

/* ---- freestanding helpers ---------------------------------------------- */

/* Write one size_t slot to zero when non-NULL. */
static void
b2666_zero1(size_t *pSp)
{
	*pSp = 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_stack_u32_init_u - reset stack depth on caller storage.
 *
 * sp: caller-owned depth slot. NULL -> no-op.
 * Buffer payload is not modified (caller may reuse or zero separately).
 */
void
gj_stack_u32_init_u(size_t *pSp)
{
	(void)NULL;

	if (pSp == NULL) {
		return;
	}
	b2666_zero1(pSp);
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_stack_u32_init_u(size_t *pSp)
    __attribute__((alias("gj_stack_u32_init_u")));
