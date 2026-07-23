/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9667: spice usb ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_spice_usb_ok_u_9667(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       SPICE USB redirection channel readiness probe.
 *   uint32_t __gj_spice_usb_ok_u_9667  (alias)
 *   __libcgj_batch9667_marker = "libcgj-batch9667"
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

const char __libcgj_batch9667_marker[] = "libcgj-batch9667";

/* Soft spice usb-ok lamp: always off (not a real USB-redir probe). */
#define B9667_SPICE_USB_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9667_spice_usb_ok(void)
{
	return B9667_SPICE_USB_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_spice_usb_ok_u_9667 - spice usb ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not open USB redirection
 * channels or call libc. No parent wires.
 */
uint32_t
gj_spice_usb_ok_u_9667(void)
{
	(void)NULL;
	return b9667_spice_usb_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_spice_usb_ok_u_9667(void)
    __attribute__((alias("gj_spice_usb_ok_u_9667")));
