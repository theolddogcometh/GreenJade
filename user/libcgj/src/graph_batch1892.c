/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1892: 48-bit canonical user VA hint.
 *
 * Surface (unique symbols):
 *   int gj_ptr_is_canonical_user_hint(uintptr_t p);
 *     — Return 1 if high bits are zero for 48-bit addressing: bits
 *       63:47 are all zero (p < 2^47). Soft user-half canonical hint;
 *       does not validate page tables or actual usermode mapping.
 *   int __gj_ptr_is_canonical_user_hint  (alias)
 *   __libcgj_batch1892_marker = "libcgj-batch1892"
 *
 * Syscall/ABI exclusive pure-data wave (1891–1900). Clean-room
 * freestanding pure C (integer only). Compiles with -ffreestanding
 * -msse2 -Wall -Wextra -Werror. No malloc, no errno, no libc. No
 * third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1892_marker[] = "libcgj-batch1892";

/* 48-bit user half: bits 63:47 must be zero → p < 2^47. */
#define B1892_USER_MAX  (1ull << 47)

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if p lies in the low 48-bit user half (high bits zero). */
static int
b1892_user_hint(uintptr_t uP)
{
	return ((uint64_t)uP < B1892_USER_MAX) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ptr_is_canonical_user_hint — 1 if high bits zero (48-bit user).
 *
 * p: virtual address as integer
 * Returns 1 when bits 63:47 are clear, else 0.
 */
int
gj_ptr_is_canonical_user_hint(uintptr_t uP)
{
	(void)NULL;
	return b1892_user_hint(uP);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_ptr_is_canonical_user_hint(uintptr_t uP)
    __attribute__((alias("gj_ptr_is_canonical_user_hint")));
