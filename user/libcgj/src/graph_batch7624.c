/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7624: setrlimit soft/hard + infinity stubs.
 *
 * Surface (unique symbols):
 *   uint32_t gj_setrlimit_res_soft_le_hard_7624(uint64_t soft,
 *                                               uint64_t hard);
 *     - Return 1 if soft <= hard (valid rlimit pair under pure unsigned
 *       compare), else 0. RLIM_INFINITY-style UINT64_MAX allowed on
 *       either side. Soft pure-data validation only.
 *   uint32_t gj_setrlimit_res_is_inf_7624(uint64_t lim);
 *     - Return 1 if lim == UINT64_MAX (soft RLIM_INFINITY), else 0.
 *   uint32_t __gj_setrlimit_res_soft_le_hard_7624  (alias)
 *   uint32_t __gj_setrlimit_res_is_inf_7624  (alias)
 *   __libcgj_batch7624_marker = "libcgj-batch7624"
 *
 * Exclusive continuum CREATE-ONLY (7621-7630: setrlimit resource stubs).
 * Unique *_7624 surfaces only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7624_marker[] = "libcgj-batch7624";

/* Soft RLIM_INFINITY stand-in (all-ones rlim_t on 64-bit). */
#define B7624_RLIM_INFINITY  UINT64_MAX

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7624_soft_le_hard(uint64_t u64Soft, uint64_t u64Hard)
{
	if (u64Soft <= u64Hard) {
		return 1u;
	}
	return 0u;
}

static uint32_t
b7624_is_inf(uint64_t u64Lim)
{
	if (u64Lim == B7624_RLIM_INFINITY) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_setrlimit_res_soft_le_hard_7624 - soft rlimit pair validity.
 *
 * Returns 1 if soft <= hard, else 0. Soft pure-data only; does not call
 * setrlimit. No parent wires.
 */
uint32_t
gj_setrlimit_res_soft_le_hard_7624(uint64_t soft, uint64_t hard)
{
	(void)NULL;
	return b7624_soft_le_hard(soft, hard);
}

/*
 * gj_setrlimit_res_is_inf_7624 - soft RLIM_INFINITY predicate.
 *
 * Returns 1 if lim == UINT64_MAX, else 0.
 */
uint32_t
gj_setrlimit_res_is_inf_7624(uint64_t lim)
{
	return b7624_is_inf(lim);
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_setrlimit_res_soft_le_hard_7624(uint64_t soft, uint64_t hard)
    __attribute__((alias("gj_setrlimit_res_soft_le_hard_7624")));

uint32_t __gj_setrlimit_res_is_inf_7624(uint64_t lim)
    __attribute__((alias("gj_setrlimit_res_is_inf_7624")));
