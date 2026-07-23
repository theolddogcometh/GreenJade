/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12214: Steam Runtime ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_steam_runtime_ok_u_12214(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       Steam Runtime probe for the proton soft continuum.
 *   uint32_t __gj_steam_runtime_ok_u_12214  (alias)
 *   __libcgj_batch12214_marker = "libcgj-batch12214"
 *
 * Exclusive continuum CREATE-ONLY (12211-12220: proton soft all→0 —
 * proton_ok_u_12211, wine_ok_u_12212, proton_ge_ok_u_12213,
 * steam_runtime_ok_u_12214, pressure_vessel_ok_u_12215,
 * compat_tool_ok_u_12216, dxvk_ok_u_12217, vkd3d_ok_u_12218,
 * proton_soft_ready_u_12219, batch_id_12220). Unique surface only; no
 * multi-def. Distinct from gj_steam_runtime_ok_u_10214 (batch10214).
 * No parent wires. No __int128. No Steam Runtime implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12214_marker[] = "libcgj-batch12214";

/* Soft Steam Runtime-ok lamp: always off (not a real runtime probe). */
#define B12214_STEAM_RUNTIME_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12214_steam_runtime_ok(void)
{
	return B12214_STEAM_RUNTIME_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_steam_runtime_ok_u_12214 - Steam Runtime ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not inspect Steam Runtime
 * or call libc. No parent wires.
 */
uint32_t
gj_steam_runtime_ok_u_12214(void)
{
	(void)NULL;
	return b12214_steam_runtime_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_steam_runtime_ok_u_12214(void)
    __attribute__((alias("gj_steam_runtime_ok_u_12214")));
