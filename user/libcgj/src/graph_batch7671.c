/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7671: gettimeofday USEC_PER_SEC id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_gtod_usec_per_sec_7671(void);
 *     - Return soft microseconds-per-second constant (1000000).
 *   uint32_t __gj_gtod_usec_per_sec_7671  (alias)
 *   __libcgj_batch7671_marker = "libcgj-batch7671"
 *
 * Exclusive continuum CREATE-ONLY (7671-7680: gettimeofday timeval stubs —
 * usec_per_sec, usec_max, usec_ok, tv_ok, tv_zero_p, tv_eq, tz_null_ok,
 * success_id, usec_norm_rem, batch_id_7680).
 * Unique gj_gtod_usec_per_sec_7671 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7671_marker[] = "libcgj-batch7671";

/* Soft timeval scale: 1 second = 1_000_000 microseconds. */
#define B7671_USEC_PER_SEC ((uint32_t)1000000u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7671_gtod_usec_per_sec(void)
{
	return B7671_USEC_PER_SEC;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_gtod_usec_per_sec_7671 - soft USEC_PER_SEC for timeval catalogs.
 *
 * Always returns 1000000. Catalog id only; does not call gettimeofday.
 * No parent wires.
 */
uint32_t
gj_gtod_usec_per_sec_7671(void)
{
	(void)NULL;
	return b7671_gtod_usec_per_sec();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_gtod_usec_per_sec_7671(void)
    __attribute__((alias("gj_gtod_usec_per_sec_7671")));
