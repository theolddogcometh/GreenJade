/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10220: proton/wine soft + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_10220(void);
 *     - Returns the compile-time graph batch number for this TU (10220).
 *   uint32_t __gj_batch_id_10220  (alias)
 *   __libcgj_batch10220_marker = "libcgj-batch10220"
 *
 * Exclusive continuum CREATE-ONLY (10211-10220: proton/wine soft product
 * stubs — proton_ok_u_10211, wine_ok_u_10212, proton_ge_ok_u_10213,
 * steam_runtime_ok_u_10214, pressure_vessel_ok_u_10215,
 * compat_tool_ok_u_10216, dxvk_ok_u_10217, vkd3d_ok_u_10218,
 * proton_soft_ready_u_10219, batch_id_10220). Unique surfaces only; no
 * multi-def. Does NOT redefine gj_batch_id / gj_graph_milestone / prior
 * batch_id_* symbols — avoid multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10220_marker[] = "libcgj-batch10220";

#define B10220_BATCH_ID  10220u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10220_id(void)
{
	return B10220_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_10220 - report this TU's graph batch number.
 *
 * Always returns 10220.
 */
uint32_t
gj_batch_id_10220(void)
{
	(void)NULL;
	return b10220_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_10220(void)
    __attribute__((alias("gj_batch_id_10220")));
