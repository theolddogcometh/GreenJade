/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12581: CUPS ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_cups_ok_u_12581(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       cups/cupsd probe for the cups/print continuum.
 *   uint32_t __gj_cups_ok_u_12581  (alias)
 *   __libcgj_batch12581_marker = "libcgj-batch12581"
 *
 * Exclusive continuum CREATE-ONLY (12581-12590: cups/print soft id
 * stubs — cups_ok_u_12581, ipp_ok_u_12582, print_job_ok_u_12583,
 * printer_discover_ok_u_12584, ppd_ok_u_12585, filter_ok_u_12586,
 * spool_ok_u_12587, usb_print_ok_u_12588, print_soft_ready_u_12589,
 * batch_id_12590). Unique surface only; no multi-def. Distinct from
 * gj_cups_ok_u_12381 (batch12381), gj_cups_ok_u_12181 (batch12181),
 * gj_cups_ok_u_10181 (batch10181) and gj_cups_printer_ok_u_9441
 * (batch9441). No parent wires. No __int128. No CUPS implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12581_marker[] = "libcgj-batch12581";

/* Soft CUPS-ok lamp: always off (not a real cups probe). */
#define B12581_CUPS_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12581_cups_ok(void)
{
	return B12581_CUPS_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cups_ok_u_12581 - CUPS ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not inspect cupsd or
 * call libc. No parent wires.
 */
uint32_t
gj_cups_ok_u_12581(void)
{
	(void)NULL;
	return b12581_cups_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_cups_ok_u_12581(void)
    __attribute__((alias("gj_cups_ok_u_12581")));
