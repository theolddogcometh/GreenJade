/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12049: soft gamescope/steamui id continuum ready.
 *
 * Surface (unique symbols):
 *   uint32_t gj_gamescope_soft_ready_u_12049(void);
 *     - Returns 1 (gamescope/steamui soft id stub continuum ready).
 *       Pure-data product tag that exclusive wave 12041-12050 surfaces
 *       are present for the gamescope soft path; not a hard gamescope
 *       runtime probe.
 *   uint32_t __gj_gamescope_soft_ready_u_12049  (alias)
 *   __libcgj_batch12049_marker = "libcgj-batch12049"
 *
 * Exclusive continuum CREATE-ONLY (12041-12050: gamescope/steamui soft
 * id stubs — gamescope_ok_u_12041, steamui_ok_u_12042,
 * bigpicture_ok_u_12043, deckui_ok_u_12044, cec_ok_u_12045,
 * overlay_ok_u_12046, fps_limit_ok_u_12047, hdr_ok_u_12048,
 * gamescope_soft_ready_u_12049, batch_id_12050). Unique surface only;
 * no multi-def. Ok units remain 0. Distinct from
 * gj_gamescope_soft_ready_u_11949 (batch11949),
 * gj_gamescope_soft_ready_u_11749, and prior *049 soft-ready lamps. No
 * parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12049_marker[] = "libcgj-batch12049";

/* Soft continuum-ready lamp for gamescope/steamui id stubs wave. */
#define B12049_GAMESCOPE_SOFT_READY  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12049_soft_ready(void)
{
	return B12049_GAMESCOPE_SOFT_READY;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_gamescope_soft_ready_u_12049 - gamescope/steamui soft id continuum ready.
 *
 * Always returns 1. Soft pure-data product tag that the exclusive
 * wave surfaces are present. Does not call libc. No parent wires.
 */
uint32_t
gj_gamescope_soft_ready_u_12049(void)
{
	(void)NULL;
	return b12049_soft_ready();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_gamescope_soft_ready_u_12049(void)
    __attribute__((alias("gj_gamescope_soft_ready_u_12049")));
