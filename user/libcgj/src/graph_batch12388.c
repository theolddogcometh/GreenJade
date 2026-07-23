/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12388: USB print ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_usb_print_ok_u_12388(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       USB printer / usblp probe for the cups/print continuum.
 *   uint32_t __gj_usb_print_ok_u_12388  (alias)
 *   __libcgj_batch12388_marker = "libcgj-batch12388"
 *
 * Exclusive continuum CREATE-ONLY (12381-12390: cups/print soft id
 * stubs — cups_ok_u_12381, ipp_ok_u_12382, print_job_ok_u_12383,
 * printer_discover_ok_u_12384, ppd_ok_u_12385, filter_ok_u_12386,
 * spool_ok_u_12387, usb_print_ok_u_12388, print_soft_ready_u_12389,
 * batch_id_12390). Unique surface only; no multi-def. Distinct from
 * gj_usb_print_ok_u_10188 (batch10188). No parent wires. No __int128.
 * No USB/CUPS implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12388_marker[] = "libcgj-batch12388";

/* Soft USB-print-ok lamp: always off (not a real USB printer probe). */
#define B12388_USB_PRINT_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12388_usb_print_ok(void)
{
	return B12388_USB_PRINT_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_usb_print_ok_u_12388 - USB print ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe usblp/sysfs or
 * call libc. No parent wires.
 */
uint32_t
gj_usb_print_ok_u_12388(void)
{
	(void)NULL;
	return b12388_usb_print_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_usb_print_ok_u_12388(void)
    __attribute__((alias("gj_usb_print_ok_u_12388")));
