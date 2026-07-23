/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch979: signed byte difference of two pointers.
 *
 * Surface (unique symbols):
 *   ptrdiff_t gj_ptr_diff_bytes(const void *a, const void *b);
 *     — Return (a - b) in bytes as ptrdiff_t (cast both to
 *       const unsigned char *). Either pointer may be NULL (treated as
 *       address 0). Pure; no dereference.
 *   ptrdiff_t __gj_ptr_diff_bytes  (alias)
 *   __libcgj_batch979_marker = "libcgj-batch979"
 *
 * Unique gj_ptr_diff_bytes surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch979_marker[] = "libcgj-batch979";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ptr_diff_bytes — signed byte distance from b to a.
 *
 * a: first pointer (minuend)
 * b: second pointer (subtrahend)
 *
 * Returns (uintptr_t)a - (uintptr_t)b as a ptrdiff_t. Does not
 * dereference either pointer.
 */
ptrdiff_t
gj_ptr_diff_bytes(const void *a, const void *b)
{
	uintptr_t uA = (uintptr_t)a;
	uintptr_t uB = (uintptr_t)b;

	return (ptrdiff_t)(uA - uB);
}

/* ---- underscored alias ------------------------------------------------- */

ptrdiff_t __gj_ptr_diff_bytes(const void *a, const void *b)
    __attribute__((alias("gj_ptr_diff_bytes")));
