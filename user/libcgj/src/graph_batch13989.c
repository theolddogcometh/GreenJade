/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13989: print soft continuum lamp (on).
 *
 * Surface (unique symbols):
 *   uint32_t gj_print_soft_ready_u_13989(void);
 *     - Returns 1 (soft lamp only). Indicates the print soft-stub
 *       surfaces in this continuum are present; not cups/printer/job/
 *       dest/ppd/ipp/queue/media readiness.
 *   uint32_t __gj_print_soft_ready_u_13989  (alias)
 *   __libcgj_batch13989_marker = "libcgj-batch13989"
 *
 * Exclusive continuum CREATE-ONLY (13981-13990: print soft stubs —
 * cups_ok_u_13981, printer_ok_u_13982, job_ok_u_13983, dest_ok_u_13984,
 * ppd_ok_u_13985, ipp_ok_u_13986, queue_ok_u_13987, media_ok_u_13988,
 * print_soft_ready_u_13989, batch_id_13990). Unique surface only; no
 * multi-def. Cups/printer/job/dest/ppd/ipp/queue/media ok units remain
 * 0. No parent wires. No __int128. No CUPS implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13989_marker[] = "libcgj-batch13989";

/* Soft continuum lamp: print soft-stub surfaces present. */
#define B13989_PRINT_SOFT_READY  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13989_print_soft_ready(void)
{
	return B13989_PRINT_SOFT_READY;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_print_soft_ready_u_13989 - print soft continuum ready lamp.
 *
 * Always returns 1. Soft pure-data product tag that exclusive wave
 * 13981-13990 surfaces are present. Does not claim cups/printer/job/
 * dest/ppd/ipp/queue/media ready and does not call cupsd/libc.
 * No parent wires.
 */
uint32_t
gj_print_soft_ready_u_13989(void)
{
	(void)NULL;
	return b13989_print_soft_ready();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_print_soft_ready_u_13989(void)
    __attribute__((alias("gj_print_soft_ready_u_13989")));
