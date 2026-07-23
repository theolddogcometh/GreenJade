/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9330: gstreamer soft continuum + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_9330(void);
 *     - Returns the compile-time graph batch number for this TU (9330).
 *   uint32_t __gj_batch_id_9330  (alias)
 *   __libcgj_batch9330_marker = "libcgj-batch9330"
 *
 * Exclusive continuum CREATE-ONLY (9321-9330: gstreamer soft id stubs —
 * gst_state_null_id_9321, gst_state_ready_id_9322,
 * gst_state_paused_id_9323, gst_state_playing_id_9324,
 * gst_pipeline_ok_u_9325, gst_element_ok_u_9326, gst_bus_ok_u_9327,
 * gst_caps_ok_u_9328, gstreamer_ready_u_9329, batch_id_9330).
 * Unique surfaces only; no multi-def. Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* symbols — avoid multi-def.
 * No parent wires. No __int128. No GStreamer implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9330_marker[] = "libcgj-batch9330";

#define B9330_BATCH_ID  9330u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9330_id(void)
{
	return B9330_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_9330 - report this TU's graph batch number.
 *
 * Always returns 9330. Link-time presence tags the exclusive
 * GStreamer soft-id continuum wave end. No parent wires.
 */
uint32_t
gj_batch_id_9330(void)
{
	(void)NULL;
	return b9330_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_9330(void)
    __attribute__((alias("gj_batch_id_9330")));
