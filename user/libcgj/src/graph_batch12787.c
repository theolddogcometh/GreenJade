/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12787: spool ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_spool_ok_u_12787(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       print spool / queue directory probe for the cups/print
 *       continuum.
 *   uint32_t __gj_spool_ok_u_12787  (alias)
 *   __libcgj_batch12787_marker = "libcgj-batch12787"
 *
 * Exclusive continuum CREATE-ONLY (12781-12790: cups/print soft id
 * stubs — cups_ok_u_12781, ipp_ok_u_12782, print_job_ok_u_12783,
 * printer_discover_ok_u_12784, ppd_ok_u_12785, filter_ok_u_12786,
 * spool_ok_u_12787, usb_print_ok_u_12788, print_soft_ready_u_12789,
 * batch_id_12790). Unique surface only; no multi-def. Distinct from
 * gj_spool_ok_u_12587 (batch12587), gj_spool_ok_u_12387 (batch12387),
 * gj_spool_ok_u_12187 (batch12187), gj_spool_ok_u_10187 (batch10187) and
 * gj_cups_queue_ok_u_9446 (batch9446). No parent wires. No __int128.
 * No CUPS implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12787_marker[] = "libcgj-batch12787";

/* Soft spool-ok lamp: always off (not a real spool probe). */
#define B12787_SPOOL_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12787_spool_ok(void)
{
	return B12787_SPOOL_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_spool_ok_u_12787 - spool ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not inspect /var/spool
 * or call libc. No parent wires.
 */
uint32_t
gj_spool_ok_u_12787(void)
{
	(void)NULL;
	return b12787_spool_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_spool_ok_u_12787(void)
    __attribute__((alias("gj_spool_ok_u_12787")));
