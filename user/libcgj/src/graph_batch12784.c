/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12784: printer discover ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_printer_discover_ok_u_12784(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       printer discovery / browse probe for the cups/print continuum.
 *   uint32_t __gj_printer_discover_ok_u_12784  (alias)
 *   __libcgj_batch12784_marker = "libcgj-batch12784"
 *
 * Exclusive continuum CREATE-ONLY (12781-12790: cups/print soft id
 * stubs — cups_ok_u_12781, ipp_ok_u_12782, print_job_ok_u_12783,
 * printer_discover_ok_u_12784, ppd_ok_u_12785, filter_ok_u_12786,
 * spool_ok_u_12787, usb_print_ok_u_12788, print_soft_ready_u_12789,
 * batch_id_12790). Unique surface only; no multi-def. Distinct from
 * gj_printer_discover_ok_u_12584 (batch12584),
 * gj_printer_discover_ok_u_12384 (batch12384),
 * gj_printer_discover_ok_u_12184 (batch12184),
 * gj_printer_discover_ok_u_10184 (batch10184) and gj_cups_dest_ok_u_9443
 * (batch9443). No parent wires. No __int128. No CUPS implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12784_marker[] = "libcgj-batch12784";

/* Soft printer-discover-ok lamp: always off (not a real browse probe). */
#define B12784_PRINTER_DISCOVER_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12784_printer_discover_ok(void)
{
	return B12784_PRINTER_DISCOVER_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_printer_discover_ok_u_12784 - printer discover ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not browse mDNS/IPP or
 * call libc. No parent wires.
 */
uint32_t
gj_printer_discover_ok_u_12784(void)
{
	(void)NULL;
	return b12784_printer_discover_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_printer_discover_ok_u_12784(void)
    __attribute__((alias("gj_printer_discover_ok_u_12784")));
