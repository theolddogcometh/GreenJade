/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1466: freestanding uint8 stack pop on caller
 * storage.
 *
 * Pairs with batch1465 (push side). Stack depth lives in a caller-owned
 * size_t (*sp). Buffer is also caller-owned. Single-threaded / cooperative.
 *
 * Surface (unique; does NOT redefine batch436 gj_stack_u32_pop or
 * batch1465 gj_stack_u8_push):
 *   int gj_stack_u8_pop(uint8_t *buf, size_t *sp, uint8_t *out);
 *     — Pop top into *out. Returns 0 on success, -1 if empty or bad args.
 *   int __gj_stack_u8_pop  (alias)
 *   __libcgj_batch1466_marker = "libcgj-batch1466"
 *
 * Semantics:
 *   *sp — number of elements currently stored (0..cap)
 *   top is at buf[*sp - 1]; pop decrements then reads that slot.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1466_marker[] = "libcgj-batch1466";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b1466_empty(size_t sp)
{
	return sp == 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_stack_u8_pop — pop one uint8_t into *out.
 *
 * Returns 0 on success; -1 if empty or NULL required args.
 * (buf is required so callers keep a uniform buffer+cursor API.)
 * *out and *sp are left untouched on failure.
 */
int
gj_stack_u8_pop(uint8_t *buf, size_t *sp, uint8_t *out)
{
	size_t cSp;

	if (buf == NULL || sp == NULL || out == NULL) {
		return -1;
	}

	cSp = *sp;
	if (b1466_empty(cSp)) {
		return -1;
	}

	cSp--;
	*out = buf[cSp];
	*sp = cSp;
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_stack_u8_pop(uint8_t *buf, size_t *sp, uint8_t *out)
    __attribute__((alias("gj_stack_u8_pop")));
