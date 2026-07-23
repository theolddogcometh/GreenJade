/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10244: soft Deck UI product-ok unit (open).
 *
 * Surface (unique symbols):
 *   uint32_t gj_deckui_ok_u_10244(void);
 *     - Returns 0 (Steam Deck UI soft product ok not complete for this
 *       continuum; still open). Soft pure-data product lamp; does not
 *       probe Steam Deck UI / Jupiter shell runtime.
 *   uint32_t __gj_deckui_ok_u_10244  (alias)
 *   __libcgj_batch10244_marker = "libcgj-batch10244"
 *
 * Exclusive continuum CREATE-ONLY (10241-10250: gamescope/steamui soft
 * product stubs — gamescope_ok_u_10241, steamui_ok_u_10242,
 * bigpicture_ok_u_10243, deckui_ok_u_10244, cec_ok_u_10245,
 * overlay_ok_u_10246, fps_limit_ok_u_10247, hdr_ok_u_10248,
 * gamescope_soft_ready_u_10249, batch_id_10250). Unique surface only;
 * no multi-def. Distinct from gj_bigpicture_ok_u_10243 and
 * gj_gamescope_ok_u_10241. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10244_marker[] = "libcgj-batch10244";

/* Deck UI soft product-ok lamp for continuum 10241-10250 (still open). */
#define B10244_DECKUI_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10244_ok(void)
{
	return B10244_DECKUI_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_deckui_ok_u_10244 - soft Deck UI product-ok unit (open).
 *
 * Always returns 0 (not complete / still open). Soft pure-data only;
 * does not probe Steam Deck UI or call libc. No parent wires.
 */
uint32_t
gj_deckui_ok_u_10244(void)
{
	(void)NULL;
	return b10244_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_deckui_ok_u_10244(void)
    __attribute__((alias("gj_deckui_ok_u_10244")));
