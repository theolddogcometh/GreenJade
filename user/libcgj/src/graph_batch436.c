/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch436: freestanding uint32 stack on caller storage.
 *
 * Stack depth lives in a caller-owned size_t (*sp). Buffer is also
 * caller-owned. Single-threaded / cooperative use (no atomics).
 *
 * Surface (unique symbols):
 *   void gj_stack_u32_init(size_t *sp);
 *     — Zero the stack pointer. NULL sp → no-op.
 *   int  gj_stack_u32_push(uint32_t *buf, size_t cap, size_t *sp, uint32_t v);
 *     — Push v at buf[*sp]. Returns 0 on success, -1 if full or bad args.
 *   int  gj_stack_u32_pop(uint32_t *buf, size_t *sp, uint32_t *out);
 *     — Pop top into *out. Returns 0 on success, -1 if empty or bad args.
 *   __gj_stack_u32_init / __gj_stack_u32_push / __gj_stack_u32_pop  (aliases)
 *   __libcgj_batch436_marker = "libcgj-batch436"
 *
 * Semantics:
 *   *sp — number of elements currently stored (0..cap)
 *   top is at buf[*sp - 1]; push writes at buf[*sp] then increments.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch436_marker[] = "libcgj-batch436";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_stack_u32_init — reset stack depth on caller storage.
 * Leaves buf contents untouched.
 */
void
gj_stack_u32_init(size_t *pSp)
{
	if (pSp == NULL) {
		return;
	}
	*pSp = 0u;
}

/*
 * gj_stack_u32_push — push one uint32_t.
 *
 * Returns 0 on success; -1 if full, zero capacity, or NULL required args.
 */
int
gj_stack_u32_push(uint32_t *pBuf, size_t cCap, size_t *pSp, uint32_t u32V)
{
	size_t cSp;

	if (pBuf == NULL || pSp == NULL || cCap == 0u) {
		return -1;
	}

	cSp = *pSp;
	if (cSp >= cCap) {
		return -1;
	}

	pBuf[cSp] = u32V;
	*pSp = cSp + 1u;
	return 0;
}

/*
 * gj_stack_u32_pop — pop one uint32_t into *out.
 *
 * Returns 0 on success; -1 if empty or NULL required args.
 * (buf is required so callers keep a uniform buffer+cursor API.)
 */
int
gj_stack_u32_pop(uint32_t *pBuf, size_t *pSp, uint32_t *pOut)
{
	size_t cSp;

	if (pBuf == NULL || pSp == NULL || pOut == NULL) {
		return -1;
	}

	cSp = *pSp;
	if (cSp == 0u) {
		return -1;
	}

	cSp--;
	*pOut = pBuf[cSp];
	*pSp = cSp;
	return 0;
}

/* ---- underscored aliases ----------------------------------------------- */

void __gj_stack_u32_init(size_t *pSp)
    __attribute__((alias("gj_stack_u32_init")));
int __gj_stack_u32_push(uint32_t *pBuf, size_t cCap, size_t *pSp, uint32_t u32V)
    __attribute__((alias("gj_stack_u32_push")));
int __gj_stack_u32_pop(uint32_t *pBuf, size_t *pSp, uint32_t *pOut)
    __attribute__((alias("gj_stack_u32_pop")));
