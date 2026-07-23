/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8975: Steam Deck path soft layout tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_steam_deck_tag_8975(void);
 *     - Returns the soft Steam Deck path/layout tag for the bar3-related
 *       steam path soft continuum (always 0x4445434b, fourCC "DECK").
 *       Not a filesystem path probe or device detect.
 *   uint32_t __gj_steam_deck_tag_8975  (alias)
 *   __libcgj_batch8975_marker = "libcgj-batch8975"
 *
 * Exclusive continuum CREATE-ONLY (8971-8980: steam path soft stubs —
 * steam_root_tag_8971, steam_apps_tag_8972, steam_runtime_tag_8973,
 * steam_proton_tag_8974, steam_deck_tag_8975, steam_path_ok_u_8976,
 * steam_install_ready_u_8977, steam_launch_ready_u_8978,
 * steam_soft_ready_u_8979, batch_id_8980). Unique surface only; no
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8975_marker[] = "libcgj-batch8975";

/* Soft Steam Deck path fourCC "DECK". */
#define B8975_STEAM_DECK_TAG  0x4445434bu

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8975_deck_tag(void)
{
	return B8975_STEAM_DECK_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_steam_deck_tag_8975 - soft Steam Deck path layout fourCC tag.
 *
 * Always returns 0x4445434b ("DECK"). Soft pure-data layout tag for the
 * 8971-8980 continuum. Does not detect Deck hardware or call libc.
 * No parent wires.
 */
uint32_t
gj_steam_deck_tag_8975(void)
{
	(void)NULL;
	return b8975_deck_tag();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_steam_deck_tag_8975(void)
    __attribute__((alias("gj_steam_deck_tag_8975")));
