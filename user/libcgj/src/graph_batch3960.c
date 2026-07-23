/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3960: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_3960(void);
 *     - Returns the compile-time graph batch number for this TU (3960).
 *   uint32_t __gj_batch_id_3960  (alias)
 *   __libcgj_batch3960_marker = "libcgj-batch3960"
 *
 * CREATE-ONLY exclusive continuum wave (3951-3960): u32_bit_reverse_u,
 * u64_bit_reverse_u, u32_byte_reverse_u, u64_byte_reverse_u,
 * u32_nibble_reverse_u, u64_nibble_reverse_u, u32_gray_next_u,
 * u64_gray_next_u, u32_gray_prev_u, batch_id_3960. Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* symbols - avoid
 * multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3960_marker[] = "libcgj-batch3960";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b3960_id(void)
{
	return 3960u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_3960 - report this TU's graph batch number.
 *
 * Always returns 3960.
 */
uint32_t
gj_batch_id_3960(void)
{
	(void)NULL;
	return b3960_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_3960(void)
    __attribute__((alias("gj_batch_id_3960")));
