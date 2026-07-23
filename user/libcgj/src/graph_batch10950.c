/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10950: exclusive gamescope soft batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_10950(void);
 *     - Returns the compile-time graph batch number for this TU (10950).
 *   uint32_t __gj_batch_id_10950  (alias)
 *   __libcgj_batch10950_marker = "libcgj-batch10950"
 *
 * Exclusive continuum CREATE-ONLY (10941-10950: gamescope soft all→0
 * stubs — gamescope_ok_u_10941, steamui_ok_u_10942,
 * bigpicture_ok_u_10943, deckui_ok_u_10944, cec_ok_u_10945,
 * overlay_ok_u_10946, fps_limit_ok_u_10947, hdr_ok_u_10948,
 * gamescope_soft_ready_u_10949, batch_id_10950). Unique
 * gj_batch_id_10950 surface only; no multi-def. Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* symbols. No
 * parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10950_marker[] = "libcgj-batch10950";

#define B10950_BATCH_ID  10950u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10950_id(void)
{
	return B10950_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_10950 - report this TU's graph batch number.
 *
 * Always returns 10950. Useful for link-time presence checks and
 * runtime graph revision probes without string compares on the marker.
 * No parent wires.
 */
uint32_t
gj_batch_id_10950(void)
{
	(void)NULL;
	return b10950_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_10950(void)
    __attribute__((alias("gj_batch_id_10950")));
