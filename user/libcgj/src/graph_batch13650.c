/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13650: gamescope/steamui soft id continuum + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_13650(void);
 *     - Returns the compile-time graph batch number for this TU (13650).
 *   uint32_t __gj_batch_id_13650  (alias)
 *   __libcgj_batch13650_marker = "libcgj-batch13650"
 *
 * Exclusive continuum CREATE-ONLY (13641-13650: gamescope/steamui soft
 * id stubs — gamescope_ok_u_13641, steamui_ok_u_13642,
 * bigpicture_ok_u_13643, deckui_ok_u_13644, cec_ok_u_13645,
 * overlay_ok_u_13646, fps_limit_ok_u_13647, hdr_ok_u_13648,
 * gamescope_soft_ready_u_13649, batch_id_13650). Unique surfaces only;
 * no multi-def. Does NOT redefine gj_batch_id / gj_graph_milestone /
 * prior batch_id_* symbols — avoid multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13650_marker[] = "libcgj-batch13650";

#define B13650_BATCH_ID  13650u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13650_id(void)
{
	return B13650_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_13650 - report this TU's graph batch number.
 *
 * Always returns 13650.
 */
uint32_t
gj_batch_id_13650(void)
{
	(void)NULL;
	return b13650_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_13650(void)
    __attribute__((alias("gj_batch_id_13650")));
