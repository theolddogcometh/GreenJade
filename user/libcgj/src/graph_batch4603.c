/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4603: wrap-safe unsigned tick elapsed.
 *
 * Surface (unique symbols):
 *   uint64_t gj_tick_elapsed_u(uint64_t start, uint64_t now);
 *     - Modular forward distance now - start on free-running u64 tick
 *       counters. Unsigned wrap is correct (C modular arithmetic).
 *   uint64_t __gj_tick_elapsed_u  (alias)
 *   __libcgj_batch4603_marker = "libcgj-batch4603"
 *
 * Exclusive continuum CREATE-ONLY (4601-4610). Unique gj_tick_elapsed_u
 * surface only; no multi-def. Distinct from gj_tick_sub_u (batch4602)
 * which saturates rather than wrapping. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4603_marker[] = "libcgj-batch4603";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * b4603_elapsed — modular (now - start). Free-running counters wrap;
 * unsigned subtract yields the forward elapsed interval.
 */
static uint64_t
b4603_elapsed(uint64_t u64Start, uint64_t u64Now)
{
	return u64Now - u64Start;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_tick_elapsed_u - wrap-safe unsigned elapsed ticks.
 *
 * start: tick sample at interval begin
 * now:   current tick sample (same free-running domain)
 *
 * Returns (now - start) modulo 2^64. Example: start=UINT64_MAX,
 * now=1 → 2. Self-contained; no parent wires.
 */
uint64_t
gj_tick_elapsed_u(uint64_t u64Start, uint64_t u64Now)
{
	(void)NULL;
	return b4603_elapsed(u64Start, u64Now);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_tick_elapsed_u(uint64_t u64Start, uint64_t u64Now)
    __attribute__((alias("gj_tick_elapsed_u")));
