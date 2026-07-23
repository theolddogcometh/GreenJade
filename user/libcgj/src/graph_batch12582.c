/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12582: IPP ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_ipp_ok_u_12582(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       Internet Printing Protocol probe for the cups/print continuum.
 *   uint32_t __gj_ipp_ok_u_12582  (alias)
 *   __libcgj_batch12582_marker = "libcgj-batch12582"
 *
 * Exclusive continuum CREATE-ONLY (12581-12590: cups/print soft id
 * stubs — cups_ok_u_12581, ipp_ok_u_12582, print_job_ok_u_12583,
 * printer_discover_ok_u_12584, ppd_ok_u_12585, filter_ok_u_12586,
 * spool_ok_u_12587, usb_print_ok_u_12588, print_soft_ready_u_12589,
 * batch_id_12590). Unique surface only; no multi-def. Distinct from
 * gj_ipp_ok_u_12382 (batch12382), gj_ipp_ok_u_12182 (batch12182),
 * gj_ipp_ok_u_10182 (batch10182) and gj_cups_ipp_ok_u_9445
 * (batch9445). No parent wires. No __int128. No CUPS/IPP
 * implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12582_marker[] = "libcgj-batch12582";

/* Soft IPP-ok lamp: always off (not a real IPP probe). */
#define B12582_IPP_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12582_ipp_ok(void)
{
	return B12582_IPP_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ipp_ok_u_12582 - IPP ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not open IPP sockets or
 * call libc. No parent wires.
 */
uint32_t
gj_ipp_ok_u_12582(void)
{
	(void)NULL;
	return b12582_ipp_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ipp_ok_u_12582(void)
    __attribute__((alias("gj_ipp_ok_u_12582")));
