/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3473: freestanding uint64 stack pop on caller
 * storage (_u exclusive surface).
 *
 * Surface (unique symbols):
 *   int gj_stack_u64_pop_u(uint64_t *buf, size_t *sp, uint64_t *out);
 *     - Pop top into *out. Returns 0 on success, -1 if empty or bad args.
 *   int __gj_stack_u64_pop_u  (alias)
 *   __libcgj_batch3473_marker = "libcgj-batch3473"
 *
 * Stack design (depth in caller-owned size_t):
 *   *sp — number of elements currently stored (0..cap)
 *   top is at buf[*sp - 1]; pop decrements then reads that slot.
 *
 * Exclusive continuum CREATE-ONLY (3471-3480). Distinct from
 * gj_u64_stack_pop (batch676) / gj_stack_u32_pop_u (batch2668) /
 * gj_stack_u32_pop (batch436) — unique stack_u64_pop_u surface only;
 * no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3473_marker[] = "libcgj-batch3473";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b3473_empty(size_t cSp)
{
	return cSp == 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_stack_u64_pop_u - pop one uint64_t into *out.
 *
 * Returns 0 on success; -1 if empty or NULL required args.
 * (buf is required so callers keep a uniform buffer+cursor API.)
 * *out and *sp are left untouched on failure.
 */
int
gj_stack_u64_pop_u(uint64_t *pBuf, size_t *pSp, uint64_t *pOut)
{
	size_t cSp;

	(void)NULL;

	if (pBuf == NULL || pSp == NULL || pOut == NULL) {
		return -1;
	}

	cSp = *pSp;
	if (b3473_empty(cSp)) {
		return -1;
	}

	cSp--;
	*pOut = pBuf[cSp];
	*pSp = cSp;
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_stack_u64_pop_u(uint64_t *pBuf, size_t *pSp, uint64_t *pOut)
    __attribute__((alias("gj_stack_u64_pop_u")));
