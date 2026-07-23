/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6922: copy_file_range flags must-zero lamp.
 *
 * Surface (unique symbols):
 *   uint32_t gj_cfr_flags_must_zero_6922(void);
 *     - Returns 1 (soft: Linux copy_file_range currently requires
 *       flags == 0; non-zero flags are rejected).
 *   uint32_t __gj_cfr_flags_must_zero_6922  (alias)
 *   __libcgj_batch6922_marker = "libcgj-batch6922"
 *
 * Exclusive continuum CREATE-ONLY (6921-6930: copy_file_range stubs).
 * Unique gj_cfr_flags_must_zero_6922 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6922_marker[] = "libcgj-batch6922";

/* Soft policy: flags must be zero for copy_file_range. */
#define B6922_MUST_ZERO  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6922_must_zero(void)
{
	return B6922_MUST_ZERO;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cfr_flags_must_zero_6922 - soft must-zero policy for cfr flags.
 *
 * Always returns 1. Pure-data catalog stub for product copy_file_range
 * flag policy; does not invoke the syscall. No parent wires.
 */
uint32_t
gj_cfr_flags_must_zero_6922(void)
{
	(void)NULL;
	return b6922_must_zero();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_cfr_flags_must_zero_6922(void)
    __attribute__((alias("gj_cfr_flags_must_zero_6922")));
