/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12181: CUPS ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_cups_ok_u_12181(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       cups/cupsd probe for the cups/print continuum.
 *   uint32_t __gj_cups_ok_u_12181  (alias)
 *   __libcgj_batch12181_marker = "libcgj-batch12181"
 *
 * Exclusive continuum CREATE-ONLY (12181-12190: cups/print soft id
 * stubs — cups_ok_u_12181, ipp_ok_u_12182, print_job_ok_u_12183,
 * printer_discover_ok_u_12184, ppd_ok_u_12185, filter_ok_u_12186,
 * spool_ok_u_12187, usb_print_ok_u_12188, print_soft_ready_u_12189,
 * batch_id_12190). Unique surface only; no multi-def. Distinct from
 * gj_cups_ok_u_11881 (batch11881), gj_cups_ok_u_11681 (batch11681),
 * gj_cups_ok_u_11481 (batch11481), gj_cups_ok_u_10181 (batch10181),
 * gj_cups_printer_ok_u_9441 (batch9441) and gj_cups_ready_u_9449
 * (batch9449). No parent wires. No __int128. No CUPS implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12181_marker[] = "libcgj-batch12181";

/* Soft CUPS-ok lamp: always off (not a real cups probe). */
#define B12181_CUPS_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12181_cups_ok(void)
{
	return B12181_CUPS_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cups_ok_u_12181 - CUPS ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not inspect cupsd or
 * call libc. No parent wires.
 */
uint32_t
gj_cups_ok_u_12181(void)
{
	(void)NULL;
	return b12181_cups_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_cups_ok_u_12181(void)
    __attribute__((alias("gj_cups_ok_u_12181")));
