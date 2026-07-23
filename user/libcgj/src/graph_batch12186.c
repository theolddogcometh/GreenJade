/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12186: filter ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_filter_ok_u_12186(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       CUPS filter / raster filter probe for the cups/print continuum.
 *   uint32_t __gj_filter_ok_u_12186  (alias)
 *   __libcgj_batch12186_marker = "libcgj-batch12186"
 *
 * Exclusive continuum CREATE-ONLY (12181-12190: cups/print soft id
 * stubs — cups_ok_u_12181, ipp_ok_u_12182, print_job_ok_u_12183,
 * printer_discover_ok_u_12184, ppd_ok_u_12185, filter_ok_u_12186,
 * spool_ok_u_12187, usb_print_ok_u_12188, print_soft_ready_u_12189,
 * batch_id_12190). Unique surface only; no multi-def. Distinct from
 * gj_filter_ok_u_11886 (batch11886), gj_filter_ok_u_11686
 * (batch11686), gj_filter_ok_u_10186 (batch10186). No parent wires.
 * No __int128. No CUPS filter implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12186_marker[] = "libcgj-batch12186";

/* Soft filter-ok lamp: always off (not a real filter probe). */
#define B12186_FILTER_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12186_filter_ok(void)
{
	return B12186_FILTER_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_filter_ok_u_12186 - filter ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not run filters or call
 * libc. No parent wires.
 */
uint32_t
gj_filter_ok_u_12186(void)
{
	(void)NULL;
	return b12186_filter_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_filter_ok_u_12186(void)
    __attribute__((alias("gj_filter_ok_u_12186")));
