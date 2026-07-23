/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6076: pure-value USB device class Hub (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_usb_class_hub_u(void);
 *     - Return the USB bDeviceClass code for Hub (0x09).
 *   uint32_t __gj_usb_class_hub_u  (alias)
 *   __libcgj_batch6076_marker = "libcgj-batch6076"
 *
 * Exclusive continuum CREATE-ONLY (6071-6080: USB device class codes for
 * product — usb_class_audio_u, usb_class_cdc_u, usb_class_hid_u,
 * usb_class_printer_u, usb_class_msc_u, usb_class_hub_u,
 * usb_class_video_u, usb_class_wireless_u, usb_class_vendor_u,
 * batch_id_6080). Unique gj_usb_class_hub_u surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6076_marker[] = "libcgj-batch6076";

/* USB bDeviceClass: Hub. */
#define B6076_CLASS_HUB  ((uint32_t)0x09u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6076_class_hub(void)
{
	return B6076_CLASS_HUB;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_usb_class_hub_u - USB Hub device class code constant 0x09.
 *
 * Always returns 0x09. Self-contained; no parent wires.
 */
uint32_t
gj_usb_class_hub_u(void)
{
	(void)NULL;
	return b6076_class_hub();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_usb_class_hub_u(void)
    __attribute__((alias("gj_usb_class_hub_u")));
