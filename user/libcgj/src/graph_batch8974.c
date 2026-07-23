/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8974: Proton path soft layout tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_steam_proton_tag_8974(void);
 *     - Returns the soft Proton / compatibility-tool path layout tag
 *       for the bar3-related steam path soft continuum (always
 *       0x50524f54, fourCC "PROT"). Not a filesystem path probe.
 *   uint32_t __gj_steam_proton_tag_8974  (alias)
 *   __libcgj_batch8974_marker = "libcgj-batch8974"
 *
 * Exclusive continuum CREATE-ONLY (8971-8980: steam path soft stubs —
 * steam_root_tag_8971, steam_apps_tag_8972, steam_runtime_tag_8973,
 * steam_proton_tag_8974, steam_deck_tag_8975, steam_path_ok_u_8976,
 * steam_install_ready_u_8977, steam_launch_ready_u_8978,
 * steam_soft_ready_u_8979, batch_id_8980). Unique surface only; no
 * multi-def. Distinct from gj_steam_path_is_compatdata (batch1865).
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8974_marker[] = "libcgj-batch8974";

/* Soft Proton path fourCC "PROT". */
#define B8974_STEAM_PROTON_TAG  0x50524f54u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8974_proton_tag(void)
{
	return B8974_STEAM_PROTON_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_steam_proton_tag_8974 - soft Proton path layout fourCC tag.
 *
 * Always returns 0x50524f54 ("PROT"). Soft pure-data layout tag for the
 * 8971-8980 continuum. Does not probe Proton/compatibilitytool.d paths
 * or call libc. No parent wires.
 */
uint32_t
gj_steam_proton_tag_8974(void)
{
	(void)NULL;
	return b8974_proton_tag();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_steam_proton_tag_8974(void)
    __attribute__((alias("gj_steam_proton_tag_8974")));
