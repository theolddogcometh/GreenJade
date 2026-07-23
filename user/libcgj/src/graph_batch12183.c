/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12183: print job ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_print_job_ok_u_12183(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       print-job / queue job probe for the cups/print continuum.
 *   uint32_t __gj_print_job_ok_u_12183  (alias)
 *   __libcgj_batch12183_marker = "libcgj-batch12183"
 *
 * Exclusive continuum CREATE-ONLY (12181-12190: cups/print soft id
 * stubs — cups_ok_u_12181, ipp_ok_u_12182, print_job_ok_u_12183,
 * printer_discover_ok_u_12184, ppd_ok_u_12185, filter_ok_u_12186,
 * spool_ok_u_12187, usb_print_ok_u_12188, print_soft_ready_u_12189,
 * batch_id_12190). Unique surface only; no multi-def. Distinct from
 * gj_print_job_ok_u_11883 (batch11883), gj_print_job_ok_u_11683
 * (batch11683), gj_print_job_ok_u_10183 (batch10183) and
 * gj_cups_job_ok_u_9442 (batch9442). No parent wires. No __int128.
 * No CUPS implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12183_marker[] = "libcgj-batch12183";

/* Soft print-job-ok lamp: always off (not a real job probe). */
#define B12183_PRINT_JOB_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12183_print_job_ok(void)
{
	return B12183_PRINT_JOB_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_print_job_ok_u_12183 - print job ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not inspect job queues or
 * call libc. No parent wires.
 */
uint32_t
gj_print_job_ok_u_12183(void)
{
	(void)NULL;
	return b12183_print_job_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_print_job_ok_u_12183(void)
    __attribute__((alias("gj_print_job_ok_u_12183")));
