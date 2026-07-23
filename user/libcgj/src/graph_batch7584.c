/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7584: timespec zero-duration stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ts_is_zero_u_7584(int32_t sec, uint32_t nsec);
 *     - Return 1 if sec == 0 and nsec == 0, else 0.
 *   uint32_t __gj_ts_is_zero_u_7584  (alias)
 *   __libcgj_batch7584_marker = "libcgj-batch7584"
 *
 * Exclusive continuum CREATE-ONLY (7581-7590: nanosleep timespec stubs —
 * nsec_ok, sec_ok, valid, is_zero, is_pos, abstime, nsec_per_sec,
 * nsec_carry, req_ok, continuum + batch_id_7590). Unique
 * gj_ts_is_zero_u_7584 surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7584_marker[] = "libcgj-batch7584";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7584_ts_is_zero(int32_t i32Sec, uint32_t u32Nsec)
{
	return (i32Sec == 0 && u32Nsec == 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ts_is_zero_u_7584 - 1 if soft timespec is a zero duration.
 *
 * sec:  soft tv_sec
 * nsec: soft tv_nsec
 *
 * Returns 1 when both fields are zero (zero-length nanosleep request).
 * Pure equality test; no parent wires. Does not call nanosleep.
 */
uint32_t
gj_ts_is_zero_u_7584(int32_t i32Sec, uint32_t u32Nsec)
{
	(void)NULL;
	return b7584_ts_is_zero(i32Sec, u32Nsec);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ts_is_zero_u_7584(int32_t i32Sec, uint32_t u32Nsec)
    __attribute__((alias("gj_ts_is_zero_u_7584")));
