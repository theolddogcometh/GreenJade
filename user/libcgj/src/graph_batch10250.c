/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10250: gamescope/steamui soft continuum + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_10250(void);
 *     - Returns the compile-time graph batch number for this TU (10250).
 *   uint32_t __gj_batch_id_10250  (alias)
 *   __libcgj_batch10250_marker = "libcgj-batch10250"
 *
 * Exclusive continuum CREATE-ONLY (10241-10250: gamescope/steamui soft
 * product stubs — gamescope_ok_u_10241, steamui_ok_u_10242,
 * bigpicture_ok_u_10243, deckui_ok_u_10244, cec_ok_u_10245,
 * overlay_ok_u_10246, fps_limit_ok_u_10247, hdr_ok_u_10248,
 * gamescope_soft_ready_u_10249, batch_id_10250). Unique surfaces only;
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

const char __libcgj_batch10250_marker[] = "libcgj-batch10250";

#define B10250_BATCH_ID  10250u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10250_id(void)
{
	return B10250_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_10250 - report this TU's graph batch number.
 *
 * Always returns 10250.
 */
uint32_t
gj_batch_id_10250(void)
{
	(void)NULL;
	return b10250_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_10250(void)
    __attribute__((alias("gj_batch_id_10250")));
