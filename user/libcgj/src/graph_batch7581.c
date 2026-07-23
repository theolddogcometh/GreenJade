/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7581: timespec nsec range stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ts_nsec_ok_u_7581(uint32_t nsec);
 *     - Return 1 if nsec is in [0, 999999999], else 0.
 *   uint32_t __gj_ts_nsec_ok_u_7581  (alias)
 *   __libcgj_batch7581_marker = "libcgj-batch7581"
 *
 * Exclusive continuum CREATE-ONLY (7581-7590: nanosleep timespec stubs —
 * nsec_ok, sec_ok, valid, is_zero, is_pos, abstime, nsec_per_sec,
 * nsec_carry, req_ok, continuum + batch_id_7590). Unique
 * gj_ts_nsec_ok_u_7581 surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7581_marker[] = "libcgj-batch7581";

/* Soft timespec tv_nsec exclusive upper bound (1e9). */
#define B7581_NSEC_PER_SEC  ((uint32_t)1000000000u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7581_ts_nsec_ok(uint32_t u32Nsec)
{
	return (u32Nsec < B7581_NSEC_PER_SEC) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ts_nsec_ok_u_7581 - 1 if nsec is a valid timespec nanosecond field.
 *
 * nsec: soft tv_nsec candidate (unsigned)
 *
 * Returns 1 when nsec is in [0, 999999999], else 0.
 * Pure range test; no parent wires. Does not call nanosleep.
 */
uint32_t
gj_ts_nsec_ok_u_7581(uint32_t u32Nsec)
{
	(void)NULL;
	return b7581_ts_nsec_ok(u32Nsec);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ts_nsec_ok_u_7581(uint32_t u32Nsec)
    __attribute__((alias("gj_ts_nsec_ok_u_7581")));
