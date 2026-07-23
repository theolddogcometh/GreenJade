/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3490: batch identity for wave 3481-3490.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_3490(void);
 *     - Returns the compile-time graph batch number for this TU (3490).
 *   uint32_t __gj_batch_id_3490  (alias)
 *   __libcgj_batch3490_marker = "libcgj-batch3490"
 *
 * Path/URI/MIME exclusive CREATE-ONLY wave (3481-3490). Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* symbols - avoid
 * multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3490_marker[] = "libcgj-batch3490";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b3490_id(void)
{
	return 3490u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_3490 - report this TU's graph batch number.
 *
 * Always returns 3490.
 */
uint32_t
gj_batch_id_3490(void)
{
	(void)NULL;
	return b3490_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_3490(void)
    __attribute__((alias("gj_batch_id_3490")));
