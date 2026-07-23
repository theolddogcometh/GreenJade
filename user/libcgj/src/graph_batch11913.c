/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11913: Proton-GE ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_proton_ge_ok_u_11913(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       Proton-GE runtime probe for the proton/wine continuum.
 *   uint32_t __gj_proton_ge_ok_u_11913  (alias)
 *   __libcgj_batch11913_marker = "libcgj-batch11913"
 *
 * Exclusive continuum CREATE-ONLY (11911-11920: proton soft all→0 —
 * proton_ok_u_11911, wine_ok_u_11912, proton_ge_ok_u_11913,
 * steam_runtime_ok_u_11914, pressure_vessel_ok_u_11915,
 * compat_tool_ok_u_11916, dxvk_ok_u_11917, vkd3d_ok_u_11918,
 * proton_soft_ready_u_11919, batch_id_11920→11920). Unique surface
 * only; no multi-def. Distinct from gj_proton_ok_u_11911 (batch11911),
 * gj_proton_ge_ok_u_11713 (batch11713), gj_proton_ge_ok_u_11513
 * (batch11513), and gj_proton_ge_ok_u_11313 (batch11313). No parent
 * wires. No __int128. No Proton-GE implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11913_marker[] = "libcgj-batch11913";

/* Soft Proton-GE-ok lamp: always off (not a real Proton-GE probe). */
#define B11913_PROTON_GE_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11913_proton_ge_ok(void)
{
	return B11913_PROTON_GE_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_proton_ge_ok_u_11913 - Proton-GE ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not inspect Proton-GE or
 * call libc. No parent wires.
 */
uint32_t
gj_proton_ge_ok_u_11913(void)
{
	(void)NULL;
	return b11913_proton_ge_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_proton_ge_ok_u_11913(void)
    __attribute__((alias("gj_proton_ge_ok_u_11913")));
