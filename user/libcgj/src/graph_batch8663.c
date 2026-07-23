/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8663: timerfd relative-time flag id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_timerfd_reltime_id_8663(void);
 *     - Return soft relative-time flag code (0; no TFD_TIMER_ABSTIME).
 *   uint32_t __gj_timerfd_reltime_id_8663  (alias)
 *   __libcgj_batch8663_marker = "libcgj-batch8663"
 *
 * Exclusive continuum CREATE-ONLY (8661-8670: timerfd/clock id stubs —
 * timerfd_ready_u, timerfd_abstime_id, timerfd_reltime_id, itimer_real_id,
 * itimer_virt_id, itimer_prof_id, nanosleep_ready_u, clock_get_ready_u,
 * clock_set_ready_u, batch_id_8670). Unique gj_timerfd_reltime_id_8663
 * surface only; no multi-def. Catalog id only; no timerfd_settime. No
 * parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8663_marker[] = "libcgj-batch8663";

/* Soft flag: relative expiry (flags == 0, not absolute). */
#define B8663_TFD_RELTIME  ((uint32_t)0u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8663_reltime_id(void)
{
	return B8663_TFD_RELTIME;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_timerfd_reltime_id_8663 - soft relative-time timerfd flag id.
 *
 * Always returns 0. Catalog id only; does not call timerfd_settime. No
 * parent wires.
 */
uint32_t
gj_timerfd_reltime_id_8663(void)
{
	(void)NULL;
	return b8663_reltime_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_timerfd_reltime_id_8663(void)
    __attribute__((alias("gj_timerfd_reltime_id_8663")));
