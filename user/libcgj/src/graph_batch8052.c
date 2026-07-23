/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8052: memset 0xFF fill-byte id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_memset_ff_id_8052(void);
 *     - Return soft 0xFF fill-byte catalog id.
 *   uint32_t __gj_memset_ff_id_8052  (alias)
 *   __libcgj_batch8052_marker = "libcgj-batch8052"
 *
 * Exclusive continuum CREATE-ONLY (8051-8060: memset pattern stubs —
 * memset_zero_id, memset_ff_id, pat_byte, pat_is_zero, pat_is_ff,
 * len_ok, pat_word, pat_ok, len_errorish, batch_id_8060).
 * Unique gj_memset_ff_id_8052 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8052_marker[] = "libcgj-batch8052";

/* Soft memset 0xFF fill-byte catalog value. */
#define B8052_FF_BYTE ((uint32_t)0xffu)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8052_memset_ff_id(void)
{
	return B8052_FF_BYTE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_memset_ff_id_8052 - soft 0xFF fill-byte catalog id.
 *
 * Always returns 0xff. Catalog id only; does not call memset(3).
 * No parent wires.
 */
uint32_t
gj_memset_ff_id_8052(void)
{
	(void)NULL;
	return b8052_memset_ff_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_memset_ff_id_8052(void)
    __attribute__((alias("gj_memset_ff_id_8052")));
