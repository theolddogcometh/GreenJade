/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14349: gamescope soft ready aggregate tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_gamescope_soft_ready_u_14349(void);
 *     - Returns 1 (soft compile-time product tag: gamescope soft
 *       continuum wave 14341-14350 is present/ready; not a runtime hard
 *       probe of any gamescope/steamui stack).
 *   uint32_t __gj_gamescope_soft_ready_u_14349  (alias)
 *   __libcgj_batch14349_marker = "libcgj-batch14349"
 *
 * Exclusive continuum CREATE-ONLY (14341-14350: gamescope soft stubs —
 * ok slots all→0; gamescope_soft_ready→1; batch_id→14350 —
 * gamescope_ok_u_14341, steamui_ok_u_14342, bigpicture_ok_u_14343,
 * deckui_ok_u_14344, cec_ok_u_14345, overlay_ok_u_14346,
 * fps_limit_ok_u_14347, hdr_ok_u_14348, gamescope_soft_ready_u_14349,
 * batch_id_14350). Unique gj_gamescope_soft_ready_u_14349 surface
 * only; no multi-def. Distinct from per-slot ok_u soft stubs
 * (14341-14348, all→0), gj_batch_id_14350, and prior
 * gj_gamescope_soft_ready_u_13449 (batch13449). No parent wires. No
 * __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14349_marker[] = "libcgj-batch14349";

/* Soft gamescope continuum ready lamp (wave present). */
#define B14349_GAMESCOPE_SOFT_READY  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14349_gamescope_soft_ready(void)
{
	return B14349_GAMESCOPE_SOFT_READY;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_gamescope_soft_ready_u_14349 - soft gamescope continuum ready tag.
 *
 * Always returns 1 (gamescope soft wave ready/present). Does not call
 * libc or hard-probe any gamescope/steamui slot. No parent wires.
 */
uint32_t
gj_gamescope_soft_ready_u_14349(void)
{
	(void)NULL;
	return b14349_gamescope_soft_ready();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_gamescope_soft_ready_u_14349(void)
    __attribute__((alias("gj_gamescope_soft_ready_u_14349")));
