/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12589: print soft continuum lamp (on).
 *
 * Surface (unique symbols):
 *   uint32_t gj_print_soft_ready_u_12589(void);
 *     - Returns 1 (soft lamp only). Indicates the cups/print soft-id
 *       stub surfaces in this continuum are present; not cups, ipp,
 *       print_job, printer_discover, ppd, filter, spool, or usb_print
 *       ok.
 *   uint32_t __gj_print_soft_ready_u_12589  (alias)
 *   __libcgj_batch12589_marker = "libcgj-batch12589"
 *
 * Exclusive continuum CREATE-ONLY (12581-12590: cups/print soft id
 * stubs — cups_ok_u_12581, ipp_ok_u_12582, print_job_ok_u_12583,
 * printer_discover_ok_u_12584, ppd_ok_u_12585, filter_ok_u_12586,
 * spool_ok_u_12587, usb_print_ok_u_12588, print_soft_ready_u_12589,
 * batch_id_12590). Unique surface only; no multi-def. cups/ipp/
 * print_job/printer_discover/ppd/filter/spool/usb_print ok units
 * remain 0. Distinct from gj_print_soft_ready_u_12389 (batch12389),
 * gj_print_soft_ready_u_12189 (batch12189),
 * gj_print_soft_ready_u_10189 (batch10189) and gj_cups_ready_u_9449
 * (batch9449). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12589_marker[] = "libcgj-batch12589";

/* Soft continuum lamp: surfaces present (soft ready only). */
#define B12589_SOFT_READY  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12589_soft_ready(void)
{
	return B12589_SOFT_READY;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_print_soft_ready_u_12589 - print soft continuum ready lamp.
 *
 * Always returns 1. Soft pure-data product tag that exclusive wave
 * 12581-12590 surfaces are present. Does not claim cups/ipp/print_job/
 * printer_discover/ppd/filter/spool/usb_print ok and does not call
 * libc. No parent wires.
 */
uint32_t
gj_print_soft_ready_u_12589(void)
{
	(void)NULL;
	return b12589_soft_ready();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_print_soft_ready_u_12589(void)
    __attribute__((alias("gj_print_soft_ready_u_12589")));
