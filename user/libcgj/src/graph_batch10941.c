/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10941: gamescope soft-ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_gamescope_ok_u_10941(void);
 *     - Returns 0 (soft stub: gamescope compositor slot is soft/unprobed;
 *       not a runtime gamescope probe).
 *   uint32_t __gj_gamescope_ok_u_10941  (alias)
 *   __libcgj_batch10941_marker = "libcgj-batch10941"
 *
 * Exclusive continuum CREATE-ONLY (10941-10950: gamescope soft all→0
 * stubs — gamescope_ok_u_10941, steamui_ok_u_10942,
 * bigpicture_ok_u_10943, deckui_ok_u_10944, cec_ok_u_10945,
 * overlay_ok_u_10946, fps_limit_ok_u_10947, hdr_ok_u_10948,
 * gamescope_soft_ready_u_10949, batch_id_10950). Unique
 * gj_gamescope_ok_u_10941 surface only; no multi-def. Distinct from
 * gj_gamescope_ok_u_10741 / 10541 / 10341 / 10241 / 10041. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10941_marker[] = "libcgj-batch10941";

/* Soft-stub gamescope ok value (unprobed). */
#define B10941_GAMESCOPE_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10941_gamescope_ok(void)
{
	return B10941_GAMESCOPE_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_gamescope_ok_u_10941 - soft gamescope compositor ok stub.
 *
 * Always returns 0 (soft/unprobed gamescope slot). Does not call libc or
 * probe gamescope. No parent wires.
 */
uint32_t
gj_gamescope_ok_u_10941(void)
{
	(void)NULL;
	return b10941_gamescope_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_gamescope_ok_u_10941(void)
    __attribute__((alias("gj_gamescope_ok_u_10941")));
