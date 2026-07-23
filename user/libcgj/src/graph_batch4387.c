/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4387: 128-bit XOR low 64 bits (_u, two u64).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u128_xor_lo64_u(uint64_t a_lo, uint64_t a_hi,
 *                               uint64_t b_lo, uint64_t b_hi);
 *     - Return low 64 bits of (a_hi:a_lo) XOR (b_hi:b_lo), i.e. a_lo^b_lo.
 *       No __int128 type; pure two-limb u64 ops (hi unused for result).
 *   uint64_t __gj_u128_xor_lo64_u  (alias)
 *   __libcgj_batch4387_marker = "libcgj-batch4387"
 *
 * Exclusive continuum CREATE-ONLY (4381-4390). Companion to
 * gj_u128_xor_hi64_u (batch4388). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4387_marker[] = "libcgj-batch4387";

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b4387_xor_lo(uint64_t u64ALo, uint64_t u64AHi, uint64_t u64BLo,
    uint64_t u64BHi)
{
	(void)u64AHi;
	(void)u64BHi;
	return u64ALo ^ u64BLo;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u128_xor_lo64_u - low limb of 128-bit XOR (two u64 limbs each).
 *
 * a_lo, a_hi: low/high limbs of operand A
 * b_lo, b_hi: low/high limbs of operand B
 *
 * Returns a_lo ^ b_lo. hi limbs accepted for API symmetry with the
 * hi64 companion; not used in the low result. No __int128.
 * Self-contained; no parent wires.
 */
uint64_t
gj_u128_xor_lo64_u(uint64_t u64ALo, uint64_t u64AHi, uint64_t u64BLo,
    uint64_t u64BHi)
{
	(void)NULL;
	return b4387_xor_lo(u64ALo, u64AHi, u64BLo, u64BHi);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u128_xor_lo64_u(uint64_t u64ALo, uint64_t u64AHi,
    uint64_t u64BLo, uint64_t u64BHi)
    __attribute__((alias("gj_u128_xor_lo64_u")));
