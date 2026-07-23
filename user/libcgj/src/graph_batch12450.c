/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12450: gamescope/steamui soft id continuum + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_12450(void);
 *     - Returns the compile-time graph batch number for this TU (12450).
 *   uint32_t __gj_batch_id_12450  (alias)
 *   __libcgj_batch12450_marker = "libcgj-batch12450"
 *
 * Exclusive continuum CREATE-ONLY (12441-12450: gamescope/steamui soft
 * id stubs — gamescope_ok_u_12441, steamui_ok_u_12442,
 * bigpicture_ok_u_12443, deckui_ok_u_12444, cec_ok_u_12445,
 * overlay_ok_u_12446, fps_limit_ok_u_12447, hdr_ok_u_12448,
 * gamescope_soft_ready_u_12449, batch_id_12450→12450). Unique surfaces
 * only; no multi-def. Does NOT redefine gj_batch_id / gj_graph_milestone /
 * prior batch_id_* symbols — avoid multi-def. Distinct from
 * gj_batch_id_12250 / gj_batch_id_12050 / gj_batch_id_11950. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12450_marker[] = "libcgj-batch12450";

#define B12450_BATCH_ID  12450u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12450_id(void)
{
	return B12450_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_12450 - report this TU's graph batch number.
 *
 * Always returns 12450. Closes the 12441-12450 gamescope soft wave.
 */
uint32_t
gj_batch_id_12450(void)
{
	(void)NULL;
	return b12450_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_12450(void)
    __attribute__((alias("gj_batch_id_12450")));
