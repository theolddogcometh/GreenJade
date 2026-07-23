/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6078: pure-value USB device class Wireless (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_usb_class_wireless_u(void);
 *     - Return the USB bDeviceClass code for Wireless Controller (0xe0).
 *   uint32_t __gj_usb_class_wireless_u  (alias)
 *   __libcgj_batch6078_marker = "libcgj-batch6078"
 *
 * Exclusive continuum CREATE-ONLY (6071-6080: USB device class codes for
 * product — usb_class_audio_u, usb_class_cdc_u, usb_class_hid_u,
 * usb_class_printer_u, usb_class_msc_u, usb_class_hub_u,
 * usb_class_video_u, usb_class_wireless_u, usb_class_vendor_u,
 * batch_id_6080). Unique gj_usb_class_wireless_u surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6078_marker[] = "libcgj-batch6078";

/* USB bDeviceClass: Wireless Controller. */
#define B6078_CLASS_WIRELESS  ((uint32_t)0xe0u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6078_class_wireless(void)
{
	return B6078_CLASS_WIRELESS;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_usb_class_wireless_u - USB Wireless Controller class code 0xe0.
 *
 * Always returns 0xe0. Self-contained; no parent wires.
 */
uint32_t
gj_usb_class_wireless_u(void)
{
	(void)NULL;
	return b6078_class_wireless();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_usb_class_wireless_u(void)
    __attribute__((alias("gj_usb_class_wireless_u")));
