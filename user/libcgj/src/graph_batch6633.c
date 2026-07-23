/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6633: timerfd CLOCK_MONOTONIC predicate stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_tfd_clock_monotonic_p_6633(uint32_t clockid);
 *     - Soft predicate: 1 when clockid == CLOCK_MONOTONIC (1); else 0.
 *   uint32_t __gj_tfd_clock_monotonic_p_6633  (alias)
 *   __libcgj_batch6633_marker = "libcgj-batch6633"
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

const char __libcgj_batch6633_marker[] = "libcgj-batch6633";

#define B6633_CLOCK_MONOTONIC  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6633_monotonic_p(uint32_t u32Clock)
{
	if (u32Clock == B6633_CLOCK_MONOTONIC) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_tfd_clock_monotonic_p_6633 - soft CLOCK_MONOTONIC predicate.
 *
 * clockid: soft clock identifier
 *
 * Returns 1 when clockid is CLOCK_MONOTONIC (1), else 0.
 * Pure integer. No parent wires.
 */
uint32_t
gj_tfd_clock_monotonic_p_6633(uint32_t clockid)
{
	(void)NULL;
	return b6633_monotonic_p(clockid);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_tfd_clock_monotonic_p_6633(uint32_t clockid)
    __attribute__((alias("gj_tfd_clock_monotonic_p_6633")));
