/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8053: memset pattern low-byte extract stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_memset_pat_byte_u_8053(uint32_t pat);
 *     - Return pat masked to one fill byte (pat & 0xff).
 *   uint32_t __gj_memset_pat_byte_u_8053  (alias)
 *   __libcgj_batch8053_marker = "libcgj-batch8053"
 *
 * Exclusive continuum CREATE-ONLY (8051-8060: memset pattern stubs —
 * memset_zero_id, memset_ff_id, pat_byte, pat_is_zero, pat_is_ff,
 * len_ok, pat_word, pat_ok, len_errorish, batch_id_8060).
 * Unique gj_memset_pat_byte_u_8053 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8053_marker[] = "libcgj-batch8053";

/* Soft fill-byte mask (one octet). */
#define B8053_BYTE_MASK ((uint32_t)0xffu)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8053_pat_byte(uint32_t u32Pat)
{
	return u32Pat & B8053_BYTE_MASK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_memset_pat_byte_u_8053 - extract soft memset fill byte.
 *
 * pat: candidate fill pattern word
 *
 * Returns pat & 0xff. Catalog extract only; does not call memset(3).
 * No parent wires.
 */
uint32_t
gj_memset_pat_byte_u_8053(uint32_t u32Pat)
{
	(void)NULL;
	return b8053_pat_byte(u32Pat);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_memset_pat_byte_u_8053(uint32_t u32Pat)
    __attribute__((alias("gj_memset_pat_byte_u_8053")));
