/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9857: USB bulk-transfer soft-ok unit.
 *
 * Surface (unique symbols):
 *   uint32_t gj_usb_bulk_ok_u_9857(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       bulk-transfer probe for the usb continuum.
 *   uint32_t __gj_usb_bulk_ok_u_9857  (alias)
 *   __libcgj_batch9857_marker = "libcgj-batch9857"
 *
 * Exclusive continuum CREATE-ONLY (9851-9860: usb soft id stubs —
 * usb_dev_ok_u, usb_hub_ok_u, usb_ep_ok_u, usb_config_ok_u,
 * usb_iface_ok_u, usb_ctrl_ok_u, usb_bulk_ok_u, usb_iso_ok_u,
 * usb_ready_u, batch_id_9860). Unique gj_usb_bulk_ok_u_9857 surface
 * only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9857_marker[] = "libcgj-batch9857";

/* Soft bulk-transfer ok lamp: always off (not a real URB probe). */
#define B9857_USB_BULK_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9857_usb_bulk_ok(void)
{
	return B9857_USB_BULK_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_usb_bulk_ok_u_9857 - USB bulk-transfer soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not submit bulk URBs,
 * stream data, or call libc. No parent wires.
 */
uint32_t
gj_usb_bulk_ok_u_9857(void)
{
	(void)NULL;
	return b9857_usb_bulk_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_usb_bulk_ok_u_9857(void)
    __attribute__((alias("gj_usb_bulk_ok_u_9857")));
