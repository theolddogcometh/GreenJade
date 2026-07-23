/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9666: vnc input ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_vnc_input_ok_u_9666(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       VNC pointer/keyboard input path readiness probe.
 *   uint32_t __gj_vnc_input_ok_u_9666  (alias)
 *   __libcgj_batch9666_marker = "libcgj-batch9666"
 *
 * Exclusive continuum CREATE-ONLY (9661-9670: spice/vnc soft id stubs —
 * spice_ok_u_9661, vnc_ok_u_9662, spice_channel_ok_u_9663,
 * vnc_fb_ok_u_9664, spice_audio_ok_u_9665, vnc_input_ok_u_9666,
 * spice_usb_ok_u_9667, remote_display_ok_u_9668,
 * remote_display_ready_u_9669, batch_id_9670). Unique surface only; no
 * multi-def. No parent wires. No __int128. No SPICE/VNC implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9666_marker[] = "libcgj-batch9666";

/* Soft vnc input-ok lamp: always off (not a real input probe). */
#define B9666_VNC_INPUT_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9666_vnc_input_ok(void)
{
	return B9666_VNC_INPUT_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_vnc_input_ok_u_9666 - vnc input ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not handle pointer or
 * keyboard events or call libc. No parent wires.
 */
uint32_t
gj_vnc_input_ok_u_9666(void)
{
	(void)NULL;
	return b9666_vnc_input_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_vnc_input_ok_u_9666(void)
    __attribute__((alias("gj_vnc_input_ok_u_9666")));
