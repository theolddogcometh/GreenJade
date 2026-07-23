/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13850: gamescope/steamui soft id continuum + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_13850(void);
 *     - Returns the compile-time graph batch number for this TU (13850).
 *   uint32_t __gj_batch_id_13850  (alias)
 *   __libcgj_batch13850_marker = "libcgj-batch13850"
 *
 * Exclusive continuum CREATE-ONLY (13841-13850: gamescope/steamui soft
 * id stubs — gamescope_ok_u_13841, steamui_ok_u_13842,
 * bigpicture_ok_u_13843, deckui_ok_u_13844, cec_ok_u_13845,
 * overlay_ok_u_13846, fps_limit_ok_u_13847, hdr_ok_u_13848,
 * gamescope_soft_ready_u_13849, batch_id_13850→13850). Unique surfaces
 * only; no multi-def. Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* symbols — avoid multi-def.
 * Distinct from gj_batch_id_13650 / gj_batch_id_13450 /
 * gj_batch_id_13250. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13850_marker[] = "libcgj-batch13850";

#define B13850_BATCH_ID  13850u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13850_id(void)
{
	return B13850_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_13850 - report this TU's graph batch number.
 *
 * Always returns 13850. Closes the 13841-13850 gamescope/steamui soft wave.
 */
uint32_t
gj_batch_id_13850(void)
{
	(void)NULL;
	return b13850_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_13850(void)
    __attribute__((alias("gj_batch_id_13850")));
