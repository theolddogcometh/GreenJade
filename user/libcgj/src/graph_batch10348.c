/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10348: hdr ok soft unit (not a probe).
 *
 * Surface (unique symbols):
 *   uint32_t gj_hdr_ok_u_10348(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       HDR display/runtime probe for the bar3-related continuum.
 *   uint32_t __gj_hdr_ok_u_10348  (alias)
 *   __libcgj_batch10348_marker = "libcgj-batch10348"
 *
 * Exclusive continuum CREATE-ONLY (10341-10350: gamescope/steamui soft
 * id stubs — gamescope_ok_u_10341, steamui_ok_u_10342,
 * bigpicture_ok_u_10343, deckui_ok_u_10344, cec_ok_u_10345,
 * overlay_ok_u_10346, fps_limit_ok_u_10347, hdr_ok_u_10348,
 * gamescope_soft_ready_u_10349, batch_id_10350). Unique surface only;
 * no multi-def. Distinct from gj_hdr_ok_u_10048 (batch10048).
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10348_marker[] = "libcgj-batch10348";

/* Soft hdr-ok lamp: always off (not a real HDR probe). */
#define B10348_HDR_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10348_ok(void)
{
	return B10348_HDR_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hdr_ok_u_10348 - hdr ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe HDR display
 * capability or call libc. No parent wires.
 */
uint32_t
gj_hdr_ok_u_10348(void)
{
	(void)NULL;
	return b10348_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_hdr_ok_u_10348(void)
    __attribute__((alias("gj_hdr_ok_u_10348")));
