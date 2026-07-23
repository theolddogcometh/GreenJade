/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10546: overlay soft-ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_overlay_ok_u_10546(void);
 *     - Returns 0 (soft stub: gamescope overlay slot is soft/unprobed;
 *       not a runtime overlay probe).
 *   uint32_t __gj_overlay_ok_u_10546  (alias)
 *   __libcgj_batch10546_marker = "libcgj-batch10546"
 *
 * Exclusive continuum CREATE-ONLY (10541-10550: gamescope soft stubs —
 * all→0). Unique gj_overlay_ok_u_10546 surface only; no multi-def.
 * Distinct from sibling gamescope soft ok_u stubs in this wave. No
 * parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10546_marker[] = "libcgj-batch10546";

/* Soft-stub overlay ok value (unprobed). */
#define B10546_OVERLAY_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10546_overlay_ok(void)
{
	return B10546_OVERLAY_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_overlay_ok_u_10546 - soft gamescope overlay ok stub.
 *
 * Always returns 0 (soft/unprobed overlay slot). Does not call libc or
 * probe overlays. No parent wires.
 */
uint32_t
gj_overlay_ok_u_10546(void)
{
	(void)NULL;
	return b10546_overlay_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_overlay_ok_u_10546(void)
    __attribute__((alias("gj_overlay_ok_u_10546")));
