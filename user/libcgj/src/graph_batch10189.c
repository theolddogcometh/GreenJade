/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10189: print soft continuum lamp (on).
 *
 * Surface (unique symbols):
 *   uint32_t gj_print_soft_ready_u_10189(void);
 *     - Returns 1 (soft lamp only). Indicates the cups/print soft-id
 *       stub surfaces in this continuum are present; not cups, ipp,
 *       print_job, printer_discover, ppd, filter, spool, or usb_print
 *       ok.
 *   uint32_t __gj_print_soft_ready_u_10189  (alias)
 *   __libcgj_batch10189_marker = "libcgj-batch10189"
 *
 * Exclusive continuum CREATE-ONLY (10181-10190: cups/print soft id
 * stubs — cups_ok_u_10181, ipp_ok_u_10182, print_job_ok_u_10183,
 * printer_discover_ok_u_10184, ppd_ok_u_10185, filter_ok_u_10186,
 * spool_ok_u_10187, usb_print_ok_u_10188, print_soft_ready_u_10189,
 * batch_id_10190). Unique surface only; no multi-def. cups/ipp/
 * print_job/printer_discover/ppd/filter/spool/usb_print ok units
 * remain 0. Distinct from gj_cups_ready_u_9449 (batch9449). No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10189_marker[] = "libcgj-batch10189";

/* Soft continuum lamp: surfaces present (soft ready only). */
#define B10189_SOFT_READY  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10189_soft_ready(void)
{
	return B10189_SOFT_READY;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_print_soft_ready_u_10189 - print soft continuum ready lamp.
 *
 * Always returns 1. Soft pure-data product tag that exclusive wave
 * 10181-10190 surfaces are present. Does not claim cups/ipp/print_job/
 * printer_discover/ppd/filter/spool/usb_print ok and does not call
 * libc. No parent wires.
 */
uint32_t
gj_print_soft_ready_u_10189(void)
{
	(void)NULL;
	return b10189_soft_ready();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_print_soft_ready_u_10189(void)
    __attribute__((alias("gj_print_soft_ready_u_10189")));
