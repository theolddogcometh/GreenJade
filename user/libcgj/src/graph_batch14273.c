/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14273: job ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_job_ok_u_14273(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       print-job validity probe.
 *   uint32_t __gj_job_ok_u_14273  (alias)
 *   __libcgj_batch14273_marker = "libcgj-batch14273"
 *
 * Exclusive continuum CREATE-ONLY (14271-14280: cups/print soft stubs —
 * cups_ok_u_14271, printer_ok_u_14272, job_ok_u_14273, dest_ok_u_14274,
 * ppd_ok_u_14275, ipp_ok_u_14276, queue_ok_u_14277, media_ok_u_14278,
 * print_soft_ready_u_14279, batch_id_14280). Unique surface only; no
 * multi-def. Distinct from gj_job_ok_u_14073 (batch14073). No parent
 * wires. No __int128. No CUPS implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14273_marker[] = "libcgj-batch14273";

/* Soft job-ok lamp: always off (not a real job probe). */
#define B14273_JOB_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14273_job_ok(void)
{
	return B14273_JOB_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_job_ok_u_14273 - print job ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not inspect jobs or call
 * cupsd/libc. No parent wires.
 */
uint32_t
gj_job_ok_u_14273(void)
{
	(void)NULL;
	return b14273_job_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_job_ok_u_14273(void)
    __attribute__((alias("gj_job_ok_u_14273")));
