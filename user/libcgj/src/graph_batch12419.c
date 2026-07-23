/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12419: proton soft continuum lamp (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_proton_soft_ready_u_12419(void);
 *     - Always returns 0. Soft lamp only; does not claim proton/wine/
 *       proton_ge/steam_runtime/pressure_vessel/compat_tool/dxvk/vkd3d
 *       ok for the continuum.
 *   uint32_t __gj_proton_soft_ready_u_12419  (alias)
 *   __libcgj_batch12419_marker = "libcgj-batch12419"
 *
 * Exclusive continuum CREATE-ONLY (12411-12420: proton soft all→0 —
 * proton_ok_u_12411, wine_ok_u_12412, proton_ge_ok_u_12413,
 * steam_runtime_ok_u_12414, pressure_vessel_ok_u_12415,
 * compat_tool_ok_u_12416, dxvk_ok_u_12417, vkd3d_ok_u_12418,
 * proton_soft_ready_u_12419, batch_id_12420→12420). Unique surface
 * only; no multi-def. Distinct from gj_proton_soft_ready_u_12219 /
 * _10219 / _10019 / _8989. proton/wine/proton_ge/steam_runtime/
 * pressure_vessel/compat_tool/dxvk/vkd3d ok units remain 0. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12419_marker[] = "libcgj-batch12419";

/* Soft continuum lamp: off (soft-ready unit is intentionally 0). */
#define B12419_SOFT_READY  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12419_soft_ready(void)
{
	return B12419_SOFT_READY;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_proton_soft_ready_u_12419 - proton soft continuum ready lamp.
 *
 * Always returns 0. Soft pure-data product tag for exclusive wave
 * 12411-12420; does not claim proton/wine/proton_ge/steam_runtime/
 * pressure_vessel/compat_tool/dxvk/vkd3d ok and does not call libc.
 * No parent wires.
 */
uint32_t
gj_proton_soft_ready_u_12419(void)
{
	(void)NULL;
	return b12419_soft_ready();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_proton_soft_ready_u_12419(void)
    __attribute__((alias("gj_proton_soft_ready_u_12419")));
