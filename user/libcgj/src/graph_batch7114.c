/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7114: clock_gettime CLOCK_THREAD_CPUTIME_ID
 * id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_clk_thread_cputime_id_7114(void);
 *     - Return soft CLOCK_THREAD_CPUTIME_ID constant (3).
 *   uint32_t __gj_clk_thread_cputime_id_7114  (alias)
 *   __libcgj_batch7114_marker = "libcgj-batch7114"
 *
 * Exclusive continuum CREATE-ONLY (7111-7120: clock_gettime clock id
 * stubs — realtime_id, monotonic_id, process_cputime_id,
 * thread_cputime_id, monotonic_raw_id, boottime_id, id_ok,
 * id_is_monotonic, id_errorish, batch_id_7120).
 * Unique gj_clk_thread_cputime_id_7114 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7114_marker[] = "libcgj-batch7114";

/* CLOCK_THREAD_CPUTIME_ID: high-resolution per-thread CPU time (3). */
#define B7114_CLOCK_THREAD_CPUTIME_ID ((uint32_t)3u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7114_clk_thread_cputime_id(void)
{
	return B7114_CLOCK_THREAD_CPUTIME_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_clk_thread_cputime_id_7114 - soft CLOCK_THREAD_CPUTIME_ID.
 *
 * Always returns 3 (Linux CLOCK_THREAD_CPUTIME_ID). Catalog id only;
 * does not call clock_gettime. No parent wires.
 */
uint32_t
gj_clk_thread_cputime_id_7114(void)
{
	(void)NULL;
	return b7114_clk_thread_cputime_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_clk_thread_cputime_id_7114(void)
    __attribute__((alias("gj_clk_thread_cputime_id_7114")));
