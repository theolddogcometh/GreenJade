/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9270: sdl soft id continuum + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_9270(void);
 *     - Returns the compile-time graph batch number for this TU (9270).
 *   uint32_t __gj_batch_id_9270  (alias)
 *   __libcgj_batch9270_marker = "libcgj-batch9270"
 *
 * Exclusive continuum CREATE-ONLY (9261-9270: sdl soft id stubs —
 * sdl_init_ok_u_9261, sdl_video_ok_u_9262, sdl_audio_ok_u_9263,
 * sdl_events_ok_u_9264, sdl_window_ok_u_9265, sdl_renderer_ok_u_9266,
 * sdl_texture_ok_u_9267, sdl_joystick_ok_u_9268, sdl_ready_u_9269,
 * batch_id_9270).
 * Unique surfaces only; no multi-def. Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* symbols — avoid multi-def.
 * No parent wires. No __int128. No SDL implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9270_marker[] = "libcgj-batch9270";

#define B9270_BATCH_ID  9270u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9270_id(void)
{
	return B9270_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_9270 - report this TU's graph batch number.
 *
 * Always returns 9270. Link-time presence tags the exclusive
 * SDL soft-id continuum wave end. No parent wires.
 */
uint32_t
gj_batch_id_9270(void)
{
	(void)NULL;
	return b9270_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_9270(void)
    __attribute__((alias("gj_batch_id_9270")));
