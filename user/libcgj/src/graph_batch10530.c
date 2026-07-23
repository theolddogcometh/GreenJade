/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10530: mesa soft id continuum + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_10530(void);
 *     - Returns the compile-time graph batch number for this TU (10530).
 *   uint32_t __gj_batch_id_10530  (alias)
 *   __libcgj_batch10530_marker = "libcgj-batch10530"
 *
 * Exclusive continuum CREATE-ONLY (10521-10530: mesa soft id stubs —
 * mesa_ok_u_10521, vulkan_ok_u_10522, radeonsi_ok_u_10523,
 * radv_ok_u_10524, amdgpu_ok_u_10525, aco_ok_u_10526, zink_ok_u_10527,
 * lavapipe_ok_u_10528, mesa_soft_ready_u_10529, batch_id_10530). Unique
 * surfaces only; no multi-def. Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* symbols — avoid multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10530_marker[] = "libcgj-batch10530";

#define B10530_BATCH_ID  10530u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10530_id(void)
{
	return B10530_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_10530 - report this TU's graph batch number.
 *
 * Always returns 10530.
 */
uint32_t
gj_batch_id_10530(void)
{
	(void)NULL;
	return b10530_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_10530(void)
    __attribute__((alias("gj_batch_id_10530")));
