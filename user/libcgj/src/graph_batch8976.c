/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8976: steam path ok soft unit (not a probe).
 *
 * Surface (unique symbols):
 *   uint32_t gj_steam_path_ok_u_8976(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       Steam install path probe for the bar3-related continuum.
 *   uint32_t __gj_steam_path_ok_u_8976  (alias)
 *   __libcgj_batch8976_marker = "libcgj-batch8976"
 *
 * Exclusive continuum CREATE-ONLY (8971-8980: steam path soft stubs —
 * steam_root_tag_8971, steam_apps_tag_8972, steam_runtime_tag_8973,
 * steam_proton_tag_8974, steam_deck_tag_8975, steam_path_ok_u_8976,
 * steam_install_ready_u_8977, steam_launch_ready_u_8978,
 * steam_soft_ready_u_8979, batch_id_8980). Unique surface only; no
 * multi-def. Distinct from gj_steam_root_ok (batch2365). No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8976_marker[] = "libcgj-batch8976";

/* Soft path-ok lamp: always off (not a real path probe). */
#define B8976_PATH_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8976_path_ok(void)
{
	return B8976_PATH_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_steam_path_ok_u_8976 - steam path ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not walk $HOME/.steam,
 * open steam.sh, or call libc. No parent wires.
 */
uint32_t
gj_steam_path_ok_u_8976(void)
{
	(void)NULL;
	return b8976_path_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_steam_path_ok_u_8976(void)
    __attribute__((alias("gj_steam_path_ok_u_8976")));
