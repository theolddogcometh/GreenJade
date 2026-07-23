/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7115: clock_gettime CLOCK_MONOTONIC_RAW id
 * stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_clk_monotonic_raw_id_7115(void);
 *     - Return soft CLOCK_MONOTONIC_RAW constant (4).
 *   uint32_t __gj_clk_monotonic_raw_id_7115  (alias)
 *   __libcgj_batch7115_marker = "libcgj-batch7115"
 *
 * Exclusive continuum CREATE-ONLY (7111-7120: clock_gettime clock id
 * stubs — realtime_id, monotonic_id, process_cputime_id,
 * thread_cputime_id, monotonic_raw_id, boottime_id, id_ok,
 * id_is_monotonic, id_errorish, batch_id_7120).
 * Unique gj_clk_monotonic_raw_id_7115 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7115_marker[] = "libcgj-batch7115";

/* CLOCK_MONOTONIC_RAW: hardware-based monotonic, no NTP adjust (4). */
#define B7115_CLOCK_MONOTONIC_RAW ((uint32_t)4u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7115_clk_monotonic_raw_id(void)
{
	return B7115_CLOCK_MONOTONIC_RAW;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_clk_monotonic_raw_id_7115 - soft CLOCK_MONOTONIC_RAW clock id.
 *
 * Always returns 4 (Linux CLOCK_MONOTONIC_RAW). Catalog id only; does
 * not call clock_gettime. No parent wires.
 */
uint32_t
gj_clk_monotonic_raw_id_7115(void)
{
	(void)NULL;
	return b7115_clk_monotonic_raw_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_clk_monotonic_raw_id_7115(void)
    __attribute__((alias("gj_clk_monotonic_raw_id_7115")));
