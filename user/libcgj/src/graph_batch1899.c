/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1899: SS selector user-mode (RPL=3) hint.
 *
 * Surface (unique symbols):
 *   int gj_ss_is_user_hint(uint64_t ss);
 *     — Return 1 if the SS selector RPL field (bits 1:0) is 3 (user),
 *       else 0. Soft ABI hint from the trap frame SS; does not validate
 *       the full GDT/LDT descriptor.
 *   int __gj_ss_is_user_hint  (alias)
 *   __libcgj_batch1899_marker = "libcgj-batch1899"
 *
 * Syscall/ABI exclusive pure-data wave (1891–1900). Clean-room
 * freestanding pure C (integer only). Compiles with -ffreestanding
 * -msse2 -Wall -Wextra -Werror. No malloc, no errno, no libc. No
 * third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1899_marker[] = "libcgj-batch1899";

/* Segment selector RPL mask; user ring is RPL 3. */
#define B1899_RPL_MASK  3ull
#define B1899_RPL_USER  3ull

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if selector RPL == 3. */
static int
b1899_ss_user(uint64_t uSs)
{
	return ((uSs & B1899_RPL_MASK) == B1899_RPL_USER) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ss_is_user_hint — 1 if SS RPL indicates user mode.
 *
 * ss: stack-segment selector from trap/iret frame
 * Returns 1 when (ss & 3) == 3, else 0.
 */
int
gj_ss_is_user_hint(uint64_t uSs)
{
	(void)NULL;
	return b1899_ss_user(uSs);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_ss_is_user_hint(uint64_t uSs)
    __attribute__((alias("gj_ss_is_user_hint")));
