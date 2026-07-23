/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12987: spool ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_spool_ok_u_12987(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       print spool / CUPS queue probe for the cups/print continuum.
 *   uint32_t __gj_spool_ok_u_12987  (alias)
 *   __libcgj_batch12987_marker = "libcgj-batch12987"
 *
 * Exclusive continuum CREATE-ONLY (12981-12990: cups/print soft id
 * stubs — cups_ok_u_12981, ipp_ok_u_12982, print_job_ok_u_12983,
 * printer_discover_ok_u_12984, ppd_ok_u_12985, filter_ok_u_12986,
 * spool_ok_u_12987, usb_print_ok_u_12988, print_soft_ready_u_12989,
 * batch_id_12990). Unique surface only; no multi-def. Distinct from
 * gj_spool_ok_u_12787 (batch12787), gj_spool_ok_u_12587 (batch12587),
 * gj_spool_ok_u_12387 (batch12387), gj_spool_ok_u_12187 (batch12187),
 * gj_spool_ok_u_10187 (batch10187) and gj_cups_queue_ok_u_9446
 * (batch9446). No parent wires. No __int128. No CUPS implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12987_marker[] = "libcgj-batch12987";

/* Soft spool-ok lamp: always off (not a real spool/queue probe). */
#define B12987_SPOOL_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12987_spool_ok(void)
{
	return B12987_SPOOL_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_spool_ok_u_12987 - spool ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not inspect print queues
 * or call libc. No parent wires.
 */
uint32_t
gj_spool_ok_u_12987(void)
{
	(void)NULL;
	return b12987_spool_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_spool_ok_u_12987(void)
    __attribute__((alias("gj_spool_ok_u_12987")));
