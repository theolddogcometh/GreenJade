/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12013: Proton-GE ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_proton_ge_ok_u_12013(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       Proton-GE runtime probe for the proton/wine continuum.
 *   uint32_t __gj_proton_ge_ok_u_12013  (alias)
 *   __libcgj_batch12013_marker = "libcgj-batch12013"
 *
 * Exclusive continuum CREATE-ONLY (12011-12020: proton soft all→0 —
 * proton_ok_u_12011, wine_ok_u_12012, proton_ge_ok_u_12013,
 * steam_runtime_ok_u_12014, pressure_vessel_ok_u_12015,
 * compat_tool_ok_u_12016, dxvk_ok_u_12017, vkd3d_ok_u_12018,
 * proton_soft_ready_u_12019, batch_id_12020→12020). Unique surface
 * only; no multi-def. Distinct from gj_proton_ok_u_12011 (batch12011),
 * gj_proton_ge_ok_u_11913 (batch11913), gj_proton_ge_ok_u_11713
 * (batch11713), and gj_proton_ge_ok_u_11513 (batch11513). No parent
 * wires. No __int128. No Proton-GE implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12013_marker[] = "libcgj-batch12013";

/* Soft Proton-GE-ok lamp: always off (not a real Proton-GE probe). */
#define B12013_PROTON_GE_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12013_proton_ge_ok(void)
{
	return B12013_PROTON_GE_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_proton_ge_ok_u_12013 - Proton-GE ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not inspect Proton-GE or
 * call libc. No parent wires.
 */
uint32_t
gj_proton_ge_ok_u_12013(void)
{
	(void)NULL;
	return b12013_proton_ge_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_proton_ge_ok_u_12013(void)
    __attribute__((alias("gj_proton_ge_ok_u_12013")));
