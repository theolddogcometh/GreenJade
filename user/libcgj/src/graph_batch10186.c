/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10186: filter ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_filter_ok_u_10186(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       CUPS filter / raster filter probe for the cups/print continuum.
 *   uint32_t __gj_filter_ok_u_10186  (alias)
 *   __libcgj_batch10186_marker = "libcgj-batch10186"
 *
 * Exclusive continuum CREATE-ONLY (10181-10190: cups/print soft id
 * stubs — cups_ok_u_10181, ipp_ok_u_10182, print_job_ok_u_10183,
 * printer_discover_ok_u_10184, ppd_ok_u_10185, filter_ok_u_10186,
 * spool_ok_u_10187, usb_print_ok_u_10188, print_soft_ready_u_10189,
 * batch_id_10190). Unique surface only; no multi-def. No parent wires.
 * No __int128. No CUPS filter implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10186_marker[] = "libcgj-batch10186";

/* Soft filter-ok lamp: always off (not a real filter probe). */
#define B10186_FILTER_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10186_filter_ok(void)
{
	return B10186_FILTER_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_filter_ok_u_10186 - filter ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not run filters or call
 * libc. No parent wires.
 */
uint32_t
gj_filter_ok_u_10186(void)
{
	(void)NULL;
	return b10186_filter_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_filter_ok_u_10186(void)
    __attribute__((alias("gj_filter_ok_u_10186")));
