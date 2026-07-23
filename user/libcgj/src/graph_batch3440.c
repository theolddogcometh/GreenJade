/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3440: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_3440(void);
 *     - Returns the compile-time graph batch number for this TU (3440).
 *   uint32_t __gj_batch_id_3440  (alias)
 *   __libcgj_batch3440_marker = "libcgj-batch3440"
 *
 * CREATE-ONLY exclusive continuum wave (3431-3440): u64_parity_u2,
 * u32_byte_swap_u, u64_byte_swap_u, u32_word_swap_u, u64_dword_swap_u,
 * u32_nibble_swap_u, u64_nibble_swap_u, u32_bit_interleave_u,
 * u32_bit_deinterleave_u, batch_id_3440. Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* symbols - avoid multi-def. No
 * parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3440_marker[] = "libcgj-batch3440";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b3440_id(void)
{
	return 3440u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_3440 - report this TU's graph batch number.
 *
 * Always returns 3440. Does not call libc. No parent wires.
 */
uint32_t
gj_batch_id_3440(void)
{
	(void)NULL;
	return b3440_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_3440(void)
    __attribute__((alias("gj_batch_id_3440")));
