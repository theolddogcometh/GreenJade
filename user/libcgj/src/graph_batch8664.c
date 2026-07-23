/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8664: ITIMER_REAL which-id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_itimer_real_id_8664(void);
 *     - Return soft ITIMER_REAL which code (0).
 *   uint32_t __gj_itimer_real_id_8664  (alias)
 *   __libcgj_batch8664_marker = "libcgj-batch8664"
 *
 * Exclusive continuum CREATE-ONLY (8661-8670: timerfd/clock id stubs —
 * timerfd_ready_u, timerfd_abstime_id, timerfd_reltime_id, itimer_real_id,
 * itimer_virt_id, itimer_prof_id, nanosleep_ready_u, clock_get_ready_u,
 * clock_set_ready_u, batch_id_8670). Unique gj_itimer_real_id_8664
 * surface only; no multi-def. Catalog id only; no setitimer/getitimer.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8664_marker[] = "libcgj-batch8664";

/* Soft which: ITIMER_REAL (wall-clock interval timer). */
#define B8664_ITIMER_REAL  ((uint32_t)0u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8664_real_id(void)
{
	return B8664_ITIMER_REAL;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_itimer_real_id_8664 - soft ITIMER_REAL which id.
 *
 * Always returns 0. Catalog id only; does not call setitimer. No parent
 * wires.
 */
uint32_t
gj_itimer_real_id_8664(void)
{
	(void)NULL;
	return b8664_real_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_itimer_real_id_8664(void)
    __attribute__((alias("gj_itimer_real_id_8664")));
