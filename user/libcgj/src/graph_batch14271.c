/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14271: CUPS ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_cups_ok_u_14271(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       CUPS / cupsd availability probe.
 *   uint32_t __gj_cups_ok_u_14271  (alias)
 *   __libcgj_batch14271_marker = "libcgj-batch14271"
 *
 * Exclusive continuum CREATE-ONLY (14271-14280: cups/print soft stubs —
 * cups_ok_u_14271, printer_ok_u_14272, job_ok_u_14273, dest_ok_u_14274,
 * ppd_ok_u_14275, ipp_ok_u_14276, queue_ok_u_14277, media_ok_u_14278,
 * print_soft_ready_u_14279, batch_id_14280). Unique surface only; no
 * multi-def. Distinct from gj_cups_ok_u_14071 (batch14071). No parent
 * wires. No __int128. No CUPS implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14271_marker[] = "libcgj-batch14271";

/* Soft CUPS ok lamp: always off (not a real cupsd probe). */
#define B14271_CUPS_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14271_cups_ok(void)
{
	return B14271_CUPS_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cups_ok_u_14271 - CUPS ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not contact cupsd or call
 * libc. No parent wires.
 */
uint32_t
gj_cups_ok_u_14271(void)
{
	(void)NULL;
	return b14271_cups_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_cups_ok_u_14271(void)
    __attribute__((alias("gj_cups_ok_u_14271")));
