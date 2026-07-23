/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7582: timespec sec non-negative stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ts_sec_ok_i_7582(int32_t sec);
 *     - Return 1 if sec >= 0 (relative nanosleep-friendly), else 0.
 *   uint32_t __gj_ts_sec_ok_i_7582  (alias)
 *   __libcgj_batch7582_marker = "libcgj-batch7582"
 *
 * Exclusive continuum CREATE-ONLY (7581-7590: nanosleep timespec stubs —
 * nsec_ok, sec_ok, valid, is_zero, is_pos, abstime, nsec_per_sec,
 * nsec_carry, req_ok, continuum + batch_id_7590). Unique
 * gj_ts_sec_ok_i_7582 surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7582_marker[] = "libcgj-batch7582";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7582_ts_sec_ok(int32_t i32Sec)
{
	return (i32Sec >= 0) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ts_sec_ok_i_7582 - 1 if sec is non-negative for relative sleep.
 *
 * sec: soft tv_sec candidate (signed)
 *
 * Returns 1 when sec >= 0, else 0. Relative nanosleep requests reject
 * negative seconds. Pure sign test; no parent wires. Does not call
 * nanosleep.
 */
uint32_t
gj_ts_sec_ok_i_7582(int32_t i32Sec)
{
	(void)NULL;
	return b7582_ts_sec_ok(i32Sec);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ts_sec_ok_i_7582(int32_t i32Sec)
    __attribute__((alias("gj_ts_sec_ok_i_7582")));
