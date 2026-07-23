/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8977: steam install ready soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_steam_install_ready_u_8977(void);
 *     - Always returns 0. Soft stub for bar3-related steam path soft
 *       continuum; install is intentionally not reported ready.
 *   uint32_t __gj_steam_install_ready_u_8977  (alias)
 *   __libcgj_batch8977_marker = "libcgj-batch8977"
 *
 * Exclusive continuum CREATE-ONLY (8971-8980: steam path soft stubs —
 * steam_root_tag_8971, steam_apps_tag_8972, steam_runtime_tag_8973,
 * steam_proton_tag_8974, steam_deck_tag_8975, steam_path_ok_u_8976,
 * steam_install_ready_u_8977, steam_launch_ready_u_8978,
 * steam_soft_ready_u_8979, batch_id_8980). Unique surface only; no
 * multi-def. Distinct from gj_steam_bootstrap_ready_5813. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8977_marker[] = "libcgj-batch8977";

/* Soft install-ready lamp: always off for this continuum. */
#define B8977_INSTALL_READY  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8977_install_ready(void)
{
	return B8977_INSTALL_READY;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_steam_install_ready_u_8977 - steam install ready soft unit flag.
 *
 * Always returns 0. Soft pure-data stub; does not inspect Steam installs
 * or call libc. No parent wires.
 */
uint32_t
gj_steam_install_ready_u_8977(void)
{
	(void)NULL;
	return b8977_install_ready();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_steam_install_ready_u_8977(void)
    __attribute__((alias("gj_steam_install_ready_u_8977")));
