/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7672: gettimeofday timeval usec max stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_gtod_usec_max_7672(void);
 *     - Return soft max valid tv_usec (999999).
 *   uint32_t __gj_gtod_usec_max_7672  (alias)
 *   __libcgj_batch7672_marker = "libcgj-batch7672"
 *
 * Exclusive continuum CREATE-ONLY (7671-7680: gettimeofday timeval stubs —
 * usec_per_sec, usec_max, usec_ok, tv_ok, tv_zero_p, tv_eq, tz_null_ok,
 * success_id, usec_norm_rem, batch_id_7680).
 * Unique gj_gtod_usec_max_7672 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7672_marker[] = "libcgj-batch7672";

/* Soft timeval.tv_usec upper bound (1e6 - 1). */
#define B7672_USEC_MAX ((uint32_t)999999u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7672_gtod_usec_max(void)
{
	return B7672_USEC_MAX;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_gtod_usec_max_7672 - soft max valid timeval microsecond field.
 *
 * Always returns 999999. Catalog bound only; does not call gettimeofday.
 * No parent wires.
 */
uint32_t
gj_gtod_usec_max_7672(void)
{
	(void)NULL;
	return b7672_gtod_usec_max();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_gtod_usec_max_7672(void)
    __attribute__((alias("gj_gtod_usec_max_7672")));
