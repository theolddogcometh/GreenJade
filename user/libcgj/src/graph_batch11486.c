/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11486: filter ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_filter_ok_u_11486(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       CUPS filter / raster filter probe for the cups/print continuum.
 *   uint32_t __gj_filter_ok_u_11486  (alias)
 *   __libcgj_batch11486_marker = "libcgj-batch11486"
 *
 * Exclusive continuum CREATE-ONLY (11481-11490: cups/print soft id
 * stubs — cups_ok_u_11481, ipp_ok_u_11482, print_job_ok_u_11483,
 * printer_discover_ok_u_11484, ppd_ok_u_11485, filter_ok_u_11486,
 * spool_ok_u_11487, usb_print_ok_u_11488, print_soft_ready_u_11489,
 * batch_id_11490). Unique surface only; no multi-def. Distinct from
 * gj_filter_ok_u_10186 (batch10186). No parent wires. No __int128.
 * No CUPS filter implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11486_marker[] = "libcgj-batch11486";

/* Soft filter-ok lamp: always off (not a real filter probe). */
#define B11486_FILTER_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11486_filter_ok(void)
{
	return B11486_FILTER_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_filter_ok_u_11486 - filter ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not run filters or call
 * libc. No parent wires.
 */
uint32_t
gj_filter_ok_u_11486(void)
{
	(void)NULL;
	return b11486_filter_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_filter_ok_u_11486(void)
    __attribute__((alias("gj_filter_ok_u_11486")));
