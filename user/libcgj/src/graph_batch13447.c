/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13447: gamescope soft fps_limit ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_fps_limit_ok_u_13447(void);
 *     - Returns 0 (soft stub: gamescope FPS-limit slot is
 *       soft/unprobed; not a runtime frame-cap configuration probe).
 *   uint32_t __gj_fps_limit_ok_u_13447  (alias)
 *   __libcgj_batch13447_marker = "libcgj-batch13447"
 *
 * Exclusive continuum CREATE-ONLY (13441-13450: gamescope soft stubs —
 * ok slots all→0; gamescope_soft_ready→1; batch_id→13450 —
 * gamescope_ok_u_13441, steamui_ok_u_13442, bigpicture_ok_u_13443,
 * deckui_ok_u_13444, cec_ok_u_13445, overlay_ok_u_13446,
 * fps_limit_ok_u_13447, hdr_ok_u_13448, gamescope_soft_ready_u_13449,
 * batch_id_13450). Unique gj_fps_limit_ok_u_13447 surface only; no
 * multi-def. Distinct from gj_fps_limit_ok_u_10047 (batch10047), and
 * sibling gamescope soft ok_u stubs in this wave. No parent wires. No
 * __int128. No FPS-limit implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13447_marker[] = "libcgj-batch13447";

/* Soft-stub fps_limit ok value (unprobed). */
#define B13447_FPS_LIMIT_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13447_fps_limit_ok(void)
{
	return B13447_FPS_LIMIT_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fps_limit_ok_u_13447 - soft fps_limit ok stub.
 *
 * Always returns 0 (soft/unprobed FPS-limit slot). Does not call libc
 * or probe gamescope frame-cap settings. No parent wires.
 */
uint32_t
gj_fps_limit_ok_u_13447(void)
{
	(void)NULL;
	return b13447_fps_limit_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fps_limit_ok_u_13447(void)
    __attribute__((alias("gj_fps_limit_ok_u_13447")));
