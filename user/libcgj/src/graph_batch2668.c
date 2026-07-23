/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2668: freestanding uint32 stack pop on caller
 * storage (_u exclusive surface).
 *
 * Surface (unique symbols):
 *   int gj_stack_u32_pop_u(uint32_t *buf, size_t *sp, uint32_t *out);
 *     - Pop top into *out. Returns 0 on success, -1 if empty or bad args.
 *   int __gj_stack_u32_pop_u  (alias)
 *   __libcgj_batch2668_marker = "libcgj-batch2668"
 *
 * Stack design (depth in caller-owned size_t):
 *   *sp — number of elements currently stored (0..cap)
 *   top is at buf[*sp - 1]; pop decrements then reads that slot.
 *
 * Distinct from gj_stack_u32_pop (batch436) / gj_stack_u8_pop
 * (batch1466) by public _u name — unique surface; no multi-def.
 * Queue/stack u32 exclusive wave (2661-2670). Caller buffers only.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2668_marker[] = "libcgj-batch2668";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b2668_empty(size_t cSp)
{
	return cSp == 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_stack_u32_pop_u - pop one uint32_t into *out.
 *
 * Returns 0 on success; -1 if empty or NULL required args.
 * (buf is required so callers keep a uniform buffer+cursor API.)
 * *out and *sp are left untouched on failure.
 */
int
gj_stack_u32_pop_u(uint32_t *pBuf, size_t *pSp, uint32_t *pOut)
{
	size_t cSp;

	(void)NULL;

	if (pBuf == NULL || pSp == NULL || pOut == NULL) {
		return -1;
	}

	cSp = *pSp;
	if (b2668_empty(cSp)) {
		return -1;
	}

	cSp--;
	*pOut = pBuf[cSp];
	*pSp = cSp;
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_stack_u32_pop_u(uint32_t *pBuf, size_t *pSp, uint32_t *pOut)
    __attribute__((alias("gj_stack_u32_pop_u")));
