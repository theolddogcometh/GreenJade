/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2642: uint16_t overflow-safe floor average
 * (exclusive _u).
 *
 * Surface (unique symbols):
 *   uint16_t gj_u16_avg_u(uint16_t a, uint16_t b);
 *     - Floor of (a + b) / 2 without overflow of a + b.
 *   uint16_t __gj_u16_avg_u  (alias)
 *   __libcgj_batch2642_marker = "libcgj-batch2642"
 *
 * Distinct from other avg surfaces — exclusive _u name only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2642_marker[] = "libcgj-batch2642";

/* ---- freestanding helpers ---------------------------------------------- */

/* Floor average via bit ops; no intermediate sum overflow. */
static uint16_t
b2642_avg(uint16_t u16A, uint16_t u16B)
{
	return (uint16_t)((u16A & u16B) + ((u16A ^ u16B) >> 1));
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u16_avg_u — floor((a + b) / 2) without overflow.
 *
 * Identity: (a & b) + ((a ^ b) >> 1)
 *   bits set in both contribute fully; differing bits contribute half
 *   of their XOR (carry-free floor of the unpaired bits).
 */
uint16_t
gj_u16_avg_u(uint16_t u16A, uint16_t u16B)
{
	(void)NULL;
	return b2642_avg(u16A, u16B);
}

/* ---- underscored alias ------------------------------------------------- */

uint16_t __gj_u16_avg_u(uint16_t u16A, uint16_t u16B)
    __attribute__((alias("gj_u16_avg_u")));
