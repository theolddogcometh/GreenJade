/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13050: gamescope/steamui soft id continuum + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_13050(void);
 *     - Returns the compile-time graph batch number for this TU (13050).
 *   uint32_t __gj_batch_id_13050  (alias)
 *   __libcgj_batch13050_marker = "libcgj-batch13050"
 *
 * Exclusive continuum CREATE-ONLY (13041-13050: gamescope/steamui soft
 * id stubs — gamescope_ok_u_13041, steamui_ok_u_13042,
 * bigpicture_ok_u_13043, deckui_ok_u_13044, cec_ok_u_13045,
 * overlay_ok_u_13046, fps_limit_ok_u_13047, hdr_ok_u_13048,
 * gamescope_soft_ready_u_13049, batch_id_13050→13050). Unique surfaces
 * only; no multi-def. Does NOT redefine gj_batch_id / gj_graph_milestone /
 * prior batch_id_* symbols — avoid multi-def. Distinct from
 * gj_batch_id_12850 / gj_batch_id_12650 / gj_batch_id_12450. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13050_marker[] = "libcgj-batch13050";

#define B13050_BATCH_ID  13050u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13050_id(void)
{
	return B13050_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_13050 - report this TU's graph batch number.
 *
 * Always returns 13050. Closes the 13041-13050 gamescope soft wave.
 */
uint32_t
gj_batch_id_13050(void)
{
	(void)NULL;
	return b13050_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_13050(void)
    __attribute__((alias("gj_batch_id_13050")));
