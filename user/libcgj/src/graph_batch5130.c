/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5130: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_5130(void);
 *     - Returns the compile-time graph batch number for this TU (5130).
 *   uint32_t __gj_batch_id_5130  (alias)
 *   __libcgj_batch5130_marker = "libcgj-batch5130"
 *
 * Exclusive continuum CREATE-ONLY (5121-5130: atomic_load_u32_u,
 * atomic_store_u32_u, atomic_add_u32_u, atomic_sub_u32_u,
 * atomic_or_u32_u, atomic_and_u32_u, atomic_xor_u32_u, atomic_cas_u32_u,
 * atomic_xchg_u32_u, batch_id_5130). Soft non-hardware atomics wave.
 * Does NOT redefine gj_batch_id / gj_graph_milestone / prior batch_id_*
 * symbols — avoid multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5130_marker[] = "libcgj-batch5130";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5130_id(void)
{
	return 5130u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_5130 - report this TU's graph batch number.
 *
 * Always returns 5130. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_5130(void)
{
	(void)NULL;
	return b5130_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_5130(void)
    __attribute__((alias("gj_batch_id_5130")));
