/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1896: #PF error-code present/protection bit.
 *
 * Surface (unique symbols):
 *   int gj_page_fault_error_prot_p(uint64_t err);
 *     — Return 1 if x86 page-fault error code bit 0 (P) is set
 *       (protection violation / page was present), else 0 (not-present).
 *       Pure error-code bit predicate.
 *   int __gj_page_fault_error_prot_p  (alias)
 *   __libcgj_batch1896_marker = "libcgj-batch1896"
 *
 * Syscall/ABI exclusive pure-data wave (1891–1900). Clean-room
 * freestanding pure C (integer only). Compiles with -ffreestanding
 * -msse2 -Wall -Wextra -Werror. No malloc, no errno, no libc. No
 * third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1896_marker[] = "libcgj-batch1896";

/* Intel SDM: #PF error code bit 0 = P (1 = protection, page present). */
#define B1896_PF_PROT  (1ull << 0)

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if P (protection / present) bit is set in error code. */
static int
b1896_prot_p(uint64_t uErr)
{
	return ((uErr & B1896_PF_PROT) != 0ull) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_page_fault_error_prot_p — 1 if #PF error code indicates protection.
 *
 * err: x86 page-fault error code (bits as delivered on the stack)
 * Returns 1 when bit 0 is set, else 0.
 */
int
gj_page_fault_error_prot_p(uint64_t uErr)
{
	(void)NULL;
	return b1896_prot_p(uErr);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_page_fault_error_prot_p(uint64_t uErr)
    __attribute__((alias("gj_page_fault_error_prot_p")));
