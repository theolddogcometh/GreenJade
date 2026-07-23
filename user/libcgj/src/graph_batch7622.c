/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7622: setrlimit RLIMIT resource value table.
 *
 * Surface (unique symbols):
 *   uint32_t gj_setrlimit_res_value_7622(uint32_t res_id);
 *     - Map resource id (0..15) to Linux x86_64 RLIMIT_* number
 *       (identity table). Unknown res_id → 0xFFFFFFFFu.
 *   uint32_t gj_setrlimit_res_count_7622(void);
 *     - Number of known RLIMIT resources in the table (always 16).
 *   uint32_t __gj_setrlimit_res_value_7622  (alias)
 *   uint32_t __gj_setrlimit_res_count_7622  (alias)
 *   __libcgj_batch7622_marker = "libcgj-batch7622"
 *
 * Exclusive continuum CREATE-ONLY (7621-7630: setrlimit resource stubs).
 * Unique *_7622 surfaces only; no multi-def. Soft pure-data RLIMIT_*
 * table only; does not call setrlimit. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7622_marker[] = "libcgj-batch7622";

#define B7622_RES_MAX  16u
#define B7622_RES_BAD  0xFFFFFFFFu

/*
 * Linux x86_64 RLIMIT_* resource numbers (local freestanding copies):
 *   0  RLIMIT_CPU        8  RLIMIT_MEMLOCK
 *   1  RLIMIT_FSIZE      9  RLIMIT_AS
 *   2  RLIMIT_DATA      10  RLIMIT_LOCKS
 *   3  RLIMIT_STACK     11  RLIMIT_SIGPENDING
 *   4  RLIMIT_CORE      12  RLIMIT_MSGQUEUE
 *   5  RLIMIT_RSS       13  RLIMIT_NICE
 *   6  RLIMIT_NPROC     14  RLIMIT_RTPRIO
 *   7  RLIMIT_NOFILE    15  RLIMIT_RTTIME
 * RLIM_NLIMITS = 16
 */
static const uint32_t s_b7622_res[B7622_RES_MAX] = {
	0u,  1u,  2u,  3u,  4u,  5u,  6u,  7u,
	8u,  9u, 10u, 11u, 12u, 13u, 14u, 15u
};

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7622_value(uint32_t u32Res)
{
	if (u32Res >= B7622_RES_MAX) {
		return B7622_RES_BAD;
	}
	return s_b7622_res[u32Res];
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_setrlimit_res_value_7622 - map resource id to RLIMIT_* number.
 *
 * res_id: 0..15 known; else 0xFFFFFFFF. Soft table only; does not call
 * setrlimit. No parent wires.
 */
uint32_t
gj_setrlimit_res_value_7622(uint32_t res_id)
{
	(void)NULL;
	return b7622_value(res_id);
}

/*
 * gj_setrlimit_res_count_7622 - known RLIMIT resource count.
 *
 * Always returns 16 (RLIM_NLIMITS).
 */
uint32_t
gj_setrlimit_res_count_7622(void)
{
	return B7622_RES_MAX;
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_setrlimit_res_value_7622(uint32_t res_id)
    __attribute__((alias("gj_setrlimit_res_value_7622")));

uint32_t __gj_setrlimit_res_count_7622(void)
    __attribute__((alias("gj_setrlimit_res_count_7622")));
