/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3990: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_3990(void);
 *     - Returns the compile-time graph batch number for this TU (3990).
 *   uint32_t __gj_batch_id_3990  (alias)
 *   __libcgj_batch3990_marker = "libcgj-batch3990"
 *
 * Exclusive continuum CREATE-ONLY (3981-3990: buddy_size_u, page_align_u,
 * cacheline_align_u, is_page_aligned_u, is_cacheline_aligned_u, kb_align_u,
 * mb_align_u, size_add_sat_u, size_sub_sat_u, batch_id_3990). Does NOT
 * redefine gj_batch_id / gj_graph_milestone / prior batch_id_* symbols —
 * avoid multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3990_marker[] = "libcgj-batch3990";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b3990_id(void)
{
	return 3990u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_3990 - report this TU's graph batch number.
 *
 * Always returns 3990. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 * No parent wires.
 */
uint32_t
gj_batch_id_3990(void)
{
	(void)NULL;
	return b3990_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_3990(void)
    __attribute__((alias("gj_batch_id_3990")));
