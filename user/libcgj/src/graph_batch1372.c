/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1372: overflow-safe uint64_t midpoint.
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_midpoint(uint64_t a, uint64_t b);
 *     — Floor midpoint of a and b without intermediate overflow:
 *       (a & b) + ((a ^ b) >> 1). Equals floor((a + b) / 2) for all
 *       uint64_t inputs (including when a + b would wrap).
 *   uint64_t __gj_u64_midpoint  (alias)
 *   __libcgj_batch1372_marker = "libcgj-batch1372"
 *
 * Unique surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1372_marker[] = "libcgj-batch1372";

/* ---- freestanding helpers ---------------------------------------------- */

/* Floor average via bit ops; no carry into a high bit that would wrap. */
static uint64_t
b1372_mid_u64(uint64_t u64A, uint64_t u64B)
{
	return (u64A & u64B) + ((u64A ^ u64B) >> 1);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_midpoint — overflow-safe floor midpoint of two uint64_t values.
 *
 * Order of arguments does not matter: midpoint(a,b) == midpoint(b,a).
 */
uint64_t
gj_u64_midpoint(uint64_t u64A, uint64_t u64B)
{
	return b1372_mid_u64(u64A, u64B);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_midpoint(uint64_t u64A, uint64_t u64B)
    __attribute__((alias("gj_u64_midpoint")));
