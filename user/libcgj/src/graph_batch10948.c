/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10948: HDR soft-ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_hdr_ok_u_10948(void);
 *     - Returns 0 (soft stub: HDR slot is soft/unprobed; not a runtime
 *       HDR capability probe).
 *   uint32_t __gj_hdr_ok_u_10948  (alias)
 *   __libcgj_batch10948_marker = "libcgj-batch10948"
 *
 * Exclusive continuum CREATE-ONLY (10941-10950: gamescope soft all→0
 * stubs — gamescope_ok_u_10941, steamui_ok_u_10942,
 * bigpicture_ok_u_10943, deckui_ok_u_10944, cec_ok_u_10945,
 * overlay_ok_u_10946, fps_limit_ok_u_10947, hdr_ok_u_10948,
 * gamescope_soft_ready_u_10949, batch_id_10950). Unique
 * gj_hdr_ok_u_10948 surface only; no multi-def. Distinct from
 * gj_hdr_ok_u_10748 / 10548 / 10348 / 10248 / 10048. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10948_marker[] = "libcgj-batch10948";

/* Soft-stub HDR ok value (unprobed). */
#define B10948_HDR_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10948_hdr_ok(void)
{
	return B10948_HDR_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hdr_ok_u_10948 - soft HDR ok stub.
 *
 * Always returns 0 (soft/unprobed HDR slot). Does not call libc or
 * probe HDR capability. No parent wires.
 */
uint32_t
gj_hdr_ok_u_10948(void)
{
	(void)NULL;
	return b10948_hdr_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_hdr_ok_u_10948(void)
    __attribute__((alias("gj_hdr_ok_u_10948")));
