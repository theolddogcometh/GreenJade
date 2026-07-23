/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8054: memset pattern is-zero stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_memset_pat_is_zero_u_8054(uint32_t pat);
 *     - Return 1 if (pat & 0xff) is 0, else 0.
 *   uint32_t __gj_memset_pat_is_zero_u_8054  (alias)
 *   __libcgj_batch8054_marker = "libcgj-batch8054"
 *
 * Exclusive continuum CREATE-ONLY (8051-8060: memset pattern stubs —
 * memset_zero_id, memset_ff_id, pat_byte, pat_is_zero, pat_is_ff,
 * len_ok, pat_word, pat_ok, len_errorish, batch_id_8060).
 * Unique gj_memset_pat_is_zero_u_8054 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8054_marker[] = "libcgj-batch8054";

#define B8054_BYTE_MASK ((uint32_t)0xffu)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8054_pat_is_zero(uint32_t u32Pat)
{
	return ((u32Pat & B8054_BYTE_MASK) == 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_memset_pat_is_zero_u_8054 - 1 if fill byte is zero.
 *
 * pat: candidate fill pattern word
 *
 * Returns 1 when (pat & 0xff) == 0, else 0. Soft catalog test; does
 * not call memset(3). No parent wires.
 */
uint32_t
gj_memset_pat_is_zero_u_8054(uint32_t u32Pat)
{
	(void)NULL;
	return b8054_pat_is_zero(u32Pat);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_memset_pat_is_zero_u_8054(uint32_t u32Pat)
    __attribute__((alias("gj_memset_pat_is_zero_u_8054")));
