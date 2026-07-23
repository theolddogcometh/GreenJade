/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14330: mesa/vulkan soft path + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_14330(void);
 *     - Returns the compile-time graph batch number for this TU (14330).
 *   uint32_t __gj_batch_id_14330  (alias)
 *   __libcgj_batch14330_marker = "libcgj-batch14330"
 *
 * Exclusive continuum CREATE-ONLY (14321-14330: mesa/vulkan soft path
 * stubs — mesa_ok_u_14321, vulkan_ok_u_14322, radeonsi_ok_u_14323,
 * radv_ok_u_14324, amdgpu_ok_u_14325, aco_ok_u_14326, zink_ok_u_14327,
 * lavapipe_ok_u_14328, mesa_soft_ready_u_14329, batch_id_14330). Unique
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

const char __libcgj_batch14330_marker[] = "libcgj-batch14330";

#define B14330_BATCH_ID  14330u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14330_id(void)
{
	return B14330_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_14330 - report this TU's graph batch number.
 *
 * Always returns 14330. Soft continuum lamp for the 14321-14330
 * mesa/vulkan soft path stubs wave. No parent wires.
 */
uint32_t
gj_batch_id_14330(void)
{
	(void)NULL;
	return b14330_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_14330(void)
    __attribute__((alias("gj_batch_id_14330")));
