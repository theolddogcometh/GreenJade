/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7111: clock_gettime CLOCK_REALTIME id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_clk_realtime_id_7111(void);
 *     - Return soft CLOCK_REALTIME constant (0).
 *   uint32_t __gj_clk_realtime_id_7111  (alias)
 *   __libcgj_batch7111_marker = "libcgj-batch7111"
 *
 * Exclusive continuum CREATE-ONLY (7111-7120: clock_gettime clock id
 * stubs — realtime_id, monotonic_id, process_cputime_id,
 * thread_cputime_id, monotonic_raw_id, boottime_id, id_ok,
 * id_is_monotonic, id_errorish, batch_id_7120).
 * Unique gj_clk_realtime_id_7111 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7111_marker[] = "libcgj-batch7111";

/* CLOCK_REALTIME: system-wide realtime clock (0). */
#define B7111_CLOCK_REALTIME ((uint32_t)0u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7111_clk_realtime_id(void)
{
	return B7111_CLOCK_REALTIME;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_clk_realtime_id_7111 - soft CLOCK_REALTIME clock id constant.
 *
 * Always returns 0 (Linux CLOCK_REALTIME). Catalog id only; does not
 * call clock_gettime. No parent wires.
 */
uint32_t
gj_clk_realtime_id_7111(void)
{
	(void)NULL;
	return b7111_clk_realtime_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_clk_realtime_id_7111(void)
    __attribute__((alias("gj_clk_realtime_id_7111")));
