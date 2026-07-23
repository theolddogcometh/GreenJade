/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8090: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_8090(void);
 *     - Returns the compile-time graph batch number for this TU (8090).
 *   uint32_t __gj_batch_id_8090  (alias)
 *   __libcgj_batch8090_marker = "libcgj-batch8090"
 *
 * Exclusive continuum CREATE-ONLY (8081-8090: strcmp order stubs —
 * ord_eq, ord_lt, ord_gt, is_eq, is_lt, is_gt, is_le, is_ge, is_ne,
 * batch_id_8090). Does NOT redefine gj_batch_id / gj_graph_milestone /
 * prior batch_id_* symbols — avoid multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8090_marker[] = "libcgj-batch8090";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8090_id(void)
{
	return 8090u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_8090 - report this TU's graph batch number.
 *
 * Always returns 8090. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_8090(void)
{
	(void)NULL;
	return b8090_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_8090(void)
    __attribute__((alias("gj_batch_id_8090")));
