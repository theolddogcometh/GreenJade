/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10943: bigpicture soft-ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_bigpicture_ok_u_10943(void);
 *     - Returns 0 (soft stub: Big Picture mode slot is soft/unprobed;
 *       not a runtime Big Picture probe).
 *   uint32_t __gj_bigpicture_ok_u_10943  (alias)
 *   __libcgj_batch10943_marker = "libcgj-batch10943"
 *
 * Exclusive continuum CREATE-ONLY (10941-10950: gamescope soft all→0
 * stubs — gamescope_ok_u_10941, steamui_ok_u_10942,
 * bigpicture_ok_u_10943, deckui_ok_u_10944, cec_ok_u_10945,
 * overlay_ok_u_10946, fps_limit_ok_u_10947, hdr_ok_u_10948,
 * gamescope_soft_ready_u_10949, batch_id_10950). Unique
 * gj_bigpicture_ok_u_10943 surface only; no multi-def. Distinct from
 * gj_bigpicture_ok_u_10743 / 10543 / 10343 / 10043. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10943_marker[] = "libcgj-batch10943";

/* Soft-stub bigpicture ok value (unprobed). */
#define B10943_BIGPICTURE_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10943_bigpicture_ok(void)
{
	return B10943_BIGPICTURE_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bigpicture_ok_u_10943 - soft Big Picture mode ok stub.
 *
 * Always returns 0 (soft/unprobed bigpicture slot). Does not call libc
 * or probe Big Picture. No parent wires.
 */
uint32_t
gj_bigpicture_ok_u_10943(void)
{
	(void)NULL;
	return b10943_bigpicture_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bigpicture_ok_u_10943(void)
    __attribute__((alias("gj_bigpicture_ok_u_10943")));
