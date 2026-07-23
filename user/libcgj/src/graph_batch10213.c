/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10213: Proton-GE ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_proton_ge_ok_u_10213(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       Proton-GE runtime probe for the proton/wine continuum.
 *   uint32_t __gj_proton_ge_ok_u_10213  (alias)
 *   __libcgj_batch10213_marker = "libcgj-batch10213"
 *
 * Exclusive continuum CREATE-ONLY (10211-10220: proton/wine soft product
 * stubs — proton_ok_u_10211, wine_ok_u_10212, proton_ge_ok_u_10213,
 * steam_runtime_ok_u_10214, pressure_vessel_ok_u_10215,
 * compat_tool_ok_u_10216, dxvk_ok_u_10217, vkd3d_ok_u_10218,
 * proton_soft_ready_u_10219, batch_id_10220). Unique surface only; no
 * multi-def. Distinct from gj_proton_ok_u_10211 (batch10211). No parent
 * wires. No __int128. No Proton-GE implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10213_marker[] = "libcgj-batch10213";

/* Soft Proton-GE-ok lamp: always off (not a real Proton-GE probe). */
#define B10213_PROTON_GE_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10213_proton_ge_ok(void)
{
	return B10213_PROTON_GE_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_proton_ge_ok_u_10213 - Proton-GE ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not inspect Proton-GE or
 * call libc. No parent wires.
 */
uint32_t
gj_proton_ge_ok_u_10213(void)
{
	(void)NULL;
	return b10213_proton_ge_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_proton_ge_ok_u_10213(void)
    __attribute__((alias("gj_proton_ge_ok_u_10213")));
