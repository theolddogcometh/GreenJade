/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10330: mesa/vulkan soft id continuum + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_10330(void);
 *     - Returns the compile-time graph batch number for this TU (10330).
 *   uint32_t __gj_batch_id_10330  (alias)
 *   __libcgj_batch10330_marker = "libcgj-batch10330"
 *
 * Exclusive continuum CREATE-ONLY (10321-10330: mesa/vulkan soft
 * id stubs — mesa_ok_u_10321, vulkan_ok_u_10322,
 * radeonsi_ok_u_10323, radv_ok_u_10324, amdgpu_ok_u_10325,
 * aco_ok_u_10326, zink_ok_u_10327, lavapipe_ok_u_10328,
 * mesa_soft_ready_u_10329, batch_id_10330). Unique surfaces only;
 * no multi-def. Does NOT redefine gj_batch_id / gj_graph_milestone /
 * prior batch_id_* symbols — avoid multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10330_marker[] = "libcgj-batch10330";

#define B10330_BATCH_ID  10330u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10330_id(void)
{
	return B10330_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_10330 - report this TU's graph batch number.
 *
 * Always returns 10330.
 */
uint32_t
gj_batch_id_10330(void)
{
	(void)NULL;
	return b10330_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_10330(void)
    __attribute__((alias("gj_batch_id_10330")));
