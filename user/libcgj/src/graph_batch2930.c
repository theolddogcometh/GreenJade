/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2930: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_2930(void);
 *     - Returns the compile-time graph batch number for this TU (2930).
 *   uint32_t __gj_batch_id_2930  (alias)
 *   __libcgj_batch2930_marker = "libcgj-batch2930"
 *
 * Exclusive continuum CREATE-ONLY (2921-2930): bitrev32/64_u,
 * pack_u16/u32 be/le_u, unpack_u16 be/le_u, unpack_u32_be_u,
 * batch_id_2930. Does NOT redefine gj_batch_id / gj_graph_milestone
 * / prior batch_id_* symbols — avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2930_marker[] = "libcgj-batch2930";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2930_id(void)
{
	return 2930u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_2930 - report this TU's graph batch number.
 *
 * Always returns 2930.
 */
uint32_t
gj_batch_id_2930(void)
{
	(void)NULL;
	return b2930_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_2930(void)
    __attribute__((alias("gj_batch_id_2930")));
