/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13182: IPP ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_ipp_ok_u_13182(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       Internet Printing Protocol probe for the cups/print continuum.
 *   uint32_t __gj_ipp_ok_u_13182  (alias)
 *   __libcgj_batch13182_marker = "libcgj-batch13182"
 *
 * Exclusive continuum CREATE-ONLY (13181-13190: cups/print soft id
 * stubs — cups_ok_u_13181, ipp_ok_u_13182, print_job_ok_u_13183,
 * printer_discover_ok_u_13184, ppd_ok_u_13185, filter_ok_u_13186,
 * spool_ok_u_13187, usb_print_ok_u_13188, print_soft_ready_u_13189,
 * batch_id_13190).
 * Unique surface only; no multi-def. Distinct from
 * gj_ipp_ok_u_12982 (batch12982), gj_ipp_ok_u_12782 (batch12782),
 * gj_ipp_ok_u_12582 (batch12582), gj_ipp_ok_u_12382 (batch12382),
 * gj_ipp_ok_u_12182 (batch12182), gj_ipp_ok_u_10182 (batch10182) and
 * gj_cups_ipp_ok_u_9445 (batch9445). No parent wires. No __int128.
 * No CUPS/IPP implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13182_marker[] = "libcgj-batch13182";

/* Soft IPP-ok lamp: always off (not a real IPP probe). */
#define B13182_IPP_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13182_ipp_ok(void)
{
	return B13182_IPP_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ipp_ok_u_13182 - IPP ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not open IPP sockets or
 * call libc. No parent wires.
 */
uint32_t
gj_ipp_ok_u_13182(void)
{
	(void)NULL;
	return b13182_ipp_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ipp_ok_u_13182(void)
    __attribute__((alias("gj_ipp_ok_u_13182")));
