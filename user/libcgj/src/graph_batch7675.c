/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7675: gettimeofday timeval zero predicate stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_gtod_tv_zero_p_u_7675(uint32_t sec, uint32_t usec);
 *     - Return 1 if both sec and usec are zero, else 0.
 *   uint32_t __gj_gtod_tv_zero_p_u_7675  (alias)
 *   __libcgj_batch7675_marker = "libcgj-batch7675"
 *
 * Exclusive continuum CREATE-ONLY (7671-7680: gettimeofday timeval stubs —
 * usec_per_sec, usec_max, usec_ok, tv_ok, tv_zero_p, tv_eq, tz_null_ok,
 * success_id, usec_norm_rem, batch_id_7680).
 * Unique gj_gtod_tv_zero_p_u_7675 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7675_marker[] = "libcgj-batch7675";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7675_gtod_tv_zero_p(uint32_t u32Sec, uint32_t u32Usec)
{
	return (u32Sec == 0u && u32Usec == 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_gtod_tv_zero_p_u_7675 - 1 if soft timeval is the zero interval.
 *
 * sec:  timeval.tv_sec as unsigned catalog value
 * usec: timeval.tv_usec as unsigned catalog value
 *
 * Returns 1 when both fields are 0, else 0. Soft predicate; does not
 * call gettimeofday. No parent wires.
 */
uint32_t
gj_gtod_tv_zero_p_u_7675(uint32_t u32Sec, uint32_t u32Usec)
{
	(void)NULL;
	return b7675_gtod_tv_zero_p(u32Sec, u32Usec);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_gtod_tv_zero_p_u_7675(uint32_t u32Sec, uint32_t u32Usec)
    __attribute__((alias("gj_gtod_tv_zero_p_u_7675")));
