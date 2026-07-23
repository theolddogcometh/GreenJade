/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7673: gettimeofday timeval usec range stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_gtod_usec_ok_u_7673(uint32_t usec);
 *     - Return 1 if usec is in [0, 999999], else 0.
 *   uint32_t __gj_gtod_usec_ok_u_7673  (alias)
 *   __libcgj_batch7673_marker = "libcgj-batch7673"
 *
 * Exclusive continuum CREATE-ONLY (7671-7680: gettimeofday timeval stubs —
 * usec_per_sec, usec_max, usec_ok, tv_ok, tv_zero_p, tv_eq, tz_null_ok,
 * success_id, usec_norm_rem, batch_id_7680).
 * Unique gj_gtod_usec_ok_u_7673 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7673_marker[] = "libcgj-batch7673";

/* Soft timeval.tv_usec upper bound (1e6 - 1). */
#define B7673_USEC_MAX ((uint32_t)999999u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7673_gtod_usec_ok(uint32_t u32Usec)
{
	return (u32Usec <= B7673_USEC_MAX) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_gtod_usec_ok_u_7673 - 1 if usec is a valid timeval microsecond field.
 *
 * usec: timeval.tv_usec as unsigned soft catalog value
 *
 * Returns 1 when usec is in [0, 999999], else 0. Soft range check;
 * does not call gettimeofday. No parent wires.
 */
uint32_t
gj_gtod_usec_ok_u_7673(uint32_t u32Usec)
{
	(void)NULL;
	return b7673_gtod_usec_ok(u32Usec);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_gtod_usec_ok_u_7673(uint32_t u32Usec)
    __attribute__((alias("gj_gtod_usec_ok_u_7673")));
