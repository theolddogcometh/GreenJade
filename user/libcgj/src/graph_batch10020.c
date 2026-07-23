/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10020: proton/dxvk soft id continuum + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_10020(void);
 *     - Returns the compile-time graph batch number for this TU (10020).
 *   uint32_t __gj_batch_id_10020  (alias)
 *   __libcgj_batch10020_marker = "libcgj-batch10020"
 *
 * Exclusive continuum CREATE-ONLY (10011-10020: proton/dxvk soft id
 * stubs — proton_ge_ok_u_10011, dxvk_ok_u_10012, vkd3d_ok_u_10013,
 * wine_prefix_ok_u_10014, steamrt_ok_u_10015, gamescope_ok_u_10016,
 * mangohud_ok_u_10017, proton_launch_ok_u_10018,
 * proton_soft_ready_u_10019, batch_id_10020). Unique surfaces only; no
 * multi-def. Does NOT redefine gj_batch_id / gj_graph_milestone /
 * prior batch_id_* symbols — avoid multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10020_marker[] = "libcgj-batch10020";

#define B10020_BATCH_ID  10020u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10020_id(void)
{
	return B10020_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_10020 - report this TU's graph batch number.
 *
 * Always returns 10020.
 */
uint32_t
gj_batch_id_10020(void)
{
	(void)NULL;
	return b10020_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_10020(void)
    __attribute__((alias("gj_batch_id_10020")));
