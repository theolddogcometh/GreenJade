/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8666: ITIMER_PROF which-id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_itimer_prof_id_8666(void);
 *     - Return soft ITIMER_PROF which code (2).
 *   uint32_t __gj_itimer_prof_id_8666  (alias)
 *   __libcgj_batch8666_marker = "libcgj-batch8666"
 *
 * Exclusive continuum CREATE-ONLY (8661-8670: timerfd/clock id stubs —
 * timerfd_ready_u, timerfd_abstime_id, timerfd_reltime_id, itimer_real_id,
 * itimer_virt_id, itimer_prof_id, nanosleep_ready_u, clock_get_ready_u,
 * clock_set_ready_u, batch_id_8670). Unique gj_itimer_prof_id_8666
 * surface only; no multi-def. Catalog id only; no setitimer/getitimer.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8666_marker[] = "libcgj-batch8666";

/* Soft which: ITIMER_PROF (user+system CPU interval timer). */
#define B8666_ITIMER_PROF  ((uint32_t)2u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8666_prof_id(void)
{
	return B8666_ITIMER_PROF;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_itimer_prof_id_8666 - soft ITIMER_PROF which id.
 *
 * Always returns 2. Catalog id only; does not call setitimer. No parent
 * wires.
 */
uint32_t
gj_itimer_prof_id_8666(void)
{
	(void)NULL;
	return b8666_prof_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_itimer_prof_id_8666(void)
    __attribute__((alias("gj_itimer_prof_id_8666")));
