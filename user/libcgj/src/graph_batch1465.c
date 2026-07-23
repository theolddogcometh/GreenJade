/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1465: freestanding uint8 stack push on caller
 * storage.
 *
 * Stack depth lives in a caller-owned size_t (*sp). Buffer is also
 * caller-owned. Single-threaded / cooperative use (no atomics).
 *
 * Surface (unique; does NOT redefine batch436 gj_stack_u32_push or
 * batch675 gj_u64_stack_push):
 *   int gj_stack_u8_push(uint8_t *buf, size_t cap, size_t *sp, uint8_t v);
 *     — Push v at buf[*sp]. Returns 0 on success, -1 if full or bad args.
 *   int __gj_stack_u8_push  (alias)
 *   __libcgj_batch1465_marker = "libcgj-batch1465"
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

const char __libcgj_batch1465_marker[] = "libcgj-batch1465";

/* ---- freestanding helpers ---------------------------------------------- */

/* True when stack depth already at capacity (full). */
static int
b1465_full(size_t sp, size_t cap)
{
	return sp >= cap;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_stack_u8_push — push one uint8_t onto caller-owned stack storage.
 *
 * Returns 0 on success; -1 if full, zero capacity, or NULL required args.
 */
int
gj_stack_u8_push(uint8_t *buf, size_t cap, size_t *sp, uint8_t v)
{
	size_t cSp;

	if (buf == NULL || sp == NULL || cap == 0u) {
		return -1;
	}

	cSp = *sp;
	if (b1465_full(cSp, cap)) {
		return -1;
	}

	buf[cSp] = v;
	*sp = cSp + 1u;
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_stack_u8_push(uint8_t *buf, size_t cap, size_t *sp, uint8_t v)
    __attribute__((alias("gj_stack_u8_push")));
