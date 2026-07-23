/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13250: gamescope/steamui soft continuum + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_13250(void);
 *     - Returns the compile-time graph batch number for this TU (13250).
 *   uint32_t __gj_batch_id_13250  (alias)
 *   __libcgj_batch13250_marker = "libcgj-batch13250"
 *
 * Exclusive continuum CREATE-ONLY (13241-13250: gamescope/steamui soft
 * product stubs — gamescope_ok_u_13241, steamui_ok_u_13242,
 * bigpicture_ok_u_13243, deckui_ok_u_13244, cec_ok_u_13245,
 * overlay_ok_u_13246, fps_limit_ok_u_13247, hdr_ok_u_13248,
 * gamescope_soft_ready_u_13249, batch_id_13250). Unique surfaces only;
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

const char __libcgj_batch13250_marker[] = "libcgj-batch13250";

#define B13250_BATCH_ID  13250u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13250_id(void)
{
	return B13250_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_13250 - report this TU's graph batch number.
 *
 * Always returns 13250.
 */
uint32_t
gj_batch_id_13250(void)
{
	(void)NULL;
	return b13250_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_13250(void)
    __attribute__((alias("gj_batch_id_13250")));
