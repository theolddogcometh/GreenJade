/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6257: input sample window milliseconds stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_input_sample_window_ms_6257(uint32_t samples, uint32_t poll_hz);
 *     - Soft sample-window length in ms: (samples * 1000) / poll_hz
 *       when poll_hz != 0, else 0. Integer only; does not allocate
 *       ring buffers.
 *   uint32_t __gj_input_sample_window_ms_6257  (alias)
 *   __libcgj_batch6257_marker = "libcgj-batch6257"
 *
 * Exclusive continuum CREATE-ONLY (6251-6260: input lag / poll rate
 * stubs). Unique gj_input_sample_window_ms_6257 surface only; no
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6257_marker[] = "libcgj-batch6257";

#define B6257_MS_PER_S  1000u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6257_window_ms(uint32_t u32Samples, uint32_t u32Hz)
{
	if (u32Hz == 0u)
		return 0u;
	return (u32Samples * B6257_MS_PER_S) / u32Hz;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_input_sample_window_ms_6257 - sample window from count and Hz.
 *
 * samples:  soft number of poll samples in the window
 * poll_hz:  soft poll/report rate in Hertz
 *
 * Returns (samples * 1000) / poll_hz, or 0 if poll_hz is 0. Soft
 * pure-data window length. No parent wires.
 */
uint32_t
gj_input_sample_window_ms_6257(uint32_t samples, uint32_t poll_hz)
{
	(void)NULL;
	return b6257_window_ms(samples, poll_hz);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_input_sample_window_ms_6257(uint32_t samples, uint32_t poll_hz)
    __attribute__((alias("gj_input_sample_window_ms_6257")));
