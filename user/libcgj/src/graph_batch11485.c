/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11485: PPD ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_ppd_ok_u_11485(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       PostScript Printer Description probe for the cups/print
 *       continuum.
 *   uint32_t __gj_ppd_ok_u_11485  (alias)
 *   __libcgj_batch11485_marker = "libcgj-batch11485"
 *
 * Exclusive continuum CREATE-ONLY (11481-11490: cups/print soft id
 * stubs — cups_ok_u_11481, ipp_ok_u_11482, print_job_ok_u_11483,
 * printer_discover_ok_u_11484, ppd_ok_u_11485, filter_ok_u_11486,
 * spool_ok_u_11487, usb_print_ok_u_11488, print_soft_ready_u_11489,
 * batch_id_11490). Unique surface only; no multi-def. Distinct from
 * gj_ppd_ok_u_10185 (batch10185). No parent wires. No __int128.
 * No CUPS/PPD implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11485_marker[] = "libcgj-batch11485";

/* Soft PPD-ok lamp: always off (not a real PPD probe). */
#define B11485_PPD_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11485_ppd_ok(void)
{
	return B11485_PPD_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ppd_ok_u_11485 - PPD ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not parse PPD files or
 * call libc. No parent wires.
 */
uint32_t
gj_ppd_ok_u_11485(void)
{
	(void)NULL;
	return b11485_ppd_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ppd_ok_u_11485(void)
    __attribute__((alias("gj_ppd_ok_u_11485")));
