/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10485: PPD ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_ppd_ok_u_10485(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       PPD (PostScript Printer Description) probe for the print soft
 *       continuum.
 *   uint32_t __gj_ppd_ok_u_10485  (alias)
 *   __libcgj_batch10485_marker = "libcgj-batch10485"
 *
 * Exclusive continuum CREATE-ONLY (10481-10490: print soft id
 * stubs — cups_ok_u_10481, ipp_ok_u_10482, print_job_ok_u_10483,
 * printer_discover_ok_u_10484, ppd_ok_u_10485, filter_ok_u_10486,
 * spool_ok_u_10487, usb_print_ok_u_10488, print_soft_ready_u_10489,
 * batch_id_10490). Unique surface only; no multi-def. Distinct from
 * prior PPD soft tags. No parent wires. No __int128. No PPD parser.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10485_marker[] = "libcgj-batch10485";

/* Soft PPD-ok lamp: always off (not a real PPD probe). */
#define B10485_PPD_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10485_ppd_ok(void)
{
	return B10485_PPD_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ppd_ok_u_10485 - PPD ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not parse PPD files or
 * call libc. No parent wires.
 */
uint32_t
gj_ppd_ok_u_10485(void)
{
	(void)NULL;
	return b10485_ppd_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ppd_ok_u_10485(void)
    __attribute__((alias("gj_ppd_ok_u_10485")));
