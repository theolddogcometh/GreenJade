/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3218: bitwise NAND of two uint32_t (u-suffix).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_nand_u(uint32_t a, uint32_t b);
 *     - Return ~(a & b). Pure integer; no builtins.
 *   uint32_t __gj_u32_nand_u  (alias)
 *   __libcgj_batch3218_marker = "libcgj-batch3218"
 *
 * Milestone 3220 exclusive continuum CREATE-ONLY (3211-3220). Unique
 * gj_u32_nand_u surface only; no multi-def. Distinct from
 * gj_u32_and_u (batch3212). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3218_marker[] = "libcgj-batch3218";

/* ---- freestanding helpers ---------------------------------------------- */

/* Bitwise NAND of two 32-bit words. */
static uint32_t
b3218_nand(uint32_t u32A, uint32_t u32B)
{
return ~(u32A & u32B);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_nand_u - bitwise NAND of two uint32_t values.
 *
 * a: left operand
 * b: right operand
 *
 * Returns ~(a & b). Does not call libc. No parent wires.
 */
uint32_t
gj_u32_nand_u(uint32_t u32A, uint32_t u32B)
{
(void)NULL;
return b3218_nand(u32A, u32B);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_nand_u(uint32_t u32A, uint32_t u32B)
    __attribute__((alias("gj_u32_nand_u")));
