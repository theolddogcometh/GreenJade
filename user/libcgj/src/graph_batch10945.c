/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10945: CEC soft-ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_cec_ok_u_10945(void);
 *     - Returns 0 (soft stub: HDMI-CEC slot is soft/unprobed; not a
 *       runtime CEC probe).
 *   uint32_t __gj_cec_ok_u_10945  (alias)
 *   __libcgj_batch10945_marker = "libcgj-batch10945"
 *
 * Exclusive continuum CREATE-ONLY (10941-10950: gamescope soft all→0
 * stubs — gamescope_ok_u_10941, steamui_ok_u_10942,
 * bigpicture_ok_u_10943, deckui_ok_u_10944, cec_ok_u_10945,
 * overlay_ok_u_10946, fps_limit_ok_u_10947, hdr_ok_u_10948,
 * gamescope_soft_ready_u_10949, batch_id_10950). Unique
 * gj_cec_ok_u_10945 surface only; no multi-def. Distinct from
 * gj_cec_ok_u_10745 / 10545 / 10345 / 10045. No parent wires. No
 * __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10945_marker[] = "libcgj-batch10945";

/* Soft-stub CEC ok value (unprobed). */
#define B10945_CEC_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10945_cec_ok(void)
{
	return B10945_CEC_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cec_ok_u_10945 - soft HDMI-CEC ok stub.
 *
 * Always returns 0 (soft/unprobed CEC slot). Does not call libc or
 * probe CEC hardware. No parent wires.
 */
uint32_t
gj_cec_ok_u_10945(void)
{
	(void)NULL;
	return b10945_cec_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_cec_ok_u_10945(void)
    __attribute__((alias("gj_cec_ok_u_10945")));
