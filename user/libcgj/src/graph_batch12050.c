/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12050: gamescope/steamui soft id continuum + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_12050(void);
 *     - Returns the compile-time graph batch number for this TU (12050).
 *   uint32_t __gj_batch_id_12050  (alias)
 *   __libcgj_batch12050_marker = "libcgj-batch12050"
 *
 * Exclusive continuum CREATE-ONLY (12041-12050: gamescope/steamui soft
 * id stubs — gamescope_ok_u_12041, steamui_ok_u_12042,
 * bigpicture_ok_u_12043, deckui_ok_u_12044, cec_ok_u_12045,
 * overlay_ok_u_12046, fps_limit_ok_u_12047, hdr_ok_u_12048,
 * gamescope_soft_ready_u_12049, batch_id_12050). Unique surfaces only;
 * no multi-def. Does NOT redefine gj_batch_id / gj_graph_milestone /
 * prior batch_id_* symbols — avoid multi-def. Distinct from
 * gj_batch_id_11950 (batch11950), gj_batch_id_11750, and prior *050
 * batch-id lamps. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12050_marker[] = "libcgj-batch12050";

#define B12050_BATCH_ID  12050u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12050_id(void)
{
	return B12050_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_12050 - report this TU's graph batch number.
 *
 * Always returns 12050.
 */
uint32_t
gj_batch_id_12050(void)
{
	(void)NULL;
	return b12050_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_12050(void)
    __attribute__((alias("gj_batch_id_12050")));
