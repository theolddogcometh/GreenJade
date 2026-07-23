/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch675: freestanding uint64 stack push on caller
 * storage.
 *
 * Stack depth lives in a caller-owned size_t (*sp). Buffer is also
 * caller-owned. Single-threaded / cooperative use (no atomics).
 *
 * Surface (unique symbols):
 *   int gj_u64_stack_push(uint64_t *buf, size_t cap, size_t *sp, uint64_t v);
 *     — Push v at buf[*sp]. Returns 0 on success, -1 if full or bad args.
 *   int __gj_u64_stack_push  (alias)
 *   __libcgj_batch675_marker = "libcgj-batch675"
 *
 * Semantics:
 *   *sp — number of elements currently stored (0..cap)
 *   top is at buf[*sp - 1]; push writes at buf[*sp] then increments.
 *
 * Does NOT define gj_stack_u32_init / gj_stack_u32_push / gj_stack_u32_pop
 * or their __gj_ aliases (graph_batch436.c) — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch675_marker[] = "libcgj-batch675";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_stack_push — push one uint64_t onto caller-owned stack storage.
 *
 * Returns 0 on success; -1 if full, zero capacity, or NULL required args.
 */
int
gj_u64_stack_push(uint64_t *pBuf, size_t cCap, size_t *pSp, uint64_t u64V)
{
	size_t cSp;

	if (pBuf == NULL || pSp == NULL || cCap == 0u) {
		return -1;
	}

	cSp = *pSp;
	if (cSp >= cCap) {
		return -1;
	}

	pBuf[cSp] = u64V;
	*pSp = cSp + 1u;
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_u64_stack_push(uint64_t *pBuf, size_t cCap, size_t *pSp, uint64_t u64V)
    __attribute__((alias("gj_u64_stack_push")));
