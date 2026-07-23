/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9420: JACK audio soft continuum + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_9420(void);
 *     - Returns the compile-time graph batch number for this TU (9420).
 *   uint32_t __gj_batch_id_9420  (alias)
 *   __libcgj_batch9420_marker = "libcgj-batch9420"
 *
 * Exclusive continuum CREATE-ONLY (9411-9420: jack audio soft id
 * stubs — jack_client_ok_u_9411, jack_port_ok_u_9412,
 * jack_buffer_ok_u_9413, jack_sample_rate_ok_u_9414,
 * jack_frames_ok_u_9415, jack_transport_ok_u_9416,
 * jack_midi_ok_u_9417, jack_latency_ok_u_9418, jack_ready_u_9419,
 * batch_id_9420). Unique surfaces only; no multi-def. Does NOT
 * redefine gj_batch_id / gj_graph_milestone / prior batch_id_*
 * symbols — avoid multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9420_marker[] = "libcgj-batch9420";

#define B9420_BATCH_ID  9420u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9420_id(void)
{
	return B9420_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_9420 - report this TU's graph batch number.
 *
 * Always returns 9420.
 */
uint32_t
gj_batch_id_9420(void)
{
	(void)NULL;
	return b9420_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_9420(void)
    __attribute__((alias("gj_batch_id_9420")));
