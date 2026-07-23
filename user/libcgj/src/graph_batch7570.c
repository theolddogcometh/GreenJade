/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7570: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_7570(void);
 *     - Returns the compile-time graph batch number for this TU (7570).
 *   uint32_t __gj_batch_id_7570  (alias)
 *   __libcgj_batch7570_marker = "libcgj-batch7570"
 *
 * Exclusive continuum CREATE-ONLY (7561-7570: timer_settime flags stubs —
 * abstime_id, relative_id, known_mask_id, has_abstime, is_relative,
 * flags_ok, flags_pack, flags_mask, flags_errorish, batch_id_7570).
 * Does NOT redefine gj_batch_id / gj_graph_milestone / prior
 * batch_id_* symbols — avoid multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7570_marker[] = "libcgj-batch7570";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7570_id(void)
{
	return 7570u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_7570 - report this TU's graph batch number.
 *
 * Always returns 7570. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_7570(void)
{
	(void)NULL;
	return b7570_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_7570(void)
    __attribute__((alias("gj_batch_id_7570")));
