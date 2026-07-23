/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9086: Deck GPU soft-ok unit.
 *
 * Surface (unique symbols):
 *   uint32_t gj_deck_gpu_ok_u_9086(void);
 *     - Returns 1 (Deck GPU soft-id continuum ok for the bar3-related
 *       product path). Pure-data product tag; does not open DRM/KMS
 *       or probe AMDGPU/Vulkan ICDs.
 *   uint32_t __gj_deck_gpu_ok_u_9086  (alias)
 *   __libcgj_batch9086_marker = "libcgj-batch9086"
 *
 * Exclusive continuum CREATE-ONLY (9081-9090: deck top50 soft stubs —
 * deck_app_slot_ok_u_9081, deck_top50_count_id_9082,
 * deck_install_ready_u_9083, deck_launch_ready_u_9084,
 * deck_controller_ok_u_9085, deck_gpu_ok_u_9086, deck_audio_ok_u_9087,
 * deck_display_ok_u_9088, deck_soft_ready_u_9089, batch_id_9090).
 * Unique surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9086_marker[] = "libcgj-batch9086";

/* Deck GPU soft-ok lamp for continuum 9081-9090. */
#define B9086_GPU_OK  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9086_gpu_ok(void)
{
	return B9086_GPU_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_deck_gpu_ok_u_9086 - Deck GPU soft-ok unit flag.
 *
 * Always returns 1. Soft pure-data only; does not open /dev/dri or
 * call libc. No parent wires.
 */
uint32_t
gj_deck_gpu_ok_u_9086(void)
{
	(void)NULL;
	return b9086_gpu_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_deck_gpu_ok_u_9086(void)
    __attribute__((alias("gj_deck_gpu_ok_u_9086")));
