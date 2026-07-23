/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12588: USB print ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_usb_print_ok_u_12588(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       USB printer / usblp probe for the cups/print continuum.
 *   uint32_t __gj_usb_print_ok_u_12588  (alias)
 *   __libcgj_batch12588_marker = "libcgj-batch12588"
 *
 * Exclusive continuum CREATE-ONLY (12581-12590: cups/print soft id
 * stubs — cups_ok_u_12581, ipp_ok_u_12582, print_job_ok_u_12583,
 * printer_discover_ok_u_12584, ppd_ok_u_12585, filter_ok_u_12586,
 * spool_ok_u_12587, usb_print_ok_u_12588, print_soft_ready_u_12589,
 * batch_id_12590). Unique surface only; no multi-def. Distinct from
 * gj_usb_print_ok_u_12388 (batch12388), gj_usb_print_ok_u_12188
 * (batch12188), gj_usb_print_ok_u_10188 (batch10188). No parent wires.
 * No __int128. No USB/CUPS implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12588_marker[] = "libcgj-batch12588";

/* Soft USB-print-ok lamp: always off (not a real USB printer probe). */
#define B12588_USB_PRINT_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12588_usb_print_ok(void)
{
	return B12588_USB_PRINT_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_usb_print_ok_u_12588 - USB print ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe usblp/sysfs or
 * call libc. No parent wires.
 */
uint32_t
gj_usb_print_ok_u_12588(void)
{
	(void)NULL;
	return b12588_usb_print_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_usb_print_ok_u_12588(void)
    __attribute__((alias("gj_usb_print_ok_u_12588")));
