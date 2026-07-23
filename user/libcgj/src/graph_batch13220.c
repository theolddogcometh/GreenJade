/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13220: proton soft continuum + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_13220(void);
 *     - Returns the compile-time graph batch number for this TU (13220).
 *   uint32_t __gj_batch_id_13220  (alias)
 *   __libcgj_batch13220_marker = "libcgj-batch13220"
 *
 * Exclusive continuum CREATE-ONLY (13211-13220: proton soft stubs —
 * ok slots all→0; proton_soft_ready→0; batch_id→13220 —
 * proton_ok_u_13211, wine_ok_u_13212, proton_ge_ok_u_13213,
 * steam_runtime_ok_u_13214, pressure_vessel_ok_u_13215,
 * compat_tool_ok_u_13216, dxvk_ok_u_13217, vkd3d_ok_u_13218,
 * proton_soft_ready_u_13219, batch_id_13220). Unique surfaces only;
 * no multi-def. Does NOT redefine gj_batch_id / gj_graph_milestone /
 * prior batch_id_* symbols — avoid multi-def. Distinct from
 * gj_batch_id_10520 (batch10520) and gj_batch_id_10220 (batch10220).
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13220_marker[] = "libcgj-batch13220";

#define B13220_BATCH_ID  13220u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13220_id(void)
{
	return B13220_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_13220 - report this TU's graph batch number.
 *
 * Always returns 13220. Useful for link-time presence checks and
 * runtime graph revision probes without string compares on the marker.
 * No parent wires.
 */
uint32_t
gj_batch_id_13220(void)
{
	(void)NULL;
	return b13220_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_13220(void)
    __attribute__((alias("gj_batch_id_13220")));
