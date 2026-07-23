/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1510: milestone 1510 batch identity
 * (compression-primitives arc close: RLE / LZ77 / bit I/O / Huffman).
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_1510(void);
 *     — Returns the compile-time graph batch number for this TU (1510).
 *   uint32_t __gj_batch_id_1510  (alias)
 *   __libcgj_batch1510_marker = "libcgj-batch1510"
 *
 * Does NOT define gj_batch_id / gj_graph_milestone / prior batch_id_* or
 * milestone_* symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1510_marker[] = "libcgj-batch1510";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_1510 — report this TU's graph batch number.
 *
 * Always returns 1510.
 */
uint32_t
gj_batch_id_1510(void)
{
	return 1510u;
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_1510(void)
    __attribute__((alias("gj_batch_id_1510")));
