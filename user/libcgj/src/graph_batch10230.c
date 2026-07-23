/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10230: mesa/vulkan soft + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_10230(void);
 *     - Returns the compile-time graph batch number for this TU (10230).
 *   uint32_t __gj_batch_id_10230  (alias)
 *   __libcgj_batch10230_marker = "libcgj-batch10230"
 *
 * Exclusive continuum CREATE-ONLY (10221-10230: mesa/vulkan soft id
 * stubs — mesa_ok_u_10221, vulkan_ok_u_10222, radeonsi_ok_u_10223,
 * radv_ok_u_10224, amdgpu_ok_u_10225, aco_ok_u_10226, zink_ok_u_10227,
 * lavapipe_ok_u_10228, mesa_soft_ready_u_10229, batch_id_10230). Unique
 * surfaces only; no multi-def. Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* symbols — avoid multi-def. No
 * parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10230_marker[] = "libcgj-batch10230";

#define B10230_BATCH_ID  10230u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10230_id(void)
{
	return B10230_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_10230 - report this TU's graph batch number.
 *
 * Always returns 10230.
 */
uint32_t
gj_batch_id_10230(void)
{
	(void)NULL;
	return b10230_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_10230(void)
    __attribute__((alias("gj_batch_id_10230")));
