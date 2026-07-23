/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7686: timeval microseconds-per-second constant.
 *
 * Surface (unique symbols):
 *   uint32_t gj_tv_usec_per_sec_7686(void);
 *     - Returns soft USEC_PER_SEC (1000000).
 *   uint32_t __gj_tv_usec_per_sec_7686  (alias)
 *   __libcgj_batch7686_marker = "libcgj-batch7686"
 *
 * Exclusive continuum CREATE-ONLY (7681-7690: settimeofday stubs —
 * usec_ok, sec_ok, valid, is_zero, is_pos, usec_per_sec, usec_carry,
 * stime_usec_zero, req_ok, continuum + batch_id_7690). Unique
 * gj_tv_usec_per_sec_7686 surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7686_marker[] = "libcgj-batch7686";

/* Soft microseconds per second for timeval normalization (1e6). */
#define B7686_USEC_PER_SEC  ((uint32_t)1000000u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7686_usec_per_sec(void)
{
	return B7686_USEC_PER_SEC;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_tv_usec_per_sec_7686 - soft USEC_PER_SEC baseline for timeval.
 *
 * Always returns 1000000. Soft pure-data product tag used by
 * settimeofday/timeval normalization stubs. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_tv_usec_per_sec_7686(void)
{
	(void)NULL;
	return b7686_usec_per_sec();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_tv_usec_per_sec_7686(void)
    __attribute__((alias("gj_tv_usec_per_sec_7686")));
