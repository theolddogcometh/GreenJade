/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10520: proton soft continuum + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_10520(void);
 *     - Returns the compile-time graph batch number for this TU (10520).
 *   uint32_t __gj_batch_id_10520  (alias)
 *   __libcgj_batch10520_marker = "libcgj-batch10520"
 *
 * Exclusive continuum CREATE-ONLY (10511-10520: proton soft stubs —
 * proton_ok_u_10511, wine_ok_u_10512, proton_ge_ok_u_10513,
 * steam_runtime_ok_u_10514, pressure_vessel_ok_u_10515,
 * compat_tool_ok_u_10516, dxvk_ok_u_10517, vkd3d_ok_u_10518,
 * proton_soft_ready_u_10519, batch_id_10520). Unique surfaces only;
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

const char __libcgj_batch10520_marker[] = "libcgj-batch10520";

#define B10520_BATCH_ID  10520u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10520_id(void)
{
	return B10520_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_10520 - report this TU's graph batch number.
 *
 * Always returns 10520.
 */
uint32_t
gj_batch_id_10520(void)
{
	(void)NULL;
	return b10520_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_10520(void)
    __attribute__((alias("gj_batch_id_10520")));
