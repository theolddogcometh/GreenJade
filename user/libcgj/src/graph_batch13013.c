/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13013: Proton-GE ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_proton_ge_ok_u_13013(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       Proton-GE runtime probe for the proton soft continuum.
 *   uint32_t __gj_proton_ge_ok_u_13013  (alias)
 *   __libcgj_batch13013_marker = "libcgj-batch13013"
 *
 * Exclusive continuum CREATE-ONLY (13011-13020: proton soft all→0
 * stubs — proton_ok_u_13011, wine_ok_u_13012, proton_ge_ok_u_13013,
 * steam_runtime_ok_u_13014, pressure_vessel_ok_u_13015,
 * compat_tool_ok_u_13016, dxvk_ok_u_13017, vkd3d_ok_u_13018,
 * proton_soft_ready_u_13019, batch_id_13020). Unique surface only; no
 * multi-def. Distinct from gj_proton_ok_u_13011 (batch13011) and
 * gj_proton_ge_ok_u_10313 / gj_proton_ge_ok_u_10213. No parent wires.
 * No __int128. No Proton-GE implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13013_marker[] = "libcgj-batch13013";

/* Soft Proton-GE-ok lamp: always off (not a real Proton-GE probe). */
#define B13013_PROTON_GE_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13013_proton_ge_ok(void)
{
	return B13013_PROTON_GE_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_proton_ge_ok_u_13013 - Proton-GE ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not inspect Proton-GE or
 * call libc. No parent wires.
 */
uint32_t
gj_proton_ge_ok_u_13013(void)
{
	(void)NULL;
	return b13013_proton_ge_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_proton_ge_ok_u_13013(void)
    __attribute__((alias("gj_proton_ge_ok_u_13013")));
