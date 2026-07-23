/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8260: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_8260(void);
 *     - Returns the compile-time graph batch number for this TU (8260).
 *   uint32_t __gj_batch_id_8260  (alias)
 *   __libcgj_batch8260_marker = "libcgj-batch8260"
 *
 * Exclusive continuum CREATE-ONLY (8251-8260: bitops / ffs-style integer
 * stubs — ffs, clz, ctz, popcount, parity, isolate_lsb, clear_lsb, mask,
 * test, batch_id_8260). Does NOT redefine gj_batch_id / gj_graph_milestone /
 * prior batch_id_* symbols — avoid multi-def. No parent wires. No
 * __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8260_marker[] = "libcgj-batch8260";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8260_id(void)
{
	return 8260u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_8260 - report this TU's graph batch number.
 *
 * Always returns 8260. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_8260(void)
{
	(void)NULL;
	return b8260_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_8260(void)
    __attribute__((alias("gj_batch_id_8260")));
