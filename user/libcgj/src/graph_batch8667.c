/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8667: nanosleep continuum ready lamp stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_nanosleep_ready_u_8667(void);
 *     - Return 1 (soft lamp: nanosleep path catalog ready).
 *   uint32_t __gj_nanosleep_ready_u_8667  (alias)
 *   __libcgj_batch8667_marker = "libcgj-batch8667"
 *
 * Exclusive continuum CREATE-ONLY (8661-8670: timerfd/clock id stubs —
 * timerfd_ready_u, timerfd_abstime_id, timerfd_reltime_id, itimer_real_id,
 * itimer_virt_id, itimer_prof_id, nanosleep_ready_u, clock_get_ready_u,
 * clock_set_ready_u, batch_id_8670). Unique gj_nanosleep_ready_u_8667
 * surface only; no multi-def. Soft pure-data tag only; no nanosleep
 * syscall. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8667_marker[] = "libcgj-batch8667";

/* Soft continuum-ready lamp for nanosleep path catalog. */
#define B8667_NANOSLEEP_READY  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8667_ready(void)
{
	return B8667_NANOSLEEP_READY;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_nanosleep_ready_u_8667 - soft nanosleep continuum ready lamp.
 *
 * Always returns 1. Pure-data product tag that the nanosleep ready
 * surface is present in the 8661-8670 wave. Does not call nanosleep.
 * No parent wires.
 */
uint32_t
gj_nanosleep_ready_u_8667(void)
{
	(void)NULL;
	return b8667_ready();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_nanosleep_ready_u_8667(void)
    __attribute__((alias("gj_nanosleep_ready_u_8667")));
