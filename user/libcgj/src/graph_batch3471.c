/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3471: freestanding uint64 stack init on caller
 * storage (_u exclusive surface).
 *
 * Surface (unique symbols):
 *   void gj_stack_u64_init_u(size_t *sp);
 *     - Zero the stack pointer / depth. NULL sp -> no-op.
 *       Leaves buffer contents untouched.
 *   void __gj_stack_u64_init_u  (alias)
 *   __libcgj_batch3471_marker = "libcgj-batch3471"
 *
 * Stack design (depth in caller-owned size_t):
 *   *sp — number of elements currently stored (0..cap)
 *   top is at buf[*sp - 1]
 *
 * Exclusive continuum CREATE-ONLY (3471-3480): stack_u64_init_u,
 * stack_u64_push_u, stack_u64_pop_u, stack_u64_peek_u, stack_u64_len_u,
 * queue_u64_init_u, queue_u64_push_u, queue_u64_pop_u, queue_u64_len_u,
 * batch_id_3480. Distinct from gj_stack_u32_init_u (batch2666) /
 * gj_stack_u32_init (batch436) / gj_u64_stack_push (batch675) — unique
 * stack_u64_*_u surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3471_marker[] = "libcgj-batch3471";

/* ---- freestanding helpers ---------------------------------------------- */

/* Write one size_t slot to zero when non-NULL. */
static void
b3471_zero1(size_t *pSp)
{
	*pSp = 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_stack_u64_init_u - reset stack depth on caller storage.
 *
 * sp: caller-owned depth slot. NULL -> no-op.
 * Buffer payload is not modified (caller may reuse or zero separately).
 * Does not call libc. No parent wires.
 */
void
gj_stack_u64_init_u(size_t *pSp)
{
	(void)NULL;

	if (pSp == NULL) {
		return;
	}
	b3471_zero1(pSp);
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_stack_u64_init_u(size_t *pSp)
    __attribute__((alias("gj_stack_u64_init_u")));
