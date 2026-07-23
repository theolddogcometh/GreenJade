/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11950: gamescope/steamui soft id continuum + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_11950(void);
 *     - Returns the compile-time graph batch number for this TU (11950).
 *   uint32_t __gj_batch_id_11950  (alias)
 *   __libcgj_batch11950_marker = "libcgj-batch11950"
 *
 * Exclusive continuum CREATE-ONLY (11941-11950: gamescope/steamui soft
 * id stubs — gamescope_ok_u_11941, steamui_ok_u_11942,
 * bigpicture_ok_u_11943, deckui_ok_u_11944, cec_ok_u_11945,
 * overlay_ok_u_11946, fps_limit_ok_u_11947, hdr_ok_u_11948,
 * gamescope_soft_ready_u_11949, batch_id_11950). Unique surfaces only;
 * no multi-def. Does NOT redefine gj_batch_id / gj_graph_milestone /
 * prior batch_id_* symbols — avoid multi-def. Distinct from
 * gj_batch_id_10050. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11950_marker[] = "libcgj-batch11950";

#define B11950_BATCH_ID  11950u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11950_id(void)
{
	return B11950_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_11950 - report this TU's graph batch number.
 *
 * Always returns 11950.
 */
uint32_t
gj_batch_id_11950(void)
{
	(void)NULL;
	return b11950_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_11950(void)
    __attribute__((alias("gj_batch_id_11950")));
