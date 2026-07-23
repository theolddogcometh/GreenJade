/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7684: timeval epoch-zero stub (settimeofday).
 *
 * Surface (unique symbols):
 *   uint32_t gj_tv_is_zero_u_7684(int32_t sec, uint32_t usec);
 *     - Return 1 if sec == 0 and usec == 0 (Unix epoch), else 0.
 *   uint32_t __gj_tv_is_zero_u_7684  (alias)
 *   __libcgj_batch7684_marker = "libcgj-batch7684"
 *
 * Exclusive continuum CREATE-ONLY (7681-7690: settimeofday stubs —
 * usec_ok, sec_ok, valid, is_zero, is_pos, usec_per_sec, usec_carry,
 * stime_usec_zero, req_ok, continuum + batch_id_7690). Unique
 * gj_tv_is_zero_u_7684 surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7684_marker[] = "libcgj-batch7684";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7684_tv_is_zero(int32_t i32Sec, uint32_t u32Usec)
{
	return (i32Sec == 0 && u32Usec == 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_tv_is_zero_u_7684 - 1 if soft timeval is the Unix epoch (zero).
 *
 * sec:  soft tv_sec
 * usec: soft tv_usec
 *
 * Returns 1 when both fields are zero (1970-01-01 00:00:00 UTC).
 * Pure equality test; no parent wires. Does not call settimeofday.
 */
uint32_t
gj_tv_is_zero_u_7684(int32_t i32Sec, uint32_t u32Usec)
{
	(void)NULL;
	return b7684_tv_is_zero(i32Sec, u32Usec);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_tv_is_zero_u_7684(int32_t i32Sec, uint32_t u32Usec)
    __attribute__((alias("gj_tv_is_zero_u_7684")));
