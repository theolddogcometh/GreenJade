/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch996: percentage of a u64 value.
 *
 * Surface (unique symbols):
 *   uint64_t gj_pct_of_u64(uint64_t x, uint64_t pct);
 *     — Return (x * pct) / 100.
 *   uint64_t __gj_pct_of_u64  (alias)
 *   __libcgj_batch996_marker = "libcgj-batch996"
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch996_marker[] = "libcgj-batch996";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pct_of_u64 — compute (x * pct) / 100.
 *
 * u64X:   base value
 * u64Pct: percentage (100 = 100%)
 *
 * Intermediate product wraps on overflow like plain C multiply.
 */
uint64_t
gj_pct_of_u64(uint64_t u64X, uint64_t u64Pct)
{
	return (u64X * u64Pct) / 100ull;
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_pct_of_u64(uint64_t u64X, uint64_t u64Pct)
    __attribute__((alias("gj_pct_of_u64")));
