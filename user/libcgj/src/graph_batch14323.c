/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14323: radeonsi ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_radeonsi_ok_u_14323(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       radeonsi/Gallium driver probe for the mesa/vulkan continuum.
 *   uint32_t __gj_radeonsi_ok_u_14323  (alias)
 *   __libcgj_batch14323_marker = "libcgj-batch14323"
 *
 * Exclusive continuum CREATE-ONLY (14321-14330: mesa/vulkan soft path
 * stubs — mesa_ok_u_14321, vulkan_ok_u_14322, radeonsi_ok_u_14323,
 * radv_ok_u_14324, amdgpu_ok_u_14325, aco_ok_u_14326, zink_ok_u_14327,
 * lavapipe_ok_u_14328, mesa_soft_ready_u_14329, batch_id_14330). Unique
 * surface only; no multi-def. Distinct from prior radeonsi soft tags
 * (14123/…). No parent wires. No __int128. No radeonsi implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14323_marker[] = "libcgj-batch14323";

/* Soft radeonsi-ok lamp: always off (not a real radeonsi probe). */
#define B14323_RADEONSI_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14323_radeonsi_ok(void)
{
	return B14323_RADEONSI_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_radeonsi_ok_u_14323 - radeonsi ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not inspect Gallium
 * radeonsi or call libc. No parent wires.
 */
uint32_t
gj_radeonsi_ok_u_14323(void)
{
	(void)NULL;
	return b14323_radeonsi_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_radeonsi_ok_u_14323(void)
    __attribute__((alias("gj_radeonsi_ok_u_14323")));
