/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10320: proton/wine soft id continuum + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_10320(void);
 *     - Returns the compile-time graph batch number for this TU (10320).
 *   uint32_t __gj_batch_id_10320  (alias)
 *   __libcgj_batch10320_marker = "libcgj-batch10320"
 *
 * Exclusive continuum CREATE-ONLY (10311-10320: proton/wine soft
 * id stubs — proton_ok_u_10311, wine_ok_u_10312,
 * proton_ge_ok_u_10313, steam_runtime_ok_u_10314,
 * pressure_vessel_ok_u_10315, compat_tool_ok_u_10316,
 * dxvk_ok_u_10317, vkd3d_ok_u_10318,
 * proton_soft_ready_u_10319, batch_id_10320). Unique surfaces only;
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

const char __libcgj_batch10320_marker[] = "libcgj-batch10320";

#define B10320_BATCH_ID  10320u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10320_id(void)
{
	return B10320_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_10320 - report this TU's graph batch number.
 *
 * Always returns 10320.
 */
uint32_t
gj_batch_id_10320(void)
{
	(void)NULL;
	return b10320_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_10320(void)
    __attribute__((alias("gj_batch_id_10320")));
