/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14130: mesa/vulkan soft path + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_14130(void);
 *     - Returns the compile-time graph batch number for this TU (14130).
 *   uint32_t __gj_batch_id_14130  (alias)
 *   __libcgj_batch14130_marker = "libcgj-batch14130"
 *
 * Exclusive continuum CREATE-ONLY (14121-14130: mesa/vulkan soft path
 * stubs — mesa_ok_u_14121, vulkan_ok_u_14122, radeonsi_ok_u_14123,
 * radv_ok_u_14124, amdgpu_ok_u_14125, aco_ok_u_14126, zink_ok_u_14127,
 * lavapipe_ok_u_14128, mesa_soft_ready_u_14129, batch_id_14130). Unique
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

const char __libcgj_batch14130_marker[] = "libcgj-batch14130";

#define B14130_BATCH_ID  14130u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14130_id(void)
{
	return B14130_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_14130 - report this TU's graph batch number.
 *
 * Always returns 14130.
 */
uint32_t
gj_batch_id_14130(void)
{
	(void)NULL;
	return b14130_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_14130(void)
    __attribute__((alias("gj_batch_id_14130")));
