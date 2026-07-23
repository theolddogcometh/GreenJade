/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12586: filter ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_filter_ok_u_12586(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       CUPS filter / raster filter probe for the cups/print continuum.
 *   uint32_t __gj_filter_ok_u_12586  (alias)
 *   __libcgj_batch12586_marker = "libcgj-batch12586"
 *
 * Exclusive continuum CREATE-ONLY (12581-12590: cups/print soft id
 * stubs — cups_ok_u_12581, ipp_ok_u_12582, print_job_ok_u_12583,
 * printer_discover_ok_u_12584, ppd_ok_u_12585, filter_ok_u_12586,
 * spool_ok_u_12587, usb_print_ok_u_12588, print_soft_ready_u_12589,
 * batch_id_12590). Unique surface only; no multi-def. Distinct from
 * gj_filter_ok_u_12386 (batch12386), gj_filter_ok_u_12186
 * (batch12186), gj_filter_ok_u_10186 (batch10186). No parent wires.
 * No __int128. No CUPS filter implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12586_marker[] = "libcgj-batch12586";

/* Soft filter-ok lamp: always off (not a real filter probe). */
#define B12586_FILTER_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12586_filter_ok(void)
{
	return B12586_FILTER_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_filter_ok_u_12586 - filter ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not run filters or call
 * libc. No parent wires.
 */
uint32_t
gj_filter_ok_u_12586(void)
{
	(void)NULL;
	return b12586_filter_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_filter_ok_u_12586(void)
    __attribute__((alias("gj_filter_ok_u_12586")));
