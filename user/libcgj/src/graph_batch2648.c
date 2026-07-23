/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2648: uint64_t overflow-safe floor midpoint
 * (exclusive _u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_mid_u(uint64_t a, uint64_t b);
 *     - Floor midpoint of a and b without intermediate overflow.
 *   uint64_t __gj_u64_mid_u  (alias)
 *   __libcgj_batch2648_marker = "libcgj-batch2648"
 *
 * Distinct from gj_u64_midpoint (batch1372) / avg surfaces — exclusive
 * _u name only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2648_marker[] = "libcgj-batch2648";

/* ---- freestanding helpers ---------------------------------------------- */

/* Floor midpoint via bit ops; no intermediate sum overflow. */
static uint64_t
b2648_mid(uint64_t u64A, uint64_t u64B)
{
	return (u64A & u64B) + ((u64A ^ u64B) >> 1);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_mid_u — overflow-safe floor midpoint of two uint64_t values.
 *
 * Order of arguments does not matter: mid(a,b) == mid(b,a).
 * Equals floor((a + b) / 2) for all inputs (including when a + b wraps).
 */
uint64_t
gj_u64_mid_u(uint64_t u64A, uint64_t u64B)
{
	(void)NULL;
	return b2648_mid(u64A, u64B);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_mid_u(uint64_t u64A, uint64_t u64B)
    __attribute__((alias("gj_u64_mid_u")));
