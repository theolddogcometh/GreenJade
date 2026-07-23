/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1894: #PF error-code write (W/R) bit.
 *
 * Surface (unique symbols):
 *   int gj_page_fault_error_write_p(uint64_t err);
 *     — Return 1 if x86 page-fault error code bit 1 (W/R) is set
 *       (fault was caused by a write), else 0. Pure error-code bit
 *       predicate; does not consult CR2 or page tables.
 *   int __gj_page_fault_error_write_p  (alias)
 *   __libcgj_batch1894_marker = "libcgj-batch1894"
 *
 * Syscall/ABI exclusive pure-data wave (1891–1900). Clean-room
 * freestanding pure C (integer only). Compiles with -ffreestanding
 * -msse2 -Wall -Wextra -Werror. No malloc, no errno, no libc. No
 * third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1894_marker[] = "libcgj-batch1894";

/* Intel SDM: #PF error code bit 1 = W/R (1 = write). */
#define B1894_PF_WRITE  (1ull << 1)

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if W/R bit is set in error code. */
static int
b1894_write_p(uint64_t uErr)
{
	return ((uErr & B1894_PF_WRITE) != 0ull) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_page_fault_error_write_p — 1 if #PF error code indicates write.
 *
 * err: x86 page-fault error code (bits as delivered on the stack)
 * Returns 1 when bit 1 is set, else 0.
 */
int
gj_page_fault_error_write_p(uint64_t uErr)
{
	(void)NULL;
	return b1894_write_p(uErr);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_page_fault_error_write_p(uint64_t uErr)
    __attribute__((alias("gj_page_fault_error_write_p")));
