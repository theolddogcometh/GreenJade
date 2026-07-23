/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13386: filter ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_filter_ok_u_13386(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       CUPS filter / raster filter probe for the cups/print continuum.
 *   uint32_t __gj_filter_ok_u_13386  (alias)
 *   __libcgj_batch13386_marker = "libcgj-batch13386"
 *
 * Exclusive continuum CREATE-ONLY (13381-13390: cups/print soft id
 * stubs — cups_ok_u_13381, ipp_ok_u_13382, print_job_ok_u_13383,
 * printer_discover_ok_u_13384, ppd_ok_u_13385, filter_ok_u_13386,
 * spool_ok_u_13387, usb_print_ok_u_13388, print_soft_ready_u_13389,
 * batch_id_13390).
 * Unique surface only; no multi-def. Distinct from
 * gj_filter_ok_u_13186 (batch13186), gj_filter_ok_u_12986
 * (batch12986), gj_filter_ok_u_12786 (batch12786),
 * gj_filter_ok_u_12586 (batch12586), gj_filter_ok_u_12386
 * (batch12386), gj_filter_ok_u_10186 (batch10186). No parent wires.
 * No __int128. No CUPS filter implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13386_marker[] = "libcgj-batch13386";

/* Soft filter-ok lamp: always off (not a real filter probe). */
#define B13386_FILTER_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13386_filter_ok(void)
{
	return B13386_FILTER_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_filter_ok_u_13386 - filter ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not run CUPS filters or
 * call libc. No parent wires.
 */
uint32_t
gj_filter_ok_u_13386(void)
{
	(void)NULL;
	return b13386_filter_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_filter_ok_u_13386(void)
    __attribute__((alias("gj_filter_ok_u_13386")));
