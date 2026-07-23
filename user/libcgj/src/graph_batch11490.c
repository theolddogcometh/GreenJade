/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11490: cups/print soft + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_11490(void);
 *     - Returns the compile-time graph batch number for this TU (11490).
 *   uint32_t __gj_batch_id_11490  (alias)
 *   __libcgj_batch11490_marker = "libcgj-batch11490"
 *
 * Exclusive continuum CREATE-ONLY (11481-11490: cups/print soft id
 * stubs — cups_ok_u_11481, ipp_ok_u_11482, print_job_ok_u_11483,
 * printer_discover_ok_u_11484, ppd_ok_u_11485, filter_ok_u_11486,
 * spool_ok_u_11487, usb_print_ok_u_11488, print_soft_ready_u_11489,
 * batch_id_11490). Unique surfaces only; no multi-def. Does NOT
 * redefine gj_batch_id / gj_graph_milestone / prior batch_id_* symbols
 * — avoid multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11490_marker[] = "libcgj-batch11490";

#define B11490_BATCH_ID  11490u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11490_id(void)
{
	return B11490_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_11490 - report this TU graph batch number.
 *
 * Always returns 11490.
 */
uint32_t
gj_batch_id_11490(void)
{
	(void)NULL;
	return b11490_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_11490(void)
    __attribute__((alias("gj_batch_id_11490")));
