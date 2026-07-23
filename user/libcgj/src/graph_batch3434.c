/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3434: 32-bit 16-bit-word half swap (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_word_swap_u(uint32_t x);
 *     - Exchange the high and low 16-bit halves of a 32-bit word
 *       (AB -> BA where A,B are 16-bit words). Pure integer.
 *   uint32_t __gj_u32_word_swap_u  (alias)
 *   __libcgj_batch3434_marker = "libcgj-batch3434"
 *
 * CREATE-ONLY exclusive continuum wave (3431-3440). Unique
 * gj_u32_word_swap_u surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no compiler builtins. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3434_marker[] = "libcgj-batch3434";

/* ---- freestanding helpers ---------------------------------------------- */

/* Swap 16-bit halves of a 32-bit word. */
static uint32_t
b3434_wordswap(uint32_t u32X)
{
	return (u32X << 16) | (u32X >> 16);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_word_swap_u - exchange high and low 16-bit words.
 *
 * x: value whose 16-bit halves are to be swapped
 *
 * Example: 0x11223344 -> 0x33441122. Does not call libc. No parent wires.
 */
uint32_t
gj_u32_word_swap_u(uint32_t u32X)
{
	(void)NULL;
	return b3434_wordswap(u32X);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_word_swap_u(uint32_t u32X)
    __attribute__((alias("gj_u32_word_swap_u")));
