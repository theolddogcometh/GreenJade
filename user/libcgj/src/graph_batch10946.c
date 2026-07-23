/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10946: overlay soft-ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_overlay_ok_u_10946(void);
 *     - Returns 0 (soft stub: gamescope overlay slot is soft/unprobed;
 *       not a runtime overlay probe).
 *   uint32_t __gj_overlay_ok_u_10946  (alias)
 *   __libcgj_batch10946_marker = "libcgj-batch10946"
 *
 * Exclusive continuum CREATE-ONLY (10941-10950: gamescope soft all→0
 * stubs — gamescope_ok_u_10941, steamui_ok_u_10942,
 * bigpicture_ok_u_10943, deckui_ok_u_10944, cec_ok_u_10945,
 * overlay_ok_u_10946, fps_limit_ok_u_10947, hdr_ok_u_10948,
 * gamescope_soft_ready_u_10949, batch_id_10950). Unique
 * gj_overlay_ok_u_10946 surface only; no multi-def. Distinct from
 * gj_overlay_ok_u_10746 / 10546 / 10346 / 10046. No parent wires. No
 * __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10946_marker[] = "libcgj-batch10946";

/* Soft-stub overlay ok value (unprobed). */
#define B10946_OVERLAY_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10946_overlay_ok(void)
{
	return B10946_OVERLAY_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_overlay_ok_u_10946 - soft gamescope overlay ok stub.
 *
 * Always returns 0 (soft/unprobed overlay slot). Does not call libc or
 * probe overlays. No parent wires.
 */
uint32_t
gj_overlay_ok_u_10946(void)
{
	(void)NULL;
	return b10946_overlay_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_overlay_ok_u_10946(void)
    __attribute__((alias("gj_overlay_ok_u_10946")));
