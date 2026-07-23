/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch676: freestanding uint64 stack pop on caller
 * storage.
 *
 * Stack depth lives in a caller-owned size_t (*sp). Buffer is also
 * caller-owned. Single-threaded / cooperative use (no atomics).
 *
 * Surface (unique symbols):
 *   int gj_u64_stack_pop(uint64_t *buf, size_t *sp, uint64_t *out);
 *     — Pop top into *out. Returns 0 on success, -1 if empty or bad args.
 *   int __gj_u64_stack_pop  (alias)
 *   __libcgj_batch676_marker = "libcgj-batch676"
 *
 * Semantics (matches batch675 push):
 *   *sp — number of elements currently stored (0..cap)
 *   top is at buf[*sp - 1]; pop decrements then reads that slot into *out.
 *
 * Does NOT define gj_u64_stack_push / __gj_u64_stack_push (batch675) or
 * gj_stack_u32_init / gj_stack_u32_push / gj_stack_u32_pop / their __gj_
 * aliases (graph_batch436.c) — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch676_marker[] = "libcgj-batch676";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_stack_pop — pop one uint64_t into *out.
 *
 * Returns 0 on success; -1 if empty or NULL required args.
 * (buf is required so callers keep a uniform buffer+cursor API.)
 * *out is left untouched on failure.
 */
int
gj_u64_stack_pop(uint64_t *pBuf, size_t *pSp, uint64_t *pOut)
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

/* ---- underscored alias ------------------------------------------------- */

int __gj_u64_stack_pop(uint64_t *pBuf, size_t *pSp, uint64_t *pOut)
    __attribute__((alias("gj_u64_stack_pop")));
