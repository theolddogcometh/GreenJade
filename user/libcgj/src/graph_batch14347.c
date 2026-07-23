/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14347: gamescope soft fps_limit ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_fps_limit_ok_u_14347(void);
 *     - Returns 0 (soft stub: gamescope FPS-limit slot is
 *       soft/unprobed; not a runtime frame-cap configuration probe).
 *   uint32_t __gj_fps_limit_ok_u_14347  (alias)
 *   __libcgj_batch14347_marker = "libcgj-batch14347"
 *
 * Exclusive continuum CREATE-ONLY (14341-14350: gamescope soft stubs —
 * ok slots all→0; gamescope_soft_ready→1; batch_id→14350 —
 * gamescope_ok_u_14341, steamui_ok_u_14342, bigpicture_ok_u_14343,
 * deckui_ok_u_14344, cec_ok_u_14345, overlay_ok_u_14346,
 * fps_limit_ok_u_14347, hdr_ok_u_14348, gamescope_soft_ready_u_14349,
 * batch_id_14350). Unique gj_fps_limit_ok_u_14347 surface only; no
 * multi-def. Distinct from gj_fps_limit_ok_u_13447 (batch13447), and
 * sibling gamescope soft ok_u stubs in this wave. No parent wires. No
 * __int128. No FPS-limit implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14347_marker[] = "libcgj-batch14347";

/* Soft-stub fps_limit ok value (unprobed). */
#define B14347_FPS_LIMIT_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14347_fps_limit_ok(void)
{
	return B14347_FPS_LIMIT_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fps_limit_ok_u_14347 - soft fps_limit ok stub.
 *
 * Always returns 0 (soft/unprobed FPS-limit slot). Does not call libc
 * or probe gamescope frame-cap settings. No parent wires.
 */
uint32_t
gj_fps_limit_ok_u_14347(void)
{
	(void)NULL;
	return b14347_fps_limit_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fps_limit_ok_u_14347(void)
    __attribute__((alias("gj_fps_limit_ok_u_14347")));
