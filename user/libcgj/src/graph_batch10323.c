/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10323: radeonsi ok soft unit (not a probe).
 *
 * Surface (unique symbols):
 *   uint32_t gj_radeonsi_ok_u_10323(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       radeonsi runtime probe for the bar3-related continuum.
 *   uint32_t __gj_radeonsi_ok_u_10323  (alias)
 *   __libcgj_batch10323_marker = "libcgj-batch10323"
 *
 * Exclusive continuum CREATE-ONLY (10321-10330: mesa/vulkan soft
 * id stubs — mesa_ok_u_10321, vulkan_ok_u_10322,
 * radeonsi_ok_u_10323, radv_ok_u_10324, amdgpu_ok_u_10325,
 * aco_ok_u_10326, zink_ok_u_10327, lavapipe_ok_u_10328,
 * mesa_soft_ready_u_10329, batch_id_10330). Unique surface only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10323_marker[] = "libcgj-batch10323";

/* Soft radeonsi-ok lamp: always off (not a real runtime probe). */
#define B10323_RADEONSI_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10323_ok(void)
{
	return B10323_RADEONSI_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_radeonsi_ok_u_10323 - radeonsi ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe radeonsi
 * gallium driver presence or call libc. No parent wires.
 */
uint32_t
gj_radeonsi_ok_u_10323(void)
{
	(void)NULL;
	return b10323_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_radeonsi_ok_u_10323(void)
    __attribute__((alias("gj_radeonsi_ok_u_10323")));
