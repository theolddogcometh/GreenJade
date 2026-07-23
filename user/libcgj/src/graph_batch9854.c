/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9854: USB config soft-ok unit (not a probe).
 *
 * Surface (unique symbols):
 *   uint32_t gj_usb_config_ok_u_9854(void);
 *     - Returns 1 (USB configuration soft-id continuum ok). Pure-data
 *       product tag; does not set configurations or parse descriptors.
 *   uint32_t __gj_usb_config_ok_u_9854  (alias)
 *   __libcgj_batch9854_marker = "libcgj-batch9854"
 *
 * Exclusive continuum CREATE-ONLY (9851-9860: usb soft id stubs —
 * usb_dev_ok_u, usb_hub_ok_u, usb_ep_ok_u, usb_config_ok_u,
 * usb_iface_ok_u, usb_ctrl_ok_u, usb_bulk_ok_u, usb_iso_ok_u,
 * usb_ready_u, batch_id_9860). Unique gj_usb_config_ok_u_9854 surface
 * only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9854_marker[] = "libcgj-batch9854";

/* Soft USB configuration-ok lamp for the continuum catalog. */
#define B9854_USB_CONFIG_OK  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9854_usb_config_ok(void)
{
	return B9854_USB_CONFIG_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_usb_config_ok_u_9854 - USB configuration soft-id continuum ok flag.
 *
 * Always returns 1. Soft pure-data product tag; does not issue
 * SET_CONFIGURATION, parse wTotalLength, or call libc. No parent wires.
 */
uint32_t
gj_usb_config_ok_u_9854(void)
{
	(void)NULL;
	return b9854_usb_config_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_usb_config_ok_u_9854(void)
    __attribute__((alias("gj_usb_config_ok_u_9854")));
