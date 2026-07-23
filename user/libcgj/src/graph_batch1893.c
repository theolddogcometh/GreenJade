/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1893: 48-bit high-canonical kernel VA hint.
 *
 * Surface (unique symbols):
 *   int gj_ptr_is_kernel_hint(uintptr_t p);
 *     — Return 1 if p is in the high 48-bit canonical kernel half:
 *       bits 63:47 all ones (p >= 0xffff800000000000). Soft kernel
 *       VA hint only; not a privilege or mapping check.
 *   int __gj_ptr_is_kernel_hint  (alias)
 *   __libcgj_batch1893_marker = "libcgj-batch1893"
 *
 * Syscall/ABI exclusive pure-data wave (1891–1900). Clean-room
 * freestanding pure C (integer only). Compiles with -ffreestanding
 * -msse2 -Wall -Wextra -Werror. No malloc, no errno, no libc. No
 * third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1893_marker[] = "libcgj-batch1893";

/* High 48-bit canonical: bits 63:47 = 1 → base 0xffff800000000000. */
#define B1893_KERN_BASE  0xffff800000000000ull

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if p is in the high-canonical kernel half. */
static int
b1893_kern_hint(uintptr_t uP)
{
	return ((uint64_t)uP >= B1893_KERN_BASE) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ptr_is_kernel_hint — 1 if high-canonical kernel VA (48-bit).
 *
 * p: virtual address as integer
 * Returns 1 when bits 63:47 are all set, else 0.
 */
int
gj_ptr_is_kernel_hint(uintptr_t uP)
{
	(void)NULL;
	return b1893_kern_hint(uP);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_ptr_is_kernel_hint(uintptr_t uP)
    __attribute__((alias("gj_ptr_is_kernel_hint")));
