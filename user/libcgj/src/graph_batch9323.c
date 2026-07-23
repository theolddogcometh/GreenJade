/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9323: GStreamer PAUSED state soft id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_gst_state_paused_id_9323(void);
 *     - Returns 3 (GST_STATE_PAUSED soft catalog value). Pure-data state
 *       id stub; does not set element state or call GStreamer/libc.
 *   uint32_t __gj_gst_state_paused_id_9323  (alias)
 *   __libcgj_batch9323_marker = "libcgj-batch9323"
 *
 * Exclusive continuum CREATE-ONLY (9321-9330: gstreamer soft id stubs —
 * gst_state_null_id_9321, gst_state_ready_id_9322,
 * gst_state_paused_id_9323, gst_state_playing_id_9324,
 * gst_pipeline_ok_u_9325, gst_element_ok_u_9326, gst_bus_ok_u_9327,
 * gst_caps_ok_u_9328, gstreamer_ready_u_9329, batch_id_9330).
 * Unique surface only; no multi-def. Distinct from
 * gj_gst_state_null_id_9321 / gj_gst_state_ready_id_9322. No parent
 * wires. No __int128. No GStreamer implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9323_marker[] = "libcgj-batch9323";

/* Soft GStreamer PAUSED state catalog id (GST_STATE_PAUSED = 3). */
#define B9323_GST_STATE_PAUSED  3u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9323_state_paused_id(void)
{
	return B9323_GST_STATE_PAUSED;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_gst_state_paused_id_9323 - soft GStreamer PAUSED state catalog id.
 *
 * Always returns 3. Soft continuum constant; does not set element
 * state or call GStreamer/libc. No parent wires.
 */
uint32_t
gj_gst_state_paused_id_9323(void)
{
	(void)NULL;
	return b9323_state_paused_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_gst_state_paused_id_9323(void)
    __attribute__((alias("gj_gst_state_paused_id_9323")));
