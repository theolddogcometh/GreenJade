/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8662: timerfd absolute-time flag id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_timerfd_abstime_id_8662(void);
 *     - Return soft TFD_TIMER_ABSTIME flag code (1).
 *   uint32_t __gj_timerfd_abstime_id_8662  (alias)
 *   __libcgj_batch8662_marker = "libcgj-batch8662"
 *
 * Exclusive continuum CREATE-ONLY (8661-8670: timerfd/clock id stubs —
 * timerfd_ready_u, timerfd_abstime_id, timerfd_reltime_id, itimer_real_id,
 * itimer_virt_id, itimer_prof_id, nanosleep_ready_u, clock_get_ready_u,
 * clock_set_ready_u, batch_id_8670). Unique gj_timerfd_abstime_id_8662
 * surface only; no multi-def. Catalog id only; no timerfd_settime. No
 * parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8662_marker[] = "libcgj-batch8662";

/* Soft flag: TFD_TIMER_ABSTIME (absolute expiry). */
#define B8662_TFD_ABSTIME  ((uint32_t)1u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8662_abstime_id(void)
{
	return B8662_TFD_ABSTIME;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_timerfd_abstime_id_8662 - soft TFD_TIMER_ABSTIME flag id.
 *
 * Always returns 1. Catalog id only; does not call timerfd_settime. No
 * parent wires.
 */
uint32_t
gj_timerfd_abstime_id_8662(void)
{
	(void)NULL;
	return b8662_abstime_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_timerfd_abstime_id_8662(void)
    __attribute__((alias("gj_timerfd_abstime_id_8662")));
