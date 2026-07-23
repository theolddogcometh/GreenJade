/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12785: PPD ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_ppd_ok_u_12785(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       PostScript Printer Description probe for the cups/print
 *       continuum.
 *   uint32_t __gj_ppd_ok_u_12785  (alias)
 *   __libcgj_batch12785_marker = "libcgj-batch12785"
 *
 * Exclusive continuum CREATE-ONLY (12781-12790: cups/print soft id
 * stubs — cups_ok_u_12781, ipp_ok_u_12782, print_job_ok_u_12783,
 * printer_discover_ok_u_12784, ppd_ok_u_12785, filter_ok_u_12786,
 * spool_ok_u_12787, usb_print_ok_u_12788, print_soft_ready_u_12789,
 * batch_id_12790). Unique surface only; no multi-def. Distinct from
 * gj_ppd_ok_u_12585 (batch12585), gj_ppd_ok_u_12385 (batch12385),
 * gj_ppd_ok_u_12185 (batch12185), gj_ppd_ok_u_10185 (batch10185) and
 * gj_cups_ppd_ok_u_9444 (batch9444). No parent wires. No __int128. No
 * CUPS/PPD implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12785_marker[] = "libcgj-batch12785";

/* Soft PPD-ok lamp: always off (not a real PPD probe). */
#define B12785_PPD_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12785_ppd_ok(void)
{
	return B12785_PPD_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ppd_ok_u_12785 - PPD ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not parse PPD files or
 * call libc. No parent wires.
 */
uint32_t
gj_ppd_ok_u_12785(void)
{
	(void)NULL;
	return b12785_ppd_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ppd_ok_u_12785(void)
    __attribute__((alias("gj_ppd_ok_u_12785")));
