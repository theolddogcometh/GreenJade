/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1897: RFLAGS IF bit (iret / interrupt flag).
 *
 * Surface (unique symbols):
 *   int gj_iret_rflags_if_p(uint64_t rflags);
 *     — Return 1 if RFLAGS bit 9 (IF, interrupt enable) is set, else 0.
 *       Used as a pure data check on the flags image restored by
 *       iret/sysret paths; does not touch hardware flags.
 *   int __gj_iret_rflags_if_p  (alias)
 *   __libcgj_batch1897_marker = "libcgj-batch1897"
 *
 * Syscall/ABI exclusive pure-data wave (1891–1900). Clean-room
 * freestanding pure C (integer only). Compiles with -ffreestanding
 * -msse2 -Wall -Wextra -Werror. No malloc, no errno, no libc. No
 * third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1897_marker[] = "libcgj-batch1897";

/* Intel SDM: RFLAGS.IF is bit 9 (0x200). */
#define B1897_RFLAGS_IF  (1ull << 9)

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if IF is set in the rflags image. */
static int
b1897_if_p(uint64_t uRflags)
{
	return ((uRflags & B1897_RFLAGS_IF) != 0ull) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_iret_rflags_if_p — 1 if RFLAGS image has IF set.
 *
 * rflags: flags word as on the interrupt/syscall stack frame
 * Returns 1 when bit 9 is set, else 0.
 */
int
gj_iret_rflags_if_p(uint64_t uRflags)
{
	(void)NULL;
	return b1897_if_p(uRflags);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_iret_rflags_if_p(uint64_t uRflags)
    __attribute__((alias("gj_iret_rflags_if_p")));
