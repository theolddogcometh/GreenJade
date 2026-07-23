/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6631: timerfd clock id soft-ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_tfd_clock_id_ok_6631(uint32_t clockid);
 *     - Soft timerfd_create clockid check: 1 when clockid is one of
 *       REALTIME(0), MONOTONIC(1), BOOTTIME(7), REALTIME_ALARM(8),
 *       BOOTTIME_ALARM(9); else 0.
 *   uint32_t __gj_tfd_clock_id_ok_6631  (alias)
 *   __libcgj_batch6631_marker = "libcgj-batch6631"
 *
 * Exclusive continuum CREATE-ONLY (6631-6640: timerfd clock id stubs —
 * clock_id_ok_6631, realtime_p_6632, monotonic_p_6633, boottime_p_6634,
 * cloexec_ok_6635, nonblock_ok_6636, flags_pack_6637, abstime_p_6638,
 * clock_flags_pack_6639, clock_ready / batch_id_6640). Unique surface
 * only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6631_marker[] = "libcgj-batch6631";

/* Soft Linux-style clockid values accepted by timerfd_create. */
#define B6631_CLOCK_REALTIME        0u
#define B6631_CLOCK_MONOTONIC       1u
#define B6631_CLOCK_BOOTTIME        7u
#define B6631_CLOCK_REALTIME_ALARM  8u
#define B6631_CLOCK_BOOTTIME_ALARM  9u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6631_clock_ok(uint32_t u32Clock)
{
	if (u32Clock == B6631_CLOCK_REALTIME
	    || u32Clock == B6631_CLOCK_MONOTONIC
	    || u32Clock == B6631_CLOCK_BOOTTIME
	    || u32Clock == B6631_CLOCK_REALTIME_ALARM
	    || u32Clock == B6631_CLOCK_BOOTTIME_ALARM) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_tfd_clock_id_ok_6631 - soft timerfd clockid validity.
 *
 * clockid: soft clock identifier (integer code)
 *
 * Returns 1 when clockid is a soft-accepted timerfd clock, else 0.
 * Pure data only. No parent wires.
 */
uint32_t
gj_tfd_clock_id_ok_6631(uint32_t clockid)
{
	(void)NULL;
	return b6631_clock_ok(clockid);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_tfd_clock_id_ok_6631(uint32_t clockid)
    __attribute__((alias("gj_tfd_clock_id_ok_6631")));
