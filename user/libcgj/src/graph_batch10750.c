/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10750: exclusive gamescope soft batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_10750(void);
 *     - Returns the compile-time graph batch number for this TU (10750).
 *   uint32_t __gj_batch_id_10750  (alias)
 *   __libcgj_batch10750_marker = "libcgj-batch10750"
 *
 * Exclusive continuum CREATE-ONLY (10741-10750: gamescope soft all→0
 * stubs — gamescope_ok_u_10741, steamui_ok_u_10742,
 * bigpicture_ok_u_10743, deckui_ok_u_10744, cec_ok_u_10745,
 * overlay_ok_u_10746, fps_limit_ok_u_10747, hdr_ok_u_10748,
 * gamescope_soft_ready_u_10749, batch_id_10750). Unique
 * gj_batch_id_10750 surface only; no multi-def. Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* symbols. No
 * parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10750_marker[] = "libcgj-batch10750";

#define B10750_BATCH_ID  10750u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10750_id(void)
{
	return B10750_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_10750 - report this TU's graph batch number.
 *
 * Always returns 10750. Useful for link-time presence checks and
 * runtime graph revision probes without string compares on the marker.
 * No parent wires.
 */
uint32_t
gj_batch_id_10750(void)
{
	(void)NULL;
	return b10750_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_10750(void)
    __attribute__((alias("gj_batch_id_10750")));
