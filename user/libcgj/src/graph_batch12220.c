/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12220: proton soft + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_12220(void);
 *     - Returns the compile-time graph batch number for this TU (12220).
 *   uint32_t __gj_batch_id_12220  (alias)
 *   __libcgj_batch12220_marker = "libcgj-batch12220"
 *
 * Exclusive continuum CREATE-ONLY (12211-12220: proton soft all→0 —
 * proton_ok_u_12211, wine_ok_u_12212, proton_ge_ok_u_12213,
 * steam_runtime_ok_u_12214, pressure_vessel_ok_u_12215,
 * compat_tool_ok_u_12216, dxvk_ok_u_12217, vkd3d_ok_u_12218,
 * proton_soft_ready_u_12219, batch_id_12220). Unique surfaces only; no
 * multi-def. Does NOT redefine gj_batch_id / gj_graph_milestone / prior
 * batch_id_* symbols — avoid multi-def. Distinct from gj_batch_id_10220
 * (batch10220). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12220_marker[] = "libcgj-batch12220";

#define B12220_BATCH_ID  12220u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12220_id(void)
{
	return B12220_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_12220 - report this TU's graph batch number.
 *
 * Always returns 12220.
 */
uint32_t
gj_batch_id_12220(void)
{
	(void)NULL;
	return b12220_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_12220(void)
    __attribute__((alias("gj_batch_id_12220")));
