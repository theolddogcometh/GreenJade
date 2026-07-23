/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13582: IPP ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_ipp_ok_u_13582(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       Internet Printing Protocol probe for the cups/print continuum.
 *   uint32_t __gj_ipp_ok_u_13582  (alias)
 *   __libcgj_batch13582_marker = "libcgj-batch13582"
 *
 * Exclusive continuum CREATE-ONLY (13581-13590: cups/print soft id
 * stubs — cups_ok_u_13581, ipp_ok_u_13582, print_job_ok_u_13583,
 * printer_discover_ok_u_13584, ppd_ok_u_13585, filter_ok_u_13586,
 * spool_ok_u_13587, usb_print_ok_u_13588, print_soft_ready_u_13589,
 * batch_id_13590→13590).
 * Unique surface only; no multi-def. Distinct from
 * gj_ipp_ok_u_13382 (batch13382), gj_ipp_ok_u_13182 (batch13182),
 * gj_ipp_ok_u_12982 (batch12982), gj_ipp_ok_u_12782 (batch12782),
 * gj_ipp_ok_u_12582 (batch12582), gj_ipp_ok_u_12382 (batch12382),
 * gj_ipp_ok_u_10182 (batch10182) and gj_cups_ipp_ok_u_9445
 * (batch9445). No parent wires. No __int128. No CUPS/IPP implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13582_marker[] = "libcgj-batch13582";

/* Soft IPP-ok lamp: always off (not a real IPP probe). */
#define B13582_IPP_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13582_ipp_ok(void)
{
	return B13582_IPP_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ipp_ok_u_13582 - IPP ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not open IPP sockets or
 * call libc. No parent wires.
 */
uint32_t
gj_ipp_ok_u_13582(void)
{
	(void)NULL;
	return b13582_ipp_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ipp_ok_u_13582(void)
    __attribute__((alias("gj_ipp_ok_u_13582")));
