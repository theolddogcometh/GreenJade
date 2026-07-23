/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12850: gamescope/steamui soft id continuum + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_12850(void);
 *     - Returns the compile-time graph batch number for this TU (12850).
 *   uint32_t __gj_batch_id_12850  (alias)
 *   __libcgj_batch12850_marker = "libcgj-batch12850"
 *
 * Exclusive continuum CREATE-ONLY (12841-12850: gamescope/steamui soft
 * id stubs — gamescope_ok_u_12841, steamui_ok_u_12842,
 * bigpicture_ok_u_12843, deckui_ok_u_12844, cec_ok_u_12845,
 * overlay_ok_u_12846, fps_limit_ok_u_12847, hdr_ok_u_12848,
 * gamescope_soft_ready_u_12849, batch_id_12850→12850). Unique surfaces
 * only; no multi-def. Does NOT redefine gj_batch_id / gj_graph_milestone /
 * prior batch_id_* symbols — avoid multi-def. Distinct from
 * gj_batch_id_12450 / gj_batch_id_12250 / gj_batch_id_12150. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12850_marker[] = "libcgj-batch12850";

#define B12850_BATCH_ID  12850u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12850_id(void)
{
	return B12850_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_12850 - report this TU's graph batch number.
 *
 * Always returns 12850. Closes the 12841-12850 gamescope soft wave.
 */
uint32_t
gj_batch_id_12850(void)
{
	(void)NULL;
	return b12850_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_12850(void)
    __attribute__((alias("gj_batch_id_12850")));
