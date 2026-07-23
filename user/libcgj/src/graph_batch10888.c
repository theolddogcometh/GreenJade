/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10888: USB print ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_usb_print_ok_u_10888(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       USB printer / usblp probe for the cups/print continuum.
 *   uint32_t __gj_usb_print_ok_u_10888  (alias)
 *   __libcgj_batch10888_marker = "libcgj-batch10888"
 *
 * Exclusive continuum CREATE-ONLY (10881-10890: cups/print soft id
 * stubs — cups_ok_u_10881, ipp_ok_u_10882, print_job_ok_u_10883,
 * printer_discover_ok_u_10884, ppd_ok_u_10885, filter_ok_u_10886,
 * spool_ok_u_10887, usb_print_ok_u_10888, print_soft_ready_u_10889,
 * batch_id_10890). Unique surface only; no multi-def. Distinct from
 * gj_usb_print_ok_u_10188 (batch10188). No parent wires. No __int128.
 * No USB/CUPS implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10888_marker[] = "libcgj-batch10888";

/* Soft USB-print-ok lamp: always off (not a real USB printer probe). */
#define B10888_USB_PRINT_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10888_usb_print_ok(void)
{
	return B10888_USB_PRINT_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_usb_print_ok_u_10888 - USB print ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe usblp/sysfs or
 * call libc. No parent wires.
 */
uint32_t
gj_usb_print_ok_u_10888(void)
{
	(void)NULL;
	return b10888_usb_print_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_usb_print_ok_u_10888(void)
    __attribute__((alias("gj_usb_print_ok_u_10888")));
