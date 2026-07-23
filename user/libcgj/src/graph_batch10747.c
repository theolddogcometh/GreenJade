/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10747: fps_limit soft-ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_fps_limit_ok_u_10747(void);
 *     - Returns 0 (soft stub: FPS limit slot is soft/unprobed; not a
 *       runtime FPS-limit probe).
 *   uint32_t __gj_fps_limit_ok_u_10747  (alias)
 *   __libcgj_batch10747_marker = "libcgj-batch10747"
 *
 * Exclusive continuum CREATE-ONLY (10741-10750: gamescope soft all→0
 * stubs — gamescope_ok_u_10741, steamui_ok_u_10742,
 * bigpicture_ok_u_10743, deckui_ok_u_10744, cec_ok_u_10745,
 * overlay_ok_u_10746, fps_limit_ok_u_10747, hdr_ok_u_10748,
 * gamescope_soft_ready_u_10749, batch_id_10750). Unique
 * gj_fps_limit_ok_u_10747 surface only; no multi-def. Distinct from
 * gj_fps_limit_ok_u_10547 / 10347 / 10047 and prior fps_limit_ok
 * surfaces. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10747_marker[] = "libcgj-batch10747";

/* Soft-stub fps_limit ok value (unprobed). */
#define B10747_FPS_LIMIT_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10747_fps_limit_ok(void)
{
	return B10747_FPS_LIMIT_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fps_limit_ok_u_10747 - soft FPS limit ok stub.
 *
 * Always returns 0 (soft/unprobed fps_limit slot). Does not call libc
 * or probe FPS limits. No parent wires.
 */
uint32_t
gj_fps_limit_ok_u_10747(void)
{
	(void)NULL;
	return b10747_fps_limit_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fps_limit_ok_u_10747(void)
    __attribute__((alias("gj_fps_limit_ok_u_10747")));
