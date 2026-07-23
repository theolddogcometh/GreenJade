/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3720: MILESTONE 3720 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_3720(void);
 *     - Returns the compile-time graph batch number for this TU (3720).
 *   uint32_t __gj_batch_id_3720  (alias)
 *   __libcgj_batch3720_marker = "libcgj-batch3720"
 *
 * Milestone for the exclusive continuum CREATE-ONLY wave (batches
 * 3711-3719: u64_to_dec_need_u, u32_to_hex_need_u, u64_to_hex_need_u,
 * parse_u32_dec_u, parse_u64_dec_u, parse_u32_hex_u, parse_u64_hex_u,
 * parse_i32_dec_u, parse_i64_dec_u). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols —
 * avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3720_marker[] = "libcgj-batch3720";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b3720_id(void)
{
	return 3720u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_3720 — report this TU's graph batch number.
 *
 * Always returns 3720.
 */
uint32_t
gj_batch_id_3720(void)
{
	(void)NULL;
	return b3720_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_3720(void)
    __attribute__((alias("gj_batch_id_3720")));
