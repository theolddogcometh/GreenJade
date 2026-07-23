/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10226: ACO ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_aco_ok_u_10226(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       ACO (AMD compiler) probe for the mesa/vulkan continuum.
 *   uint32_t __gj_aco_ok_u_10226  (alias)
 *   __libcgj_batch10226_marker = "libcgj-batch10226"
 *
 * Exclusive continuum CREATE-ONLY (10221-10230: mesa/vulkan soft id
 * stubs — mesa_ok_u_10221, vulkan_ok_u_10222, radeonsi_ok_u_10223,
 * radv_ok_u_10224, amdgpu_ok_u_10225, aco_ok_u_10226, zink_ok_u_10227,
 * lavapipe_ok_u_10228, mesa_soft_ready_u_10229, batch_id_10230). Unique
 * surface only; no multi-def. Distinct from prior aco soft tags.
 * No parent wires. No __int128. No ACO implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10226_marker[] = "libcgj-batch10226";

/* Soft ACO-ok lamp: always off (not a real ACO probe). */
#define B10226_ACO_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10226_aco_ok(void)
{
	return B10226_ACO_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_aco_ok_u_10226 - ACO ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not inspect the AMD
 * compiler backend or call libc. No parent wires.
 */
uint32_t
gj_aco_ok_u_10226(void)
{
	(void)NULL;
	return b10226_aco_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_aco_ok_u_10226(void)
    __attribute__((alias("gj_aco_ok_u_10226")));
