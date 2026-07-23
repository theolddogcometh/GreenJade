/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12250: gamescope/steamui soft id continuum + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_12250(void);
 *     - Returns the compile-time graph batch number for this TU (12250).
 *   uint32_t __gj_batch_id_12250  (alias)
 *   __libcgj_batch12250_marker = "libcgj-batch12250"
 *
 * Exclusive continuum CREATE-ONLY (12241-12250: gamescope/steamui soft
 * id stubs — gamescope_ok_u_12241, steamui_ok_u_12242,
 * bigpicture_ok_u_12243, deckui_ok_u_12244, cec_ok_u_12245,
 * overlay_ok_u_12246, fps_limit_ok_u_12247, hdr_ok_u_12248,
 * gamescope_soft_ready_u_12249, batch_id_12250→12250). Unique surfaces
 * only; no multi-def. Does NOT redefine gj_batch_id / gj_graph_milestone /
 * prior batch_id_* symbols — avoid multi-def. Distinct from
 * gj_batch_id_11950 / gj_batch_id_11750 / gj_batch_id_11550. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12250_marker[] = "libcgj-batch12250";

#define B12250_BATCH_ID  12250u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12250_id(void)
{
	return B12250_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_12250 - report this TU's graph batch number.
 *
 * Always returns 12250. Closes the 12241-12250 gamescope soft wave.
 */
uint32_t
gj_batch_id_12250(void)
{
	(void)NULL;
	return b12250_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_12250(void)
    __attribute__((alias("gj_batch_id_12250")));
