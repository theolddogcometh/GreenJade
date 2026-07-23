/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10482: IPP ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_ipp_ok_u_10482(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       IPP (Internet Printing Protocol) probe for the print soft
 *       continuum.
 *   uint32_t __gj_ipp_ok_u_10482  (alias)
 *   __libcgj_batch10482_marker = "libcgj-batch10482"
 *
 * Exclusive continuum CREATE-ONLY (10481-10490: print soft id
 * stubs — cups_ok_u_10481, ipp_ok_u_10482, print_job_ok_u_10483,
 * printer_discover_ok_u_10484, ppd_ok_u_10485, filter_ok_u_10486,
 * spool_ok_u_10487, usb_print_ok_u_10488, print_soft_ready_u_10489,
 * batch_id_10490). Unique surface only; no multi-def. Distinct from
 * prior print/IPP soft tags. No parent wires. No __int128. No IPP
 * implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10482_marker[] = "libcgj-batch10482";

/* Soft IPP-ok lamp: always off (not a real IPP probe). */
#define B10482_IPP_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10482_ipp_ok(void)
{
	return B10482_IPP_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ipp_ok_u_10482 - IPP ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not inspect IPP transport
 * or call libc. No parent wires.
 */
uint32_t
gj_ipp_ok_u_10482(void)
{
	(void)NULL;
	return b10482_ipp_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ipp_ok_u_10482(void)
    __attribute__((alias("gj_ipp_ok_u_10482")));
