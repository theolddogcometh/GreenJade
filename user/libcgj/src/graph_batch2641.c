/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2641: uint8_t overflow-safe floor average
 * (exclusive _u).
 *
 * Surface (unique symbols):
 *   uint8_t gj_u8_avg_u(uint8_t a, uint8_t b);
 *     - Floor of (a + b) / 2 without overflow of a + b.
 *   uint8_t __gj_u8_avg_u  (alias)
 *   __libcgj_batch2641_marker = "libcgj-batch2641"
 *
 * Distinct from other avg surfaces — exclusive _u name only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2641_marker[] = "libcgj-batch2641";

/* ---- freestanding helpers ---------------------------------------------- */

/* Floor average via bit ops; no intermediate sum overflow. */
static uint8_t
b2641_avg(uint8_t u8A, uint8_t u8B)
{
	return (uint8_t)((u8A & u8B) + ((u8A ^ u8B) >> 1));
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u8_avg_u — floor((a + b) / 2) without overflow.
 *
 * Identity: (a & b) + ((a ^ b) >> 1)
 *   bits set in both contribute fully; differing bits contribute half
 *   of their XOR (carry-free floor of the unpaired bits).
 */
uint8_t
gj_u8_avg_u(uint8_t u8A, uint8_t u8B)
{
	(void)NULL;
	return b2641_avg(u8A, u8B);
}

/* ---- underscored alias ------------------------------------------------- */

uint8_t __gj_u8_avg_u(uint8_t u8A, uint8_t u8B)
    __attribute__((alias("gj_u8_avg_u")));
