/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10043: bigpicture ok soft unit (not a probe).
 *
 * Surface (unique symbols):
 *   uint32_t gj_bigpicture_ok_u_10043(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       Big Picture mode probe for the bar3-related continuum.
 *   uint32_t __gj_bigpicture_ok_u_10043  (alias)
 *   __libcgj_batch10043_marker = "libcgj-batch10043"
 *
 * Exclusive continuum CREATE-ONLY (10041-10050: gamescope/steamui soft
 * id stubs — gamescope_ok_u_10041, steamui_ok_u_10042,
 * bigpicture_ok_u_10043, deckui_ok_u_10044, cec_ok_u_10045,
 * overlay_ok_u_10046, fps_limit_ok_u_10047, hdr_ok_u_10048,
 * gamescope_soft_ready_u_10049, batch_id_10050). Unique surface only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10043_marker[] = "libcgj-batch10043";

/* Soft bigpicture-ok lamp: always off (not a real mode probe). */
#define B10043_BIGPICTURE_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10043_ok(void)
{
	return B10043_BIGPICTURE_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bigpicture_ok_u_10043 - bigpicture ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe Big Picture
 * mode or call libc. No parent wires.
 */
uint32_t
gj_bigpicture_ok_u_10043(void)
{
	(void)NULL;
	return b10043_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bigpicture_ok_u_10043(void)
    __attribute__((alias("gj_bigpicture_ok_u_10043")));
