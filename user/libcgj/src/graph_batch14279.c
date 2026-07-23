/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14279: print soft continuum lamp (on).
 *
 * Surface (unique symbols):
 *   uint32_t gj_print_soft_ready_u_14279(void);
 *     - Returns 1 (soft lamp only). Indicates the print soft-stub
 *       surfaces in this continuum are present; not cups/printer/job/
 *       dest/ppd/ipp/queue/media readiness.
 *   uint32_t __gj_print_soft_ready_u_14279  (alias)
 *   __libcgj_batch14279_marker = "libcgj-batch14279"
 *
 * Exclusive continuum CREATE-ONLY (14271-14280: cups/print soft stubs —
 * cups_ok_u_14271, printer_ok_u_14272, job_ok_u_14273, dest_ok_u_14274,
 * ppd_ok_u_14275, ipp_ok_u_14276, queue_ok_u_14277, media_ok_u_14278,
 * print_soft_ready_u_14279, batch_id_14280). Unique surface only; no
 * multi-def. Cups/printer/job/dest/ppd/ipp/queue/media ok units remain
 * 0. Distinct from gj_print_soft_ready_u_14079 (batch14079). No parent
 * wires. No __int128. No CUPS implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14279_marker[] = "libcgj-batch14279";

/* Soft continuum lamp: print soft-stub surfaces present. */
#define B14279_PRINT_SOFT_READY  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14279_print_soft_ready(void)
{
	return B14279_PRINT_SOFT_READY;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_print_soft_ready_u_14279 - print soft continuum ready lamp.
 *
 * Always returns 1. Soft pure-data product tag that exclusive wave
 * 14271-14280 surfaces are present. Does not claim cups/printer/job/
 * dest/ppd/ipp/queue/media ready and does not call cupsd/libc.
 * No parent wires.
 */
uint32_t
gj_print_soft_ready_u_14279(void)
{
	(void)NULL;
	return b14279_print_soft_ready();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_print_soft_ready_u_14279(void)
    __attribute__((alias("gj_print_soft_ready_u_14279")));
