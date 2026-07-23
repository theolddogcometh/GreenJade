/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4802: event-loop timeout clamp to INT_MAX.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ev_timeout_ms_u(uint32_t ms);
 *     - Clamp a millisecond timeout into [0, INT_MAX] as uint32_t.
 *       Values above 0x7fffffff saturate to INT_MAX; lower bound is
 *       already 0 in the unsigned domain.
 *   uint32_t __gj_ev_timeout_ms_u  (alias)
 *   __libcgj_batch4802_marker = "libcgj-batch4802"
 *
 * Exclusive continuum CREATE-ONLY (4801-4810). Unique
 * gj_ev_timeout_ms_u surface only; no multi-def. Distinct from
 * gj_u32_clamp01_u (batch4001) and gj_u32_clamp_u (batch2703).
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4802_marker[] = "libcgj-batch4802";

/* INT_MAX as freestanding u32 (limits.h not available). */
#define B4802_INT_MAX  0x7fffffffu

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4802_timeout_ms(uint32_t u32Ms)
{
	if (u32Ms > B4802_INT_MAX) {
		return B4802_INT_MAX;
	}
	return u32Ms;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ev_timeout_ms_u - clamp an event-loop timeout in milliseconds to INT_MAX.
 *
 * ms: requested timeout in milliseconds (unsigned)
 *
 * Returns ms when ms <= INT_MAX (0x7fffffff), otherwise INT_MAX.
 * Suitable for APIs that accept signed int timeouts. No parent wires.
 */
uint32_t
gj_ev_timeout_ms_u(uint32_t u32Ms)
{
	(void)NULL;
	return b4802_timeout_ms(u32Ms);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ev_timeout_ms_u(uint32_t u32Ms)
    __attribute__((alias("gj_ev_timeout_ms_u")));
