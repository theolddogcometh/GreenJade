/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4463: add one 16-bit word into a csum sum (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_csum_add_u(uint32_t sum, uint16_t word);
 *     - Add a 16-bit network word into a 32-bit ones-complement partial
 *       sum with one end-around carry fold of the high 16 bits.
 *   uint32_t __gj_csum_add_u  (alias)
 *   __libcgj_batch4463_marker = "libcgj-batch4463"
 *
 * Exclusive continuum CREATE-ONLY (4461-4470). Distinct from
 * gj_inet_csum_partial_u (batch2171) and gj_checksum_add16_u —
 * unique single-word add surface; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4463_marker[] = "libcgj-batch4463";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * sum += word, then end-around carry once so high halves feed back.
 * Result remains a 32-bit partial (may still have bits above 16).
 */
static uint32_t
b4463_add(uint32_t u32Sum, uint16_t u16Word)
{
	u32Sum += (uint32_t)u16Word;
	u32Sum = (u32Sum & 0xffffu) + (u32Sum >> 16);
	return u32Sum;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_csum_add_u - add one 16-bit word into an Internet checksum sum.
 *
 * sum:  incoming 32-bit partial ones-complement accumulator
 * word: 16-bit network-order word to include
 *
 * Returns updated partial sum after end-around carry. Self-contained.
 */
uint32_t
gj_csum_add_u(uint32_t sum, uint16_t word)
{
	(void)NULL;
	return b4463_add(sum, word);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_csum_add_u(uint32_t sum, uint16_t word)
    __attribute__((alias("gj_csum_add_u")));
