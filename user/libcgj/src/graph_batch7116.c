/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7116: clock_gettime CLOCK_BOOTTIME id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_clk_boottime_id_7116(void);
 *     - Return soft CLOCK_BOOTTIME constant (7).
 *   uint32_t __gj_clk_boottime_id_7116  (alias)
 *   __libcgj_batch7116_marker = "libcgj-batch7116"
 *
 * Exclusive continuum CREATE-ONLY (7111-7120: clock_gettime clock id
 * stubs — realtime_id, monotonic_id, process_cputime_id,
 * thread_cputime_id, monotonic_raw_id, boottime_id, id_ok,
 * id_is_monotonic, id_errorish, batch_id_7120).
 * Unique gj_clk_boottime_id_7116 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7116_marker[] = "libcgj-batch7116";

/* CLOCK_BOOTTIME: monotonic including suspend time (7). */
#define B7116_CLOCK_BOOTTIME ((uint32_t)7u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7116_clk_boottime_id(void)
{
	return B7116_CLOCK_BOOTTIME;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_clk_boottime_id_7116 - soft CLOCK_BOOTTIME clock id constant.
 *
 * Always returns 7 (Linux CLOCK_BOOTTIME). Catalog id only; does not
 * call clock_gettime. No parent wires.
 */
uint32_t
gj_clk_boottime_id_7116(void)
{
	(void)NULL;
	return b7116_clk_boottime_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_clk_boottime_id_7116(void)
    __attribute__((alias("gj_clk_boottime_id_7116")));
