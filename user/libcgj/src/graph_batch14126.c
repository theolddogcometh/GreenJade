/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14126: ACO ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_aco_ok_u_14126(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       ACO (AMD compiler) probe for the mesa/vulkan continuum.
 *   uint32_t __gj_aco_ok_u_14126  (alias)
 *   __libcgj_batch14126_marker = "libcgj-batch14126"
 *
 * Exclusive continuum CREATE-ONLY (14121-14130: mesa/vulkan soft path
 * stubs — mesa_ok_u_14121, vulkan_ok_u_14122, radeonsi_ok_u_14123,
 * radv_ok_u_14124, amdgpu_ok_u_14125, aco_ok_u_14126, zink_ok_u_14127,
 * lavapipe_ok_u_14128, mesa_soft_ready_u_14129, batch_id_14130). Unique
 * surface only; no multi-def. Distinct from prior aco soft tags.
 * No parent wires. No __int128. No ACO implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14126_marker[] = "libcgj-batch14126";

/* Soft ACO-ok lamp: always off (not a real ACO probe). */
#define B14126_ACO_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14126_aco_ok(void)
{
	return B14126_ACO_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_aco_ok_u_14126 - ACO ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not inspect the AMD
 * compiler backend or call libc. No parent wires.
 */
uint32_t
gj_aco_ok_u_14126(void)
{
	(void)NULL;
	return b14126_aco_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_aco_ok_u_14126(void)
    __attribute__((alias("gj_aco_ok_u_14126")));
