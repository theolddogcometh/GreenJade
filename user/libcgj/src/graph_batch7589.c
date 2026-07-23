/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7589: nanosleep relative request validity stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_nanosleep_req_ok_u_7589(int32_t sec, uint32_t nsec);
 *     - Return 1 if relative nanosleep request fields are valid
 *       (sec >= 0 and nsec < 1e9), else 0.
 *   uint32_t __gj_nanosleep_req_ok_u_7589  (alias)
 *   __libcgj_batch7589_marker = "libcgj-batch7589"
 *
 * Exclusive continuum CREATE-ONLY (7581-7590: nanosleep timespec stubs —
 * nsec_ok, sec_ok, valid, is_zero, is_pos, abstime, nsec_per_sec,
 * nsec_carry, req_ok, continuum + batch_id_7590). Unique
 * gj_nanosleep_req_ok_u_7589 surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7589_marker[] = "libcgj-batch7589";

/* Soft timespec tv_nsec exclusive upper bound (1e9). */
#define B7589_NSEC_PER_SEC  ((uint32_t)1000000000u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7589_nanosleep_req_ok(int32_t i32Sec, uint32_t u32Nsec)
{
	if (i32Sec < 0)
		return 0u;
	if (u32Nsec >= B7589_NSEC_PER_SEC)
		return 0u;
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_nanosleep_req_ok_u_7589 - 1 if relative nanosleep req is field-valid.
 *
 * sec:  soft requested tv_sec
 * nsec: soft requested tv_nsec
 *
 * Returns 1 when fields form a valid relative nanosleep request
 * (non-negative sec, nsec in range). Zero duration is ok. Self-contained;
 * does not call sibling helpers. No parent wires. Does not call nanosleep.
 */
uint32_t
gj_nanosleep_req_ok_u_7589(int32_t i32Sec, uint32_t u32Nsec)
{
	(void)NULL;
	return b7589_nanosleep_req_ok(i32Sec, u32Nsec);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_nanosleep_req_ok_u_7589(int32_t i32Sec, uint32_t u32Nsec)
    __attribute__((alias("gj_nanosleep_req_ok_u_7589")));
