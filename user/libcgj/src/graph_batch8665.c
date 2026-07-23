/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8665: ITIMER_VIRTUAL which-id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_itimer_virt_id_8665(void);
 *     - Return soft ITIMER_VIRTUAL which code (1).
 *   uint32_t __gj_itimer_virt_id_8665  (alias)
 *   __libcgj_batch8665_marker = "libcgj-batch8665"
 *
 * Exclusive continuum CREATE-ONLY (8661-8670: timerfd/clock id stubs —
 * timerfd_ready_u, timerfd_abstime_id, timerfd_reltime_id, itimer_real_id,
 * itimer_virt_id, itimer_prof_id, nanosleep_ready_u, clock_get_ready_u,
 * clock_set_ready_u, batch_id_8670). Unique gj_itimer_virt_id_8665
 * surface only; no multi-def. Catalog id only; no setitimer/getitimer.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8665_marker[] = "libcgj-batch8665";

/* Soft which: ITIMER_VIRTUAL (user-mode CPU interval timer). */
#define B8665_ITIMER_VIRT  ((uint32_t)1u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8665_virt_id(void)
{
	return B8665_ITIMER_VIRT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_itimer_virt_id_8665 - soft ITIMER_VIRTUAL which id.
 *
 * Always returns 1. Catalog id only; does not call setitimer. No parent
 * wires.
 */
uint32_t
gj_itimer_virt_id_8665(void)
{
	(void)NULL;
	return b8665_virt_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_itimer_virt_id_8665(void)
    __attribute__((alias("gj_itimer_virt_id_8665")));
