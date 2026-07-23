/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10227: Zink ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_zink_ok_u_10227(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       Zink (OpenGL-on-Vulkan) probe for the mesa/vulkan continuum.
 *   uint32_t __gj_zink_ok_u_10227  (alias)
 *   __libcgj_batch10227_marker = "libcgj-batch10227"
 *
 * Exclusive continuum CREATE-ONLY (10221-10230: mesa/vulkan soft id
 * stubs — mesa_ok_u_10221, vulkan_ok_u_10222, radeonsi_ok_u_10223,
 * radv_ok_u_10224, amdgpu_ok_u_10225, aco_ok_u_10226, zink_ok_u_10227,
 * lavapipe_ok_u_10228, mesa_soft_ready_u_10229, batch_id_10230). Unique
 * surface only; no multi-def. Distinct from prior zink soft tags.
 * No parent wires. No __int128. No Zink implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10227_marker[] = "libcgj-batch10227";

/* Soft Zink-ok lamp: always off (not a real Zink probe). */
#define B10227_ZINK_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10227_zink_ok(void)
{
	return B10227_ZINK_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_zink_ok_u_10227 - Zink ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not inspect Zink or call
 * libc. No parent wires.
 */
uint32_t
gj_zink_ok_u_10227(void)
{
	(void)NULL;
	return b10227_zink_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_zink_ok_u_10227(void)
    __attribute__((alias("gj_zink_ok_u_10227")));
