/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13787: queue ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_queue_ok_u_13787(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       print-queue validity probe.
 *   uint32_t __gj_queue_ok_u_13787  (alias)
 *   __libcgj_batch13787_marker = "libcgj-batch13787"
 *
 * Exclusive continuum CREATE-ONLY (13781-13790: print soft stubs —
 * cups_ok_u_13781, printer_ok_u_13782, job_ok_u_13783, dest_ok_u_13784,
 * ppd_ok_u_13785, ipp_ok_u_13786, queue_ok_u_13787, media_ok_u_13788,
 * print_soft_ready_u_13789, batch_id_13790). Unique surface only; no
 * multi-def. No parent wires. No __int128. No CUPS implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13787_marker[] = "libcgj-batch13787";

/* Soft queue-ok lamp: always off (not a real queue probe). */
#define B13787_QUEUE_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13787_queue_ok(void)
{
	return B13787_QUEUE_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_queue_ok_u_13787 - print queue ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not inspect queues or
 * call cupsd/libc. No parent wires.
 */
uint32_t
gj_queue_ok_u_13787(void)
{
	(void)NULL;
	return b13787_queue_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_queue_ok_u_13787(void)
    __attribute__((alias("gj_queue_ok_u_13787")));
