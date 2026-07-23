/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12184: printer discover ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_printer_discover_ok_u_12184(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       printer discovery / browse probe for the cups/print continuum.
 *   uint32_t __gj_printer_discover_ok_u_12184  (alias)
 *   __libcgj_batch12184_marker = "libcgj-batch12184"
 *
 * Exclusive continuum CREATE-ONLY (12181-12190: cups/print soft id
 * stubs — cups_ok_u_12181, ipp_ok_u_12182, print_job_ok_u_12183,
 * printer_discover_ok_u_12184, ppd_ok_u_12185, filter_ok_u_12186,
 * spool_ok_u_12187, usb_print_ok_u_12188, print_soft_ready_u_12189,
 * batch_id_12190). Unique surface only; no multi-def. Distinct from
 * gj_printer_discover_ok_u_11884 (batch11884),
 * gj_printer_discover_ok_u_11684 (batch11684),
 * gj_printer_discover_ok_u_10184 (batch10184) and
 * gj_cups_dest_ok_u_9443 (batch9443). No parent wires. No __int128.
 * No CUPS implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12184_marker[] = "libcgj-batch12184";

/* Soft printer-discover-ok lamp: always off (not a real browse probe). */
#define B12184_PRINTER_DISCOVER_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12184_printer_discover_ok(void)
{
	return B12184_PRINTER_DISCOVER_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_printer_discover_ok_u_12184 - printer discover ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not browse mDNS/IPP or
 * call libc. No parent wires.
 */
uint32_t
gj_printer_discover_ok_u_12184(void)
{
	(void)NULL;
	return b12184_printer_discover_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_printer_discover_ok_u_12184(void)
    __attribute__((alias("gj_printer_discover_ok_u_12184")));
