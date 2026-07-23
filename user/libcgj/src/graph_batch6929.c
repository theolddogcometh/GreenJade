/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6929: copy_file_range arg count + known flags.
 *
 * Surface (unique symbols):
 *   uint32_t gj_cfr_arg_count_6929(void);
 *     - Returns 6 (soft: fd_in, off_in, fd_out, off_out, len, flags).
 *   uint32_t gj_cfr_known_flags_6929(void);
 *     - Returns 0 (soft known flags baseline; currently none defined).
 *   uint32_t gj_cfr_known_flags_ok_6929(uint32_t flags);
 *     - Return 1 if flags is a subset of known baseline
 *       ((flags & ~known) == 0); with known == 0 this means flags == 0.
 *   uint32_t __gj_cfr_arg_count_6929  (alias)
 *   uint32_t __gj_cfr_known_flags_6929  (alias)
 *   uint32_t __gj_cfr_known_flags_ok_6929  (alias)
 *   __libcgj_batch6929_marker = "libcgj-batch6929"
 *
 * Exclusive continuum CREATE-ONLY (6921-6930: copy_file_range stubs).
 * Unique gj_cfr_*_6929 surfaces only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6929_marker[] = "libcgj-batch6929";

/* Soft: six-argument copy_file_range surface. */
#define B6929_ARG_COUNT    6u
/* Soft known flags baseline (none currently defined by Linux). */
#define B6929_KNOWN_FLAGS  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6929_arg_count(void)
{
	return B6929_ARG_COUNT;
}

static uint32_t
b6929_known_flags(void)
{
	return B6929_KNOWN_FLAGS;
}

static uint32_t
b6929_known_flags_ok(uint32_t u32Flags)
{
	uint32_t u32Known = B6929_KNOWN_FLAGS;

	return ((u32Flags & ~u32Known) == 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cfr_arg_count_6929 - soft copy_file_range argument count.
 *
 * Always returns 6. Pure-data layout tag; no parent wires.
 */
uint32_t
gj_cfr_arg_count_6929(void)
{
	(void)NULL;
	return b6929_arg_count();
}

/*
 * gj_cfr_known_flags_6929 - soft known copy_file_range flags mask.
 *
 * Always returns 0. Soft pure-data product tag; does not call
 * copy_file_range. No parent wires.
 */
uint32_t
gj_cfr_known_flags_6929(void)
{
	return b6929_known_flags();
}

/*
 * gj_cfr_known_flags_ok_6929 - flags is subset of known baseline.
 *
 * flags: soft copy_file_range flags bitmask
 *
 * Returns 1 if no unknown bits present; else 0. With known == 0 this
 * accepts only zero flags.
 */
uint32_t
gj_cfr_known_flags_ok_6929(uint32_t u32Flags)
{
	return b6929_known_flags_ok(u32Flags);
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_cfr_arg_count_6929(void)
    __attribute__((alias("gj_cfr_arg_count_6929")));

uint32_t __gj_cfr_known_flags_6929(void)
    __attribute__((alias("gj_cfr_known_flags_6929")));

uint32_t __gj_cfr_known_flags_ok_6929(uint32_t u32Flags)
    __attribute__((alias("gj_cfr_known_flags_ok_6929")));
