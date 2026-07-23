/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9670: spice/vnc soft continuum + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_9670(void);
 *     - Returns the compile-time graph batch number for this TU (9670).
 *   uint32_t __gj_batch_id_9670  (alias)
 *   __libcgj_batch9670_marker = "libcgj-batch9670"
 *
 * Exclusive continuum CREATE-ONLY (9661-9670: spice/vnc soft id stubs —
 * spice_ok_u_9661, vnc_ok_u_9662, spice_channel_ok_u_9663,
 * vnc_fb_ok_u_9664, spice_audio_ok_u_9665, vnc_input_ok_u_9666,
 * spice_usb_ok_u_9667, remote_display_ok_u_9668,
 * remote_display_ready_u_9669, batch_id_9670). Unique surfaces only; no
 * multi-def. Does NOT redefine gj_batch_id / gj_graph_milestone / prior
 * batch_id_* symbols — avoid multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9670_marker[] = "libcgj-batch9670";

#define B9670_BATCH_ID  9670u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9670_id(void)
{
	return B9670_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_9670 - report this TU's graph batch number.
 *
 * Always returns 9670.
 */
uint32_t
gj_batch_id_9670(void)
{
	(void)NULL;
	return b9670_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_9670(void)
    __attribute__((alias("gj_batch_id_9670")));
