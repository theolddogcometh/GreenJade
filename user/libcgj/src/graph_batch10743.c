/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10743: bigpicture soft-ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_bigpicture_ok_u_10743(void);
 *     - Returns 0 (soft stub: Big Picture mode slot is soft/unprobed;
 *       not a runtime Big Picture probe).
 *   uint32_t __gj_bigpicture_ok_u_10743  (alias)
 *   __libcgj_batch10743_marker = "libcgj-batch10743"
 *
 * Exclusive continuum CREATE-ONLY (10741-10750: gamescope soft all→0
 * stubs — gamescope_ok_u_10741, steamui_ok_u_10742,
 * bigpicture_ok_u_10743, deckui_ok_u_10744, cec_ok_u_10745,
 * overlay_ok_u_10746, fps_limit_ok_u_10747, hdr_ok_u_10748,
 * gamescope_soft_ready_u_10749, batch_id_10750). Unique
 * gj_bigpicture_ok_u_10743 surface only; no multi-def. Distinct from
 * gj_bigpicture_ok_u_10543 / 10343 / 10043. No parent wires. No
 * __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10743_marker[] = "libcgj-batch10743";

/* Soft-stub bigpicture ok value (unprobed). */
#define B10743_BIGPICTURE_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10743_bigpicture_ok(void)
{
	return B10743_BIGPICTURE_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bigpicture_ok_u_10743 - soft Big Picture mode ok stub.
 *
 * Always returns 0 (soft/unprobed bigpicture slot). Does not call libc
 * or probe Big Picture. No parent wires.
 */
uint32_t
gj_bigpicture_ok_u_10743(void)
{
	(void)NULL;
	return b10743_bigpicture_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bigpicture_ok_u_10743(void)
    __attribute__((alias("gj_bigpicture_ok_u_10743")));
