/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11287: spool ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_spool_ok_u_11287(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       print spool / queue directory probe for the cups/print
 *       continuum.
 *   uint32_t __gj_spool_ok_u_11287  (alias)
 *   __libcgj_batch11287_marker = "libcgj-batch11287"
 *
 * Exclusive continuum CREATE-ONLY (11281-11290: cups/print soft id
 * stubs — cups_ok_u_11281, ipp_ok_u_11282, print_job_ok_u_11283,
 * printer_discover_ok_u_11284, ppd_ok_u_11285, filter_ok_u_11286,
 * spool_ok_u_11287, usb_print_ok_u_11288, print_soft_ready_u_11289,
 * batch_id_11290). Unique surface only; no multi-def. Distinct from
 * gj_spool_ok_u_10187 (batch10187) and gj_cups_queue_ok_u_9446
 * (batch9446). No parent wires. No __int128. No CUPS implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11287_marker[] = "libcgj-batch11287";

/* Soft spool-ok lamp: always off (not a real spool probe). */
#define B11287_SPOOL_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11287_spool_ok(void)
{
	return B11287_SPOOL_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_spool_ok_u_11287 - spool ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not inspect /var/spool
 * or call libc. No parent wires.
 */
uint32_t
gj_spool_ok_u_11287(void)
{
	(void)NULL;
	return b11287_spool_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_spool_ok_u_11287(void)
    __attribute__((alias("gj_spool_ok_u_11287")));
