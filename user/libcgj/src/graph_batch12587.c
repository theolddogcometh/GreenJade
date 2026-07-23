/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12587: spool ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_spool_ok_u_12587(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       print spool / queue directory probe for the cups/print
 *       continuum.
 *   uint32_t __gj_spool_ok_u_12587  (alias)
 *   __libcgj_batch12587_marker = "libcgj-batch12587"
 *
 * Exclusive continuum CREATE-ONLY (12581-12590: cups/print soft id
 * stubs — cups_ok_u_12581, ipp_ok_u_12582, print_job_ok_u_12583,
 * printer_discover_ok_u_12584, ppd_ok_u_12585, filter_ok_u_12586,
 * spool_ok_u_12587, usb_print_ok_u_12588, print_soft_ready_u_12589,
 * batch_id_12590). Unique surface only; no multi-def. Distinct from
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

const char __libcgj_batch12587_marker[] = "libcgj-batch12587";

/* Soft spool-ok lamp: always off (not a real spool probe). */
#define B12587_SPOOL_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12587_spool_ok(void)
{
	return B12587_SPOOL_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_spool_ok_u_12587 - spool ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not inspect /var/spool
 * or call libc. No parent wires.
 */
uint32_t
gj_spool_ok_u_12587(void)
{
	(void)NULL;
	return b12587_spool_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_spool_ok_u_12587(void)
    __attribute__((alias("gj_spool_ok_u_12587")));
