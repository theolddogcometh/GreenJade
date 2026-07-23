/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12782: IPP ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_ipp_ok_u_12782(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       Internet Printing Protocol probe for the cups/print continuum.
 *   uint32_t __gj_ipp_ok_u_12782  (alias)
 *   __libcgj_batch12782_marker = "libcgj-batch12782"
 *
 * Exclusive continuum CREATE-ONLY (12781-12790: cups/print soft id
 * stubs — cups_ok_u_12781, ipp_ok_u_12782, print_job_ok_u_12783,
 * printer_discover_ok_u_12784, ppd_ok_u_12785, filter_ok_u_12786,
 * spool_ok_u_12787, usb_print_ok_u_12788, print_soft_ready_u_12789,
 * batch_id_12790). Unique surface only; no multi-def. Distinct from
 * gj_ipp_ok_u_12582 (batch12582), gj_ipp_ok_u_12382 (batch12382),
 * gj_ipp_ok_u_12182 (batch12182), gj_ipp_ok_u_10182 (batch10182) and
 * gj_cups_ipp_ok_u_9445 (batch9445). No parent wires. No __int128. No
 * CUPS/IPP implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12782_marker[] = "libcgj-batch12782";

/* Soft IPP-ok lamp: always off (not a real IPP probe). */
#define B12782_IPP_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12782_ipp_ok(void)
{
	return B12782_IPP_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ipp_ok_u_12782 - IPP ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not open IPP sockets or
 * call libc. No parent wires.
 */
uint32_t
gj_ipp_ok_u_12782(void)
{
	(void)NULL;
	return b12782_ipp_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ipp_ok_u_12782(void)
    __attribute__((alias("gj_ipp_ok_u_12782")));
