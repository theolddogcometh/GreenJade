/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8341: time seconds validity stub (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_time_sec_ok_u_8341(uint64_t sec);
 *     - Soft predicate: 1 for any unsigned seconds value (unsigned
 *       second fields are always in-range for duration/ts use).
 *   uint32_t __gj_time_sec_ok_u_8341  (alias)
 *   __libcgj_batch8341_marker = "libcgj-batch8341"
 *
 * Exclusive continuum CREATE-ONLY (8341-8350: time/clock unit stubs —
 * sec_ok, nsec_ok, timespec_norm, ms_to_ns, us_to_ns, ns_to_ms,
 * clock_mono_id, clock_realtime_id, time_epoch_id, batch_id). Unique
 * gj_time_sec_ok_u_8341 surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8341_marker[] = "libcgj-batch8341";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8341_sec_ok(uint64_t u64Sec)
{
	/* Unsigned seconds have no negative/out-of-range domain. */
	(void)u64Sec;
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_time_sec_ok_u_8341 - soft seconds-field validity predicate.
 *
 * sec: unsigned whole-seconds component
 *
 * Always returns 1. Soft pure-integer unit stub; no parent wires.
 */
uint32_t
gj_time_sec_ok_u_8341(uint64_t sec)
{
	(void)NULL;
	return b8341_sec_ok(sec);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_time_sec_ok_u_8341(uint64_t sec)
    __attribute__((alias("gj_time_sec_ok_u_8341")));
