/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11350: gamescope/steamui soft + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_11350(void);
 *     - Returns the compile-time graph batch number for this TU (11350).
 *   uint32_t __gj_batch_id_11350  (alias)
 *   __libcgj_batch11350_marker = "libcgj-batch11350"
 *
 * Exclusive continuum CREATE-ONLY (11341-11350: gamescope/steamui soft
 * id stubs — gamescope_ok_u_11341, steamui_ok_u_11342,
 * bigpicture_ok_u_11343, deckui_ok_u_11344, cec_ok_u_11345,
 * overlay_ok_u_11346, fps_limit_ok_u_11347, hdr_ok_u_11348,
 * gamescope_soft_ready_u_11349, batch_id_11350→11350). Unique surfaces
 * only; no multi-def. Does NOT redefine gj_batch_id / gj_graph_milestone
 * / prior batch_id_* symbols — avoid multi-def. Distinct from
 * gj_batch_id_11150 (batch11150) and gj_batch_id_10950 (batch10950).
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11350_marker[] = "libcgj-batch11350";

#define B11350_BATCH_ID  11350u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11350_id(void)
{
	return B11350_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_11350 - report this TU's graph batch number.
 *
 * Always returns 11350. Closes the 11341-11350 gamescope/steamui soft
 * continuum.
 */
uint32_t
gj_batch_id_11350(void)
{
	(void)NULL;
	return b11350_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_11350(void)
    __attribute__((alias("gj_batch_id_11350")));
