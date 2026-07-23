/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12185: PPD ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_ppd_ok_u_12185(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       PostScript Printer Description probe for the cups/print
 *       continuum.
 *   uint32_t __gj_ppd_ok_u_12185  (alias)
 *   __libcgj_batch12185_marker = "libcgj-batch12185"
 *
 * Exclusive continuum CREATE-ONLY (12181-12190: cups/print soft id
 * stubs — cups_ok_u_12181, ipp_ok_u_12182, print_job_ok_u_12183,
 * printer_discover_ok_u_12184, ppd_ok_u_12185, filter_ok_u_12186,
 * spool_ok_u_12187, usb_print_ok_u_12188, print_soft_ready_u_12189,
 * batch_id_12190). Unique surface only; no multi-def. Distinct from
 * gj_ppd_ok_u_11885 (batch11885), gj_ppd_ok_u_11685 (batch11685),
 * gj_ppd_ok_u_10185 (batch10185) and gj_cups_ppd_ok_u_9444
 * (batch9444). No parent wires. No __int128. No CUPS/PPD
 * implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12185_marker[] = "libcgj-batch12185";

/* Soft PPD-ok lamp: always off (not a real PPD probe). */
#define B12185_PPD_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12185_ppd_ok(void)
{
	return B12185_PPD_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ppd_ok_u_12185 - PPD ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not parse PPD files or
 * call libc. No parent wires.
 */
uint32_t
gj_ppd_ok_u_12185(void)
{
	(void)NULL;
	return b12185_ppd_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ppd_ok_u_12185(void)
    __attribute__((alias("gj_ppd_ok_u_12185")));
