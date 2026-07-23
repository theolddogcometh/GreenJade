/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9960: steam deck product soft + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_9960(void);
 *     - Returns the compile-time graph batch number for this TU (9960).
 *   uint32_t __gj_batch_id_9960  (alias)
 *   __libcgj_batch9960_marker = "libcgj-batch9960"
 *
 * Exclusive continuum CREATE-ONLY (9951-9960: steam deck product soft
 * stubs — steam_deck_hw_ok_u_9951, steam_deck_gpu_ok_u_9952,
 * steam_deck_audio_ok_u_9953, steam_deck_input_ok_u_9954,
 * steam_deck_display_ok_u_9955, steam_top50_slot_ok_u_9956,
 * steam_install_path_ok_u_9957, steam_launch_ok_u_9958,
 * steam_deck_soft_ready_u_9959, batch_id_9960). Unique surfaces only;
 * no multi-def. Does NOT redefine gj_batch_id / gj_graph_milestone /
 * prior batch_id_* symbols — avoid multi-def. bar3 path. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9960_marker[] = "libcgj-batch9960";

#define B9960_BATCH_ID  9960u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9960_id(void)
{
	return B9960_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_9960 - report this TU's graph batch number.
 *
 * Always returns 9960.
 */
uint32_t
gj_batch_id_9960(void)
{
	(void)NULL;
	return b9960_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_9960(void)
    __attribute__((alias("gj_batch_id_9960")));
