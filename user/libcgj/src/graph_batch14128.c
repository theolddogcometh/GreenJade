/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14128: lavapipe ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_lavapipe_ok_u_14128(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       lavapipe (software Vulkan) probe for the mesa/vulkan continuum.
 *   uint32_t __gj_lavapipe_ok_u_14128  (alias)
 *   __libcgj_batch14128_marker = "libcgj-batch14128"
 *
 * Exclusive continuum CREATE-ONLY (14121-14130: mesa/vulkan soft path
 * stubs — mesa_ok_u_14121, vulkan_ok_u_14122, radeonsi_ok_u_14123,
 * radv_ok_u_14124, amdgpu_ok_u_14125, aco_ok_u_14126, zink_ok_u_14127,
 * lavapipe_ok_u_14128, mesa_soft_ready_u_14129, batch_id_14130). Unique
 * surface only; no multi-def. Distinct from prior lavapipe soft tags.
 * No parent wires. No __int128. No lavapipe implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14128_marker[] = "libcgj-batch14128";

/* Soft lavapipe-ok lamp: always off (not a real lavapipe probe). */
#define B14128_LAVAPIPE_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14128_lavapipe_ok(void)
{
	return B14128_LAVAPIPE_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_lavapipe_ok_u_14128 - lavapipe ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not inspect lavapipe or
 * call libc. No parent wires.
 */
uint32_t
gj_lavapipe_ok_u_14128(void)
{
	(void)NULL;
	return b14128_lavapipe_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_lavapipe_ok_u_14128(void)
    __attribute__((alias("gj_lavapipe_ok_u_14128")));
