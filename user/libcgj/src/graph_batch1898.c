/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1898: CS selector user-mode (RPL=3) hint.
 *
 * Surface (unique symbols):
 *   int gj_cs_is_user_hint(uint64_t cs);
 *     — Return 1 if the CS selector RPL field (bits 1:0) is 3 (user),
 *       else 0. Soft ABI hint from the trap frame CS; does not validate
 *       the full GDT/LDT descriptor.
 *   int __gj_cs_is_user_hint  (alias)
 *   __libcgj_batch1898_marker = "libcgj-batch1898"
 *
 * Syscall/ABI exclusive pure-data wave (1891–1900). Clean-room
 * freestanding pure C (integer only). Compiles with -ffreestanding
 * -msse2 -Wall -Wextra -Werror. No malloc, no errno, no libc. No
 * third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1898_marker[] = "libcgj-batch1898";

/* Segment selector RPL mask; user ring is RPL 3. */
#define B1898_RPL_MASK  3ull
#define B1898_RPL_USER  3ull

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if selector RPL == 3. */
static int
b1898_cs_user(uint64_t uCs)
{
	return ((uCs & B1898_RPL_MASK) == B1898_RPL_USER) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cs_is_user_hint — 1 if CS RPL indicates user mode.
 *
 * cs: code-segment selector from trap/iret frame
 * Returns 1 when (cs & 3) == 3, else 0.
 */
int
gj_cs_is_user_hint(uint64_t uCs)
{
	(void)NULL;
	return b1898_cs_user(uCs);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_cs_is_user_hint(uint64_t uCs)
    __attribute__((alias("gj_cs_is_user_hint")));
