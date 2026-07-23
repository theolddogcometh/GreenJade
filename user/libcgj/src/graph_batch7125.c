/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7125: timer_create CLOCK_MONOTONIC_RAW stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_tc_clk_monotonic_raw_u_7125(uint32_t clockid);
 *     - Return 1 if clockid is CLOCK_MONOTONIC_RAW (4), else 0.
 *   uint32_t __gj_tc_clk_monotonic_raw_u_7125  (alias)
 *   __libcgj_batch7125_marker = "libcgj-batch7125"
 *
 * Exclusive continuum CREATE-ONLY (7121-7130: timer_create clock stubs —
 * realtime, monotonic, process_cputime, thread_cputime, monotonic_raw,
 * boottime, timer_abstime, sigev_none, clk_ok, batch_id_7130). Unique
 * gj_tc_clk_monotonic_raw_u_7125 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7125_marker[] = "libcgj-batch7125";

/* Soft CLOCK_MONOTONIC_RAW clockid (value 4; Linux extension). */
#define B7125_CLOCK_MONOTONIC_RAW ((uint32_t)4u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7125_tc_clk_monotonic_raw(uint32_t u32Clk)
{
	return (u32Clk == B7125_CLOCK_MONOTONIC_RAW) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_tc_clk_monotonic_raw_u_7125 - 1 if clockid is CLOCK_MONOTONIC_RAW.
 *
 * clockid: timer_create / clock_* clockid_t argument
 *
 * Returns 1 when clockid equals CLOCK_MONOTONIC_RAW (4), else 0.
 * Pure integer compare; no parent wires.
 */
uint32_t
gj_tc_clk_monotonic_raw_u_7125(uint32_t u32Clockid)
{
	(void)NULL;
	return b7125_tc_clk_monotonic_raw(u32Clockid);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_tc_clk_monotonic_raw_u_7125(uint32_t u32Clockid)
    __attribute__((alias("gj_tc_clk_monotonic_raw_u_7125")));
