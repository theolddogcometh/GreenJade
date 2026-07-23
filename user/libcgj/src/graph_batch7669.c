/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7669: clock_t soft error-sentinel stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_clock_t_errorish_u_7669(uint32_t soft_clk);
 *     - Return 1 if soft_clk is the all-ones errorish sentinel
 *       (soft stand-in for (clock_t)-1), else 0.
 *   uint32_t __gj_clock_t_errorish_u_7669  (alias)
 *   __libcgj_batch7669_marker = "libcgj-batch7669"
 *
 * Exclusive continuum CREATE-ONLY (7661-7670: times clock_t stubs —
 * clocks_per_sec, tms_utime_id, tms_stime_id, tms_cutime_id,
 * tms_cstime_id, is_utime, field_ok, field_pack, clock_t_errorish,
 * continuum + batch_id_7670). Unique gj_clock_t_errorish_u_7669 surface
 * only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7669_marker[] = "libcgj-batch7669";

/*
 * Soft (clock_t)-1 as uint32_t all-ones. times()/clock() failure returns
 * (clock_t)-1; freestanding stub uses this 32-bit view only.
 */
#define B7669_CLOCK_T_ERROR  ((uint32_t)0xFFFFFFFFu)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7669_errorish(uint32_t u32SoftClk)
{
	if (u32SoftClk == B7669_CLOCK_T_ERROR) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_clock_t_errorish_u_7669 - 1 if soft_clk is the error sentinel.
 *
 * soft_clk: soft clock_t value as uint32_t (all-ones = errorish)
 *
 * Returns 1 when soft_clk is 0xFFFFFFFF; else 0. Soft catalog check;
 * does not call times() or clock(). No parent wires.
 */
uint32_t
gj_clock_t_errorish_u_7669(uint32_t soft_clk)
{
	(void)NULL;
	return b7669_errorish(soft_clk);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_clock_t_errorish_u_7669(uint32_t soft_clk)
    __attribute__((alias("gj_clock_t_errorish_u_7669")));
