/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1895: #PF error-code user (U/S) bit.
 *
 * Surface (unique symbols):
 *   int gj_page_fault_error_user_p(uint64_t err);
 *     — Return 1 if x86 page-fault error code bit 2 (U/S) is set
 *       (fault occurred in user mode), else 0. Pure error-code bit
 *       predicate; does not inspect CPL or CR2.
 *   int __gj_page_fault_error_user_p  (alias)
 *   __libcgj_batch1895_marker = "libcgj-batch1895"
 *
 * Syscall/ABI exclusive pure-data wave (1891–1900). Clean-room
 * freestanding pure C (integer only). Compiles with -ffreestanding
 * -msse2 -Wall -Wextra -Werror. No malloc, no errno, no libc. No
 * third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1895_marker[] = "libcgj-batch1895";

/* Intel SDM: #PF error code bit 2 = U/S (1 = user-mode access). */
#define B1895_PF_USER  (1ull << 2)

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if U/S bit is set in error code. */
static int
b1895_user_p(uint64_t uErr)
{
	return ((uErr & B1895_PF_USER) != 0ull) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_page_fault_error_user_p — 1 if #PF error code indicates user mode.
 *
 * err: x86 page-fault error code (bits as delivered on the stack)
 * Returns 1 when bit 2 is set, else 0.
 */
int
gj_page_fault_error_user_p(uint64_t uErr)
{
	(void)NULL;
	return b1895_user_p(uErr);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_page_fault_error_user_p(uint64_t uErr)
    __attribute__((alias("gj_page_fault_error_user_p")));
