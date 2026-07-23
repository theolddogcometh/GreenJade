/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11550: gamescope/steamui soft + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_11550(void);
 *     - Returns the compile-time graph batch number for this TU (11550).
 *   uint32_t __gj_batch_id_11550  (alias)
 *   __libcgj_batch11550_marker = "libcgj-batch11550"
 *
 * Exclusive continuum CREATE-ONLY (11541-11550: gamescope/steamui soft
 * id stubs — gamescope_ok_u_11541, steamui_ok_u_11542,
 * bigpicture_ok_u_11543, deckui_ok_u_11544, cec_ok_u_11545,
 * overlay_ok_u_11546, fps_limit_ok_u_11547, hdr_ok_u_11548,
 * gamescope_soft_ready_u_11549, batch_id_11550→11550). Unique surfaces
 * only; no multi-def. Does NOT redefine gj_batch_id / gj_graph_milestone
 * / prior batch_id_* symbols — avoid multi-def. Distinct from
 * gj_batch_id_11350 (batch11350) and gj_batch_id_11150 (batch11150).
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11550_marker[] = "libcgj-batch11550";

#define B11550_BATCH_ID  11550u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11550_id(void)
{
	return B11550_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_11550 - report this TU's graph batch number.
 *
 * Always returns 11550. Closes the 11541-11550 gamescope/steamui soft
 * continuum.
 */
uint32_t
gj_batch_id_11550(void)
{
	(void)NULL;
	return b11550_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_11550(void)
    __attribute__((alias("gj_batch_id_11550")));
