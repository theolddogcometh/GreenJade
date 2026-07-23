/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7688: stime path usec-zero stub (settimeofday).
 *
 * Surface (unique symbols):
 *   uint32_t gj_stime_usec_zero_u_7688(uint32_t usec);
 *     - Return 1 if usec == 0 (legacy stime→settimeofday path), else 0.
 *   uint32_t __gj_stime_usec_zero_u_7688  (alias)
 *   __libcgj_batch7688_marker = "libcgj-batch7688"
 *
 * Exclusive continuum CREATE-ONLY (7681-7690: settimeofday stubs —
 * usec_ok, sec_ok, valid, is_zero, is_pos, usec_per_sec, usec_carry,
 * stime_usec_zero, req_ok, continuum + batch_id_7690). Unique
 * gj_stime_usec_zero_u_7688 surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7688_marker[] = "libcgj-batch7688";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7688_stime_usec_zero(uint32_t u32Usec)
{
	return (u32Usec == 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_stime_usec_zero_u_7688 - 1 if usec is zero (stime→settimeofday path).
 *
 * usec: soft tv_usec for a settimeofday request that models stime(2)
 *
 * Legacy stime sets timeval.tv_usec to 0 before calling settimeofday.
 * Returns 1 when usec is zero. Pure equality test; no parent wires.
 * Does not call settimeofday or stime.
 */
uint32_t
gj_stime_usec_zero_u_7688(uint32_t u32Usec)
{
	(void)NULL;
	return b7688_stime_usec_zero(u32Usec);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_stime_usec_zero_u_7688(uint32_t u32Usec)
    __attribute__((alias("gj_stime_usec_zero_u_7688")));
