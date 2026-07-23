/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12650: gamescope/steamui soft id continuum + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_12650(void);
 *     - Returns the compile-time graph batch number for this TU (12650).
 *   uint32_t __gj_batch_id_12650  (alias)
 *   __libcgj_batch12650_marker = "libcgj-batch12650"
 *
 * Exclusive continuum CREATE-ONLY (12641-12650: gamescope/steamui soft
 * id stubs — gamescope_ok_u_12641, steamui_ok_u_12642,
 * bigpicture_ok_u_12643, deckui_ok_u_12644, cec_ok_u_12645,
 * overlay_ok_u_12646, fps_limit_ok_u_12647, hdr_ok_u_12648,
 * gamescope_soft_ready_u_12649, batch_id_12650→12650). Unique surfaces
 * only; no multi-def. Does NOT redefine gj_batch_id / gj_graph_milestone /
 * prior batch_id_* symbols — avoid multi-def. Distinct from
 * gj_batch_id_12450 / gj_batch_id_12250 / gj_batch_id_12050. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12650_marker[] = "libcgj-batch12650";

#define B12650_BATCH_ID  12650u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12650_id(void)
{
	return B12650_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_12650 - report this TU's graph batch number.
 *
 * Always returns 12650. Closes the 12641-12650 gamescope soft wave.
 */
uint32_t
gj_batch_id_12650(void)
{
	(void)NULL;
	return b12650_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_12650(void)
    __attribute__((alias("gj_batch_id_12650")));
