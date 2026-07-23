/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7682: timeval sec absolute-ok stub (settimeofday).
 *
 * Surface (unique symbols):
 *   uint32_t gj_tv_sec_ok_i_7682(int32_t sec);
 *     - Return 1 for any signed epoch second (absolute wall clock admits
 *       pre-epoch times).
 *   uint32_t __gj_tv_sec_ok_i_7682  (alias)
 *   __libcgj_batch7682_marker = "libcgj-batch7682"
 *
 * Exclusive continuum CREATE-ONLY (7681-7690: settimeofday stubs —
 * usec_ok, sec_ok, valid, is_zero, is_pos, usec_per_sec, usec_carry,
 * stime_usec_zero, req_ok, continuum + batch_id_7690). Unique
 * gj_tv_sec_ok_i_7682 surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7682_marker[] = "libcgj-batch7682";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7682_tv_sec_ok(int32_t i32Sec)
{
	/* Absolute wall-clock settimeofday admits any epoch second,
	 * including pre-1970 (negative) values. Soft int32 field always ok. */
	(void)i32Sec;
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_tv_sec_ok_i_7682 - 1 if sec is an admissible absolute epoch second.
 *
 * sec: soft tv_sec candidate (signed)
 *
 * Always returns 1. Absolute settimeofday requests accept any signed
 * epoch second. Pure admit test; no parent wires. Does not call
 * settimeofday.
 */
uint32_t
gj_tv_sec_ok_i_7682(int32_t i32Sec)
{
	(void)NULL;
	return b7682_tv_sec_ok(i32Sec);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_tv_sec_ok_i_7682(int32_t i32Sec)
    __attribute__((alias("gj_tv_sec_ok_i_7682")));
