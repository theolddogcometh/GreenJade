/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13421: mesa ok soft unit (not a probe).
 *
 * Surface (unique symbols):
 *   uint32_t gj_mesa_ok_u_13421(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       mesa runtime probe for the mesa soft continuum.
 *   uint32_t __gj_mesa_ok_u_13421  (alias)
 *   __libcgj_batch13421_marker = "libcgj-batch13421"
 *
 * Exclusive continuum CREATE-ONLY (13421-13430: mesa soft stubs —
 * all soft lamps →0; batch_id→13430 —
 * mesa_ok_u_13421, vulkan_ok_u_13422, radeonsi_ok_u_13423,
 * radv_ok_u_13424, amdgpu_ok_u_13425, aco_ok_u_13426, zink_ok_u_13427,
 * lavapipe_ok_u_13428, mesa_soft_ready_u_13429, batch_id_13430).
 * Unique surface only; no multi-def. Distinct from gj_mesa_ok_u_13221,
 * gj_mesa_ok_u_13021, gj_mesa_ok_u_12821, gj_mesa_ok_u_12621,
 * gj_mesa_ok_u_12421, gj_mesa_ok_u_12221, gj_mesa_ok_u_12021,
 * gj_mesa_ok_u_11921, gj_mesa_ok_u_11721, gj_mesa_ok_u_11521,
 * gj_mesa_ok_u_11321, gj_mesa_ok_u_11121, gj_mesa_ok_u_10921,
 * gj_mesa_ok_u_10721, gj_mesa_ok_u_10521, gj_mesa_ok_u_10321,
 * gj_mesa_ok_u_10221 and gj_mesa_ok_u_10021. No parent wires. No
 * __int128. No Mesa implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13421_marker[] = "libcgj-batch13421";

/* Soft mesa-ok lamp: always off (not a real runtime probe). */
#define B13421_MESA_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13421_ok(void)
{
	return B13421_MESA_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mesa_ok_u_13421 - mesa ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe mesa stack
 * presence or call libc. No parent wires.
 */
uint32_t
gj_mesa_ok_u_13421(void)
{
	(void)NULL;
	return b13421_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_mesa_ok_u_13421(void)
    __attribute__((alias("gj_mesa_ok_u_13421")));
