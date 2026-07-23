/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7677: gettimeofday timezone NULL ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_gtod_tz_null_ok_u_7677(uint32_t tz_is_null);
 *     - Return 1 always: NULL timezone arg is valid for gettimeofday.
 *   uint32_t __gj_gtod_tz_null_ok_u_7677  (alias)
 *   __libcgj_batch7677_marker = "libcgj-batch7677"
 *
 * Exclusive continuum CREATE-ONLY (7671-7680: gettimeofday timeval stubs —
 * usec_per_sec, usec_max, usec_ok, tv_ok, tv_zero_p, tv_eq, tz_null_ok,
 * success_id, usec_norm_rem, batch_id_7680).
 * Unique gj_gtod_tz_null_ok_u_7677 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7677_marker[] = "libcgj-batch7677";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7677_gtod_tz_null_ok(uint32_t u32TzIsNull)
{
	/* POSIX: tz may be NULL; non-NULL is also accepted by catalog. */
	(void)u32TzIsNull;
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_gtod_tz_null_ok_u_7677 - soft timezone-arg validity for gettimeofday.
 *
 * tz_is_null: non-zero if timezone pointer is NULL, else 0
 *
 * Always returns 1 (NULL or non-NULL tz both catalog-ok). Soft check;
 * does not call gettimeofday. No parent wires.
 */
uint32_t
gj_gtod_tz_null_ok_u_7677(uint32_t u32TzIsNull)
{
	(void)NULL;
	return b7677_gtod_tz_null_ok(u32TzIsNull);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_gtod_tz_null_ok_u_7677(uint32_t u32TzIsNull)
    __attribute__((alias("gj_gtod_tz_null_ok_u_7677")));
