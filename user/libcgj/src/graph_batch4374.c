/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4374: milliseconds to seconds floor (_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_ms_to_sec_u(uint64_t ms);
 *     - Return floor(ms / 1000) whole seconds.
 *   uint64_t __gj_ms_to_sec_u  (alias)
 *   __libcgj_batch4374_marker = "libcgj-batch4374"
 *
 * Exclusive continuum CREATE-ONLY (4371-4380). Inverse of
 * gj_sec_to_ms_u (batch4373). Distinct from gj_ms_to_sec_floor
 * (batch1974) and gj_ms_to_secs (batch485) — unique gj_ms_to_sec_u
 * surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4374_marker[] = "libcgj-batch4374";

/* Milliseconds per second (SI: 1 s = 1000 ms). */
#define B4374_MS_PER_SEC  1000ull

/* ---- freestanding helpers ---------------------------------------------- */

/* Floor-divide milliseconds by second size. */
static uint64_t
b4374_div_sec(uint64_t u64Ms)
{
	return u64Ms / B4374_MS_PER_SEC;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ms_to_sec_u - convert milliseconds to whole seconds (floor).
 *
 * ms: milliseconds
 *
 * Returns floor(ms / 1000). Examples: 0 → 0, 999 → 0, 1000 → 1.
 * Self-contained; no parent wires.
 */
uint64_t
gj_ms_to_sec_u(uint64_t u64Ms)
{
	(void)NULL;
	return b4374_div_sec(u64Ms);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_ms_to_sec_u(uint64_t u64Ms)
    __attribute__((alias("gj_ms_to_sec_u")));
