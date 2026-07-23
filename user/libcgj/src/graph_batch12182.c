/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12182: IPP ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_ipp_ok_u_12182(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       Internet Printing Protocol probe for the cups/print continuum.
 *   uint32_t __gj_ipp_ok_u_12182  (alias)
 *   __libcgj_batch12182_marker = "libcgj-batch12182"
 *
 * Exclusive continuum CREATE-ONLY (12181-12190: cups/print soft id
 * stubs — cups_ok_u_12181, ipp_ok_u_12182, print_job_ok_u_12183,
 * printer_discover_ok_u_12184, ppd_ok_u_12185, filter_ok_u_12186,
 * spool_ok_u_12187, usb_print_ok_u_12188, print_soft_ready_u_12189,
 * batch_id_12190). Unique surface only; no multi-def. Distinct from
 * gj_ipp_ok_u_11882 (batch11882), gj_ipp_ok_u_11682 (batch11682),
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

const char __libcgj_batch12182_marker[] = "libcgj-batch12182";

/* Soft IPP-ok lamp: always off (not a real IPP probe). */
#define B12182_IPP_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12182_ipp_ok(void)
{
	return B12182_IPP_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ipp_ok_u_12182 - IPP ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not open IPP sockets or
 * call libc. No parent wires.
 */
uint32_t
gj_ipp_ok_u_12182(void)
{
	(void)NULL;
	return b12182_ipp_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ipp_ok_u_12182(void)
    __attribute__((alias("gj_ipp_ok_u_12182")));
