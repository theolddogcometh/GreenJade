/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2785: uint32_t overflow-safe floor midpoint
 * (_u full name).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_midpoint_u(uint32_t a, uint32_t b);
 *     - Floor midpoint of a and b without intermediate overflow.
 *       Equals floor((a + b) / 2) for all uint32_t inputs.
 *   uint32_t __gj_u32_midpoint_u  (alias)
 *   __libcgj_batch2785_marker = "libcgj-batch2785"
 *
 * Integer-root exclusive wave (2781-2790). Distinct from gj_u32_mid_u
 * (batch2647) — unique midpoint_u name; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No __int128. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2785_marker[] = "libcgj-batch2785";

/* ---- freestanding helpers ---------------------------------------------- */

/* Floor midpoint via bit ops; no intermediate sum overflow. */
static uint32_t
b2785_mid(uint32_t u32A, uint32_t u32B)
{
	return (u32A & u32B) + ((u32A ^ u32B) >> 1);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_midpoint_u - overflow-safe floor midpoint of two uint32_t values.
 *
 * a, b: endpoints (order does not matter: mid(a,b) == mid(b,a))
 *
 * Returns floor((a + b) / 2) for all inputs, including when a + b
 * would wrap as a plain sum.
 */
uint32_t
gj_u32_midpoint_u(uint32_t u32A, uint32_t u32B)
{
	(void)NULL;
	return b2785_mid(u32A, u32B);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_midpoint_u(uint32_t u32A, uint32_t u32B)
    __attribute__((alias("gj_u32_midpoint_u")));
