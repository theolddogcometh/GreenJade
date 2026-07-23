/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11750: gamescope/steamui soft + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_11750(void);
 *     - Returns the compile-time graph batch number for this TU (11750).
 *   uint32_t __gj_batch_id_11750  (alias)
 *   __libcgj_batch11750_marker = "libcgj-batch11750"
 *
 * Exclusive continuum CREATE-ONLY (11741-11750: gamescope/steamui soft
 * id stubs — gamescope_ok_u_11741, steamui_ok_u_11742,
 * bigpicture_ok_u_11743, deckui_ok_u_11744, cec_ok_u_11745,
 * overlay_ok_u_11746, fps_limit_ok_u_11747, hdr_ok_u_11748,
 * gamescope_soft_ready_u_11749, batch_id_11750→11750). Unique surfaces
 * only; no multi-def. Does NOT redefine gj_batch_id / gj_graph_milestone
 * / prior batch_id_* symbols — avoid multi-def. Distinct from
 * gj_batch_id_11550 (batch11550) and gj_batch_id_11350 (batch11350).
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11750_marker[] = "libcgj-batch11750";

#define B11750_BATCH_ID  11750u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11750_id(void)
{
	return B11750_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_11750 - report this TU's graph batch number.
 *
 * Always returns 11750. Closes the 11741-11750 gamescope/steamui soft
 * continuum.
 */
uint32_t
gj_batch_id_11750(void)
{
	(void)NULL;
	return b11750_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_11750(void)
    __attribute__((alias("gj_batch_id_11750")));
