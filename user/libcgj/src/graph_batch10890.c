/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10890: cups/print soft + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_10890(void);
 *     - Returns the compile-time graph batch number for this TU (10890).
 *   uint32_t __gj_batch_id_10890  (alias)
 *   __libcgj_batch10890_marker = "libcgj-batch10890"
 *
 * Exclusive continuum CREATE-ONLY (10881-10890: cups/print soft id
 * stubs — cups_ok_u_10881, ipp_ok_u_10882, print_job_ok_u_10883,
 * printer_discover_ok_u_10884, ppd_ok_u_10885, filter_ok_u_10886,
 * spool_ok_u_10887, usb_print_ok_u_10888, print_soft_ready_u_10889,
 * batch_id_10890). Unique surfaces only; no multi-def. Does NOT
 * redefine gj_batch_id / gj_graph_milestone / prior batch_id_* symbols
 * — avoid multi-def. Distinct from gj_batch_id_10190 (batch10190). No
 * parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10890_marker[] = "libcgj-batch10890";

#define B10890_BATCH_ID  10890u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10890_id(void)
{
	return B10890_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_10890 - report this TU's graph batch number.
 *
 * Always returns 10890.
 */
uint32_t
gj_batch_id_10890(void)
{
	(void)NULL;
	return b10890_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_10890(void)
    __attribute__((alias("gj_batch_id_10890")));
