/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7683: timeval field validity stub (settimeofday).
 *
 * Surface (unique symbols):
 *   uint32_t gj_tv_valid_u_7683(int32_t sec, uint32_t usec);
 *     - Return 1 if usec is in [0, 999999] (sec always admitted), else 0.
 *   uint32_t __gj_tv_valid_u_7683  (alias)
 *   __libcgj_batch7683_marker = "libcgj-batch7683"
 *
 * Exclusive continuum CREATE-ONLY (7681-7690: settimeofday stubs —
 * usec_ok, sec_ok, valid, is_zero, is_pos, usec_per_sec, usec_carry,
 * stime_usec_zero, req_ok, continuum + batch_id_7690). Unique
 * gj_tv_valid_u_7683 surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7683_marker[] = "libcgj-batch7683";

/* Soft timeval tv_usec exclusive upper bound (1e6). */
#define B7683_USEC_PER_SEC  ((uint32_t)1000000u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7683_tv_valid(int32_t i32Sec, uint32_t u32Usec)
{
	(void)i32Sec;
	if (u32Usec >= B7683_USEC_PER_SEC)
		return 0u;
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_tv_valid_u_7683 - 1 if soft timeval fields are settimeofday-valid.
 *
 * sec:  soft tv_sec (signed; any epoch second admitted)
 * usec: soft tv_usec (unsigned)
 *
 * Returns 1 when usec < 1e6. Self-contained; does not call sibling
 * gj_tv_* helpers. No parent wires. Does not call settimeofday.
 */
uint32_t
gj_tv_valid_u_7683(int32_t i32Sec, uint32_t u32Usec)
{
	(void)NULL;
	return b7683_tv_valid(i32Sec, u32Usec);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_tv_valid_u_7683(int32_t i32Sec, uint32_t u32Usec)
    __attribute__((alias("gj_tv_valid_u_7683")));
