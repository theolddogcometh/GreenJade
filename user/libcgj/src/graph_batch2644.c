/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2644: uint64_t overflow-safe floor average
 * (exclusive _u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_avg_u(uint64_t a, uint64_t b);
 *     - Floor of (a + b) / 2 without overflow of a + b.
 *   uint64_t __gj_u64_avg_u  (alias)
 *   __libcgj_batch2644_marker = "libcgj-batch2644"
 *
 * Distinct from gj_u64_avg (batch615) / gj_avg_u64 (batch381) — exclusive
 * _u name surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2644_marker[] = "libcgj-batch2644";

/* ---- freestanding helpers ---------------------------------------------- */

/* Floor average via bit ops; no intermediate sum overflow. */
static uint64_t
b2644_avg(uint64_t u64A, uint64_t u64B)
{
	return (u64A & u64B) + ((u64A ^ u64B) >> 1);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_avg_u — floor((a + b) / 2) without overflow.
 *
 * Identity: (a & b) + ((a ^ b) >> 1)
 *   bits set in both contribute fully; differing bits contribute half
 *   of their XOR (carry-free floor of the unpaired bits).
 */
uint64_t
gj_u64_avg_u(uint64_t u64A, uint64_t u64B)
{
	(void)NULL;
	return b2644_avg(u64A, u64B);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_avg_u(uint64_t u64A, uint64_t u64B)
    __attribute__((alias("gj_u64_avg_u")));
