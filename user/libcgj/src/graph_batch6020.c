/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6020: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_6020(void);
 *     - Returns the compile-time graph batch number for this TU (6020).
 *   uint32_t __gj_batch_id_6020  (alias)
 *   __libcgj_batch6020_marker = "libcgj-batch6020"
 *
 * Post-6000 steam path deepen exclusive CREATE-ONLY (6011-6020:
 * path_has_userdata_6011, path_has_workshop_6012,
 * path_has_downloading_6013, path_has_appcache_6014,
 * path_has_sourcemods_6015, path_has_steam_music_6016,
 * path_has_steam_temp_6017, path_has_drive_c_6018,
 * steam_path_comp_is_appid_6019, batch_id_6020). Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* symbols —
 * avoid multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6020_marker[] = "libcgj-batch6020";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6020_id(void)
{
	return 6020u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_6020 - report this TU's graph batch number.
 *
 * Always returns 6020. Link-time presence tags the steam path deepen
 * continuum end. Does not call libc. No parent wires.
 */
uint32_t
gj_batch_id_6020(void)
{
	(void)NULL;
	return b6020_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_6020(void)
    __attribute__((alias("gj_batch_id_6020")));
