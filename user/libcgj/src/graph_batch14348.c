/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14348: gamescope soft HDR ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_hdr_ok_u_14348(void);
 *     - Returns 0 (soft stub: gamescope HDR slot is soft/unprobed; not
 *       a runtime HDR/WCG display capability probe).
 *   uint32_t __gj_hdr_ok_u_14348  (alias)
 *   __libcgj_batch14348_marker = "libcgj-batch14348"
 *
 * Exclusive continuum CREATE-ONLY (14341-14350: gamescope soft stubs —
 * ok slots all→0; gamescope_soft_ready→1; batch_id→14350 —
 * gamescope_ok_u_14341, steamui_ok_u_14342, bigpicture_ok_u_14343,
 * deckui_ok_u_14344, cec_ok_u_14345, overlay_ok_u_14346,
 * fps_limit_ok_u_14347, hdr_ok_u_14348, gamescope_soft_ready_u_14349,
 * batch_id_14350). Unique gj_hdr_ok_u_14348 surface only; no multi-def.
 * Distinct from gj_hdr_ok_u_13448 (batch13448), and sibling gamescope
 * soft ok_u stubs in this wave. No parent wires. No __int128. No HDR
 * implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14348_marker[] = "libcgj-batch14348";

/* Soft-stub HDR ok value (unprobed). */
#define B14348_HDR_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14348_hdr_ok(void)
{
	return B14348_HDR_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hdr_ok_u_14348 - soft HDR ok stub.
 *
 * Always returns 0 (soft/unprobed HDR slot). Does not call libc or
 * probe gamescope HDR/WCG capability. No parent wires.
 */
uint32_t
gj_hdr_ok_u_14348(void)
{
	(void)NULL;
	return b14348_hdr_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_hdr_ok_u_14348(void)
    __attribute__((alias("gj_hdr_ok_u_14348")));
