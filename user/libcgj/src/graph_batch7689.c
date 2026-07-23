/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7689: settimeofday request validity stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_settod_req_ok_u_7689(int32_t sec, uint32_t usec);
 *     - Return 1 if settimeofday request fields are valid
 *       (usec < 1e6; sec any epoch second), else 0.
 *   uint32_t __gj_settod_req_ok_u_7689  (alias)
 *   __libcgj_batch7689_marker = "libcgj-batch7689"
 *
 * Exclusive continuum CREATE-ONLY (7681-7690: settimeofday stubs —
 * usec_ok, sec_ok, valid, is_zero, is_pos, usec_per_sec, usec_carry,
 * stime_usec_zero, req_ok, continuum + batch_id_7690). Unique
 * gj_settod_req_ok_u_7689 surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7689_marker[] = "libcgj-batch7689";

/* Soft timeval tv_usec exclusive upper bound (1e6). */
#define B7689_USEC_PER_SEC  ((uint32_t)1000000u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7689_settod_req_ok(int32_t i32Sec, uint32_t u32Usec)
{
	(void)i32Sec;
	if (u32Usec >= B7689_USEC_PER_SEC)
		return 0u;
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_settod_req_ok_u_7689 - 1 if settimeofday request fields are valid.
 *
 * sec:  soft requested tv_sec (any epoch second admitted)
 * usec: soft requested tv_usec
 *
 * Returns 1 when fields form a valid settimeofday request (usec in
 * range). Self-contained; does not call sibling helpers. No parent
 * wires. Does not call settimeofday.
 */
uint32_t
gj_settod_req_ok_u_7689(int32_t i32Sec, uint32_t u32Usec)
{
	(void)NULL;
	return b7689_settod_req_ok(i32Sec, u32Usec);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_settod_req_ok_u_7689(int32_t i32Sec, uint32_t u32Usec)
    __attribute__((alias("gj_settod_req_ok_u_7689")));
