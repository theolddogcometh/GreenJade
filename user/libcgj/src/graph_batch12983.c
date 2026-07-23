/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12983: print job ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_print_job_ok_u_12983(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       print-job / CUPS job probe for the cups/print continuum.
 *   uint32_t __gj_print_job_ok_u_12983  (alias)
 *   __libcgj_batch12983_marker = "libcgj-batch12983"
 *
 * Exclusive continuum CREATE-ONLY (12981-12990: cups/print soft id
 * stubs — cups_ok_u_12981, ipp_ok_u_12982, print_job_ok_u_12983,
 * printer_discover_ok_u_12984, ppd_ok_u_12985, filter_ok_u_12986,
 * spool_ok_u_12987, usb_print_ok_u_12988, print_soft_ready_u_12989,
 * batch_id_12990). Unique surface only; no multi-def. Distinct from
 * gj_print_job_ok_u_12783 (batch12783), gj_print_job_ok_u_12583
 * (batch12583), gj_print_job_ok_u_12383 (batch12383),
 * gj_print_job_ok_u_12183 (batch12183), gj_print_job_ok_u_10183
 * (batch10183) and gj_cups_job_ok_u_9442 (batch9442). No parent
 * wires. No __int128. No CUPS implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12983_marker[] = "libcgj-batch12983";

/* Soft print-job-ok lamp: always off (not a real job probe). */
#define B12983_PRINT_JOB_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12983_print_job_ok(void)
{
	return B12983_PRINT_JOB_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_print_job_ok_u_12983 - print job ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not inspect CUPS jobs or
 * call libc. No parent wires.
 */
uint32_t
gj_print_job_ok_u_12983(void)
{
	(void)NULL;
	return b12983_print_job_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_print_job_ok_u_12983(void)
    __attribute__((alias("gj_print_job_ok_u_12983")));
