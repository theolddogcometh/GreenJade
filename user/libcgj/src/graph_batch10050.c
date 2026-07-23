/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10050: gamescope/steamui soft id continuum + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_10050(void);
 *     - Returns the compile-time graph batch number for this TU (10050).
 *   uint32_t __gj_batch_id_10050  (alias)
 *   __libcgj_batch10050_marker = "libcgj-batch10050"
 *
 * Exclusive continuum CREATE-ONLY (10041-10050: gamescope/steamui soft
 * id stubs — gamescope_ok_u_10041, steamui_ok_u_10042,
 * bigpicture_ok_u_10043, deckui_ok_u_10044, cec_ok_u_10045,
 * overlay_ok_u_10046, fps_limit_ok_u_10047, hdr_ok_u_10048,
 * gamescope_soft_ready_u_10049, batch_id_10050). Unique surfaces only;
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

const char __libcgj_batch10050_marker[] = "libcgj-batch10050";

#define B10050_BATCH_ID  10050u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10050_id(void)
{
	return B10050_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_10050 - report this TU's graph batch number.
 *
 * Always returns 10050.
 */
uint32_t
gj_batch_id_10050(void)
{
	(void)NULL;
	return b10050_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_10050(void)
    __attribute__((alias("gj_batch_id_10050")));
