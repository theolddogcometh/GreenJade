/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9852: USB hub soft-ok unit (not a probe).
 *
 * Surface (unique symbols):
 *   uint32_t gj_usb_hub_ok_u_9852(void);
 *     - Returns 1 (USB hub soft-id continuum ok). Pure-data product
 *       tag; does not probe hub descriptors or walk topology trees.
 *   uint32_t __gj_usb_hub_ok_u_9852  (alias)
 *   __libcgj_batch9852_marker = "libcgj-batch9852"
 *
 * Exclusive continuum CREATE-ONLY (9851-9860: usb soft id stubs —
 * usb_dev_ok_u, usb_hub_ok_u, usb_ep_ok_u, usb_config_ok_u,
 * usb_iface_ok_u, usb_ctrl_ok_u, usb_bulk_ok_u, usb_iso_ok_u,
 * usb_ready_u, batch_id_9860). Unique gj_usb_hub_ok_u_9852 surface
 * only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9852_marker[] = "libcgj-batch9852";

/* Soft USB hub-ok lamp for the continuum catalog. */
#define B9852_USB_HUB_OK  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9852_usb_hub_ok(void)
{
	return B9852_USB_HUB_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_usb_hub_ok_u_9852 - USB hub soft-id continuum ok flag.
 *
 * Always returns 1. Soft pure-data product tag; does not open hubs,
 * walk ports, or call libc. No parent wires.
 */
uint32_t
gj_usb_hub_ok_u_9852(void)
{
	(void)NULL;
	return b9852_usb_hub_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_usb_hub_ok_u_9852(void)
    __attribute__((alias("gj_usb_hub_ok_u_9852")));
