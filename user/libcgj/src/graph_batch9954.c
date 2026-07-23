/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9954: Steam Deck input ok soft unit.
 *
 * Surface (unique symbols):
 *   uint32_t gj_steam_deck_input_ok_u_9954(void);
 *     - Returns 1 (Steam Deck input soft-id continuum ok). Pure-data
 *       product tag; does not probe controllers/input or call libc.
 *   uint32_t __gj_steam_deck_input_ok_u_9954  (alias)
 *   __libcgj_batch9954_marker = "libcgj-batch9954"
 *
 * Exclusive continuum CREATE-ONLY (9951-9960: steam deck product soft
 * stubs — steam_deck_hw_ok_u_9951, steam_deck_gpu_ok_u_9952,
 * steam_deck_audio_ok_u_9953, steam_deck_input_ok_u_9954,
 * steam_deck_display_ok_u_9955, steam_top50_slot_ok_u_9956,
 * steam_install_path_ok_u_9957, steam_launch_ok_u_9958,
 * steam_deck_soft_ready_u_9959, batch_id_9960). Unique surface only;
 * no multi-def. bar3 path. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9954_marker[] = "libcgj-batch9954";

#define B9954_STEAM_DECK_INPUT_OK  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9954_input_ok(void)
{
	return B9954_STEAM_DECK_INPUT_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_steam_deck_input_ok_u_9954 - Steam Deck input soft-id ok flag.
 *
 * Always returns 1. Soft pure-data product tag for the bar3 steam deck
 * product continuum; does not probe input devices. No parent wires.
 */
uint32_t
gj_steam_deck_input_ok_u_9954(void)
{
	(void)NULL;
	return b9954_input_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_steam_deck_input_ok_u_9954(void)
    __attribute__((alias("gj_steam_deck_input_ok_u_9954")));
