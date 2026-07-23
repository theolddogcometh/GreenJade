/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2645: uint8_t overflow-safe floor midpoint
 * (exclusive _u).
 *
 * Surface (unique symbols):
 *   uint8_t gj_u8_mid_u(uint8_t a, uint8_t b);
 *     - Floor midpoint of a and b without intermediate overflow.
 *   uint8_t __gj_u8_mid_u  (alias)
 *   __libcgj_batch2645_marker = "libcgj-batch2645"
 *
 * Distinct from avg_u / other mid surfaces — exclusive _u name only;
 * no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2645_marker[] = "libcgj-batch2645";

/* ---- freestanding helpers ---------------------------------------------- */

/* Floor midpoint via bit ops; no intermediate sum overflow. */
static uint8_t
b2645_mid(uint8_t u8A, uint8_t u8B)
{
	return (uint8_t)((u8A & u8B) + ((u8A ^ u8B) >> 1));
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u8_mid_u — overflow-safe floor midpoint of two uint8_t values.
 *
 * Order of arguments does not matter: mid(a,b) == mid(b,a).
 * Equals floor((a + b) / 2) for all inputs.
 */
uint8_t
gj_u8_mid_u(uint8_t u8A, uint8_t u8B)
{
	(void)NULL;
	return b2645_mid(u8A, u8B);
}

/* ---- underscored alias ------------------------------------------------- */

uint8_t __gj_u8_mid_u(uint8_t u8A, uint8_t u8B)
    __attribute__((alias("gj_u8_mid_u")));
