/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9263: soft SDL audio-ok unit flag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_sdl_audio_ok_u_9263(void);
 *     - Returns 1 (SDL audio soft-id continuum ok). Pure-data product
 *       tag; does not open an audio device or call SDL_AudioInit.
 *   uint32_t __gj_sdl_audio_ok_u_9263  (alias)
 *   __libcgj_batch9263_marker = "libcgj-batch9263"
 *
 * Exclusive continuum CREATE-ONLY (9261-9270: sdl soft id stubs —
 * sdl_init_ok_u_9261, sdl_video_ok_u_9262, sdl_audio_ok_u_9263,
 * sdl_events_ok_u_9264, sdl_window_ok_u_9265, sdl_renderer_ok_u_9266,
 * sdl_texture_ok_u_9267, sdl_joystick_ok_u_9268, sdl_ready_u_9269,
 * batch_id_9270). Unique surface only; no multi-def. No parent wires.
 * No __int128. No SDL implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9263_marker[] = "libcgj-batch9263";

#define B9263_SDL_AUDIO_OK  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9263_sdl_audio_ok(void)
{
	return B9263_SDL_AUDIO_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sdl_audio_ok_u_9263 - SDL audio soft-id continuum ok flag.
 *
 * Always returns 1. Soft pure-data product tag; does not open an audio
 * device or call SDL audio APIs. No parent wires.
 */
uint32_t
gj_sdl_audio_ok_u_9263(void)
{
	(void)NULL;
	return b9263_sdl_audio_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_sdl_audio_ok_u_9263(void)
    __attribute__((alias("gj_sdl_audio_ok_u_9263")));
