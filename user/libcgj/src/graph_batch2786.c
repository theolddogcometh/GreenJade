/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2786: uint64_t overflow-safe floor midpoint
 * (_u full name).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_midpoint_u(uint64_t a, uint64_t b);
 *     - Floor midpoint of a and b without intermediate overflow.
 *       Equals floor((a + b) / 2) for all uint64_t inputs.
 *   uint64_t __gj_u64_midpoint_u  (alias)
 *   __libcgj_batch2786_marker = "libcgj-batch2786"
 *
 * Integer-root exclusive wave (2781-2790). Distinct from gj_u64_midpoint
 * (batch1372) and gj_u64_mid_u (batch2648) — unique midpoint_u name;
 * no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No __int128. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2786_marker[] = "libcgj-batch2786";

/* ---- freestanding helpers ---------------------------------------------- */

/* Floor midpoint via bit ops; no intermediate sum overflow. */
static uint64_t
b2786_mid(uint64_t u64A, uint64_t u64B)
{
	return (u64A & u64B) + ((u64A ^ u64B) >> 1);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_midpoint_u - overflow-safe floor midpoint of two uint64_t values.
 *
 * a, b: endpoints (order does not matter: mid(a,b) == mid(b,a))
 *
 * Returns floor((a + b) / 2) for all inputs, including when a + b
 * would wrap as a plain sum.
 */
uint64_t
gj_u64_midpoint_u(uint64_t u64A, uint64_t u64B)
{
	(void)NULL;
	return b2786_mid(u64A, u64B);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_midpoint_u(uint64_t u64A, uint64_t u64B)
    __attribute__((alias("gj_u64_midpoint_u")));
