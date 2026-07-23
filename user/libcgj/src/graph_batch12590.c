/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12590: cups/print soft + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_12590(void);
 *     - Returns the compile-time graph batch number for this TU (12590).
 *   uint32_t __gj_batch_id_12590  (alias)
 *   __libcgj_batch12590_marker = "libcgj-batch12590"
 *
 * Exclusive continuum CREATE-ONLY (12581-12590: cups/print soft id
 * stubs — cups_ok_u_12581, ipp_ok_u_12582, print_job_ok_u_12583,
 * printer_discover_ok_u_12584, ppd_ok_u_12585, filter_ok_u_12586,
 * spool_ok_u_12587, usb_print_ok_u_12588, print_soft_ready_u_12589,
 * batch_id_12590). Unique surfaces only; no multi-def. Does NOT
 * redefine gj_batch_id / gj_graph_milestone / prior batch_id_* symbols
 * — avoid multi-def. Distinct from gj_batch_id_12390 (batch12390) and
 * gj_batch_id_12190 (batch12190). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12590_marker[] = "libcgj-batch12590";

#define B12590_BATCH_ID  12590u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12590_id(void)
{
	return B12590_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_12590 - report this TU's graph batch number.
 *
 * Always returns 12590.
 */
uint32_t
gj_batch_id_12590(void)
{
	(void)NULL;
	return b12590_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_12590(void)
    __attribute__((alias("gj_batch_id_12590")));
