/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12781: CUPS ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_cups_ok_u_12781(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       cups/cupsd probe for the cups/print continuum.
 *   uint32_t __gj_cups_ok_u_12781  (alias)
 *   __libcgj_batch12781_marker = "libcgj-batch12781"
 *
 * Exclusive continuum CREATE-ONLY (12781-12790: cups/print soft id
 * stubs — cups_ok_u_12781, ipp_ok_u_12782, print_job_ok_u_12783,
 * printer_discover_ok_u_12784, ppd_ok_u_12785, filter_ok_u_12786,
 * spool_ok_u_12787, usb_print_ok_u_12788, print_soft_ready_u_12789,
 * batch_id_12790). Unique surface only; no multi-def. Distinct from
 * gj_cups_ok_u_12581 (batch12581), gj_cups_ok_u_12381 (batch12381),
 * gj_cups_ok_u_12181 (batch12181), gj_cups_ok_u_10181 (batch10181) and
 * gj_cups_printer_ok_u_9441 (batch9441). No parent wires. No __int128.
 * No CUPS implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12781_marker[] = "libcgj-batch12781";

/* Soft CUPS-ok lamp: always off (not a real cups probe). */
#define B12781_CUPS_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12781_cups_ok(void)
{
	return B12781_CUPS_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cups_ok_u_12781 - CUPS ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not inspect cupsd or
 * call libc. No parent wires.
 */
uint32_t
gj_cups_ok_u_12781(void)
{
	(void)NULL;
	return b12781_cups_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_cups_ok_u_12781(void)
    __attribute__((alias("gj_cups_ok_u_12781")));
