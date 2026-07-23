/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14327: Zink ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_zink_ok_u_14327(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       Zink (OpenGL-on-Vulkan) probe for the mesa/vulkan continuum.
 *   uint32_t __gj_zink_ok_u_14327  (alias)
 *   __libcgj_batch14327_marker = "libcgj-batch14327"
 *
 * Exclusive continuum CREATE-ONLY (14321-14330: mesa/vulkan soft path
 * stubs — mesa_ok_u_14321, vulkan_ok_u_14322, radeonsi_ok_u_14323,
 * radv_ok_u_14324, amdgpu_ok_u_14325, aco_ok_u_14326, zink_ok_u_14327,
 * lavapipe_ok_u_14328, mesa_soft_ready_u_14329, batch_id_14330). Unique
 * surface only; no multi-def. Distinct from prior zink soft tags
 * (14127/…). No parent wires. No __int128. No Zink implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14327_marker[] = "libcgj-batch14327";

/* Soft Zink-ok lamp: always off (not a real Zink probe). */
#define B14327_ZINK_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14327_zink_ok(void)
{
	return B14327_ZINK_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_zink_ok_u_14327 - Zink ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not inspect Zink or call
 * libc. No parent wires.
 */
uint32_t
gj_zink_ok_u_14327(void)
{
	(void)NULL;
	return b14327_zink_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_zink_ok_u_14327(void)
    __attribute__((alias("gj_zink_ok_u_14327")));
