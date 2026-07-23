/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7583: timespec field validity stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ts_valid_u_7583(int32_t sec, uint32_t nsec);
 *     - Return 1 if sec >= 0 and nsec in [0, 999999999], else 0.
 *   uint32_t __gj_ts_valid_u_7583  (alias)
 *   __libcgj_batch7583_marker = "libcgj-batch7583"
 *
 * Exclusive continuum CREATE-ONLY (7581-7590: nanosleep timespec stubs —
 * nsec_ok, sec_ok, valid, is_zero, is_pos, abstime, nsec_per_sec,
 * nsec_carry, req_ok, continuum + batch_id_7590). Unique
 * gj_ts_valid_u_7583 surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7583_marker[] = "libcgj-batch7583";

/* Soft timespec tv_nsec exclusive upper bound (1e9). */
#define B7583_NSEC_PER_SEC  ((uint32_t)1000000000u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7583_ts_valid(int32_t i32Sec, uint32_t u32Nsec)
{
	if (i32Sec < 0)
		return 0u;
	if (u32Nsec >= B7583_NSEC_PER_SEC)
		return 0u;
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ts_valid_u_7583 - 1 if soft timespec fields are nanosleep-valid.
 *
 * sec:  soft tv_sec (signed)
 * nsec: soft tv_nsec (unsigned)
 *
 * Returns 1 when sec >= 0 and nsec < 1e9. Self-contained; does not call
 * sibling gj_ts_* helpers. No parent wires. Does not call nanosleep.
 */
uint32_t
gj_ts_valid_u_7583(int32_t i32Sec, uint32_t u32Nsec)
{
	(void)NULL;
	return b7583_ts_valid(i32Sec, u32Nsec);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ts_valid_u_7583(int32_t i32Sec, uint32_t u32Nsec)
    __attribute__((alias("gj_ts_valid_u_7583")));
