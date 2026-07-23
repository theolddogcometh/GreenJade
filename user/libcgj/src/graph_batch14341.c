/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14341: gamescope soft ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_gamescope_ok_u_14341(void);
 *     - Returns 0 (soft stub: gamescope compositor slot is
 *       soft/unprobed; not a runtime gamescope/wlroots probe).
 *   uint32_t __gj_gamescope_ok_u_14341  (alias)
 *   __libcgj_batch14341_marker = "libcgj-batch14341"
 *
 * Exclusive continuum CREATE-ONLY (14341-14350: gamescope soft stubs —
 * ok slots all→0; gamescope_soft_ready→1; batch_id→14350 —
 * gamescope_ok_u_14341, steamui_ok_u_14342, bigpicture_ok_u_14343,
 * deckui_ok_u_14344, cec_ok_u_14345, overlay_ok_u_14346,
 * fps_limit_ok_u_14347, hdr_ok_u_14348, gamescope_soft_ready_u_14349,
 * batch_id_14350). Unique gj_gamescope_ok_u_14341 surface only; no
 * multi-def. Distinct from gj_gamescope_ok_u_13441 (batch13441),
 * gj_gamescope_ok_u_10541 (batch10541), and sibling gamescope soft
 * ok_u stubs in this wave. No parent wires. No __int128. No
 * gamescope implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14341_marker[] = "libcgj-batch14341";

/* Soft-stub gamescope ok value (unprobed). */
#define B14341_GAMESCOPE_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14341_gamescope_ok(void)
{
	return B14341_GAMESCOPE_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_gamescope_ok_u_14341 - soft gamescope ok stub.
 *
 * Always returns 0 (soft/unprobed gamescope slot). Does not call libc
 * or probe gamescope compositor presence. No parent wires.
 */
uint32_t
gj_gamescope_ok_u_14341(void)
{
	(void)NULL;
	return b14341_gamescope_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_gamescope_ok_u_14341(void)
    __attribute__((alias("gj_gamescope_ok_u_14341")));
