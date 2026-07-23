/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14422: steam music ok soft unit (not a probe).
 *
 * Surface (unique symbols):
 *   uint32_t gj_steam_music_ok_u_14422(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       Steam Music probe for the steam media soft continuum.
 *   uint32_t __gj_steam_music_ok_u_14422  (alias)
 *   __libcgj_batch14422_marker = "libcgj-batch14422"
 *
 * Exclusive continuum CREATE-ONLY (14421-14430: steam media soft
 * stubs — ok slots all→0; soft_ready→0; batch_id→14430). Unique gj_steam_music_ok_u_14422
 * surface only; no multi-def. Distinct from gj_steam_runtime_ok_u_* / gj_steam_net_ok_u_* and
 * sibling steam media soft stubs in this wave. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14422_marker[] = "libcgj-batch14422";

/* Steam media soft lamp: always off. */
#define B14422_STEAM_MEDIA  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14422_ok(void)
{
	return B14422_STEAM_MEDIA;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_steam_music_ok_u_14422 - steam music ok soft unit
 *
 * Always returns 0u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_steam_music_ok_u_14422(void)
{
	(void)NULL;
	return b14422_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_steam_music_ok_u_14422(void)
    __attribute__((alias("gj_steam_music_ok_u_14422")));
