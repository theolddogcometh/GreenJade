/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13446: gamescope soft overlay ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_overlay_ok_u_13446(void);
 *     - Returns 0 (soft stub: gamescope/Steam overlay slot is
 *       soft/unprobed; not a runtime overlay inject probe).
 *   uint32_t __gj_overlay_ok_u_13446  (alias)
 *   __libcgj_batch13446_marker = "libcgj-batch13446"
 *
 * Exclusive continuum CREATE-ONLY (13441-13450: gamescope soft stubs —
 * ok slots all→0; gamescope_soft_ready→1; batch_id→13450 —
 * gamescope_ok_u_13441, steamui_ok_u_13442, bigpicture_ok_u_13443,
 * deckui_ok_u_13444, cec_ok_u_13445, overlay_ok_u_13446,
 * fps_limit_ok_u_13447, hdr_ok_u_13448, gamescope_soft_ready_u_13449,
 * batch_id_13450). Unique gj_overlay_ok_u_13446 surface only; no
 * multi-def. Distinct from gj_overlay_ok_u_10046 (batch10046), and
 * sibling gamescope soft ok_u stubs in this wave. No parent wires. No
 * __int128. No overlay implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13446_marker[] = "libcgj-batch13446";

/* Soft-stub overlay ok value (unprobed). */
#define B13446_OVERLAY_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13446_overlay_ok(void)
{
	return B13446_OVERLAY_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_overlay_ok_u_13446 - soft overlay ok stub.
 *
 * Always returns 0 (soft/unprobed overlay slot). Does not call libc or
 * probe gamescope/Steam overlays. No parent wires.
 */
uint32_t
gj_overlay_ok_u_13446(void)
{
	(void)NULL;
	return b13446_overlay_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_overlay_ok_u_13446(void)
    __attribute__((alias("gj_overlay_ok_u_13446")));
