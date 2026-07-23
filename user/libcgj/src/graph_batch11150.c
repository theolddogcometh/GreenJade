/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11150: gamescope/steamui soft + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_11150(void);
 *     - Returns the compile-time graph batch number for this TU (11150).
 *   uint32_t __gj_batch_id_11150  (alias)
 *   __libcgj_batch11150_marker = "libcgj-batch11150"
 *
 * Exclusive continuum CREATE-ONLY (11141-11150: gamescope/steamui soft
 * id stubs — gamescope_ok_u_11141, steamui_ok_u_11142,
 * bigpicture_ok_u_11143, deckui_ok_u_11144, cec_ok_u_11145,
 * overlay_ok_u_11146, fps_limit_ok_u_11147, hdr_ok_u_11148,
 * gamescope_soft_ready_u_11149, batch_id_11150). Unique surfaces only;
 * no multi-def. Does NOT redefine gj_batch_id / gj_graph_milestone /
 * prior batch_id_* symbols — avoid multi-def. Distinct from
 * gj_batch_id_10950 (batch10950) and gj_batch_id_10050 (batch10050).
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11150_marker[] = "libcgj-batch11150";

#define B11150_BATCH_ID  11150u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11150_id(void)
{
	return B11150_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_11150 - report this TU's graph batch number.
 *
 * Always returns 11150. Closes the 11141-11150 gamescope/steamui soft
 * continuum.
 */
uint32_t
gj_batch_id_11150(void)
{
	(void)NULL;
	return b11150_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_11150(void)
    __attribute__((alias("gj_batch_id_11150")));
