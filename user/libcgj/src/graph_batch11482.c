/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11482: IPP ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_ipp_ok_u_11482(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       Internet Printing Protocol probe for the cups/print continuum.
 *   uint32_t __gj_ipp_ok_u_11482  (alias)
 *   __libcgj_batch11482_marker = "libcgj-batch11482"
 *
 * Exclusive continuum CREATE-ONLY (11481-11490: cups/print soft id
 * stubs — cups_ok_u_11481, ipp_ok_u_11482, print_job_ok_u_11483,
 * printer_discover_ok_u_11484, ppd_ok_u_11485, filter_ok_u_11486,
 * spool_ok_u_11487, usb_print_ok_u_11488, print_soft_ready_u_11489,
 * batch_id_11490). Unique surface only; no multi-def. Distinct from
 * gj_ipp_ok_u_10182 (batch10182). No parent wires. No __int128.
 * No CUPS/IPP implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11482_marker[] = "libcgj-batch11482";

/* Soft IPP-ok lamp: always off (not a real IPP probe). */
#define B11482_IPP_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11482_ipp_ok(void)
{
	return B11482_IPP_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ipp_ok_u_11482 - IPP ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not open IPP sockets or
 * call libc. No parent wires.
 */
uint32_t
gj_ipp_ok_u_11482(void)
{
	(void)NULL;
	return b11482_ipp_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ipp_ok_u_11482(void)
    __attribute__((alias("gj_ipp_ok_u_11482")));
