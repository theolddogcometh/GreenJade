/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14150: gamescope/steamui soft continuum + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_14150(void);
 *     - Returns the compile-time graph batch number for this TU (14150).
 *   uint32_t __gj_batch_id_14150  (alias)
 *   __libcgj_batch14150_marker = "libcgj-batch14150"
 *
 * Exclusive continuum CREATE-ONLY (14141-14150: gamescope/steamui soft
 * product stubs — gamescope_ok_u_14141, steamui_ok_u_14142,
 * bigpicture_ok_u_14143, deckui_ok_u_14144, cec_ok_u_14145,
 * overlay_ok_u_14146, fps_limit_ok_u_14147, hdr_ok_u_14148,
 * gamescope_soft_ready_u_14149, batch_id_14150). Unique surfaces only;
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

const char __libcgj_batch14150_marker[] = "libcgj-batch14150";

#define B14150_BATCH_ID  14150u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14150_id(void)
{
	return B14150_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_14150 - report this TU's graph batch number.
 *
 * Always returns 14150. Closes the 14141-14150 gamescope/steamui soft
 * product continuum wave.
 */
uint32_t
gj_batch_id_14150(void)
{
	(void)NULL;
	return b14150_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_14150(void)
    __attribute__((alias("gj_batch_id_14150")));
