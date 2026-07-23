/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9957: steam install path ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_steam_install_path_ok_u_9957(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       Steam install path probe for the bar3 steam deck product
 *       continuum.
 *   uint32_t __gj_steam_install_path_ok_u_9957  (alias)
 *   __libcgj_batch9957_marker = "libcgj-batch9957"
 *
 * Exclusive continuum CREATE-ONLY (9951-9960: steam deck product soft
 * stubs — steam_deck_hw_ok_u_9951, steam_deck_gpu_ok_u_9952,
 * steam_deck_audio_ok_u_9953, steam_deck_input_ok_u_9954,
 * steam_deck_display_ok_u_9955, steam_top50_slot_ok_u_9956,
 * steam_install_path_ok_u_9957, steam_launch_ok_u_9958,
 * steam_deck_soft_ready_u_9959, batch_id_9960). Unique surface only;
 * no multi-def. Distinct from gj_steam_path_ok_u_8976. bar3 path.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9957_marker[] = "libcgj-batch9957";

/* Soft install-path-ok lamp: always off (not a real path probe). */
#define B9957_INSTALL_PATH_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9957_install_path_ok(void)
{
	return B9957_INSTALL_PATH_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_steam_install_path_ok_u_9957 - steam install path ok soft unit.
 *
 * Always returns 0. Soft continuum stub; does not walk $HOME/.steam,
 * open steam.sh, or call libc. No parent wires.
 */
uint32_t
gj_steam_install_path_ok_u_9957(void)
{
	(void)NULL;
	return b9957_install_path_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_steam_install_path_ok_u_9957(void)
    __attribute__((alias("gj_steam_install_path_ok_u_9957")));
