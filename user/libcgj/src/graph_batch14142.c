/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14142: soft steamui product-ok unit (open).
 *
 * Surface (unique symbols):
 *   uint32_t gj_steamui_ok_u_14142(void);
 *     - Returns 0 (SteamUI soft product ok not complete for this
 *       continuum; still open). Soft pure-data product lamp; does not
 *       probe steamui / Steam client UI runtime.
 *   uint32_t __gj_steamui_ok_u_14142  (alias)
 *   __libcgj_batch14142_marker = "libcgj-batch14142"
 *
 * Exclusive continuum CREATE-ONLY (14141-14150: gamescope/steamui soft
 * product stubs — gamescope_ok_u_14141, steamui_ok_u_14142,
 * bigpicture_ok_u_14143, deckui_ok_u_14144, cec_ok_u_14145,
 * overlay_ok_u_14146, fps_limit_ok_u_14147, hdr_ok_u_14148,
 * gamescope_soft_ready_u_14149, batch_id_14150). Unique surface only;
 * no multi-def. Distinct from gj_steamui_ok_u_10242 (batch10242),
 * gj_steamui_ok_u_10042 (batch10042), and gj_gamescope_ok_u_14141.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14142_marker[] = "libcgj-batch14142";

/* SteamUI soft product-ok lamp for continuum 14141-14150 (still open). */
#define B14142_STEAMUI_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14142_ok(void)
{
	return B14142_STEAMUI_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_steamui_ok_u_14142 - soft steamui product-ok unit (open).
 *
 * Always returns 0 (not complete / still open). Soft pure-data only;
 * does not probe Steam client UI or call libc. No parent wires.
 */
uint32_t
gj_steamui_ok_u_14142(void)
{
	(void)NULL;
	return b14142_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_steamui_ok_u_14142(void)
    __attribute__((alias("gj_steamui_ok_u_14142")));
