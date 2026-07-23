/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2646: uint16_t overflow-safe floor midpoint
 * (exclusive _u).
 *
 * Surface (unique symbols):
 *   uint16_t gj_u16_mid_u(uint16_t a, uint16_t b);
 *     - Floor midpoint of a and b without intermediate overflow.
 *   uint16_t __gj_u16_mid_u  (alias)
 *   __libcgj_batch2646_marker = "libcgj-batch2646"
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

const char __libcgj_batch2646_marker[] = "libcgj-batch2646";

/* ---- freestanding helpers ---------------------------------------------- */

/* Floor midpoint via bit ops; no intermediate sum overflow. */
static uint16_t
b2646_mid(uint16_t u16A, uint16_t u16B)
{
	return (uint16_t)((u16A & u16B) + ((u16A ^ u16B) >> 1));
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u16_mid_u — overflow-safe floor midpoint of two uint16_t values.
 *
 * Order of arguments does not matter: mid(a,b) == mid(b,a).
 * Equals floor((a + b) / 2) for all inputs.
 */
uint16_t
gj_u16_mid_u(uint16_t u16A, uint16_t u16B)
{
	(void)NULL;
	return b2646_mid(u16A, u16B);
}

/* ---- underscored alias ------------------------------------------------- */

uint16_t __gj_u16_mid_u(uint16_t u16A, uint16_t u16B)
    __attribute__((alias("gj_u16_mid_u")));
