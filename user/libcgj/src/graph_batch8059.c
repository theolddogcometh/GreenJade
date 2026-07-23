/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8059: memset length errorish stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_memset_len_errorish_u_8059(uint32_t n);
 *     - Return 1 if n is zero (soft errorish length), else 0.
 *   uint32_t __gj_memset_len_errorish_u_8059  (alias)
 *   __libcgj_batch8059_marker = "libcgj-batch8059"
 *
 * Exclusive continuum CREATE-ONLY (8051-8060: memset pattern stubs —
 * memset_zero_id, memset_ff_id, pat_byte, pat_is_zero, pat_is_ff,
 * len_ok, pat_word, pat_ok, len_errorish, batch_id_8060).
 * Unique gj_memset_len_errorish_u_8059 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8059_marker[] = "libcgj-batch8059";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8059_len_errorish(uint32_t u32N)
{
	return (u32N == 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_memset_len_errorish_u_8059 - 1 if soft memset length is empty.
 *
 * n: candidate byte count
 *
 * Returns 1 when n == 0 (no-op / empty length, treated as errorish in
 * catalog terms), else 0. Does not call memset(3). No parent wires.
 */
uint32_t
gj_memset_len_errorish_u_8059(uint32_t u32N)
{
	(void)NULL;
	return b8059_len_errorish(u32N);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_memset_len_errorish_u_8059(uint32_t u32N)
    __attribute__((alias("gj_memset_len_errorish_u_8059")));
