/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12411: Proton ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_proton_ok_u_12411(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       Proton/Wine runtime probe for the proton soft continuum.
 *   uint32_t __gj_proton_ok_u_12411  (alias)
 *   __libcgj_batch12411_marker = "libcgj-batch12411"
 *
 * Exclusive continuum CREATE-ONLY (12411-12420: proton soft all→0 —
 * proton_ok_u_12411, wine_ok_u_12412, proton_ge_ok_u_12413,
 * steam_runtime_ok_u_12414, pressure_vessel_ok_u_12415,
 * compat_tool_ok_u_12416, dxvk_ok_u_12417, vkd3d_ok_u_12418,
 * proton_soft_ready_u_12419, batch_id_12420→12420). Unique surface
 * only; no multi-def. Distinct from gj_proton_ok_u_12211 (batch12211),
 * gj_proton_ok_u_11911 (batch11911), and gj_proton_ok_u_10211
 * (batch10211). No parent wires. No __int128. No Proton/Wine
 * implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12411_marker[] = "libcgj-batch12411";

/* Soft Proton-ok lamp: always off (not a real Proton probe). */
#define B12411_PROTON_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12411_proton_ok(void)
{
	return B12411_PROTON_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_proton_ok_u_12411 - Proton ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not inspect Proton or
 * call libc. No parent wires.
 */
uint32_t
gj_proton_ok_u_12411(void)
{
	(void)NULL;
	return b12411_proton_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_proton_ok_u_12411(void)
    __attribute__((alias("gj_proton_ok_u_12411")));
