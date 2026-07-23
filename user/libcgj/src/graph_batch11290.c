/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11290: cups/print soft + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_11290(void);
 *     - Returns the compile-time graph batch number for this TU (11290).
 *   uint32_t __gj_batch_id_11290  (alias)
 *   __libcgj_batch11290_marker = "libcgj-batch11290"
 *
 * Exclusive continuum CREATE-ONLY (11281-11290: cups/print soft id
 * stubs — cups_ok_u_11281, ipp_ok_u_11282, print_job_ok_u_11283,
 * printer_discover_ok_u_11284, ppd_ok_u_11285, filter_ok_u_11286,
 * spool_ok_u_11287, usb_print_ok_u_11288, print_soft_ready_u_11289,
 * batch_id_11290). Unique surfaces only; no multi-def. Does NOT
 * redefine gj_batch_id / gj_graph_milestone / prior batch_id_* symbols
 * — avoid multi-def. Distinct from gj_batch_id_10190 (batch10190).
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11290_marker[] = "libcgj-batch11290";

#define B11290_BATCH_ID  11290u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11290_id(void)
{
	return B11290_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_11290 - report this TU's graph batch number.
 *
 * Always returns 11290.
 */
uint32_t
gj_batch_id_11290(void)
{
	(void)NULL;
	return b11290_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_11290(void)
    __attribute__((alias("gj_batch_id_11290")));
