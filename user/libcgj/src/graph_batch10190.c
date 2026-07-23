/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10190: cups/print soft + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_10190(void);
 *     - Returns the compile-time graph batch number for this TU (10190).
 *   uint32_t __gj_batch_id_10190  (alias)
 *   __libcgj_batch10190_marker = "libcgj-batch10190"
 *
 * Exclusive continuum CREATE-ONLY (10181-10190: cups/print soft id
 * stubs — cups_ok_u_10181, ipp_ok_u_10182, print_job_ok_u_10183,
 * printer_discover_ok_u_10184, ppd_ok_u_10185, filter_ok_u_10186,
 * spool_ok_u_10187, usb_print_ok_u_10188, print_soft_ready_u_10189,
 * batch_id_10190). Unique surfaces only; no multi-def. Does NOT
 * redefine gj_batch_id / gj_graph_milestone / prior batch_id_* symbols
 * — avoid multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10190_marker[] = "libcgj-batch10190";

#define B10190_BATCH_ID  10190u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10190_id(void)
{
	return B10190_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_10190 - report this TU's graph batch number.
 *
 * Always returns 10190.
 */
uint32_t
gj_batch_id_10190(void)
{
	(void)NULL;
	return b10190_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_10190(void)
    __attribute__((alias("gj_batch_id_10190")));
