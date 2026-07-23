/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6477: broadcast/stream video budget (kbps).
 *
 * Surface (unique symbols):
 *   uint32_t gj_bstream_video_budget_kbps_6477(uint32_t width,
 *                                              uint32_t height,
 *                                              uint32_t fps);
 *     - Soft video bitrate budget from resolution and frame rate:
 *       (width * height * fps) / 100000, clamped to [250, 50000] kbps.
 *       Zero width/height/fps yields the soft minimum 250.
 *   uint32_t __gj_bstream_video_budget_kbps_6477  (alias)
 *   __libcgj_batch6477_marker = "libcgj-batch6477"
 *
 * Exclusive continuum CREATE-ONLY (6471-6480: broadcast/streaming
 * bitrate stubs — bitrate_clamp_kbps_6471, cbr_target_ok_6472,
 * vbr_range_ok_6473, abr_rung_kbps_6474, bytes_to_kbps_6475,
 * audio_bitrate_ok_6476, video_budget_kbps_6477, mux_overhead_kbps_6478,
 * budget_gate_6479, continuum + batch_id_6480). Unique surface only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6477_marker[] = "libcgj-batch6477";

#define B6477_BUDGET_MIN   250u
#define B6477_BUDGET_MAX 50000u
#define B6477_DIV       100000u
#define B6477_U32_MAX  0xffffffffu

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6477_budget(uint32_t u32W, uint32_t u32H, uint32_t u32Fps)
{
	uint32_t u32Pix;
	uint32_t u32Prod;
	uint32_t u32Kbps;

	if (u32W == 0u || u32H == 0u || u32Fps == 0u) {
		return B6477_BUDGET_MIN;
	}
	/* width * height with overflow guard */
	if (u32W > (B6477_U32_MAX / u32H)) {
		return B6477_BUDGET_MAX;
	}
	u32Pix = u32W * u32H;
	if (u32Pix > (B6477_U32_MAX / u32Fps)) {
		return B6477_BUDGET_MAX;
	}
	u32Prod = u32Pix * u32Fps;
	u32Kbps = u32Prod / B6477_DIV;
	if (u32Kbps < B6477_BUDGET_MIN) {
		return B6477_BUDGET_MIN;
	}
	if (u32Kbps > B6477_BUDGET_MAX) {
		return B6477_BUDGET_MAX;
	}
	return u32Kbps;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bstream_video_budget_kbps_6477 - soft video bitrate from res/fps.
 *
 * width:  frame width in pixels
 * height: frame height in pixels
 * fps:    frames per second
 *
 * Returns (w*h*fps)/100000 clamped to [250, 50000]. Pure integer.
 * Does not call libc. No parent wires.
 */
uint32_t
gj_bstream_video_budget_kbps_6477(uint32_t width, uint32_t height,
    uint32_t fps)
{
	(void)NULL;
	return b6477_budget(width, height, fps);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bstream_video_budget_kbps_6477(uint32_t width,
    uint32_t height, uint32_t fps)
    __attribute__((alias("gj_bstream_video_budget_kbps_6477")));
