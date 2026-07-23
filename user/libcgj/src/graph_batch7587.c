/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7587: timespec nanoseconds-per-second constant.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ts_nsec_per_sec_7587(void);
 *     - Returns soft NSEC_PER_SEC (1000000000).
 *   uint32_t __gj_ts_nsec_per_sec_7587  (alias)
 *   __libcgj_batch7587_marker = "libcgj-batch7587"
 *
 * Exclusive continuum CREATE-ONLY (7581-7590: nanosleep timespec stubs —
 * nsec_ok, sec_ok, valid, is_zero, is_pos, abstime, nsec_per_sec,
 * nsec_carry, req_ok, continuum + batch_id_7590). Unique
 * gj_ts_nsec_per_sec_7587 surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7587_marker[] = "libcgj-batch7587";

/* Soft nanoseconds per second for timespec normalization (1e9). */
#define B7587_NSEC_PER_SEC  ((uint32_t)1000000000u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7587_nsec_per_sec(void)
{
	return B7587_NSEC_PER_SEC;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ts_nsec_per_sec_7587 - soft NSEC_PER_SEC baseline for timespec.
 *
 * Always returns 1000000000. Soft pure-data product tag used by
 * nanosleep/timespec normalization stubs. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_ts_nsec_per_sec_7587(void)
{
	(void)NULL;
	return b7587_nsec_per_sec();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ts_nsec_per_sec_7587(void)
    __attribute__((alias("gj_ts_nsec_per_sec_7587")));
