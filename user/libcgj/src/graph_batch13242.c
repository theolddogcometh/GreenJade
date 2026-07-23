/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13242: soft steamui product-ok unit (open).
 *
 * Surface (unique symbols):
 *   uint32_t gj_steamui_ok_u_13242(void);
 *     - Returns 0 (SteamUI soft product ok not complete for this
 *       continuum; still open). Soft pure-data product lamp; does not
 *       probe steamui / Steam client UI runtime.
 *   uint32_t __gj_steamui_ok_u_13242  (alias)
 *   __libcgj_batch13242_marker = "libcgj-batch13242"
 *
 * Exclusive continuum CREATE-ONLY (13241-13250: gamescope/steamui soft
 * product stubs — gamescope_ok_u_13241, steamui_ok_u_13242,
 * bigpicture_ok_u_13243, deckui_ok_u_13244, cec_ok_u_13245,
 * overlay_ok_u_13246, fps_limit_ok_u_13247, hdr_ok_u_13248,
 * gamescope_soft_ready_u_13249, batch_id_13250). Unique surface only;
 * no multi-def. Distinct from gj_gamescope_ok_u_13241 and related
 * Steam Deck / Big Picture path tags. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13242_marker[] = "libcgj-batch13242";

/* SteamUI soft product-ok lamp for continuum 13241-13250 (still open). */
#define B13242_STEAMUI_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13242_ok(void)
{
	return B13242_STEAMUI_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_steamui_ok_u_13242 - soft steamui product-ok unit (open).
 *
 * Always returns 0 (not complete / still open). Soft pure-data only;
 * does not probe Steam client UI or call libc. No parent wires.
 */
uint32_t
gj_steamui_ok_u_13242(void)
{
	(void)NULL;
	return b13242_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_steamui_ok_u_13242(void)
    __attribute__((alias("gj_steamui_ok_u_13242")));
