/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9414: JACK sample rate ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_jack_sample_rate_ok_u_9414(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       JACK sample-rate / jack_get_sample_rate validity probe.
 *   uint32_t __gj_jack_sample_rate_ok_u_9414  (alias)
 *   __libcgj_batch9414_marker = "libcgj-batch9414"
 *
 * Exclusive continuum CREATE-ONLY (9411-9420: jack audio soft id
 * stubs — jack_client_ok_u_9411, jack_port_ok_u_9412,
 * jack_buffer_ok_u_9413, jack_sample_rate_ok_u_9414,
 * jack_frames_ok_u_9415, jack_transport_ok_u_9416,
 * jack_midi_ok_u_9417, jack_latency_ok_u_9418, jack_ready_u_9419,
 * batch_id_9420). Unique surface only; no multi-def. No parent wires.
 * No __int128. No JACK/libjack implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9414_marker[] = "libcgj-batch9414";

/* Soft JACK sample-rate-ok lamp: always off (not a real SR probe). */
#define B9414_JACK_SAMPLE_RATE_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9414_jack_sample_rate_ok(void)
{
	return B9414_JACK_SAMPLE_RATE_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_jack_sample_rate_ok_u_9414 - JACK sample rate ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not query JACK sample
 * rates or call libjack/libc. No parent wires.
 */
uint32_t
gj_jack_sample_rate_ok_u_9414(void)
{
	(void)NULL;
	return b9414_jack_sample_rate_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_jack_sample_rate_ok_u_9414(void)
    __attribute__((alias("gj_jack_sample_rate_ok_u_9414")));
