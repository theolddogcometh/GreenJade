/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7112: clock_gettime CLOCK_MONOTONIC id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_clk_monotonic_id_7112(void);
 *     - Return soft CLOCK_MONOTONIC constant (1).
 *   uint32_t __gj_clk_monotonic_id_7112  (alias)
 *   __libcgj_batch7112_marker = "libcgj-batch7112"
 *
 * Exclusive continuum CREATE-ONLY (7111-7120: clock_gettime clock id
 * stubs — realtime_id, monotonic_id, process_cputime_id,
 * thread_cputime_id, monotonic_raw_id, boottime_id, id_ok,
 * id_is_monotonic, id_errorish, batch_id_7120).
 * Unique gj_clk_monotonic_id_7112 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7112_marker[] = "libcgj-batch7112";

/* CLOCK_MONOTONIC: monotonic clock since some unspecified epoch (1). */
#define B7112_CLOCK_MONOTONIC ((uint32_t)1u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7112_clk_monotonic_id(void)
{
	return B7112_CLOCK_MONOTONIC;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_clk_monotonic_id_7112 - soft CLOCK_MONOTONIC clock id constant.
 *
 * Always returns 1 (Linux CLOCK_MONOTONIC). Catalog id only; does not
 * call clock_gettime. No parent wires.
 */
uint32_t
gj_clk_monotonic_id_7112(void)
{
	(void)NULL;
	return b7112_clk_monotonic_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_clk_monotonic_id_7112(void)
    __attribute__((alias("gj_clk_monotonic_id_7112")));
