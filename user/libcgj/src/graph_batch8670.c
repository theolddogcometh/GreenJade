/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8670: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_8670(void);
 *     - Returns the compile-time graph batch number for this TU (8670).
 *   uint32_t __gj_batch_id_8670  (alias)
 *   __libcgj_batch8670_marker = "libcgj-batch8670"
 *
 * Exclusive continuum CREATE-ONLY (8661-8670: timerfd/clock id stubs —
 * timerfd_ready_u, timerfd_abstime_id, timerfd_reltime_id, itimer_real_id,
 * itimer_virt_id, itimer_prof_id, nanosleep_ready_u, clock_get_ready_u,
 * clock_set_ready_u, batch_id_8670). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* symbols — avoid multi-def. Soft
 * pure-data catalog only; no timer/clock syscalls. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8670_marker[] = "libcgj-batch8670";

#define B8670_BATCH_ID  8670u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8670_id(void)
{
	return B8670_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_8670 - report this TU's graph batch number.
 *
 * Always returns 8670. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_8670(void)
{
	(void)NULL;
	return b8670_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_8670(void)
    __attribute__((alias("gj_batch_id_8670")));
