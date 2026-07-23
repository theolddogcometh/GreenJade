/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10350: gamescope/steamui soft id continuum + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_10350(void);
 *     - Returns the compile-time graph batch number for this TU (10350).
 *   uint32_t __gj_batch_id_10350  (alias)
 *   __libcgj_batch10350_marker = "libcgj-batch10350"
 *
 * Exclusive continuum CREATE-ONLY (10341-10350: gamescope/steamui soft
 * id stubs — gamescope_ok_u_10341, steamui_ok_u_10342,
 * bigpicture_ok_u_10343, deckui_ok_u_10344, cec_ok_u_10345,
 * overlay_ok_u_10346, fps_limit_ok_u_10347, hdr_ok_u_10348,
 * gamescope_soft_ready_u_10349, batch_id_10350). Unique surfaces only;
 * no multi-def. Does NOT redefine gj_batch_id / gj_graph_milestone /
 * prior batch_id_* symbols — avoid multi-def. Distinct from
 * gj_batch_id_10050 (batch10050). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10350_marker[] = "libcgj-batch10350";

#define B10350_BATCH_ID  10350u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10350_id(void)
{
	return B10350_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_10350 - report this TU's graph batch number.
 *
 * Always returns 10350.
 */
uint32_t
gj_batch_id_10350(void)
{
	(void)NULL;
	return b10350_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_10350(void)
    __attribute__((alias("gj_batch_id_10350")));
