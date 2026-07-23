/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10290: bar3 checklist soft wave batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_10290(void);
 *     - Returns the compile-time graph batch number for this TU (10290).
 *   uint32_t __gj_batch_id_10290  (alias)
 *   __libcgj_batch10290_marker = "libcgj-batch10290"
 *
 * Bar3 checklist soft product stubs CREATE-ONLY (10281-10290). Unique
 * gj_batch_id_10290 surface only; no multi-def. Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* symbols - avoid
 * multi-def. Closes the 10281-10290 soft checklist wave. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10290_marker[] = "libcgj-batch10290";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10290_id(void)
{
	return 10290u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_10290 - report this TU's graph batch number.
 *
 * Always returns 10290. Does not call libc. No parent wires.
 */
uint32_t
gj_batch_id_10290(void)
{
	(void)NULL;
	return b10290_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_10290(void)
    __attribute__((alias("gj_batch_id_10290")));
