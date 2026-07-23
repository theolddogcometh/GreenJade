/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10511: proton ok soft unit (not a probe).
 *
 * Surface (unique symbols):
 *   uint32_t gj_proton_ok_u_10511(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       Proton runtime probe for the proton soft continuum.
 *   uint32_t __gj_proton_ok_u_10511  (alias)
 *   __libcgj_batch10511_marker = "libcgj-batch10511"
 *
 * Exclusive continuum CREATE-ONLY (10511-10520: proton soft stubs —
 * proton_ok_u_10511, wine_ok_u_10512, proton_ge_ok_u_10513,
 * steam_runtime_ok_u_10514, pressure_vessel_ok_u_10515,
 * compat_tool_ok_u_10516, dxvk_ok_u_10517, vkd3d_ok_u_10518,
 * proton_soft_ready_u_10519, batch_id_10520). Unique surface only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10511_marker[] = "libcgj-batch10511";

/* Soft proton-ok lamp: always off (not a real runtime probe). */
#define B10511_PROTON_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10511_ok(void)
{
	return B10511_PROTON_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_proton_ok_u_10511 - proton ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe Proton
 * presence or call libc. No parent wires.
 */
uint32_t
gj_proton_ok_u_10511(void)
{
	(void)NULL;
	return b10511_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_proton_ok_u_10511(void)
    __attribute__((alias("gj_proton_ok_u_10511")));
