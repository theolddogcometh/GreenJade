/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10019: soft Proton/dxvk id stub continuum ready.
 *
 * Surface (unique symbols):
 *   uint32_t gj_proton_soft_ready_u_10019(void);
 *     - Returns 1 (proton/dxvk soft id stub continuum ready). Pure-data
 *       product tag that exclusive wave 10011-10020 surfaces are present
 *       for the bar3-related path; not a hard Proton runtime probe.
 *   uint32_t __gj_proton_soft_ready_u_10019  (alias)
 *   __libcgj_batch10019_marker = "libcgj-batch10019"
 *
 * Exclusive continuum CREATE-ONLY (10011-10020: proton/dxvk soft id
 * stubs — proton_ge_ok_u_10011, dxvk_ok_u_10012, vkd3d_ok_u_10013,
 * wine_prefix_ok_u_10014, steamrt_ok_u_10015, gamescope_ok_u_10016,
 * mangohud_ok_u_10017, proton_launch_ok_u_10018,
 * proton_soft_ready_u_10019, batch_id_10020). Unique surface only; no
 * multi-def. Distinct from gj_proton_soft_ready_u_8989 (prior wave)
 * and the open ok_u lamps in this wave. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10019_marker[] = "libcgj-batch10019";

/* Soft continuum-ready lamp for proton/dxvk id stubs wave. */
#define B10019_PROTON_SOFT_READY  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10019_soft_ready(void)
{
	return B10019_PROTON_SOFT_READY;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_proton_soft_ready_u_10019 - proton/dxvk soft id stub continuum ready.
 *
 * Always returns 1. Soft pure-data product tag that the exclusive
 * wave surfaces are present. Does not call libc. No parent wires.
 */
uint32_t
gj_proton_soft_ready_u_10019(void)
{
	(void)NULL;
	return b10019_soft_ready();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_proton_soft_ready_u_10019(void)
    __attribute__((alias("gj_proton_soft_ready_u_10019")));
