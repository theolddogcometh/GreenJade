/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7544: sigaction flag known-mask stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_sigaction_flag_mask_7544(void);
 *     - Returns the OR of all known Linux x86_64 sa_flags bits
 *       (SA_NOCLDSTOP|SA_NOCLDWAIT|SA_SIGINFO|SA_ONSTACK|SA_RESTART|
 *       SA_NODEFER|SA_RESETHAND). Soft pure-data mask; not a live
 *       sigaction query.
 *   uint32_t gj_sigaction_flag_test_7544(uint32_t sa_flags, uint32_t bit);
 *     - Return 1 if (sa_flags & bit) != 0, else 0. Soft bitwise test.
 *   uint32_t __gj_sigaction_flag_mask_7544  (alias)
 *   uint32_t __gj_sigaction_flag_test_7544  (alias)
 *   __libcgj_batch7544_marker = "libcgj-batch7544"
 *
 * Exclusive continuum CREATE-ONLY (7541-7550: sigaction flag stubs).
 * Unique *_7544 surfaces only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7544_marker[] = "libcgj-batch7544";

/* Known sa_flags OR (local freestanding SA_* copies). */
#define B7544_SA_NOCLDSTOP  0x00000001u
#define B7544_SA_NOCLDWAIT  0x00000002u
#define B7544_SA_SIGINFO    0x00000004u
#define B7544_SA_ONSTACK    0x08000000u
#define B7544_SA_RESTART    0x10000000u
#define B7544_SA_NODEFER    0x40000000u
#define B7544_SA_RESETHAND  0x80000000u

#define B7544_SA_KNOWN_MASK  (B7544_SA_NOCLDSTOP | B7544_SA_NOCLDWAIT | \
			      B7544_SA_SIGINFO | B7544_SA_ONSTACK | \
			      B7544_SA_RESTART | B7544_SA_NODEFER | \
			      B7544_SA_RESETHAND)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7544_mask(void)
{
	return B7544_SA_KNOWN_MASK;
}

static uint32_t
b7544_test(uint32_t u32Flags, uint32_t u32Bit)
{
	if ((u32Flags & u32Bit) != 0u) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sigaction_flag_mask_7544 - known sa_flags OR mask.
 *
 * Always returns the compile-time known SA_* bit OR. Soft pure-data
 * only. No parent wires.
 */
uint32_t
gj_sigaction_flag_mask_7544(void)
{
	(void)NULL;
	return b7544_mask();
}

/*
 * gj_sigaction_flag_test_7544 - soft bitwise presence test.
 *
 * Returns 1 if (sa_flags & bit) != 0, else 0. Does not call libc.
 */
uint32_t
gj_sigaction_flag_test_7544(uint32_t sa_flags, uint32_t bit)
{
	return b7544_test(sa_flags, bit);
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_sigaction_flag_mask_7544(void)
    __attribute__((alias("gj_sigaction_flag_mask_7544")));

uint32_t __gj_sigaction_flag_test_7544(uint32_t sa_flags, uint32_t bit)
    __attribute__((alias("gj_sigaction_flag_test_7544")));
