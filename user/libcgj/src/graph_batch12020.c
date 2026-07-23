/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12020: proton soft + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_12020(void);
 *     - Returns the compile-time graph batch number for this TU (12020).
 *   uint32_t __gj_batch_id_12020  (alias)
 *   __libcgj_batch12020_marker = "libcgj-batch12020"
 *
 * Exclusive continuum CREATE-ONLY (12011-12020: proton soft all→0 —
 * proton_ok_u_12011, wine_ok_u_12012, proton_ge_ok_u_12013,
 * steam_runtime_ok_u_12014, pressure_vessel_ok_u_12015,
 * compat_tool_ok_u_12016, dxvk_ok_u_12017, vkd3d_ok_u_12018,
 * proton_soft_ready_u_12019, batch_id_12020→12020). Unique surface
 * only; no multi-def. Does NOT redefine gj_batch_id / gj_graph_milestone
 * / prior batch_id_* symbols — avoid multi-def. Distinct from
 * gj_batch_id_11920 (batch11920), gj_batch_id_11720 (batch11720), and
 * gj_batch_id_11520 (batch11520). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12020_marker[] = "libcgj-batch12020";

#define B12020_BATCH_ID  12020u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12020_id(void)
{
	return B12020_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_12020 - report this TU's graph batch number.
 *
 * Always returns 12020. Useful for link-time presence checks and
 * runtime graph revision probes without string compares on the marker.
 * No parent wires.
 */
uint32_t
gj_batch_id_12020(void)
{
	(void)NULL;
	return b12020_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_12020(void)
    __attribute__((alias("gj_batch_id_12020")));
