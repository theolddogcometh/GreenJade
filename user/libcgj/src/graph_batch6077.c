/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6077: pure-value USB device class Video (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_usb_class_video_u(void);
 *     - Return the USB bDeviceClass code for Video (0x0e).
 *   uint32_t __gj_usb_class_video_u  (alias)
 *   __libcgj_batch6077_marker = "libcgj-batch6077"
 *
 * Exclusive continuum CREATE-ONLY (6071-6080: USB device class codes for
 * product — usb_class_audio_u, usb_class_cdc_u, usb_class_hid_u,
 * usb_class_printer_u, usb_class_msc_u, usb_class_hub_u,
 * usb_class_video_u, usb_class_wireless_u, usb_class_vendor_u,
 * batch_id_6080). Unique gj_usb_class_video_u surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6077_marker[] = "libcgj-batch6077";

/* USB bDeviceClass: Video. */
#define B6077_CLASS_VIDEO  ((uint32_t)0x0eu)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6077_class_video(void)
{
	return B6077_CLASS_VIDEO;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_usb_class_video_u - USB Video device class code constant 0x0e.
 *
 * Always returns 0x0e. Self-contained; no parent wires.
 */
uint32_t
gj_usb_class_video_u(void)
{
	(void)NULL;
	return b6077_class_video();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_usb_class_video_u(void)
    __attribute__((alias("gj_usb_class_video_u")));
