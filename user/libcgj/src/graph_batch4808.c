/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4808: event-loop ready-event count pass-through.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ev_ready_count_u(uint32_t n);
 *     - Return the ready-event count n unchanged (identity helper for
 *       event-loop ready-set size reporting).
 *   uint32_t __gj_ev_ready_count_u  (alias)
 *   __libcgj_batch4808_marker = "libcgj-batch4808"
 *
 * Exclusive continuum CREATE-ONLY (4801-4810). Unique
 * gj_ev_ready_count_u surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4808_marker[] = "libcgj-batch4808";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4808_ready_count(uint32_t u32N)
{
	return u32N;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ev_ready_count_u - report an event-loop ready-event count.
 *
 * n: number of ready events
 *
 * Returns n unchanged. Soft integer surface for ready-set size plumbing.
 * No parent wires.
 */
uint32_t
gj_ev_ready_count_u(uint32_t u32N)
{
	(void)NULL;
	return b4808_ready_count(u32N);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ev_ready_count_u(uint32_t u32N)
    __attribute__((alias("gj_ev_ready_count_u")));
