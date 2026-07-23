/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13181: CUPS ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_cups_ok_u_13181(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       cups/cupsd probe for the cups/print continuum.
 *   uint32_t __gj_cups_ok_u_13181  (alias)
 *   __libcgj_batch13181_marker = "libcgj-batch13181"
 *
 * Exclusive continuum CREATE-ONLY (13181-13190: cups/print soft id
 * stubs — cups_ok_u_13181, ipp_ok_u_13182, print_job_ok_u_13183,
 * printer_discover_ok_u_13184, ppd_ok_u_13185, filter_ok_u_13186,
 * spool_ok_u_13187, usb_print_ok_u_13188, print_soft_ready_u_13189,
 * batch_id_13190).
 * Unique surface only; no multi-def. Distinct from
 * gj_cups_ok_u_12981 (batch12981), gj_cups_ok_u_12781 (batch12781),
 * gj_cups_ok_u_12581 (batch12581), gj_cups_ok_u_12381 (batch12381),
 * gj_cups_ok_u_12181 (batch12181), gj_cups_ok_u_10181 (batch10181)
 * and gj_cups_printer_ok_u_9441 (batch9441). No parent wires. No
 * __int128. No CUPS implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13181_marker[] = "libcgj-batch13181";

/* Soft CUPS-ok lamp: always off (not a real cups probe). */
#define B13181_CUPS_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13181_cups_ok(void)
{
	return B13181_CUPS_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cups_ok_u_13181 - CUPS ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not inspect cupsd or
 * call libc. No parent wires.
 */
uint32_t
gj_cups_ok_u_13181(void)
{
	(void)NULL;
	return b13181_cups_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_cups_ok_u_13181(void)
    __attribute__((alias("gj_cups_ok_u_13181")));
