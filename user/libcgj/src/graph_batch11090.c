/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11090: cups/print soft + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_11090(void);
 *     - Returns the compile-time graph batch number for this TU (11090).
 *   uint32_t __gj_batch_id_11090  (alias)
 *   __libcgj_batch11090_marker = "libcgj-batch11090"
 *
 * Exclusive continuum CREATE-ONLY (11081-11090: cups/print soft id
 * stubs — cups_ok_u_11081, ipp_ok_u_11082, print_job_ok_u_11083,
 * printer_discover_ok_u_11084, ppd_ok_u_11085, filter_ok_u_11086,
 * spool_ok_u_11087, usb_print_ok_u_11088, print_soft_ready_u_11089,
 * batch_id_11090). Unique surfaces only; no multi-def. Does NOT
 * redefine gj_batch_id / gj_graph_milestone / prior batch_id_* symbols
 * — avoid multi-def. Distinct from gj_batch_id_10890 (batch10890) and
 * gj_batch_id_10190 (batch10190). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11090_marker[] = "libcgj-batch11090";

#define B11090_BATCH_ID  11090u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11090_id(void)
{
	return B11090_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_11090 - report this TU's graph batch number.
 *
 * Always returns 11090.
 */
uint32_t
gj_batch_id_11090(void)
{
	(void)NULL;
	return b11090_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_11090(void)
    __attribute__((alias("gj_batch_id_11090")));
