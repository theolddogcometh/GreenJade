/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10527: zink ok soft unit (not a probe).
 *
 * Surface (unique symbols):
 *   uint32_t gj_zink_ok_u_10527(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       zink runtime probe for the bar3-related continuum.
 *   uint32_t __gj_zink_ok_u_10527  (alias)
 *   __libcgj_batch10527_marker = "libcgj-batch10527"
 *
 * Exclusive continuum CREATE-ONLY (10521-10530: mesa soft id stubs —
 * mesa_ok_u_10521, vulkan_ok_u_10522, radeonsi_ok_u_10523,
 * radv_ok_u_10524, amdgpu_ok_u_10525, aco_ok_u_10526, zink_ok_u_10527,
 * lavapipe_ok_u_10528, mesa_soft_ready_u_10529, batch_id_10530). Unique
 * surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10527_marker[] = "libcgj-batch10527";

/* Soft zink-ok lamp: always off (not a real runtime probe). */
#define B10527_ZINK_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10527_ok(void)
{
	return B10527_ZINK_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_zink_ok_u_10527 - zink ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe zink
 * driver presence or call libc. No parent wires.
 */
uint32_t
gj_zink_ok_u_10527(void)
{
	(void)NULL;
	return b10527_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_zink_ok_u_10527(void)
    __attribute__((alias("gj_zink_ok_u_10527")));
