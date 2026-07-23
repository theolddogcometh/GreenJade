/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13830: mesa soft stubs batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_13830(void);
 *     - Returns the compile-time graph batch number for this TU (13830).
 *   uint32_t __gj_batch_id_13830  (alias)
 *   __libcgj_batch13830_marker = "libcgj-batch13830"
 *
 * Exclusive continuum CREATE-ONLY (13821-13830: mesa soft stubs —
 * all soft lamps →0; batch_id→13830 —
 * mesa_ok_u_13821, vulkan_ok_u_13822, radeonsi_ok_u_13823,
 * radv_ok_u_13824, amdgpu_ok_u_13825, aco_ok_u_13826, zink_ok_u_13827,
 * lavapipe_ok_u_13828, mesa_soft_ready_u_13829, batch_id_13830).
 * Unique surface only; no multi-def. Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* symbols — avoid multi-def.
 * Closes the 13821-13830 continuum (mesa/vulkan/radeonsi/radv/amdgpu/
 * aco/zink/lavapipe soft ok stubs + mesa soft ready, all→0). Distinct
 * from gj_batch_id_13630, gj_batch_id_13430, gj_batch_id_13230,
 * gj_batch_id_13030, gj_batch_id_12830, gj_batch_id_12630,
 * gj_batch_id_12430, gj_batch_id_12230, gj_batch_id_12030,
 * gj_batch_id_11930, gj_batch_id_11730, gj_batch_id_11530,
 * gj_batch_id_11330, gj_batch_id_11130, gj_batch_id_10930,
 * gj_batch_id_10730, gj_batch_id_10530, gj_batch_id_10330 and
 * gj_batch_id_10230. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13830_marker[] = "libcgj-batch13830";

#define B13830_BATCH_ID  13830u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13830_id(void)
{
	return B13830_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_13830 - report this TU's graph batch number.
 *
 * Always returns 13830. Soft pure-data identity for the mesa soft
 * stubs continuum close. Does not call libc. No parent wires.
 */
uint32_t
gj_batch_id_13830(void)
{
	(void)NULL;
	return b13830_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_13830(void)
    __attribute__((alias("gj_batch_id_13830")));
