/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13623: radeonsi ok soft unit (not a probe).
 *
 * Surface (unique symbols):
 *   uint32_t gj_radeonsi_ok_u_13623(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       radeonsi/Gallium OpenGL DRI probe for the mesa soft continuum.
 *   uint32_t __gj_radeonsi_ok_u_13623  (alias)
 *   __libcgj_batch13623_marker = "libcgj-batch13623"
 *
 * Exclusive continuum CREATE-ONLY (13621-13630: mesa soft stubs —
 * all soft lamps →0; batch_id→13630 —
 * mesa_ok_u_13621, vulkan_ok_u_13622, radeonsi_ok_u_13623,
 * radv_ok_u_13624, amdgpu_ok_u_13625, aco_ok_u_13626, zink_ok_u_13627,
 * lavapipe_ok_u_13628, mesa_soft_ready_u_13629, batch_id_13630).
 * Unique surface only; no multi-def. Distinct from
 * gj_radeonsi_ok_u_13423, gj_radeonsi_ok_u_10523, gj_radeonsi_ok_u_10323,
 * gj_radeonsi_ok_u_10223, gj_mesa_ok_u_13621 and gj_vulkan_ok_u_13622.
 * No parent wires. No __int128. No radeonsi implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13623_marker[] = "libcgj-batch13623";

/* Soft radeonsi-ok lamp: always off (not a real runtime probe). */
#define B13623_RADEONSI_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13623_ok(void)
{
	return B13623_RADEONSI_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_radeonsi_ok_u_13623 - radeonsi ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe radeonsi DRI
 * presence or call libc. No parent wires.
 */
uint32_t
gj_radeonsi_ok_u_13623(void)
{
	(void)NULL;
	return b13623_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_radeonsi_ok_u_13623(void)
    __attribute__((alias("gj_radeonsi_ok_u_13623")));
