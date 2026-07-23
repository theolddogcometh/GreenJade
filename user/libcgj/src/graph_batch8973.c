/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8973: Steam Runtime path soft layout tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_steam_runtime_tag_8973(void);
 *     - Returns the soft Steam Runtime path layout tag for the
 *       bar3-related steam path soft continuum (always 0x5352544d,
 *       fourCC "SRTM"). Not a filesystem path probe.
 *   uint32_t __gj_steam_runtime_tag_8973  (alias)
 *   __libcgj_batch8973_marker = "libcgj-batch8973"
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

const char __libcgj_batch8973_marker[] = "libcgj-batch8973";

/* Soft Steam Runtime path fourCC "SRTM". */
#define B8973_STEAM_RUNTIME_TAG  0x5352544du

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8973_runtime_tag(void)
{
	return B8973_STEAM_RUNTIME_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_steam_runtime_tag_8973 - soft Steam Runtime path layout fourCC tag.
 *
 * Always returns 0x5352544d ("SRTM"). Soft pure-data layout tag for the
 * 8971-8980 continuum. Does not probe steam-runtime paths or call libc.
 * No parent wires.
 */
uint32_t
gj_steam_runtime_tag_8973(void)
{
	(void)NULL;
	return b8973_runtime_tag();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_steam_runtime_tag_8973(void)
    __attribute__((alias("gj_steam_runtime_tag_8973")));
