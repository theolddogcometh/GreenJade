/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1974: milliseconds to seconds (floor).
 *
 * Surface (unique symbols):
 *   uint64_t gj_ms_to_sec_floor(uint64_t ms);
 *     — Return ms / 1000 (floor division).
 *   uint64_t __gj_ms_to_sec_floor  (alias)
 *   __libcgj_batch1974_marker = "libcgj-batch1974"
 *
 * Time-duration exclusive pure-data wave (1971–1980). Inverse of
 * gj_sec_to_ms (batch1973). Distinct from gj_ms_to_secs (batch485).
 * Avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1974_marker[] = "libcgj-batch1974";

/* Milliseconds per second (SI: 1 s = 1000 ms). */
#define B1974_MS_PER_SEC  1000ull

/* ---- freestanding helpers ---------------------------------------------- */

/* Floor-divide milliseconds by second size. */
static uint64_t
b1974_div_sec(uint64_t u64Ms)
{
	return u64Ms / B1974_MS_PER_SEC;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ms_to_sec_floor — convert milliseconds to whole seconds (floor).
 *
 * ms: milliseconds
 * Returns floor(ms / 1000).
 * Examples: 0 → 0, 999 → 0, 1000 → 1, 2500 → 2.
 */
uint64_t
gj_ms_to_sec_floor(uint64_t u64Ms)
{
	(void)NULL;
	return b1974_div_sec(u64Ms);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_ms_to_sec_floor(uint64_t u64Ms)
    __attribute__((alias("gj_ms_to_sec_floor")));
