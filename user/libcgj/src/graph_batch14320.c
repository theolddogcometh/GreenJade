/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14320: proton soft continuum + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_14320(void);
 *     - Returns the compile-time graph batch number for this TU (14320).
 *   uint32_t __gj_batch_id_14320  (alias)
 *   __libcgj_batch14320_marker = "libcgj-batch14320"
 *
 * Exclusive continuum CREATE-ONLY (14311-14320: proton soft stubs —
 * proton_ok_u_14311, wine_ok_u_14312, proton_ge_ok_u_14313,
 * steam_runtime_ok_u_14314, pressure_vessel_ok_u_14315,
 * compat_tool_ok_u_14316, dxvk_ok_u_14317, vkd3d_ok_u_14318,
 * proton_soft_ready_u_14319, batch_id_14320). Unique surfaces only;
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

const char __libcgj_batch14320_marker[] = "libcgj-batch14320";

#define B14320_BATCH_ID  14320u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14320_id(void)
{
	return B14320_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_14320 - report this TU's graph batch number.
 *
 * Always returns 14320.
 */
uint32_t
gj_batch_id_14320(void)
{
	(void)NULL;
	return b14320_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_14320(void)
    __attribute__((alias("gj_batch_id_14320")));
