/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13450: exclusive gamescope soft batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_13450(void);
 *     - Returns the compile-time graph batch number for this TU (13450).
 *   uint32_t __gj_batch_id_13450  (alias)
 *   __libcgj_batch13450_marker = "libcgj-batch13450"
 *
 * Exclusive continuum CREATE-ONLY (13441-13450: gamescope soft stubs —
 * ok slots all→0; gamescope_soft_ready→1; batch_id→13450 —
 * gamescope_ok_u_13441, steamui_ok_u_13442, bigpicture_ok_u_13443,
 * deckui_ok_u_13444, cec_ok_u_13445, overlay_ok_u_13446,
 * fps_limit_ok_u_13447, hdr_ok_u_13448, gamescope_soft_ready_u_13449,
 * batch_id_13450). Unique gj_batch_id_13450 surface only; no multi-def.
 * Does NOT redefine gj_batch_id / gj_graph_milestone / prior batch_id_*
 * symbols. Distinct from gj_batch_id_10050 (batch10050). No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13450_marker[] = "libcgj-batch13450";

#define B13450_BATCH_ID  13450u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13450_id(void)
{
	return B13450_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_13450 - report this TU's graph batch number.
 *
 * Always returns 13450. Useful for link-time presence checks and
 * runtime graph revision probes without string compares on the marker.
 * No parent wires.
 */
uint32_t
gj_batch_id_13450(void)
{
	(void)NULL;
	return b13450_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_13450(void)
    __attribute__((alias("gj_batch_id_13450")));
