/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6071: pure-value USB device class Audio (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_usb_class_audio_u(void);
 *     - Return the USB bDeviceClass code for Audio (0x01).
 *   uint32_t __gj_usb_class_audio_u  (alias)
 *   __libcgj_batch6071_marker = "libcgj-batch6071"
 *
 * Exclusive continuum CREATE-ONLY (6071-6080: USB device class codes for
 * product — usb_class_audio_u, usb_class_cdc_u, usb_class_hid_u,
 * usb_class_printer_u, usb_class_msc_u, usb_class_hub_u,
 * usb_class_video_u, usb_class_wireless_u, usb_class_vendor_u,
 * batch_id_6080). Unique gj_usb_class_audio_u surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6071_marker[] = "libcgj-batch6071";

/* USB bDeviceClass: Audio. */
#define B6071_CLASS_AUDIO  ((uint32_t)0x01u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6071_class_audio(void)
{
	return B6071_CLASS_AUDIO;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_usb_class_audio_u - USB Audio device class code constant 0x01.
 *
 * Always returns 0x01. Self-contained; no parent wires.
 */
uint32_t
gj_usb_class_audio_u(void)
{
	(void)NULL;
	return b6071_class_audio();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_usb_class_audio_u(void)
    __attribute__((alias("gj_usb_class_audio_u")));
