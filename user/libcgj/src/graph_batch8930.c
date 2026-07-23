/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8930: drm/kms id continuum + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_8930(void);
 *     - Returns the compile-time graph batch number for this TU (8930).
 *   uint32_t __gj_batch_id_8930  (alias)
 *   __libcgj_batch8930_marker = "libcgj-batch8930"
 *
 * Exclusive continuum CREATE-ONLY (8921-8930: drm/kms id stubs —
 * mode_ok_u_8921, connector_ok_u_8922, crtc_ok_u_8923, plane_ok_u_8924,
 * fb_ok_u_8925, dumb_ok_u_8926, prime_ok_u_8927, atomic_ok_u_8928,
 * ready_u_8929, batch_id_8930). Unique surfaces only; no multi-def.
 * Does NOT redefine gj_batch_id / gj_graph_milestone / prior batch_id_*
 * symbols — avoid multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8930_marker[] = "libcgj-batch8930";

#define B8930_BATCH_ID  8930u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8930_id(void)
{
	return B8930_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_8930 - report this TU's graph batch number.
 *
 * Always returns 8930.
 */
uint32_t
gj_batch_id_8930(void)
{
	(void)NULL;
	return b8930_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_8930(void)
    __attribute__((alias("gj_batch_id_8930")));
