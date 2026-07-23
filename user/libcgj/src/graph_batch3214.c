/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3214: bitwise OR of two uint32_t (u-suffix).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_or_u(uint32_t a, uint32_t b);
 *     - Return a | b. Pure integer; no builtins.
 *   uint32_t __gj_u32_or_u  (alias)
 *   __libcgj_batch3214_marker = "libcgj-batch3214"
 *
 * Milestone 3220 exclusive continuum CREATE-ONLY (3211-3220). Unique
 * gj_u32_or_u surface only; no multi-def. Distinct from
 * gj_u32_or_reduce (batch1245). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3214_marker[] = "libcgj-batch3214";

/* ---- freestanding helpers ---------------------------------------------- */

/* Bitwise OR of two 32-bit words. */
static uint32_t
b3214_or(uint32_t u32A, uint32_t u32B)
{
return u32A | u32B;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_or_u - bitwise OR of two uint32_t values.
 *
 * a: left operand
 * b: right operand
 *
 * Returns a | b. Does not call libc. No parent wires.
 */
uint32_t
gj_u32_or_u(uint32_t u32A, uint32_t u32B)
{
(void)NULL;
return b3214_or(u32A, u32B);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_or_u(uint32_t u32A, uint32_t u32B)
    __attribute__((alias("gj_u32_or_u")));
