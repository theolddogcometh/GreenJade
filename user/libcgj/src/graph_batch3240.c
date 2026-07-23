/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3240: batch identity for exclusive wave 3231-3240.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_3240(void);
 *     - Returns the compile-time graph batch number for this TU (3240).
 *   uint32_t __gj_batch_id_3240  (alias)
 *   __libcgj_batch3240_marker = "libcgj-batch3240"
 *
 * CREATE-ONLY exclusive bitops wave (3231-3240): extract_bits_u,
 * deposit_bits_u (u32/u64), clear_bits_u (u32/u64), set_bits_u (u32/u64),
 * toggle_bits_u (u32/u64), batch_id_3240. Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* symbols - avoid multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3240_marker[] = "libcgj-batch3240";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b3240_id(void)
{
	return 3240u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_3240 - report this TU's graph batch number.
 *
 * Always returns 3240. Link-time presence of this symbol tags the wave.
 * Does not call libc. No parent wires.
 */
uint32_t
gj_batch_id_3240(void)
{
	(void)NULL;
	return b3240_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_3240(void)
    __attribute__((alias("gj_batch_id_3240")));
