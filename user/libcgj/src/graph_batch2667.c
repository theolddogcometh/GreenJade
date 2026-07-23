/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2667: freestanding uint32 stack push on caller
 * storage (_u exclusive surface).
 *
 * Surface (unique symbols):
 *   int gj_stack_u32_push_u(uint32_t *buf, size_t cap, size_t *sp,
 *                           uint32_t v);
 *     - Push v at buf[*sp]. Returns 0 on success, -1 if full or bad args.
 *   int __gj_stack_u32_push_u  (alias)
 *   __libcgj_batch2667_marker = "libcgj-batch2667"
 *
 * Stack design (depth in caller-owned size_t):
 *   *sp — number of elements currently stored (0..cap)
 *   top is at buf[*sp - 1]; push writes at buf[*sp] then increments.
 *
 * Distinct from gj_stack_u32_push (batch436) / gj_stack_u8_push
 * (batch1465) by public _u name — unique surface; no multi-def.
 * Queue/stack u32 exclusive wave (2661-2670). Caller buffers only.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2667_marker[] = "libcgj-batch2667";

/* ---- freestanding helpers ---------------------------------------------- */

/* True when stack depth already at capacity (full). */
static int
b2667_full(size_t cSp, size_t cCap)
{
	return cSp >= cCap;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_stack_u32_push_u - push one uint32_t onto caller-owned stack storage.
 *
 * Returns 0 on success; -1 if full, zero capacity, or NULL required args.
 * *sp is left untouched on failure.
 */
int
gj_stack_u32_push_u(uint32_t *pBuf, size_t cCap, size_t *pSp, uint32_t u32V)
{
	size_t cSp;

	(void)NULL;

	if (pBuf == NULL || pSp == NULL || cCap == 0u) {
		return -1;
	}

	cSp = *pSp;
	if (b2667_full(cSp, cCap)) {
		return -1;
	}

	pBuf[cSp] = u32V;
	*pSp = cSp + 1u;
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_stack_u32_push_u(uint32_t *pBuf, size_t cCap, size_t *pSp,
                          uint32_t u32V)
    __attribute__((alias("gj_stack_u32_push_u")));
