/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14429: steam media soft ready lamp (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_steam_media_soft_ready_u_14429(void);
 *     - Always returns 0. Soft continuum ready lamp intentionally off;
 *       pure-data stub for the exclusive wave 14421-14430 surfaces.
 *   uint32_t __gj_steam_media_soft_ready_u_14429  (alias)
 *   __libcgj_batch14429_marker = "libcgj-batch14429"
 *
 * Exclusive continuum CREATE-ONLY (14421-14430: steam media soft
 * stubs — ok slots all→0; soft_ready→0; batch_id→14430). Unique gj_steam_media_soft_ready_u_14429
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

const char __libcgj_batch14429_marker[] = "libcgj-batch14429";

/* Steam media soft lamp: always off. */
#define B14429_STEAM_MEDIA  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14429_ok(void)
{
	return B14429_STEAM_MEDIA;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_steam_media_soft_ready_u_14429 - steam media soft ready lamp
 *
 * Always returns 0u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_steam_media_soft_ready_u_14429(void)
{
	(void)NULL;
	return b14429_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_steam_media_soft_ready_u_14429(void)
    __attribute__((alias("gj_steam_media_soft_ready_u_14429")));
