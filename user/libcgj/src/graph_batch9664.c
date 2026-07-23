/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9664: vnc framebuffer ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_vnc_fb_ok_u_9664(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       VNC framebuffer / pixel-format readiness probe.
 *   uint32_t __gj_vnc_fb_ok_u_9664  (alias)
 *   __libcgj_batch9664_marker = "libcgj-batch9664"
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

const char __libcgj_batch9664_marker[] = "libcgj-batch9664";

/* Soft vnc framebuffer-ok lamp: always off (not a real FB probe). */
#define B9664_VNC_FB_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9664_vnc_fb_ok(void)
{
	return B9664_VNC_FB_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_vnc_fb_ok_u_9664 - vnc framebuffer ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not map framebuffers or
 * call libc. No parent wires.
 */
uint32_t
gj_vnc_fb_ok_u_9664(void)
{
	(void)NULL;
	return b9664_vnc_fb_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_vnc_fb_ok_u_9664(void)
    __attribute__((alias("gj_vnc_fb_ok_u_9664")));
