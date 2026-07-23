/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12420: proton soft + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_12420(void);
 *     - Returns the compile-time graph batch number for this TU (12420).
 *   uint32_t __gj_batch_id_12420  (alias)
 *   __libcgj_batch12420_marker = "libcgj-batch12420"
 *
 * Exclusive continuum CREATE-ONLY (12411-12420: proton soft all→0 —
 * proton_ok_u_12411, wine_ok_u_12412, proton_ge_ok_u_12413,
 * steam_runtime_ok_u_12414, pressure_vessel_ok_u_12415,
 * compat_tool_ok_u_12416, dxvk_ok_u_12417, vkd3d_ok_u_12418,
 * proton_soft_ready_u_12419, batch_id_12420→12420). Unique surfaces
 * only; no multi-def. Does NOT redefine gj_batch_id / gj_graph_milestone
 * / prior batch_id_* symbols — avoid multi-def. Distinct from
 * gj_batch_id_12220 (batch12220) and gj_batch_id_10220 (batch10220).
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12420_marker[] = "libcgj-batch12420";

#define B12420_BATCH_ID  12420u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12420_id(void)
{
	return B12420_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_12420 - report this TU's graph batch number.
 *
 * Always returns 12420.
 */
uint32_t
gj_batch_id_12420(void)
{
	(void)NULL;
	return b12420_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_12420(void)
    __attribute__((alias("gj_batch_id_12420")));
