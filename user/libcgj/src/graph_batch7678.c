/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7678: gettimeofday soft success return id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_gtod_success_id_7678(void);
 *     - Return soft gettimeofday success status (0).
 *   uint32_t __gj_gtod_success_id_7678  (alias)
 *   __libcgj_batch7678_marker = "libcgj-batch7678"
 *
 * Exclusive continuum CREATE-ONLY (7671-7680: gettimeofday timeval stubs —
 * usec_per_sec, usec_max, usec_ok, tv_ok, tv_zero_p, tv_eq, tz_null_ok,
 * success_id, usec_norm_rem, batch_id_7680).
 * Unique gj_gtod_success_id_7678 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7678_marker[] = "libcgj-batch7678";

/* Soft gettimeofday(2) success return value. */
#define B7678_GTOD_SUCCESS ((uint32_t)0u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7678_gtod_success_id(void)
{
	return B7678_GTOD_SUCCESS;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_gtod_success_id_7678 - soft gettimeofday success status constant.
 *
 * Always returns 0 (POSIX success). Catalog id only; does not call
 * gettimeofday. No parent wires.
 */
uint32_t
gj_gtod_success_id_7678(void)
{
	(void)NULL;
	return b7678_gtod_success_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_gtod_success_id_7678(void)
    __attribute__((alias("gj_gtod_success_id_7678")));
