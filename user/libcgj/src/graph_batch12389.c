/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12389: print soft continuum lamp (on).
 *
 * Surface (unique symbols):
 *   uint32_t gj_print_soft_ready_u_12389(void);
 *     - Returns 1 (soft lamp only). Indicates the cups/print soft-id
 *       stub surfaces in this continuum are present; not cups, ipp,
 *       print_job, printer_discover, ppd, filter, spool, or usb_print
 *       ok.
 *   uint32_t __gj_print_soft_ready_u_12389  (alias)
 *   __libcgj_batch12389_marker = "libcgj-batch12389"
 *
 * Exclusive continuum CREATE-ONLY (12381-12390: cups/print soft id
 * stubs — cups_ok_u_12381, ipp_ok_u_12382, print_job_ok_u_12383,
 * printer_discover_ok_u_12384, ppd_ok_u_12385, filter_ok_u_12386,
 * spool_ok_u_12387, usb_print_ok_u_12388, print_soft_ready_u_12389,
 * batch_id_12390). Unique surface only; no multi-def. cups/ipp/
 * print_job/printer_discover/ppd/filter/spool/usb_print ok units
 * remain 0. Distinct from gj_print_soft_ready_u_10189 (batch10189)
 * and gj_cups_ready_u_9449 (batch9449). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12389_marker[] = "libcgj-batch12389";

/* Soft continuum lamp: surfaces present (soft ready only). */
#define B12389_SOFT_READY  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12389_soft_ready(void)
{
	return B12389_SOFT_READY;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_print_soft_ready_u_12389 - print soft continuum ready lamp.
 *
 * Always returns 1. Soft pure-data product tag that exclusive wave
 * 12381-12390 surfaces are present. Does not claim cups/ipp/print_job/
 * printer_discover/ppd/filter/spool/usb_print ok and does not call
 * libc. No parent wires.
 */
uint32_t
gj_print_soft_ready_u_12389(void)
{
	(void)NULL;
	return b12389_soft_ready();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_print_soft_ready_u_12389(void)
    __attribute__((alias("gj_print_soft_ready_u_12389")));
