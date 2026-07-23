/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2647: uint32_t overflow-safe floor midpoint
 * (exclusive _u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_mid_u(uint32_t a, uint32_t b);
 *     - Floor midpoint of a and b without intermediate overflow.
 *   uint32_t __gj_u32_mid_u  (alias)
 *   __libcgj_batch2647_marker = "libcgj-batch2647"
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

const char __libcgj_batch2647_marker[] = "libcgj-batch2647";

/* ---- freestanding helpers ---------------------------------------------- */

/* Floor midpoint via bit ops; no intermediate sum overflow. */
static uint32_t
b2647_mid(uint32_t u32A, uint32_t u32B)
{
	return (u32A & u32B) + ((u32A ^ u32B) >> 1);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_mid_u — overflow-safe floor midpoint of two uint32_t values.
 *
 * Order of arguments does not matter: mid(a,b) == mid(b,a).
 * Equals floor((a + b) / 2) for all inputs.
 */
uint32_t
gj_u32_mid_u(uint32_t u32A, uint32_t u32B)
{
	(void)NULL;
	return b2647_mid(u32A, u32B);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_mid_u(uint32_t u32A, uint32_t u32B)
    __attribute__((alias("gj_u32_mid_u")));
