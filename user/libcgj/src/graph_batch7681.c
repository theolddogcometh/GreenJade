/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7681: timeval usec range stub (settimeofday).
 *
 * Surface (unique symbols):
 *   uint32_t gj_tv_usec_ok_u_7681(uint32_t usec);
 *     - Return 1 if usec is in [0, 999999], else 0.
 *   uint32_t __gj_tv_usec_ok_u_7681  (alias)
 *   __libcgj_batch7681_marker = "libcgj-batch7681"
 *
 * Exclusive continuum CREATE-ONLY (7681-7690: settimeofday stubs —
 * usec_ok, sec_ok, valid, is_zero, is_pos, usec_per_sec, usec_carry,
 * stime_usec_zero, req_ok, continuum + batch_id_7690). Unique
 * gj_tv_usec_ok_u_7681 surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7681_marker[] = "libcgj-batch7681";

/* Soft timeval tv_usec exclusive upper bound (1e6). */
#define B7681_USEC_PER_SEC  ((uint32_t)1000000u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7681_tv_usec_ok(uint32_t u32Usec)
{
	return (u32Usec < B7681_USEC_PER_SEC) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_tv_usec_ok_u_7681 - 1 if usec is a valid timeval microsecond field.
 *
 * usec: soft tv_usec candidate (unsigned)
 *
 * Returns 1 when usec is in [0, 999999], else 0.
 * Pure range test; no parent wires. Does not call settimeofday.
 */
uint32_t
gj_tv_usec_ok_u_7681(uint32_t u32Usec)
{
	(void)NULL;
	return b7681_tv_usec_ok(u32Usec);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_tv_usec_ok_u_7681(uint32_t u32Usec)
    __attribute__((alias("gj_tv_usec_ok_u_7681")));
