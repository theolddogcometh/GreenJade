/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10240: audio/pipewire soft batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_10240(void);
 *     - Returns the compile-time graph batch number for this TU (10240).
 *   uint32_t __gj_batch_id_10240  (alias)
 *   __libcgj_batch10240_marker = "libcgj-batch10240"
 *
 * Exclusive audio/pipewire soft product CREATE-ONLY (10231-10240:
 * pipewire_ok, wireplumber_ok, pulse_compat_ok, alsa_ok, jack_ok,
 * audio_latency_ok, bluetooth_audio_ok, hdmi_audio_ok,
 * audio_soft_ready, batch_id_10240). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* symbols - avoid multi-def.
 * Soft pure-data catalog only. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10240_marker[] = "libcgj-batch10240";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10240_id(void)
{
	return 10240u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_10240 - report this TU's graph batch number.
 *
 * Always returns 10240. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_10240(void)
{
	(void)NULL;
	return b10240_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_10240(void)
    __attribute__((alias("gj_batch_id_10240")));
