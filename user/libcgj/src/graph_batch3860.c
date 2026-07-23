/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3860: MILESTONE / wave batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_3860(void);
 *     - Returns the compile-time graph batch number for this TU (3860).
 *   uint32_t __gj_batch_id_3860  (alias)
 *   __libcgj_batch3860_marker = "libcgj-batch3860"
 *
 * Exclusive continuum CREATE-ONLY (3851-3860): ceil/floor/round div
 * and mul_add/mul_sub wave. Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* symbols — avoid multi-def.
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3860_marker[] = "libcgj-batch3860";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b3860_id(void)
{
	return 3860u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_3860 - report this TU's graph batch number.
 *
 * Always returns 3860. No parent wires.
 */
uint32_t
gj_batch_id_3860(void)
{
	(void)NULL;
	return b3860_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_3860(void)
    __attribute__((alias("gj_batch_id_3860")));
