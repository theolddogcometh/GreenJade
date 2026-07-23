/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13249: soft gamescope continuum ready (open).
 *
 * Surface (unique symbols):
 *   uint32_t gj_gamescope_soft_ready_u_13249(void);
 *     - Returns 0 (gamescope/steamui soft product stub continuum not
 *       hard-ready; still open). Soft pure-data product tag for wave
 *       13241-13250 presence path; not a hard gamescope runtime probe.
 *   uint32_t __gj_gamescope_soft_ready_u_13249  (alias)
 *   __libcgj_batch13249_marker = "libcgj-batch13249"
 *
 * Exclusive continuum CREATE-ONLY (13241-13250: gamescope/steamui soft
 * product stubs — gamescope_ok_u_13241, steamui_ok_u_13242,
 * bigpicture_ok_u_13243, deckui_ok_u_13244, cec_ok_u_13245,
 * overlay_ok_u_13246, fps_limit_ok_u_13247, hdr_ok_u_13248,
 * gamescope_soft_ready_u_13249, batch_id_13250). Unique surface only;
 * no multi-def. Distinct from gj_gamescope_ok_u_13241 (component lamp)
 * and prior gamescope_soft_ready_u_* waves. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13249_marker[] = "libcgj-batch13249";

/* Soft continuum-ready lamp for gamescope/steamui wave (still open). */
#define B13249_GAMESCOPE_SOFT_READY  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13249_soft_ready(void)
{
	return B13249_GAMESCOPE_SOFT_READY;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_gamescope_soft_ready_u_13249 - gamescope/steamui soft continuum ready.
 *
 * Always returns 0 (not hard-ready / still open). Soft pure-data product
 * tag for the exclusive wave path. Does not call libc. No parent wires.
 */
uint32_t
gj_gamescope_soft_ready_u_13249(void)
{
	(void)NULL;
	return b13249_soft_ready();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_gamescope_soft_ready_u_13249(void)
    __attribute__((alias("gj_gamescope_soft_ready_u_13249")));
