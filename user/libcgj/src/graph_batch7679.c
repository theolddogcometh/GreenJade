/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7679: gettimeofday usec normalize remainder stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_gtod_usec_norm_rem_u_7679(uint32_t usec);
 *     - Return usec modulo 1000000 (soft normalized tv_usec remainder).
 *   uint32_t __gj_gtod_usec_norm_rem_u_7679  (alias)
 *   __libcgj_batch7679_marker = "libcgj-batch7679"
 *
 * Exclusive continuum CREATE-ONLY (7671-7680: gettimeofday timeval stubs —
 * usec_per_sec, usec_max, usec_ok, tv_ok, tv_zero_p, tv_eq, tz_null_ok,
 * success_id, usec_norm_rem, batch_id_7680).
 * Unique gj_gtod_usec_norm_rem_u_7679 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7679_marker[] = "libcgj-batch7679";

/* Soft timeval scale: 1 second = 1_000_000 microseconds. */
#define B7679_USEC_PER_SEC ((uint32_t)1000000u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7679_gtod_usec_norm_rem(uint32_t u32Usec)
{
	return u32Usec % B7679_USEC_PER_SEC;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_gtod_usec_norm_rem_u_7679 - soft-normalize usec into [0, 999999].
 *
 * usec: raw microsecond catalog value (may be >= 1000000)
 *
 * Returns usec % 1000000. Pure integer remainder; does not call
 * gettimeofday. No parent wires.
 */
uint32_t
gj_gtod_usec_norm_rem_u_7679(uint32_t u32Usec)
{
	(void)NULL;
	return b7679_gtod_usec_norm_rem(u32Usec);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_gtod_usec_norm_rem_u_7679(uint32_t u32Usec)
    __attribute__((alias("gj_gtod_usec_norm_rem_u_7679")));
