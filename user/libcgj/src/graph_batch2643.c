/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2643: uint32_t overflow-safe floor average
 * (exclusive _u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_avg_u(uint32_t a, uint32_t b);
 *     - Floor of (a + b) / 2 without overflow of a + b.
 *   uint32_t __gj_u32_avg_u  (alias)
 *   __libcgj_batch2643_marker = "libcgj-batch2643"
 *
 * Distinct from other avg surfaces — exclusive _u name only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2643_marker[] = "libcgj-batch2643";

/* ---- freestanding helpers ---------------------------------------------- */

/* Floor average via bit ops; no intermediate sum overflow. */
static uint32_t
b2643_avg(uint32_t u32A, uint32_t u32B)
{
	return (u32A & u32B) + ((u32A ^ u32B) >> 1);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_avg_u — floor((a + b) / 2) without overflow.
 *
 * Identity: (a & b) + ((a ^ b) >> 1)
 *   bits set in both contribute fully; differing bits contribute half
 *   of their XOR (carry-free floor of the unpaired bits).
 */
uint32_t
gj_u32_avg_u(uint32_t u32A, uint32_t u32B)
{
	(void)NULL;
	return b2643_avg(u32A, u32B);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_avg_u(uint32_t u32A, uint32_t u32B)
    __attribute__((alias("gj_u32_avg_u")));
