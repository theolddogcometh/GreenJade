/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6550: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_6550(void);
 *     - Returns the compile-time graph batch number for this TU (6550).
 *   uint32_t __gj_batch_id_6550  (alias)
 *   __libcgj_batch6550_marker = "libcgj-batch6550"
 *
 * Exclusive continuum CREATE-ONLY (6541-6550: AHCI port status stubs —
 * pxssts_det, pxssts_spd, pxssts_ipm, det_present, spd_gen, pxtfd_bsy,
 * pxtfd_drq, pxcmd_st, port_idle, batch_id_6550).
 * Does NOT redefine gj_batch_id / gj_graph_milestone / prior batch_id_*
 * symbols — avoid multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6550_marker[] = "libcgj-batch6550";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6550_id(void)
{
	return 6550u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_6550 - report this TU's graph batch number.
 *
 * Always returns 6550. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_6550(void)
{
	(void)NULL;
	return b6550_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_6550(void)
    __attribute__((alias("gj_batch_id_6550")));
