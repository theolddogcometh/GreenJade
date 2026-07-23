/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12213: Proton-GE ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_proton_ge_ok_u_12213(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       Proton-GE runtime probe for the proton soft continuum.
 *   uint32_t __gj_proton_ge_ok_u_12213  (alias)
 *   __libcgj_batch12213_marker = "libcgj-batch12213"
 *
 * Exclusive continuum CREATE-ONLY (12211-12220: proton soft all→0 —
 * proton_ok_u_12211, wine_ok_u_12212, proton_ge_ok_u_12213,
 * steam_runtime_ok_u_12214, pressure_vessel_ok_u_12215,
 * compat_tool_ok_u_12216, dxvk_ok_u_12217, vkd3d_ok_u_12218,
 * proton_soft_ready_u_12219, batch_id_12220). Unique surface only; no
 * multi-def. Distinct from gj_proton_ge_ok_u_10213 / _10011 and
 * gj_proton_ok_u_12211 (batch12211). No parent wires. No __int128. No
 * Proton-GE implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12213_marker[] = "libcgj-batch12213";

/* Soft Proton-GE-ok lamp: always off (not a real Proton-GE probe). */
#define B12213_PROTON_GE_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12213_proton_ge_ok(void)
{
	return B12213_PROTON_GE_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_proton_ge_ok_u_12213 - Proton-GE ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not inspect Proton-GE or
 * call libc. No parent wires.
 */
uint32_t
gj_proton_ge_ok_u_12213(void)
{
	(void)NULL;
	return b12213_proton_ge_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_proton_ge_ok_u_12213(void)
    __attribute__((alias("gj_proton_ge_ok_u_12213")));
