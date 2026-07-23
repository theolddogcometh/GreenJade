/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13588: USB print ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_usb_print_ok_u_13588(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       USB printer / usblp probe for the cups/print continuum.
 *   uint32_t __gj_usb_print_ok_u_13588  (alias)
 *   __libcgj_batch13588_marker = "libcgj-batch13588"
 *
 * Exclusive continuum CREATE-ONLY (13581-13590: cups/print soft id
 * stubs — cups_ok_u_13581, ipp_ok_u_13582, print_job_ok_u_13583,
 * printer_discover_ok_u_13584, ppd_ok_u_13585, filter_ok_u_13586,
 * spool_ok_u_13587, usb_print_ok_u_13588, print_soft_ready_u_13589,
 * batch_id_13590→13590).
 * Unique surface only; no multi-def. Distinct from
 * gj_usb_print_ok_u_13388 (batch13388), gj_usb_print_ok_u_13188
 * (batch13188), gj_usb_print_ok_u_12988 (batch12988),
 * gj_usb_print_ok_u_12788 (batch12788), gj_usb_print_ok_u_12588
 * (batch12588), gj_usb_print_ok_u_12388 (batch12388) and
 * gj_usb_print_ok_u_10188 (batch10188). No parent wires. No __int128.
 * No USB/CUPS implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13588_marker[] = "libcgj-batch13588";

/* Soft USB-print-ok lamp: always off (not a real USB printer probe). */
#define B13588_USB_PRINT_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13588_usb_print_ok(void)
{
	return B13588_USB_PRINT_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_usb_print_ok_u_13588 - USB print ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe usblp/sysfs or
 * call libc. No parent wires.
 */
uint32_t
gj_usb_print_ok_u_13588(void)
{
	(void)NULL;
	return b13588_usb_print_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_usb_print_ok_u_13588(void)
    __attribute__((alias("gj_usb_print_ok_u_13588")));
