/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3472: freestanding uint64 stack push on caller
 * storage (_u exclusive surface).
 *
 * Surface (unique symbols):
 *   int gj_stack_u64_push_u(uint64_t *buf, size_t cap, size_t *sp,
 *                           uint64_t v);
 *     - Push v at buf[*sp]. Returns 0 on success, -1 if full or bad args.
 *   int __gj_stack_u64_push_u  (alias)
 *   __libcgj_batch3472_marker = "libcgj-batch3472"
 *
 * Stack design (depth in caller-owned size_t):
 *   *sp — number of elements currently stored (0..cap)
 *   top is at buf[*sp - 1]; push writes at buf[*sp] then increments.
 *
 * Exclusive continuum CREATE-ONLY (3471-3480). Distinct from
 * gj_u64_stack_push (batch675) / gj_stack_u32_push_u (batch2667) /
 * gj_stack_u32_push (batch436) — unique stack_u64_push_u surface only;
 * no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3472_marker[] = "libcgj-batch3472";

/* ---- freestanding helpers ---------------------------------------------- */

/* True when stack depth already at capacity (full). */
static int
b3472_full(size_t cSp, size_t cCap)
{
	return cSp >= cCap;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_stack_u64_push_u - push one uint64_t onto caller-owned stack storage.
 *
 * Returns 0 on success; -1 if full, zero capacity, or NULL required args.
 * *sp is left untouched on failure.
 */
int
gj_stack_u64_push_u(uint64_t *pBuf, size_t cCap, size_t *pSp, uint64_t u64V)
{
	size_t cSp;

	(void)NULL;

	if (pBuf == NULL || pSp == NULL || cCap == 0u) {
		return -1;
	}

	cSp = *pSp;
	if (b3472_full(cSp, cCap)) {
		return -1;
	}

	pBuf[cSp] = u64V;
	*pSp = cSp + 1u;
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_stack_u64_push_u(uint64_t *pBuf, size_t cCap, size_t *pSp,
                          uint64_t u64V)
    __attribute__((alias("gj_stack_u64_push_u")));
