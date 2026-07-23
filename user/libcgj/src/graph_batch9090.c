/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9090: deck top50 soft continuum + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_9090(void);
 *     - Returns the compile-time graph batch number for this TU (9090).
 *   uint32_t __gj_batch_id_9090  (alias)
 *   __libcgj_batch9090_marker = "libcgj-batch9090"
 *
 * Exclusive continuum CREATE-ONLY (9081-9090: deck top50 soft stubs —
 * deck_app_slot_ok_u_9081, deck_top50_count_id_9082,
 * deck_install_ready_u_9083, deck_launch_ready_u_9084,
 * deck_controller_ok_u_9085, deck_gpu_ok_u_9086, deck_audio_ok_u_9087,
 * deck_display_ok_u_9088, deck_soft_ready_u_9089, batch_id_9090).
 * Unique surfaces only; no multi-def. Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* symbols — avoid multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9090_marker[] = "libcgj-batch9090";

#define B9090_BATCH_ID  9090u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9090_id(void)
{
	return B9090_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_9090 - report this TU's graph batch number.
 *
 * Always returns 9090.
 */
uint32_t
gj_batch_id_9090(void)
{
	(void)NULL;
	return b9090_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_9090(void)
    __attribute__((alias("gj_batch_id_9090")));
