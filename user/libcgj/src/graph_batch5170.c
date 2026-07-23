/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5170: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_5170(void);
 *     - Returns the compile-time graph batch number for this TU (5170).
 *   uint32_t __gj_batch_id_5170  (alias)
 *   __libcgj_batch5170_marker = "libcgj-batch5170"
 *
 * Exclusive continuum CREATE-ONLY (5161-5170: fb_bpp32_u, fb_bpp24_u,
 * fb_bpp16_u, fb_stride_u, fb_size_u, fb_offset_u, fb_pixel_count_u,
 * fb_is_valid_u, fb_rgb565_u, batch_id_5170). Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* symbols — avoid
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5170_marker[] = "libcgj-batch5170";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5170_id(void)
{
	return 5170u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_5170 - report this TU's graph batch number.
 *
 * Always returns 5170. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_5170(void)
{
	(void)NULL;
	return b5170_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_5170(void)
    __attribute__((alias("gj_batch_id_5170")));
