/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7129: timer_create clockid soft-ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_tc_clk_ok_u_7129(uint32_t clockid);
 *     - Return 1 if clockid is a soft-known clock covered by this wave
 *       (0,1,2,3,4,7), else 0.
 *   uint32_t __gj_tc_clk_ok_u_7129  (alias)
 *   __libcgj_batch7129_marker = "libcgj-batch7129"
 *
 * Exclusive continuum CREATE-ONLY (7121-7130: timer_create clock stubs —
 * realtime, monotonic, process_cputime, thread_cputime, monotonic_raw,
 * boottime, timer_abstime, sigev_none, clk_ok, batch_id_7130). Unique
 * gj_tc_clk_ok_u_7129 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7129_marker[] = "libcgj-batch7129";

/* Soft-known clockids covered by this exclusive wave. */
#define B7129_CLOCK_REALTIME           ((uint32_t)0u)
#define B7129_CLOCK_MONOTONIC          ((uint32_t)1u)
#define B7129_CLOCK_PROCESS_CPUTIME_ID ((uint32_t)2u)
#define B7129_CLOCK_THREAD_CPUTIME_ID  ((uint32_t)3u)
#define B7129_CLOCK_MONOTONIC_RAW      ((uint32_t)4u)
#define B7129_CLOCK_BOOTTIME           ((uint32_t)7u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7129_tc_clk_ok(uint32_t u32Clk)
{
	if (u32Clk == B7129_CLOCK_REALTIME ||
	    u32Clk == B7129_CLOCK_MONOTONIC ||
	    u32Clk == B7129_CLOCK_PROCESS_CPUTIME_ID ||
	    u32Clk == B7129_CLOCK_THREAD_CPUTIME_ID ||
	    u32Clk == B7129_CLOCK_MONOTONIC_RAW ||
	    u32Clk == B7129_CLOCK_BOOTTIME) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_tc_clk_ok_u_7129 - 1 if clockid is a soft-known wave clockid.
 *
 * clockid: timer_create / clock_* clockid_t argument
 *
 * Returns 1 for REALTIME/MONOTONIC/PROCESS_CPUTIME_ID/
 * THREAD_CPUTIME_ID/MONOTONIC_RAW/BOOTTIME (0,1,2,3,4,7), else 0.
 * Soft catalog check; no parent wires.
 */
uint32_t
gj_tc_clk_ok_u_7129(uint32_t u32Clockid)
{
	(void)NULL;
	return b7129_tc_clk_ok(u32Clockid);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_tc_clk_ok_u_7129(uint32_t u32Clockid)
    __attribute__((alias("gj_tc_clk_ok_u_7129")));
