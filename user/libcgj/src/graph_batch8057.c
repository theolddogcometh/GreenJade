/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8057: memset pattern word expand stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_memset_pat_word_u_8057(uint32_t pat);
 *     - Expand fill byte to 0x01010101u * (pat & 0xff).
 *   uint32_t __gj_memset_pat_word_u_8057  (alias)
 *   __libcgj_batch8057_marker = "libcgj-batch8057"
 *
 * Exclusive continuum CREATE-ONLY (8051-8060: memset pattern stubs —
 * memset_zero_id, memset_ff_id, pat_byte, pat_is_zero, pat_is_ff,
 * len_ok, pat_word, pat_ok, len_errorish, batch_id_8060).
 * Unique gj_memset_pat_word_u_8057 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8057_marker[] = "libcgj-batch8057";

#define B8057_BYTE_MASK  ((uint32_t)0xffu)
#define B8057_WORD_SPLAT ((uint32_t)0x01010101u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8057_pat_word(uint32_t u32Pat)
{
	return B8057_WORD_SPLAT * (u32Pat & B8057_BYTE_MASK);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_memset_pat_word_u_8057 - soft 32-bit repeated fill pattern.
 *
 * pat: candidate fill pattern word
 *
 * Returns 0x01010101 * (pat & 0xff), the classic word-splat used by
 * wide memset fills. Catalog expand only; does not call memset(3).
 * No parent wires.
 */
uint32_t
gj_memset_pat_word_u_8057(uint32_t u32Pat)
{
	(void)NULL;
	return b8057_pat_word(u32Pat);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_memset_pat_word_u_8057(uint32_t u32Pat)
    __attribute__((alias("gj_memset_pat_word_u_8057")));
