/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3978: slab slot capacity from span length (_u).
 *
 * Surface (unique symbols):
 *   size_t gj_slab_capacity_u(size_t total, size_t stride);
 *     - How many fixed-stride slots fit in total bytes:
 *       floor(total / stride). stride == 0 → 0. Pure sizing helper for
 *       slab / pool layouts.
 *   size_t __gj_slab_capacity_u  (alias)
 *   __libcgj_batch3978_marker = "libcgj-batch3978"
 *
 * Exclusive continuum CREATE-ONLY (3971-3980). Distinct from
 * gj_slab_obj_size (batch974), gj_slab_size_for_index (batch1526), and
 * gj_slab_offset_u (batch3977) — unique gj_slab_capacity_u surface
 * only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3978_marker[] = "libcgj-batch3978";

/* ---- freestanding helpers ---------------------------------------------- */

static size_t
b3978_capacity(size_t cbTotal, size_t uStride)
{
	if (uStride == 0u) {
		return 0u;
	}
	return cbTotal / uStride;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_slab_capacity_u - number of stride-sized slots in total bytes.
 *
 * total:  available span length in bytes
 * stride: element size in bytes (0 → 0)
 *
 * Returns floor(total / stride).
 * No parent wires.
 */
size_t
gj_slab_capacity_u(size_t cbTotal, size_t uStride)
{
	(void)NULL;
	return b3978_capacity(cbTotal, uStride);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_slab_capacity_u(size_t cbTotal, size_t uStride)
    __attribute__((alias("gj_slab_capacity_u")));
