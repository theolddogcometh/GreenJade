/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5310: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_5310(void);
 *     - Returns the compile-time graph batch number for this TU (5310).
 *   uint32_t __gj_batch_id_5310  (alias)
 *   __libcgj_batch5310_marker = "libcgj-batch5310"
 *
 * Exclusive continuum CREATE-ONLY (5301-5310: display mode unique —
 * mode_refresh_60_u, mode_refresh_144_u, mode_is_hd_u, mode_is_fhd_u,
 * mode_is_qhd_u, mode_is_uhd_u, mode_pixels_u, mode_aspect_num_u,
 * mode_aspect_den_u, batch_id_5310). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* symbols — avoid multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5310_marker[] = "libcgj-batch5310";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5310_id(void)
{
	return 5310u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_5310 - report this TU's graph batch number.
 *
 * Always returns 5310. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_5310(void)
{
	(void)NULL;
	return b5310_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_5310(void)
    __attribute__((alias("gj_batch_id_5310")));
