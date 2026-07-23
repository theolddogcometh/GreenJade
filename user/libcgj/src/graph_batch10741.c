/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10741: gamescope soft-ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_gamescope_ok_u_10741(void);
 *     - Returns 0 (soft stub: gamescope compositor slot is soft/unprobed;
 *       not a runtime gamescope probe).
 *   uint32_t __gj_gamescope_ok_u_10741  (alias)
 *   __libcgj_batch10741_marker = "libcgj-batch10741"
 *
 * Exclusive continuum CREATE-ONLY (10741-10750: gamescope soft all→0
 * stubs — gamescope_ok_u_10741, steamui_ok_u_10742,
 * bigpicture_ok_u_10743, deckui_ok_u_10744, cec_ok_u_10745,
 * overlay_ok_u_10746, fps_limit_ok_u_10747, hdr_ok_u_10748,
 * gamescope_soft_ready_u_10749, batch_id_10750). Unique
 * gj_gamescope_ok_u_10741 surface only; no multi-def. Distinct from
 * gj_gamescope_ok_u_10541 / 10341 / 10241 / 10041. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10741_marker[] = "libcgj-batch10741";

/* Soft-stub gamescope ok value (unprobed). */
#define B10741_GAMESCOPE_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10741_gamescope_ok(void)
{
	return B10741_GAMESCOPE_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_gamescope_ok_u_10741 - soft gamescope compositor ok stub.
 *
 * Always returns 0 (soft/unprobed gamescope slot). Does not call libc or
 * probe gamescope. No parent wires.
 */
uint32_t
gj_gamescope_ok_u_10741(void)
{
	(void)NULL;
	return b10741_gamescope_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_gamescope_ok_u_10741(void)
    __attribute__((alias("gj_gamescope_ok_u_10741")));
