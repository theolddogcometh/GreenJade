/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3217: bitwise XOR of two uint64_t (u-suffix).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_xor_u(uint64_t a, uint64_t b);
 *     - Return a ^ b. Pure integer; no builtins.
 *   uint64_t __gj_u64_xor_u  (alias)
 *   __libcgj_batch3217_marker = "libcgj-batch3217"
 *
 * Milestone 3220 exclusive continuum CREATE-ONLY (3211-3220). Unique
 * gj_u64_xor_u surface only; no multi-def. Distinct from
 * gj_bitmap_xor_u (batch2861) / gj_u32_xor_u (batch3216). No parent
 * wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3217_marker[] = "libcgj-batch3217";

/* ---- freestanding helpers ---------------------------------------------- */

/* Bitwise XOR of two 64-bit words. */
static uint64_t
b3217_xor(uint64_t u64A, uint64_t u64B)
{
return u64A ^ u64B;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_xor_u - bitwise XOR of two uint64_t values.
 *
 * a: left operand
 * b: right operand
 *
 * Returns a ^ b. Does not call libc. No parent wires.
 */
uint64_t
gj_u64_xor_u(uint64_t u64A, uint64_t u64B)
{
(void)NULL;
return b3217_xor(u64A, u64B);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_xor_u(uint64_t u64A, uint64_t u64B)
    __attribute__((alias("gj_u64_xor_u")));
