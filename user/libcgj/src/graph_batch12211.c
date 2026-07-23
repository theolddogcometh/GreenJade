/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12211: Proton ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_proton_ok_u_12211(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       Proton/Wine runtime probe for the proton soft continuum.
 *   uint32_t __gj_proton_ok_u_12211  (alias)
 *   __libcgj_batch12211_marker = "libcgj-batch12211"
 *
 * Exclusive continuum CREATE-ONLY (12211-12220: proton soft all→0 —
 * proton_ok_u_12211, wine_ok_u_12212, proton_ge_ok_u_12213,
 * steam_runtime_ok_u_12214, pressure_vessel_ok_u_12215,
 * compat_tool_ok_u_12216, dxvk_ok_u_12217, vkd3d_ok_u_12218,
 * proton_soft_ready_u_12219, batch_id_12220). Unique surface only; no
 * multi-def. Distinct from gj_proton_ok_u_10211 (batch10211). No parent
 * wires. No __int128. No Proton/Wine implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12211_marker[] = "libcgj-batch12211";

/* Soft Proton-ok lamp: always off (not a real Proton probe). */
#define B12211_PROTON_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12211_proton_ok(void)
{
	return B12211_PROTON_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_proton_ok_u_12211 - Proton ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not inspect Proton or
 * call libc. No parent wires.
 */
uint32_t
gj_proton_ok_u_12211(void)
{
	(void)NULL;
	return b12211_proton_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_proton_ok_u_12211(void)
    __attribute__((alias("gj_proton_ok_u_12211")));
