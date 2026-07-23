/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10942: steamui soft-ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_steamui_ok_u_10942(void);
 *     - Returns 0 (soft stub: Steam UI slot is soft/unprobed; not a
 *       runtime Steam UI probe).
 *   uint32_t __gj_steamui_ok_u_10942  (alias)
 *   __libcgj_batch10942_marker = "libcgj-batch10942"
 *
 * Exclusive continuum CREATE-ONLY (10941-10950: gamescope soft all→0
 * stubs — gamescope_ok_u_10941, steamui_ok_u_10942,
 * bigpicture_ok_u_10943, deckui_ok_u_10944, cec_ok_u_10945,
 * overlay_ok_u_10946, fps_limit_ok_u_10947, hdr_ok_u_10948,
 * gamescope_soft_ready_u_10949, batch_id_10950). Unique
 * gj_steamui_ok_u_10942 surface only; no multi-def. Distinct from
 * gj_steamui_ok_u_10742 / 10542 / 10342 / 10042. No parent wires. No
 * __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10942_marker[] = "libcgj-batch10942";

/* Soft-stub steamui ok value (unprobed). */
#define B10942_STEAMUI_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10942_steamui_ok(void)
{
	return B10942_STEAMUI_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_steamui_ok_u_10942 - soft Steam UI ok stub.
 *
 * Always returns 0 (soft/unprobed steamui slot). Does not call libc or
 * probe Steam UI. No parent wires.
 */
uint32_t
gj_steamui_ok_u_10942(void)
{
	(void)NULL;
	return b10942_steamui_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_steamui_ok_u_10942(void)
    __attribute__((alias("gj_steamui_ok_u_10942")));
