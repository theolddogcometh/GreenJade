/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14350: exclusive gamescope soft batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_14350(void);
 *     - Returns the compile-time graph batch number for this TU (14350).
 *   uint32_t __gj_batch_id_14350  (alias)
 *   __libcgj_batch14350_marker = "libcgj-batch14350"
 *
 * Exclusive continuum CREATE-ONLY (14341-14350: gamescope soft stubs —
 * ok slots all→0; gamescope_soft_ready→1; batch_id→14350 —
 * gamescope_ok_u_14341, steamui_ok_u_14342, bigpicture_ok_u_14343,
 * deckui_ok_u_14344, cec_ok_u_14345, overlay_ok_u_14346,
 * fps_limit_ok_u_14347, hdr_ok_u_14348, gamescope_soft_ready_u_14349,
 * batch_id_14350). Unique gj_batch_id_14350 surface only; no multi-def.
 * Does NOT redefine gj_batch_id / gj_graph_milestone / prior batch_id_*
 * symbols. Distinct from gj_batch_id_13450 (batch13450). No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14350_marker[] = "libcgj-batch14350";

#define B14350_BATCH_ID  14350u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14350_id(void)
{
	return B14350_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_14350 - report this TU's graph batch number.
 *
 * Always returns 14350. Useful for link-time presence checks and
 * runtime graph revision probes without string compares on the marker.
 * No parent wires.
 */
uint32_t
gj_batch_id_14350(void)
{
	(void)NULL;
	return b14350_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_14350(void)
    __attribute__((alias("gj_batch_id_14350")));
