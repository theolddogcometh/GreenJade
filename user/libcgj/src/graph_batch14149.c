/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14149: soft gamescope continuum ready (open).
 *
 * Surface (unique symbols):
 *   uint32_t gj_gamescope_soft_ready_u_14149(void);
 *     - Returns 0 (gamescope/steamui soft product stub continuum not
 *       hard-ready; still open). Soft pure-data product tag for wave
 *       14141-14150 presence path; not a hard gamescope runtime probe.
 *   uint32_t __gj_gamescope_soft_ready_u_14149  (alias)
 *   __libcgj_batch14149_marker = "libcgj-batch14149"
 *
 * Exclusive continuum CREATE-ONLY (14141-14150: gamescope/steamui soft
 * product stubs — gamescope_ok_u_14141, steamui_ok_u_14142,
 * bigpicture_ok_u_14143, deckui_ok_u_14144, cec_ok_u_14145,
 * overlay_ok_u_14146, fps_limit_ok_u_14147, hdr_ok_u_14148,
 * gamescope_soft_ready_u_14149, batch_id_14150). Unique surface only;
 * no multi-def. Distinct from gj_gamescope_soft_ready_u_10249
 * (batch10249), gj_gamescope_ok_u_14141 (component lamp), and
 * gj_proton_soft_ready_u_8989 (batch8989). No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14149_marker[] = "libcgj-batch14149";

/* Soft continuum-ready lamp for gamescope/steamui wave (still open). */
#define B14149_GAMESCOPE_SOFT_READY  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14149_soft_ready(void)
{
	return B14149_GAMESCOPE_SOFT_READY;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_gamescope_soft_ready_u_14149 - gamescope/steamui soft continuum ready.
 *
 * Always returns 0 (not hard-ready / still open). Soft pure-data product
 * tag for the exclusive wave path. Does not call libc. No parent wires.
 */
uint32_t
gj_gamescope_soft_ready_u_14149(void)
{
	(void)NULL;
	return b14149_soft_ready();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_gamescope_soft_ready_u_14149(void)
    __attribute__((alias("gj_gamescope_soft_ready_u_14149")));
