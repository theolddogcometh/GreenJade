/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12984: printer discover ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_printer_discover_ok_u_12984(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       printer discovery / CUPS dest probe for the cups/print
 *       continuum.
 *   uint32_t __gj_printer_discover_ok_u_12984  (alias)
 *   __libcgj_batch12984_marker = "libcgj-batch12984"
 *
 * Exclusive continuum CREATE-ONLY (12981-12990: cups/print soft id
 * stubs — cups_ok_u_12981, ipp_ok_u_12982, print_job_ok_u_12983,
 * printer_discover_ok_u_12984, ppd_ok_u_12985, filter_ok_u_12986,
 * spool_ok_u_12987, usb_print_ok_u_12988, print_soft_ready_u_12989,
 * batch_id_12990). Unique surface only; no multi-def. Distinct from
 * gj_printer_discover_ok_u_12784 (batch12784),
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

const char __libcgj_batch12984_marker[] = "libcgj-batch12984";

/* Soft printer-discover-ok lamp: always off (not a real dest probe). */
#define B12984_PRINTER_DISCOVER_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12984_printer_discover_ok(void)
{
	return B12984_PRINTER_DISCOVER_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_printer_discover_ok_u_12984 - printer discover ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not enumerate printers
 * or call libc. No parent wires.
 */
uint32_t
gj_printer_discover_ok_u_12984(void)
{
	(void)NULL;
	return b12984_printer_discover_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_printer_discover_ok_u_12984(void)
    __attribute__((alias("gj_printer_discover_ok_u_12984")));
