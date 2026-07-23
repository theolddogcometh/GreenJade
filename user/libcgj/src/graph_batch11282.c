/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11282: IPP ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_ipp_ok_u_11282(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       Internet Printing Protocol probe for the cups/print continuum.
 *   uint32_t __gj_ipp_ok_u_11282  (alias)
 *   __libcgj_batch11282_marker = "libcgj-batch11282"
 *
 * Exclusive continuum CREATE-ONLY (11281-11290: cups/print soft id
 * stubs — cups_ok_u_11281, ipp_ok_u_11282, print_job_ok_u_11283,
 * printer_discover_ok_u_11284, ppd_ok_u_11285, filter_ok_u_11286,
 * spool_ok_u_11287, usb_print_ok_u_11288, print_soft_ready_u_11289,
 * batch_id_11290). Unique surface only; no multi-def. Distinct from
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

const char __libcgj_batch11282_marker[] = "libcgj-batch11282";

/* Soft IPP-ok lamp: always off (not a real IPP probe). */
#define B11282_IPP_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11282_ipp_ok(void)
{
	return B11282_IPP_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ipp_ok_u_11282 - IPP ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not open IPP sockets or
 * call libc. No parent wires.
 */
uint32_t
gj_ipp_ok_u_11282(void)
{
	(void)NULL;
	return b11282_ipp_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ipp_ok_u_11282(void)
    __attribute__((alias("gj_ipp_ok_u_11282")));
