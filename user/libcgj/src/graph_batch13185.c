/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13185: PPD ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_ppd_ok_u_13185(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       PostScript Printer Description probe for the cups/print
 *       continuum.
 *   uint32_t __gj_ppd_ok_u_13185  (alias)
 *   __libcgj_batch13185_marker = "libcgj-batch13185"
 *
 * Exclusive continuum CREATE-ONLY (13181-13190: cups/print soft id
 * stubs — cups_ok_u_13181, ipp_ok_u_13182, print_job_ok_u_13183,
 * printer_discover_ok_u_13184, ppd_ok_u_13185, filter_ok_u_13186,
 * spool_ok_u_13187, usb_print_ok_u_13188, print_soft_ready_u_13189,
 * batch_id_13190).
 * Unique surface only; no multi-def. Distinct from
 * gj_ppd_ok_u_12985 (batch12985), gj_ppd_ok_u_12785 (batch12785),
 * gj_ppd_ok_u_12585 (batch12585), gj_ppd_ok_u_12385 (batch12385),
 * gj_ppd_ok_u_12185 (batch12185), gj_ppd_ok_u_10185 (batch10185) and
 * gj_cups_ppd_ok_u_9444 (batch9444). No parent wires. No __int128.
 * No CUPS/PPD implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13185_marker[] = "libcgj-batch13185";

/* Soft PPD-ok lamp: always off (not a real PPD probe). */
#define B13185_PPD_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13185_ppd_ok(void)
{
	return B13185_PPD_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ppd_ok_u_13185 - PPD ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not parse PPD files or
 * call libc. No parent wires.
 */
uint32_t
gj_ppd_ok_u_13185(void)
{
	(void)NULL;
	return b13185_ppd_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ppd_ok_u_13185(void)
    __attribute__((alias("gj_ppd_ok_u_13185")));
