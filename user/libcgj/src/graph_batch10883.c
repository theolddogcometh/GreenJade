/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10883: print job ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_print_job_ok_u_10883(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       print-job / queue job probe for the cups/print continuum.
 *   uint32_t __gj_print_job_ok_u_10883  (alias)
 *   __libcgj_batch10883_marker = "libcgj-batch10883"
 *
 * Exclusive continuum CREATE-ONLY (10881-10890: cups/print soft id
 * stubs — cups_ok_u_10881, ipp_ok_u_10882, print_job_ok_u_10883,
 * printer_discover_ok_u_10884, ppd_ok_u_10885, filter_ok_u_10886,
 * spool_ok_u_10887, usb_print_ok_u_10888, print_soft_ready_u_10889,
 * batch_id_10890). Unique surface only; no multi-def. Distinct from
 * gj_print_job_ok_u_10183 (batch10183) and gj_cups_job_ok_u_9442
 * (batch9442). No parent wires. No __int128. No CUPS implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10883_marker[] = "libcgj-batch10883";

/* Soft print-job-ok lamp: always off (not a real job probe). */
#define B10883_PRINT_JOB_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10883_print_job_ok(void)
{
	return B10883_PRINT_JOB_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_print_job_ok_u_10883 - print job ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not inspect job queues or
 * call libc. No parent wires.
 */
uint32_t
gj_print_job_ok_u_10883(void)
{
	(void)NULL;
	return b10883_print_job_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_print_job_ok_u_10883(void)
    __attribute__((alias("gj_print_job_ok_u_10883")));
