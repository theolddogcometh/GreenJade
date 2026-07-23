/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12613: Proton-GE ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_proton_ge_ok_u_12613(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       Proton-GE runtime probe for the proton soft continuum.
 *   uint32_t __gj_proton_ge_ok_u_12613  (alias)
 *   __libcgj_batch12613_marker = "libcgj-batch12613"
 *
 * Exclusive continuum CREATE-ONLY (12611-12620: proton soft all→0 —
 * proton_ok_u_12611, wine_ok_u_12612, proton_ge_ok_u_12613,
 * steam_runtime_ok_u_12614, pressure_vessel_ok_u_12615,
 * compat_tool_ok_u_12616, dxvk_ok_u_12617, vkd3d_ok_u_12618,
 * proton_soft_ready_u_12619, batch_id_12620→12620). Unique surface
 * only; no multi-def. Distinct from gj_proton_ge_ok_u_12213
 * (batch12213), gj_proton_ge_ok_u_12013 (batch12013), and
 * gj_proton_ok_u_12611 (batch12611). No parent wires. No __int128. No
 * Proton-GE implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12613_marker[] = "libcgj-batch12613";

/* Soft Proton-GE-ok lamp: always off (not a real Proton-GE probe). */
#define B12613_PROTON_GE_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12613_proton_ge_ok(void)
{
	return B12613_PROTON_GE_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_proton_ge_ok_u_12613 - Proton-GE ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not inspect Proton-GE or
 * call libc. No parent wires.
 */
uint32_t
gj_proton_ge_ok_u_12613(void)
{
	(void)NULL;
	return b12613_proton_ge_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_proton_ge_ok_u_12613(void)
    __attribute__((alias("gj_proton_ge_ok_u_12613")));
