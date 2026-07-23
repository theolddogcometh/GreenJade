/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3435: 64-bit 32-bit-dword half swap (_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_dword_swap_u(uint64_t x);
 *     - Exchange the high and low 32-bit dwords of a 64-bit word
 *       (AB -> BA where A,B are 32-bit dwords). Pure integer.
 *   uint64_t __gj_u64_dword_swap_u  (alias)
 *   __libcgj_batch3435_marker = "libcgj-batch3435"
 *
 * CREATE-ONLY exclusive continuum wave (3431-3440). Unique
 * gj_u64_dword_swap_u surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no compiler builtins. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3435_marker[] = "libcgj-batch3435";

/* ---- freestanding helpers ---------------------------------------------- */

/* Swap 32-bit halves of a 64-bit word. */
static uint64_t
b3435_dwordswap(uint64_t u64X)
{
	return (u64X << 32) | (u64X >> 32);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_dword_swap_u - exchange high and low 32-bit dwords.
 *
 * x: value whose 32-bit halves are to be swapped
 *
 * Example: 0x1122334455667788 -> 0x5566778811223344.
 * Does not call libc. No parent wires.
 */
uint64_t
gj_u64_dword_swap_u(uint64_t u64X)
{
	(void)NULL;
	return b3435_dwordswap(u64X);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_dword_swap_u(uint64_t u64X)
    __attribute__((alias("gj_u64_dword_swap_u")));
