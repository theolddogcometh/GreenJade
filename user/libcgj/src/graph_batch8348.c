/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8348: CLOCK_REALTIME identity tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_clock_realtime_id_8348(void);
 *     - Returns 0: soft CLOCK_REALTIME catalog id for this exclusive
 *       time/clock continuum (POSIX CLOCK_REALTIME == 0).
 *   uint32_t __gj_clock_realtime_id_8348  (alias)
 *   __libcgj_batch8348_marker = "libcgj-batch8348"
 *
 * Exclusive continuum CREATE-ONLY (8341-8350: time/clock unit stubs).
 * Distinct from gj_tfd_clock_realtime_p_6632 (batch6632 predicate).
 * Unique gj_clock_realtime_id_8348 surface only; no multi-def. No
 * parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8348_marker[] = "libcgj-batch8348";

/* Soft CLOCK_REALTIME identity (POSIX clockid value 0). */
#define B8348_CLOCK_REALTIME_ID  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8348_realtime_id(void)
{
	return B8348_CLOCK_REALTIME_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_clock_realtime_id_8348 - CLOCK_REALTIME identity tag.
 *
 * Always returns 0. Soft pure data product tag for the continuum;
 * does not call clock_gettime. No parent wires.
 */
uint32_t
gj_clock_realtime_id_8348(void)
{
	(void)NULL;
	return b8348_realtime_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_clock_realtime_id_8348(void)
    __attribute__((alias("gj_clock_realtime_id_8348")));
